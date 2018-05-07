#include <string>
#include <atomic>

#include "madara/knowledge/ThreadSafeContext.h"
#include "madara/knowledge/KnowledgeBase.h"
#include "madara/utility/Utility.h"

namespace knowledge = madara::knowledge;
namespace logger = madara::logger;

typedef  madara::knowledge::KnowledgeRecord::Integer  Integer;

template<typename... Args>
inline void log(Args&&... args) {
  madara_logger_ptr_log (logger::global_logger.get(), logger::LOG_ALWAYS,
    std::forward<Args>(args)...);
}

#define LOG(exp) \
  ({ \
   decltype((exp)) e = (exp); \
   std::ostringstream msg; \
   msg << #exp " == " << e; \
   log("INFO    : %s\n", msg.str().c_str()); \
   e; \
  })

#define LOG_AS(exp, type) \
  ({ \
   decltype(exp) e = (exp); \
   std::ostringstream msg; \
   msg << #exp " == " << ((type)e); \
   log("INFO    : %s\n", msg.str().c_str()); \
   e; \
  })

#define TEST_OP(lhs, op, rhs) \
  do { \
    decltype(lhs) l = (lhs); \
    decltype(rhs) r = (rhs); \
    std::ostringstream msg; \
    msg << #lhs " [" << l << "] " #op " " #rhs " [" << r <<"] "; \
    std::string smsg = msg.str(); \
    const char *cmsg = smsg.c_str(); \
    if (l op r) { \
      log("SUCCESS : %s\n", cmsg); \
    } else { \
      log("FAIL    : %s\n", cmsg); \
    } \
  } while(0)

#define TEST_EQ(lhs, rhs) TEST_OP(lhs, ==, rhs)
#define TEST_NE(lhs, rhs) TEST_OP(lhs, !=, rhs)
#define TEST_LT(lhs, rhs) TEST_OP(lhs, <,  rhs)
#define TEST_LE(lhs, rhs) TEST_OP(lhs, <=, rhs)
#define TEST_GT(lhs, rhs) TEST_OP(lhs, >,  rhs)
#define TEST_GE(lhs, rhs) TEST_OP(lhs, >=, rhs)

void test_unshared_record (void)
{
  knowledge::KnowledgeRecord rec;

  std::string str = "Hello World";
  const char *orig_ptr = str.c_str();
  rec.set_value (std::move(str));

  std::string str_out = rec.to_string();

  TEST_NE(orig_ptr, str_out.c_str());
}

void test_shared_record (void)
{
  knowledge::KnowledgeBase kb;
  knowledge::KnowledgeRecord rec;

  std::string str = "Hello World";
  auto s1 = std::make_shared<std::string>(std::move(str));
  const char *orig_ptr = s1->c_str();
  rec.emplace_shared_string (std::move(s1));

  std::shared_ptr<std::string> str_out = rec.share_string();

  TEST_EQ(orig_ptr, str_out->c_str());

  std::string big_str = "This is a string that might be much longer and be expensive to copy.";
  const char * orig_sptr = big_str.c_str();

  // Creates a vector with 4000 entries, all 42, without any copying
  knowledge::KnowledgeRecord ints(knowledge::tags::integers, 4000, 42);

  std::shared_ptr<std::vector<int64_t>> iptr = ints.share_integers();
  TEST_NE(iptr.get(), (void*)0);
  int64_t *orig_iptr = &(*iptr)[0];

  kb.set(".my_string", std::move(big_str)); // std::move avoids copying the string data
  kb.set(".my_array", ints); // std::move not needed here to avoid copying the integer data,
                             // but would be slightly more efficient as it would avoid touching ref counts

  TEST_EQ(kb.get(".my_array").retrieve_index(0).to_integer(), 42);

  std::shared_ptr<std::string> big_str_out = kb.take_string(".my_string"); // leaves .my_string empty
  const char *out_sptr = big_str_out->c_str();

  std::shared_ptr<std::vector<int64_t>> ints_out = kb.share_integers(".my_array"); // shared with .my_array still in kb
  int64_t *out_iptr = &(*ints_out)[0];

  TEST_EQ((void*)orig_sptr, (void*)out_sptr);
  TEST_EQ((void*)orig_iptr, (void*)out_iptr);

  TEST_EQ(ints_out->at(0), 42);

  kb.set_index(".my_array", 0, 47); // Causes a copy to be made, so we can modify without changing ints_out

  TEST_EQ(kb.get(".my_array").retrieve_index(0).to_integer(), 47);
  TEST_EQ(ints_out->at(0), 42);
}

int main (int, char **)
{
  TEST_EQ(sizeof(knowledge::KnowledgeRecord), 48UL);
  test_unshared_record ();
  test_shared_record ();

  return 0;
}
