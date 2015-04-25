
#include "com_madara_containers_NativeDoubleVector.h"
#include "madara/knowledge_engine/containers/Native_Double_Vector.h"

namespace engine = Madara::Knowledge_Engine;
namespace containers = engine::Containers;
typedef containers::Native_Double_Vector    Native_Double_Vector;

/*
 * Class:     com_madara_containers_NativeDoubleVector
 * Method:    jni_NativeDoubleVector
 * Signature: ()J
 */
MADARA_Export jlong JNICALL Java_com_madara_containers_NativeDoubleVector_jni_1NativeDoubleVector__
  (JNIEnv *, jobject)
{
  return (jlong) new Native_Double_Vector ();
}

/*
 * Class:     com_madara_containers_NativeDoubleVector
 * Method:    jni_NativeDoubleVector
 * Signature: (J)J
 */
MADARA_Export jlong JNICALL Java_com_madara_containers_NativeDoubleVector_jni_1NativeDoubleVector__J
  (JNIEnv * env, jobject, jlong cptr)
{
  Native_Double_Vector * result (0);
  Native_Double_Vector * source = (Native_Double_Vector *) cptr;

  if (source)
  {
    result = new Native_Double_Vector (*source);
  }

  return (jlong) result;
}

/*
 * Class:     com_madara_containers_NativeDoubleVector
 * Method:    jni_freeNativeDoubleVector
 * Signature: (J)V
 */
MADARA_Export void JNICALL Java_com_madara_containers_NativeDoubleVector_jni_1freeNativeDoubleVector
  (JNIEnv * env, jclass, jlong cptr)
{
  delete (Native_Double_Vector *) cptr;
}

/*
 * Class:     com_madara_containers_NativeDoubleVector
 * Method:    jni_set
 * Signature: (JID)V
 */
MADARA_Export void JNICALL Java_com_madara_containers_NativeDoubleVector_jni_1set
  (JNIEnv * env, jobject, jlong cptr, jint index, jdouble value)
{
  Native_Double_Vector * current = (Native_Double_Vector *) cptr;

  if (current)
  {
    current->set (index, value);
  }
}

/*
 * Class:     com_madara_containers_NativeDoubleVector
 * Method:    jni_getName
 * Signature: (J)Ljava/lang/String;
 */
MADARA_Export jstring JNICALL Java_com_madara_containers_NativeDoubleVector_jni_1getName
  (JNIEnv * env, jobject, jlong cptr)
{
  jstring result;
  Native_Double_Vector * current = (Native_Double_Vector *) cptr;

  if (current)
  {
    result = env->NewStringUTF (current->get_name ().c_str ());
  }
  else
  {
    result = env->NewStringUTF ("");
  }

  return result;
}

/*
 * Class:     com_madara_containers_NativeDoubleVector
 * Method:    jni_setName
 * Signature: (JJJLjava/lang/String;)V
 */
MADARA_Export void JNICALL Java_com_madara_containers_NativeDoubleVector_jni_1setName
  (JNIEnv * env, jobject, jlong cptr, jlong type, jlong context, jstring name)
{
  Native_Double_Vector * current = (Native_Double_Vector *) cptr;

  if (current)
  {
    const char * str_name = env->GetStringUTFChars (name, 0);

    if (type == 0)
    {
      engine::Knowledge_Base * kb = (engine::Knowledge_Base *) context;
      current->set_name (str_name, *kb);
    }
    else if (type == 1)
    {
      engine::Variables * vars = (engine::Variables *) context;
      current->set_name (str_name, *vars);
    }

    env->ReleaseStringUTFChars (name, str_name);
  }
}

/*
 * Class:     com_madara_containers_NativeDoubleVector
 * Method:    jni_get
 * Signature: (JI)D
 */
MADARA_Export jdouble JNICALL Java_com_madara_containers_NativeDoubleVector_jni_1get
  (JNIEnv * env, jobject, jlong cptr, jint index)
{
  jdouble result (0);

  Native_Double_Vector * current = (Native_Double_Vector *) cptr;
  if (current)
    result = (*current) [index];

  return result;
}

/*
 * Class:     com_madara_containers_NativeDoubleVector
 * Method:    jni_toRecord
 * Signature: (JI)J
 */
MADARA_Export jlong JNICALL Java_com_madara_containers_NativeDoubleVector_jni_1toRecord__JI
  (JNIEnv * env, jobject, jlong cptr, jint index)
{
  Madara::Knowledge_Record * result (0);

  Native_Double_Vector * current = (Native_Double_Vector *) cptr;
  if (current)
    result = new Madara::Knowledge_Record (current->to_record (index));

  return (jlong) result;
}

/*
 * Class:     com_madara_containers_NativeDoubleVector
 * Method:    jni_toRecord
 * Signature: (J)J
 */
MADARA_Export jlong JNICALL Java_com_madara_containers_NativeDoubleVector_jni_1toRecord__J
  (JNIEnv * env, jobject, jlong cptr)
{
  Madara::Knowledge_Record * result (0);

  Native_Double_Vector * current =
    (Native_Double_Vector *) cptr;
  if (current)
    result = new Madara::Knowledge_Record (current->to_record ());

  return (jlong) result;
}

/*
 * Class:     com_madara_containers_NativeDoubleVector
 * Method:    jni_toArray
 * Signature: (J)[Ljava/lang/Object;
 */
MADARA_Export jobjectArray JNICALL Java_com_madara_containers_NativeDoubleVector_jni_1toArray
  (JNIEnv * env, jobject, jlong cptr)
{
  jclass kr_class = env->FindClass ("com/madara/KnowledgeRecord");
  jobjectArray list;
  if (kr_class && cptr != 0)
  {
    jmethodID method = env->GetStaticMethodID (kr_class,
      "fromPointer", " (J)Lcom/madara/KnowledgeRecord;");
    Madara::Knowledge_Vector records;
    Native_Double_Vector * current =
      (Native_Double_Vector *) cptr;
    current->copy_to (records);
    jsize size = (jsize)records.size ();

    list = env->NewObjectArray ( (jsize)records.size (), kr_class, 0);

    if (method)
    {
      for (jsize i = 0; i < size; ++i)
      {
        std::cout << "record[" << i << "] = " << records[i] << "\n";
        jobject result = env->CallStaticObjectMethod (
          kr_class, method, (jlong)records[i].clone ());
        env->SetObjectArrayElement (list, i, result);
      }
    }
  }
  return list;
}

/*
 * Class:     com_madara_containers_NativeDoubleVector
 * Method:    jni_size
 * Signature: (J)J
 */
MADARA_Export jlong JNICALL Java_com_madara_containers_NativeDoubleVector_jni_1size
  (JNIEnv * env, jobject, jlong cptr)
{
  jlong result (0);

  Native_Double_Vector * current =
    (Native_Double_Vector *) cptr;
  if (current)
    result = (jlong) current->size ();

  return (jlong) result;
}

/*
 * Class:     com_madara_containers_NativeDoubleVector
 * Method:    jni_resize
 * Signature: (JJ)V
 */
MADARA_Export void JNICALL Java_com_madara_containers_NativeDoubleVector_jni_1resize
  (JNIEnv * env, jobject, jlong cptr, jlong length)
{
  Native_Double_Vector * current =
    (Native_Double_Vector *) cptr;

  if (current)
    current->resize (length);
}

/*
 * Class:     com_madara_containers_DoubleVector
 * Method:    modify
 * Signature: (J)V
 */
void JNICALL Java_com_madara_containers_NativeDoubleVector_jni_1modify
  (JNIEnv *, jobject, jlong cptr)
{
  Native_Double_Vector * current =
    (Native_Double_Vector *) cptr;

  if (current)
    current->modify ();
}
