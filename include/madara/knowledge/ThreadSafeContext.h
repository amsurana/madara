#ifndef MADARA_THREADSAFECONTEXT_H
#define MADARA_THREADSAFECONTEXT_H

/**
 * @file ThreadSafeContext.h
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains a thread safe context for state information
 */

#include "ace/Guard_T.h"
#include "ace/Recursive_Thread_Mutex.h"
#include "ace/Condition_Recursive_Thread_Mutex.h"
#include "ace/Synch.h"
#include "madara/logger/Logger.h"

#ifndef ACE_LACKS_PRAGMA_ONCE
# pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */

#include <string>
#include <map>
#include "madara/utility/inttypes.h"

#include "madara/MADARA_export.h"
#include "madara/LockType.h"
#include "madara/knowledge/KnowledgeRecord.h"
#include "madara/knowledge/VariableReference.h"
#include "madara/knowledge/FunctionMap.h"
#include "madara/knowledge/KnowledgeUpdateSettings.h"
#include "madara/knowledge/KnowledgeReferenceSettings.h"
#include "madara/knowledge/CompiledExpression.h"

#ifdef _MADARA_JAVA_
#include "madara_jni.h"
#endif

#ifdef _MADARA_PYTHON_CALLBACKS_
#include "boost/python/object.hpp"
#endif

namespace madara
{
  namespace expression
  {
    class Interpreter;
  }

  namespace knowledge
  { 
    /**
      * Typedef for set of copyable keys. @see copy. We use map instead
      * of set so we are not wasting significant memory/time with copying
      * the key to the value (a STL set uses the type as both the key
      * and value).
      **/
    typedef std::map <std::string, bool> CopySet;

    /**
     * @class ThreadSafeContext
     * @brief This class stores variables and their values for use by any entity
     *        needing state information in a thread safe way
     */
    class MADARA_Export ThreadSafeContext
    {
    public:
      typedef ACE_Condition <MADARA_LOCK_TYPE> Condition;

      /**
       * Constructor.
       **/
      ThreadSafeContext ();

      /**
       * Destructor.
       **/
      ~ThreadSafeContext (void);

      /**
       * Atomically returns the value of a variable.
       * @param   key       unique identifier of the variable
       * @param   settings  the settings for referring to variables
       * @return         the madara::KnowledgeRecord::Integer value for the variable
       **/
      madara::KnowledgeRecord
        get (const std::string & key,
             const KnowledgeReferenceSettings & settings =
                     KnowledgeReferenceSettings ()) const;
      
      /**
       * Atomically returns the value of a variable.
       * @param   variable  reference to a variable (@see get_ref)
       * @param   settings  the settings for referring to variables
       * @return         the madara::KnowledgeRecord::Integer value for the variable
       **/
      madara::KnowledgeRecord
        get (const VariableReference & variable,
             const KnowledgeReferenceSettings & settings =
                     KnowledgeReferenceSettings ()) const;
      
      /**
       * Atomically returns a reference to the variable. Variable references are
       * efficient mechanisms for reference variables individually--similar to
       * speedups seen from CompiledExpression.
       * @param   key       unique identifier of the variable
       * @param settings         settings for referring to knowledge variables
       * @return            reference to the variable in the context
       **/
      VariableReference
        get_ref (const std::string & key,
             const KnowledgeReferenceSettings & settings =
                     KnowledgeReferenceSettings ());

      /**
      * Atomically returns a reference to the variable.
      * @param   key       unique identifier of the variable
      * @param settings    settings for referring to knowledge variables
      * @return            reference to the variable in the context. If variable
      *                    doesn't exist, then a null reference is returned.
      **/
      VariableReference
        get_ref (const std::string & key,
        const KnowledgeReferenceSettings & settings =
        KnowledgeReferenceSettings ()) const;

      /**
       * Retrieves a value at a specified index within a knowledge array
       * @param key              knowledge location
       * @param index            index within the array
       * @param settings         settings for referring to knowledge variables
       * @return                 value at knowledge location
       **/
      madara::KnowledgeRecord retrieve_index (const std::string & key,
             size_t index,
             const KnowledgeReferenceSettings & settings =
                     KnowledgeReferenceSettings ());
      
      /**
       * Retrieves a value at a specified index within a knowledge array
       * @param   variable   reference to a variable (@see get_ref)
       * @param   index      index within the array
       * @param   settings   settings for referring to knowledge variables
       * @return             value at knowledge location
       **/
      madara::KnowledgeRecord retrieve_index (
             const VariableReference & variable,
             size_t index,
             const KnowledgeReferenceSettings & settings =
                     KnowledgeReferenceSettings ());

      /**
       * Retrieves a knowledge record from the key. This function is useful
       * for performance reasons and also for using a KnowledgeRecord that
       * can be one of multiple types
       * @param   key    unique identifier of the variable. Allows variable 
       *                 expansion.
       * @param  settings  the settings for referring to variables
       * @return         the knowledge record for the variable
       **/
      madara::KnowledgeRecord * get_record (const std::string & key,
             const KnowledgeReferenceSettings & settings =
                     KnowledgeReferenceSettings ());
      
      /**
       * Atomically sets the value of a variable to an XML string.
       * @param   key       unique identifier of the variable
       * @param   value     new value of the variable
       * @param   size      indicates the size of the value buffer
       * @param   settings  settings for applying the update
       * @return   0 if the value was set. -1 if null key
       **/
      int set_xml (const std::string & key,
        const char * value, size_t size, 
        const KnowledgeUpdateSettings & settings = 
              KnowledgeUpdateSettings ());
      
      /**
       * Atomically sets the value of a variable to an XML string.
       * @param   variable  reference to a variable (@see get_ref)
       * @param   value     new value of the variable
       * @param   size      indicates the size of the value buffer
       * @param   settings  settings for applying the update
       * @return   0 if the value was set. -1 if null key
       **/
      int set_xml (const VariableReference & variable,
        const char * value, size_t size, 
        const KnowledgeUpdateSettings & settings = 
              KnowledgeUpdateSettings ());
      
      /**
       * Atomically sets the value of a variable to a JPEG image
       * @param   key       unique identifier of the variable
       * @param   value     new value of the variable
       * @param   size      indicates the size of the value buffer
       * @param   settings  settings for applying the update
       * @return   0 if the value was set. -1 if null key
       **/
      int set_jpeg (const std::string & key,
        const unsigned char * value, size_t size,
        const KnowledgeUpdateSettings & settings = 
              KnowledgeUpdateSettings ());
      
      /**
       * Atomically sets the value of a variable to a JPEG image
       * @param   variable  reference to a variable (@see get_ref)
       * @param   value     new value of the variable
       * @param   size      indicates the size of the value buffer
       * @param   settings  settings for applying the update
       * @return   0 if the value was set. -1 if null key
       **/
      int set_jpeg (const VariableReference & variable,
        const unsigned char * value, size_t size,
        const KnowledgeUpdateSettings & settings = 
              KnowledgeUpdateSettings ());
      
      /**
       * Atomically sets the value of a variable to an arbitrary string.
       * @param   key       unique identifier of the variable
       * @param   value     new value of the variable
       * @param   size      indicates the size of the value buffer
       * @param   settings  settings for applying the update
       * @return   0 if the value was set. -1 if null key
       **/
      int set_file (const std::string & key,
        const unsigned char * value, size_t size, 
        const KnowledgeUpdateSettings & settings = 
              KnowledgeUpdateSettings ());
      
      /**
       * Atomically sets the value of a variable to an arbitrary string.
       * @param   variable  reference to a variable (@see get_ref)
       * @param   value     new value of the variable
       * @param   size      indicates the size of the value buffer
       * @param   settings  settings for applying the update
       * @return   0 if the value was set. -1 if null key
       **/
      int set_file (const VariableReference & variable,
        const unsigned char * value, size_t size, 
        const KnowledgeUpdateSettings & settings = 
              KnowledgeUpdateSettings ());
      
      /**
       * Atomically sets the value of a variable to an XML string.
       * @param   key       unique identifier of the variable
       * @param   value     new value of the variable
       * @param   size      indicates the size of the value buffer
       * @param   settings  settings for applying the update
       * @return   0 if the value was set. -1 if null key
       **/
      int set_text (const std::string & key,
        const char * value, size_t size, 
        const KnowledgeUpdateSettings & settings = 
              KnowledgeUpdateSettings ());
      
      /**
       * Atomically sets the value of a variable to a text file's contents.
       * @param   variable  reference to a variable (@see get_ref)
       * @param   value     new value of the variable
       * @param   size      indicates the size of the value buffer
       * @param   settings  settings for applying the update
       * @return   0 if the value was set. -1 if null key
       **/
      int set_text (const VariableReference & variable,
        const char * value, size_t size, 
        const KnowledgeUpdateSettings & settings = 
              KnowledgeUpdateSettings ());
      
      /**
       * Atomically sets the value of a variable to the specific record.
       * Note, this does not copy meta information (e.g. quality, clock).
       * @param   key       unique identifier of the variable
       * @param   value     new value of the variable
       * @param   settings  settings for applying the update
       * @return   0 if the value was set. -1 if null key
       **/
      int set (const std::string & key,
        const madara::KnowledgeRecord & value, 
        const KnowledgeUpdateSettings & settings = 
              KnowledgeUpdateSettings ());
      
      /**
       * Atomically sets the value of a variable to the specific record.
       * Note, this does not copy meta information (e.g. quality, clock).
       * @param   variable  reference to a variable (@see get_ref)
       * @param   value     new value of the variable
       * @param   settings  settings for applying the update
       * @return   0 if the value was set. -1 if null key
       **/
      int set (const VariableReference & variable,
        const madara::KnowledgeRecord & value, 
        const KnowledgeUpdateSettings & settings = 
              KnowledgeUpdateSettings ());
      
      /**
       * Atomically sets the value of a variable to an integer.
       * @param   key       unique identifier of the variable
       * @param   value     new value of the variable
       * @param   settings  settings for applying the update
       * @return   0 if the value was set. -1 if null key
       **/
      int set (const std::string & key,
        madara::KnowledgeRecord::Integer value, 
        const KnowledgeUpdateSettings & settings = 
              KnowledgeUpdateSettings ());
      
      /**
       * Atomically sets the value of a variable to an integer.
       * @param   variable  reference to a variable (@see get_ref)
       * @param   value     new value of the variable
       * @param   settings  settings for applying the update
       * @return   0 if the value was set. -1 if null key
       **/
      int set (const VariableReference & variable,
        madara::KnowledgeRecord::Integer value, 
        const KnowledgeUpdateSettings & settings = 
              KnowledgeUpdateSettings ());
      
      /**
       * Atomically sets the value of an array index to a double.
       * @param   key       unique identifier of the variable
       * @param   index     index within array
       * @param   value     new value of the array index
       * @param   settings  settings for applying the update
       * @return   0 if the value was set. -1 if null key
       **/
      int set_index (const std::string & key,
        size_t index, KnowledgeRecord::Integer value, 
        const KnowledgeUpdateSettings & settings = 
              KnowledgeUpdateSettings ());
      
      /**
       * Atomically sets the value of an array index to a double.
       * @param   variable  reference to a variable (@see get_ref)
       * @param   index     index within array
       * @param   value     new value of the array index
       * @param   settings  settings for applying the update
       * @return   0 if the value was set. -1 if null key
       **/
      int set_index (const VariableReference & variable,
        size_t index, KnowledgeRecord::Integer value, 
        const KnowledgeUpdateSettings & settings = 
              KnowledgeUpdateSettings ());
      
      /**
       * Atomically sets the value of a variable to an integer array.
       * @param   key       unique identifier of the variable
       * @param   value     an array of Integers
       * @param   size      number of elements in the array
       * @param   settings  settings for applying the update
       * @return   0 if the value was set. -1 if null key
       **/
      int set (const std::string & key,
        const madara::KnowledgeRecord::Integer * value,
        uint32_t size,
        const KnowledgeUpdateSettings & settings = 
              KnowledgeUpdateSettings ());
      
      /**
       * Atomically sets the value of a variable to an integer array.
       * @param   variable  reference to a variable (@see get_ref)
       * @param   value     an array of Integers
       * @param   size      number of elements in the array
       * @param   settings  settings for applying the update
       * @return   0 if the value was set. -1 if null key
       **/
      int set (const VariableReference & variable,
        const madara::KnowledgeRecord::Integer * value,
        uint32_t size,
        const KnowledgeUpdateSettings & settings = 
              KnowledgeUpdateSettings ());
      
      /**
       * Atomically sets the value of a variable to an integer array.
       * @param   key       unique identifier of the variable
       * @param   value     a STL vector of Integers
       * @param   settings  settings for applying the update
       * @return   0 if the value was set. -1 if null key
       **/
      int set (const std::string & key,
        const std::vector <KnowledgeRecord::Integer> & value,
        const KnowledgeUpdateSettings & settings = 
              KnowledgeUpdateSettings ());
      
      /**
       * Atomically sets the value of a variable to an integer array.
       * @param   variable  reference to a variable (@see get_ref)
       * @param   value     a STL vector of Integers
       * @param   settings  settings for applying the update
       * @return   0 if the value was set. -1 if null key
       **/
      int set (const VariableReference & variable,
        const std::vector <KnowledgeRecord::Integer> & value,
        const KnowledgeUpdateSettings & settings = 
              KnowledgeUpdateSettings ());
      
      /**
       * Atomically sets the value of a variable to a double.
       * @param   key       unique identifier of the variable
       * @param   value     new value of the variable
       * @param   settings  settings for applying the update
       * @return   0 if the value was set. -1 if null key
       **/
      int set (const std::string & key,
        double value, 
        const KnowledgeUpdateSettings & settings = 
              KnowledgeUpdateSettings ());
      
      /**
       * Atomically sets the value of a variable to a double.
       * @param   variable  reference to a variable (@see get_ref)
       * @param   value     new value of the variable
       * @param   settings  settings for applying the update
       * @return   0 if the value was set. -1 if null key
       **/
      int set (const VariableReference & variable,
        double value, 
        const KnowledgeUpdateSettings & settings = 
              KnowledgeUpdateSettings ());
      
      /**
       * Atomically sets the value of an array index to a double.
       * @param   key       unique identifier of the variable
       * @param   index     index within array
       * @param   value     new value of the array index
       * @param   settings  settings for applying the update
       * @return   0 if the value was set. -1 if null key
       **/
      int set_index (const std::string & key,
        size_t index, double value, 
        const KnowledgeUpdateSettings & settings = 
              KnowledgeUpdateSettings ());
      
      /**
       * Atomically sets the value of an array index to a double.
       * @param   variable  reference to a variable (@see get_ref)
       * @param   index     index within array
       * @param   value     new value of the array index
       * @param   settings  settings for applying the update
       * @return   0 if the value was set. -1 if null key
       **/
      int set_index (const VariableReference & variable,
        size_t index, double value, 
        const KnowledgeUpdateSettings & settings = 
              KnowledgeUpdateSettings ());
      
      /**
       * Atomically sets the value of a variable to a double array.
       * @param   key       unique identifier of the variable
       * @param   value     an array of doubles
       * @param   size      number of elements in the array
       * @param   settings  settings for applying the update
       * @return   0 if the value was set. -1 if null key
       **/
      int set (const std::string & key,
        const double * value,
        uint32_t size,
        const KnowledgeUpdateSettings & settings = 
              KnowledgeUpdateSettings ());
      
      /**
       * Atomically sets the value of a variable to a double array.
       * @param   variable  reference to a variable (@see get_ref)
       * @param   value     an array of doubles
       * @param   size      number of elements in the array
       * @param   settings  settings for applying the update
       * @return   0 if the value was set. -1 if null key
       **/
      int set (const VariableReference & variable,
        const double * value,
        uint32_t size,
        const KnowledgeUpdateSettings & settings = 
              KnowledgeUpdateSettings ());
      
      /**
       * Atomically sets the value of a variable to a double array.
       * @param   key       unique identifier of the variable
       * @param   value     a STL vector of doubles
       * @param   settings  settings for applying the update
       * @return   0 if the value was set. -1 if null key
       **/
      int set (const std::string & key,
        const std::vector <double> & value,
        const KnowledgeUpdateSettings & settings = 
              KnowledgeUpdateSettings ());
      
      /**
       * Atomically sets the value of a variable to a double array.
       * @param   variable  reference to a variable (@see get_ref)
       * @param   value     a STL vector of doubles
       * @param   settings  settings for applying the update
       * @return   0 if the value was set. -1 if null key
       **/
      int set (const VariableReference & variable,
        const std::vector <double> & value,
        const KnowledgeUpdateSettings & settings = 
              KnowledgeUpdateSettings ());
      
      /**
       * Atomically sets the value of a variable to a string
       * @param   key       unique identifier of the variable
       * @param   value     new value of the variable
       * @param   settings  settings for applying the update
       * @return   0 if the value was set. -1 if null key
       **/
      int set (const std::string & key,
        const std::string & value, 
        const KnowledgeUpdateSettings & settings = 
              KnowledgeUpdateSettings ());
      
      /**
       * Atomically sets the value of a variable to a string
       * @param   variable  reference to a variable (@see get_ref)
       * @param   value     new value of the variable
       * @param   settings  settings for applying the update
       * @return   0 if the value was set. -1 if null key
       **/
      int set (const VariableReference & variable,
        const std::string & value, 
        const KnowledgeUpdateSettings & settings = 
              KnowledgeUpdateSettings ());
      
      /**
       * Atomically reads a file into a variable
       * @param   key       unique identifier of the variable
       * @param   filename  file to read
       * @param   settings  settings for applying the update
       * @return   0 if the value was set. -1 if unsuccessful
       **/
      int
      read_file (
        const std::string & key,
        const std::string & filename,
        const KnowledgeUpdateSettings & settings = 
              KnowledgeUpdateSettings ());
      
      /**
       * Atomically reads a file into a variable
       * @param   variable  reference to a variable (@see get_ref)
       * @param   filename  file to read
       * @param   settings  settings for applying the update
       * @return   0 if the value was set. -1 if unsuccessful
       **/
      int
      read_file (
        const VariableReference & variable,
        const std::string & filename,
        const KnowledgeUpdateSettings & settings = 
              KnowledgeUpdateSettings ());

      /**
       * Atomically sets if the variable value will be different
       * @param   key       unique identifier of the variable
       * @param   value     new value of the variable
       * @param   quality   quality to set the variable at (type of priority)
       * @param   clock     clock value of the update
       * @param   settings  settings for applying the update
       * @return   1 if the value was changed. 0 if not changed.
       *          -1 if null key
       **/
      int set_if_unequal (const std::string & key,
        madara::KnowledgeRecord::Integer value, 
        uint32_t quality, uint64_t clock, 
        const KnowledgeUpdateSettings & settings = 
              KnowledgeUpdateSettings ());
      
      /**
       * Atomically sets if the variable value will be different
       * @param   key       unique identifier of the variable
       * @param   value     new value of the variable
       * @param   quality   quality to set the variable at (type of priority)
       * @param   clock     clock value of the update
       * @param   settings  settings for applying the update
       * @return   1 if the value was changed. 0 if not changed.
       *          -1 if null key
       **/
      int set_if_unequal (const std::string & key,
        double value, 
        uint32_t quality, uint64_t clock, 
        const KnowledgeUpdateSettings & settings = 
              KnowledgeUpdateSettings ());
      
      /**
       * Atomically sets if the variable value will be different
       * @param   key       unique identifier of the variable
       * @param   value     new value of the variable
       * @param   quality   quality to set the variable at (type of priority)
       * @param   clock     clock value of the update
       * @param   settings  settings for applying the update
       * @return   1 if the value was changed. 0 if not changed.
       *          -1 if null key
       **/
      int set_if_unequal (const std::string & key,
        const std::string & value, 
        uint32_t quality, uint64_t clock, 
        const KnowledgeUpdateSettings & settings = 
              KnowledgeUpdateSettings ());
      
      /**
       * Atomically sets if the variable value meets update conditions.
       * Appropriate conditions include clock being >= old clock, quality
       * >= old quality, etc.
       * @param   key       unique identifier of the variable
       * @param   rhs     new value of the variable
       * @param   settings  settings for applying the update
       * @return   1 if the value was changed. 0 if not changed.
       *          -1 if null key
       **/
      int update_record_from_external (
        const std::string & key, const KnowledgeRecord & rhs,
        const KnowledgeUpdateSettings & settings = 
              KnowledgeUpdateSettings ());
      
      /**
       * Atomically gets quality of a variable
       * @param   key       unique identifier of the 
       * @param   settings       settings for referring to a knowledge variable
       * @return   quality associated with the variable
       **/
      uint32_t get_quality (const std::string & key,
             const KnowledgeReferenceSettings & settings =
                     KnowledgeReferenceSettings ());
      
      /**
       * Atomically gets write quality of this process for a variable
       * @param   key       unique identifier of the variable
       * @param   settings       settings for referring to a knowledge variable
       * @return   write quality associated with the variable
       **/
      uint32_t get_write_quality (const std::string & key,
             const KnowledgeReferenceSettings & settings =
                     KnowledgeReferenceSettings ());
      
      /**
       * Atomically sets quality of this process for a variable
       * @param   key            unique identifier of the variable
       * @param   quality        quality of this process
       * @param   force_update   force an update to variable, even if lower
       * @param   settings       settings for referring to a knowledge variable
       * @return   write quality associated with the variable
       **/
      uint32_t set_quality (const std::string & key, 
        uint32_t quality, bool force_update,
        const KnowledgeReferenceSettings & settings);

       /**
       * Force a change to be registered, waking up anyone waiting on entry
       **/
      void set_changed (void);

      /**
       * Atomically sets write quality of this process for a variable
       * @param   key            unique identifier of the variable
       * @param   quality        write quality of this process
       * @param   settings       settings for referring to a knowledge variable
       **/
      void set_write_quality (const std::string & key, uint32_t quality,
        const KnowledgeReferenceSettings & settings);

      /**
       * Retrieves a list of modified variables. Useful for building a
       * disseminatable knowledge update.
       * @return  the modified knowledge records
       **/
      const KnowledgeRecords & get_modifieds (void) const;

      /**
      * Retrieves a stringified list of all modified variables that are ready
      * to send over transport on next send_modifieds call
      * @return  stringified list of the modified knowledge records
      **/
      std::string debug_modifieds (void) const;

      /**
       * Retrieves a list of modified local variables. Useful for building a
       * comprehensive checkpoint.
       * @return  the modified knowledge records
       **/
      const KnowledgeRecords & get_local_modified (void) const;

      /**
       * Reset all variables to be unmodified. This will clear all global
       * knowledge updates. Use with caution.
       **/
      void reset_modified (void);

      /**
       * Changes all global variables to modified at current clock.
       **/
      void apply_modified (void);
      
      /**
       * Marks the variable reference as updated
       * @param   variable  reference to a variable (@see get_ref)
       **/
      void mark_modified (const VariableReference & variable);
      
      /**
       * Changes global variables to modified at current clock.
       * @param  key     the key of the record you are changing
       * @param  record  record of the key in the context (should exist)
       * @param  settings  the settings for referring to variables
       **/
      void mark_modified (const std::string & key,
        madara::KnowledgeRecord & record,
        const KnowledgeReferenceSettings & settings =
          KnowledgeReferenceSettings (false));
      
      /**
       * Changes local variables to modified at current clock.
       * @param  key     the key of the record you are changing
       * @param  record  record of the key in the context (should exist)
       * @param  settings  the settings for referring to variables
       **/
      void mark_local_modified (const std::string & key,
        madara::KnowledgeRecord & record,
        const KnowledgeReferenceSettings & settings =
          KnowledgeReferenceSettings (false));

      /**
       * Resets a variable to unmodified
       * @param   key            unique identifier of the variable
       **/
      void reset_modified (const std::string & key);

      /**
       * Atomically increments the value of the variable
       * @param   key            unique identifier of the variable
       * @param   settings  settings for applying the update
       * @return                 new value of variable
       **/
      madara::KnowledgeRecord inc (const std::string & key, 
        const KnowledgeUpdateSettings & settings = 
              KnowledgeUpdateSettings ());
      
      /**
       * Atomically increments the value of the variable
       * @param   variable  reference to a variable (@see get_ref)
       * @param   settings  settings for applying the update
       * @return                 new value of variable
       **/
      madara::KnowledgeRecord inc (const VariableReference & variable, 
        const KnowledgeUpdateSettings & settings = 
              KnowledgeUpdateSettings ());

      /**
       * Wait for a change to happen to the context.
       * @param   extra_release  perform extra release of lock for nested locks
       **/
      void wait_for_change (bool extra_release = false);

      /**
       * Atomically decrements the value of the variable
       * @param   key            unique identifier of the variable
       * @param   settings  settings for applying the update
       * @return                 new value of variable
       **/
      madara::KnowledgeRecord dec (const std::string & key, 
        const KnowledgeUpdateSettings & settings = 
              KnowledgeUpdateSettings ());
      
      /**
       * Atomically decrements the value of the variable
       * @param   variable  reference to a variable (@see get_ref)
       * @param   settings  settings for applying the update
       * @return                 new value of variable
       **/
      madara::KnowledgeRecord dec (const VariableReference & variable, 
        const KnowledgeUpdateSettings & settings = 
              KnowledgeUpdateSettings ());
      
      /**
       * Deletes the key
       * @param   key            unique identifier of the variable
       * @param   settings       settings for referring to variables
       * @return                 true if variable exists
       **/
      bool delete_variable (const std::string & key,
             const KnowledgeReferenceSettings & settings =
                     KnowledgeReferenceSettings ());
      
      /**
       * Deletes the expression from the interpreter cache
       * @param   expression     the KaRL logic in the interpreter context
       * @return                 true if variable exists
       **/
      bool delete_expression (const std::string & expression);

      /**
       * Atomically checks to see if a variable already exists
       * @param   key            unique identifier of the variable
       * @param   settings       settings for referring to variables
       * @return                 true if variable exists
       **/
      bool exists (const std::string & key,
             const KnowledgeReferenceSettings & settings =
                     KnowledgeReferenceSettings ()) const;
      
      /**
       * Checks if a knowledge variable exists in the context
       *
       * @param variable        knowledge variable reference
       * @param settings        settings for referring to knowledge variables
       * @return                true if location has been set
       **/
      bool exists (const VariableReference & variable,
        const KnowledgeReferenceSettings & settings =
          KnowledgeReferenceSettings (false)) const;

      /**
       * Atomically prints all variables and values in the context
       * @param   level          log level. @see LogMacros.h
       **/
      void print (unsigned int level) const;

      /**
       * Atomically Print a statement, similar to printf (variable expansions
       * allowed) e.g., input = "MyVar{.id} = {MyVar{.id}}\n";
       * @param   statement      templated statement to print from
       * @param   level          log level. @see LogMacros.h
       **/
      void print (const std::string & statement, unsigned int level) const;

      /**
      * Clears the context
      * @param  erase   If true, completely erase keys.
      *                 If false, reset knowledge
      **/
      void clear (bool erase = false);

      /**
       * Locks the mutex on this context. Warning: this will cause
       * all operations to block until the unlock call is made.
       **/
      void lock (void) const;

      /**
       * Unlocks the mutex on this context.
       **/
      void unlock (void) const;
      
      /**
       * Atomically increments the Lamport clock and returns the new
       * clock time (intended for sending knowledge updates).
       * @param   settings  settings for applying the update
       * @return        new clock time
       **/
      uint64_t inc_clock (const KnowledgeUpdateSettings & settings = 
              KnowledgeUpdateSettings ());

      /**
       * Atomically increments the Lamport clock of a variable and returns the
       * new clock time (intended for sending knowledge updates).
       * @param   key   unique identifier of the variable
       * @param   settings  settings for applying the update
       * @return        new clock time for variable
       **/
      uint64_t inc_clock (const std::string & key,
        const KnowledgeUpdateSettings & settings = 
              KnowledgeUpdateSettings ());

      /**
       * Atomically sets the lamport clock.
       * @param  clock  new global clock
       * @return        new clock time
       **/
      uint64_t set_clock (uint64_t clock);

      /**
       * Atomically sets the Lamport clock of a variable and returns the
       * new clock time (intended for sending knowledge updates).
       * @param   key       unique identifier of the variable
       * @param   clock     new variable clock
       * @param   settings  settings for applying the update
       * @return           new clock time for variable
       **/
      uint64_t set_clock (const std::string & key, 
        uint64_t clock,
             const KnowledgeReferenceSettings & settings =
                     KnowledgeReferenceSettings ());

      /**
       * Atomically gets the Lamport clock
       * @return           current global clock
       **/
      uint64_t get_clock (void) const;

      /**
       * Atomically gets the Lamport clock of a variable
       * @param   key       unique identifier of the variable
       * @param   settings  settings for reading the variable
       * @return           current variable clock
       **/
      uint64_t get_clock (
        const std::string & key,
             const KnowledgeReferenceSettings & settings =
                     KnowledgeReferenceSettings ()) const;

      /**
       * Signals that this thread is done with the context. Anyone
       * waiting on the underlying condition is awoken.
       **/
      void signal (bool lock = true) const;

      /**
       * Expands a string with variable expansion. For instance, if
       * .id == 5, and a statement of "MyVar{.id} = {.id} * 30" then
       * then expanded statement would be "MyVar5 = 5 * 30".
       * @param  statement  statement to expand. Useful for printing.
       * @return            variable expanded statement
       **/
      std::string expand_statement (const std::string & statement) const;
      
      /**
       * Copies variables and values from source to this context. PERFORMANCE
       * NOTES: worst case depends on size of copy_set. If empty, performance
       * is always O (n), where n is number of variables in the source context.
       * If copy_set is not empty, then performance is O (m log n) where m is
       * the number of variables in copy_set and n is number of variables in
       * the source context.
       * <br>&nbsp;<br>
       * Note that this is a deep copy due to the fact
       * that source and destination are expected to have their own thread
       * management (ref-counted variables can be problematic if shallow copy).
       *
       * @param  source    the source context to copy from
       * @param  copy_set  a map of variables that should be copied. If
       *                   empty, then everything is copied. If not empty,
       *                   only the supplied variables will be copied.
       * @param  clean_copy  if true, clear the destination context (this)
       *                     before copying.
       **/
      void copy (const ThreadSafeContext & source,
        const CopySet & copy_set = CopySet (),
        bool clean_copy = false);


#ifndef _MADARA_NO_KARL_

      /**
       * Compiles a KaRL expression into an expression tree
       *
       * @param expression         expression to compile
       * @return                   compiled, optimized expression tree
       **/
      CompiledExpression
        compile (const std::string & expression);
      
      /**
       * Defines an external function
       * @param  name       name of the function
       * @param  func       external function to call with this name
       * @param  settings   settings for referring to variables
       **/
      void define_function (const std::string & name,
        KnowledgeRecord (*func) (FunctionArguments &, Variables &),
             const KnowledgeReferenceSettings & settings =
                     KnowledgeReferenceSettings ());
      
      /**
       * Defines a named function that can distinguish the name it was called
       * with in MADARA
       * @param  name       name of the function
       * @param  func       external function to call with this name
       * @param  settings   settings for referring to variables
       **/
      void define_function (const std::string & name,
        KnowledgeRecord (*func) (const char *, FunctionArguments &, Variables &),
        const KnowledgeReferenceSettings & settings =
                     KnowledgeReferenceSettings ());

#ifdef _MADARA_JAVA_
      /**
       * Defines a named java function
       * @param  name       name of the function
       * @param  callable   external java object to call with this name
       * @param  settings   settings for referring to variables
       **/
      void define_function (const std::string & name, jobject callable,
                            const KnowledgeReferenceSettings & settings =
                            KnowledgeReferenceSettings ());
#endif
      
#ifdef _MADARA_PYTHON_CALLBACKS_
      /**
       * Defines a named python function
       * @param  name       name of the function
       * @param  callable   external python function to call with this name
       * @param  settings   settings for referring to variables
       **/
      void define_function (const std::string & name, boost::python::object callable,
                            const KnowledgeReferenceSettings & settings =
                     KnowledgeReferenceSettings ());
#endif

      /**
       * Defines a MADARA KaRL function
       * @param  name       name of the function
       * @param  expression KaRL function body      
       * @param  settings   settings for referring to variables 
       **/
      void define_function (const std::string & name,
        const std::string & expression,
        const KnowledgeReferenceSettings & settings =
                     KnowledgeReferenceSettings ());
      
      /**
       * Defines a MADARA KaRL function
       * @param  name       name of the function
       * @param  expression KaRL function body     
       * @param  settings   settings for referring to variables  
       **/
      void define_function (const std::string & name,
        const CompiledExpression & expression,
        const KnowledgeReferenceSettings & settings =
                     KnowledgeReferenceSettings ());

      /**
       * Retrieves an external function
       * @param  name       name of the function to retrieve
       * @param  settings   settings for referring to variables
       * @return            the mapped external function
       **/
      Function * retrieve_function (const std::string & name,
             const KnowledgeReferenceSettings & settings =
                     KnowledgeReferenceSettings ());
      
      /**
       * Evaluate a compiled expression. Please note that if you update
       * any variables here, they will not be sent through any transports
       * until you call through the KnowledgeBase.
       * @param   expression  A compiled expressio to run.
       * @param   settings    settings for applying the update
       * @return              result of the evaluation
       **/
      KnowledgeRecord evaluate (CompiledExpression expression,
        const KnowledgeUpdateSettings & settings = 
              KnowledgeUpdateSettings ());
      
      /**
       * Evaluate a component node-rooted tree. Please note that if you update
       * any variables here, they will not be sent through any transports
       * until you call through the KnowledgeBase.
       * @param   root        Root of an expression tree
       * @param   settings    settings for applying the update
       * @return              result of the evaluation
       **/
      KnowledgeRecord evaluate (
        expression::ComponentNode * root,
        const KnowledgeUpdateSettings & settings = 
                    KnowledgeUpdateSettings ());

#endif // _MADARA_NO_KARL_

      /**
       * Saves all keys and values into a string, using the underlying
       * KnowledgeRecord::to_string function. This is an optimized
       * version that allows the specification of a target string to
       * avoid string copying (which can be expensive with longer
       * strings across multiple function boundaries). This function differs
       * from KnowledgeRecord to_string in that it is intended to save the
       * database in a format that can be easily parseable. Consequently,
       * strings are delineated in this function by being included in single
       * quotes. Arrays are delineated with array indices [].
       * 
       * This is not appropriate
       * for saving the context if it has binary data inside of it, as
       * only the size of the data entry would be saved.
       * @param target  where to store the resulting string operation.
       * @param array_delimiter  for any arrays, the characters in between
       *                         data elements.
       * @param record_delimiter the characters to place between individual
       *                         key/value pairs
       * @param key_val_delimiter the characters to place between keys and
       *                          values.
       **/
      void to_string (std::string & target,
        const std::string & array_delimiter = ",",
        const std::string & record_delimiter = ";\n",
        const std::string & key_val_delimiter = "=") const;


      /**
       * Fills a vector with Knowledge Records that begin with a common subject
       * and have a finite range of integer values.
       * @param   subject     The common subject of the variable names. For
       *                      instance, if we are looking for a range of vars
       *                      like "var0", "var1", "var2", then the common
       *                      subject would be "var".
       * @param   start       An inclusive start index
       * @param   end         An inclusive end index
       * @param   target      The vector that will be filled with
       *                      Knowledge Record instances within the subject
       *                      range.
       * @return              entries in the resulting vector
       **/
      size_t  to_vector (const std::string & subject,
                              unsigned int start,
                              unsigned int end,
                              std::vector <KnowledgeRecord> & target);
      
      /**
       * Fills a variable map with Knowledge Records that match an expression.
       * At the moment, this expression must be of the form "subject*"
       * @param   subject     An expression that matches the variable names
       *                      that are of interest. Wildcards may only be
       *                      at the end.
       * @param   target      The map that will be filled with variable names
       *                      and the Knowledge Records that correspond to
       * @return              entries in the resulting map
       **/
      size_t  to_map    (const std::string & subject,
                       std::map <std::string, KnowledgeRecord> & target);

      /**
       * Adds a file to the logger
       * @param  filename  the file to add to logger
       **/
      void add_logger (const std::string & filename);

      /**
      * Gets the log level
      * @return the maximum detail level to print
      **/
      int get_log_level (void);

      /**
      * Sets the log level 
      * @param  level  the maximum detail level to print
      **/
      void set_log_level (int level);

      /**
       * Gets the logger used for information printing
       * @return the context's logger
       **/
      logger::Logger & get_logger (void) const;

      /**
      * Attaches a logger to be used for printing
      * @param logger  the logger the context should lose
      **/
     void attach_logger (logger::Logger & logger) const;

      /**
      * Fills a variable map with list of keys according to a matching prefix,
      * suffix, and delimiter hierarchy. This is useful for understanding the
      * logical hierarchy of your variables (and also a key utility of
      * containers like @see containers::FlexMap).
      * @param   prefix      Text that must be present at the front of the key
      * @param   delimiter   Text that signifies a logical boundary in hierarchy If
      *                      empty, no check is performed.
      * @param   suffix      Text that must be present at the end of the key. If
      *                      empty, no check is performed.
      * @param   next_keys   The immediate keys in the hierarchy after prefix
      * @param   result      The map that will be filled with full variable names
      *                      and the Knowledge Records that correspond to
      * @param   just_keys   if true, do not fill result, only next_keys
      * @return              entries in the resulting map
      **/
      size_t  to_map (const std::string & prefix,
        const std::string & delimiter,
        const std::string & suffix,
        std::vector <std::string> & next_keys,
        std::map <std::string, KnowledgeRecord> & result,
        bool just_keys);

      /**
       * Saves the context to a file
       * @param   filename    name of the file to open
       * @param   id          unique identifier of the context holder
       * @return              -1 if file open failed<br />
       *                      -2 if file write failed<br />
       *                      >0 if successful (number of bytes written)
       **/
      int64_t save_context (const std::string & filename,
        const std::string & id = "") const;

      /**
      * Saves the context to a file as karl assignments, rather than binary
      * @param   filename    name of the file to save to
      * @return  total bytes written
      **/
      int64_t save_as_karl (const std::string & filename) const;

      /**
       * Loads the context from a file
       * @param   filename    name of the file to open
       * @param   id          unique identifier of the context holder (will
       *                      be changed by the function)
       * @param   settings    settings for applying the update
       * @return              -1 if file open failed<br />
       *                      -2 if file read failed<br />
       *                      >0 if successful (number of bytes written)
       **/
      int64_t load_context (const std::string & filename,
        std::string & id,
        const KnowledgeUpdateSettings & settings = 
              KnowledgeUpdateSettings (true, true, true, false));

      /**
       * Saves a checkpoint of a list of changes to a file
       * @param   filename    name of the file to open
       * @param   id          unique identifier of the context holder
       **/

      int64_t save_checkpoint (const std::string & filename,
        const std::string & id = "") const;
      
    private:
      /**
       * method for marking a record modified and signaling changes
       * @param   name     variable name
       * @param   record   record to place in the changed_map
       * @param   settings settings for applying modification and signalling
       **/
      void mark_and_signal (const char * name, KnowledgeRecord * record,
                            const KnowledgeUpdateSettings & settings);

      typedef ACE_Guard<MADARA_LOCK_TYPE> ContextGuard;

      /// Hash table containing variable names and values.
      madara::KnowledgeMap map_;
      mutable MADARA_LOCK_TYPE mutex_;
      mutable Condition changed_;
      std::vector< std::string> expansion_splitters_;
      mutable uint64_t clock_;
      KnowledgeRecords changed_map_;
      KnowledgeRecords local_changed_map_;

      /// map of function names to functions
      FunctionMap functions_;
      
      /// KaRL interpreter
      madara::expression::Interpreter  *   interpreter_;

      /// Logger for printing
      mutable logger::Logger * logger_;
    };
  }
}

#include "ThreadSafeContext.inl"

#endif