#include "Double_Vector.h"
#include "madara/knowledge_engine/Context_Guard.h"


Madara::Knowledge_Engine::Containers::Double_Vector::Double_Vector (
  const Knowledge_Update_Settings & settings,
  const std::string & delimiter)
  : Base_Container ("", settings), context_ (0), delimiter_ (delimiter)
{
}
   
Madara::Knowledge_Engine::Containers::Double_Vector::Double_Vector (
  const std::string & name,
  Knowledge_Base & knowledge,
  int size,
  bool delete_vars,
  const Knowledge_Update_Settings & settings,
  const std::string & delimiter)
  : Base_Container (name, settings), context_ (&(knowledge.get_context ())),
  delimiter_ (delimiter)
{
  size_ = get_size_ref ();
  resize (size, delete_vars);
}
  
Madara::Knowledge_Engine::Containers::Double_Vector::Double_Vector (
  const std::string & name,
  Variables & knowledge,
  int size,
  bool delete_vars,
  const Knowledge_Update_Settings & settings,
  const std::string & delimiter)
  : Base_Container (name, settings), context_ (knowledge.get_context ()),
  delimiter_ (delimiter)
{
  size_ = get_size_ref ();
  resize (size, delete_vars);
}

Madara::Knowledge_Engine::Containers::Double_Vector::Double_Vector (
  const Double_Vector & rhs)
: Base_Container (rhs), context_ (rhs.context_),
  vector_ (rhs.vector_),
  delimiter_ (rhs.delimiter_),
  size_ (rhs.size_)
{
}


Madara::Knowledge_Engine::Containers::Double_Vector::~Double_Vector ()
{

}
  
void
Madara::Knowledge_Engine::Containers::Double_Vector::modify (void)
{
  if (context_ && name_ != "")
  {
    Context_Guard context_guard (*context_);
    for (size_t index = 0; index < vector_.size (); ++index)
      context_->mark_modified (vector_[index]);

    context_->mark_modified (size_);
  }
}

std::string
Madara::Knowledge_Engine::Containers::Double_Vector::get_debug_info (void)
{
  std::stringstream result;

  result << "Double Vector: ";

  if (context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);
    size_t elements = vector_.size ();

    result << this->name_;
    result << " [" << elements << "]";
    result << " = [";

    if (elements > 0)
    {
      result << context_->get (vector_[0]).to_string ();

      for (size_t index = 1; index < elements; ++index)
      {
        result << ", " << context_->get (vector_[index]).to_string ();
      }
    }

    result << "]";
  }

  return result.str ();
}

void
Madara::Knowledge_Engine::Containers::Double_Vector::modify_ (void)
{
  modify ();
}

std::string
Madara::Knowledge_Engine::Containers::Double_Vector::get_debug_info_ (void)
{
  return get_debug_info ();
}

Madara::Knowledge_Engine::Containers::Base_Container *
Madara::Knowledge_Engine::Containers::Double_Vector::clone (void) const
{
  return new Double_Vector (*this);
}

void
Madara::Knowledge_Engine::Containers::Double_Vector::modify (size_t index)
{
  if (context_ && name_ != "" && index < vector_.size ())
  {
    Context_Guard context_guard (*context_);
    context_->mark_modified (vector_[index]);
  }
}

void
Madara::Knowledge_Engine::Containers::Double_Vector::operator= (
  const Double_Vector & rhs)
{
  if (this != &rhs)
  {
    Guard guard (mutex_), guard2 (rhs.mutex_);

    this->context_ = rhs.context_;
    this->name_ = rhs.name_;
    this->settings_ = rhs.settings_;
    this->size_ = rhs.size_;
    this->vector_ = rhs.vector_;
    this->delimiter_ = rhs.delimiter_;
  }
}

void
Madara::Knowledge_Engine::Containers::Double_Vector::push_back (type value)
{
  if (context_ && name_ != "")
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    if (!size_.is_valid ())
    {
      size_ = get_size_ref ();
    }

    size_t i = size ();
    resize ((int)i + 1);
    set (i, value);
  }
}

Madara::Knowledge_Engine::Variable_Reference
Madara::Knowledge_Engine::Containers::Double_Vector::get_size_ref (void)
{
  Variable_Reference ref;
  
  if (context_ && name_ != "")
  {
    Knowledge_Update_Settings keep_local (true);
    std::stringstream buffer;

    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    buffer << name_;
    buffer << delimiter_;
    buffer << "size";

    ref = context_->get_ref (buffer.str (), keep_local);
  }

  return ref;
}

void
Madara::Knowledge_Engine::Containers::Double_Vector::resize (
  int size, bool delete_vars)
{
  if (context_ && name_ != "")
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    if (!size_.is_valid ())
    {
      size_ = get_size_ref ();
    }

    if (size >= 0)
    {
      size_t old_size = vector_.size ();

      if (old_size != size)
      {
        vector_.resize (size);
        
        context_->set (size_, Knowledge_Record::Integer (size), settings_);

        if ((size_t)size > old_size)
        {
          for (; old_size < (size_t)size; ++old_size)
          {
            std::stringstream buffer;
            buffer << name_;
            buffer << delimiter_;
            buffer << old_size;
            vector_[old_size] = context_->get_ref (buffer.str (), settings_);
          }
        }
        else if (delete_vars)
        {
          for (; (size_t)size < old_size; ++size)
          {
            std::stringstream buffer;
            buffer << name_;
            buffer << delimiter_;
            buffer << size;

            context_->delete_variable (buffer.str (), settings_);
          }
        }
      }
    }
    else
    {
      // dynamically allocate size from the context
      size_t cur_size =
        (size_t) context_->get (size_, settings_).to_integer ();

      size_t old_size = vector_.size ();

      if (old_size != cur_size)
      {
        vector_.resize (cur_size);

        if (cur_size > old_size)
        {
          for (; old_size < (size_t)cur_size; ++old_size)
          {
            std::stringstream buffer;
            buffer << name_;
            buffer << delimiter_;
            buffer << old_size;
            vector_[old_size] = context_->get_ref (buffer.str (), settings_);
          }
        }
        else if (delete_vars)
        {
          for (; (size_t)cur_size < old_size; ++cur_size)
          {
            std::stringstream buffer;
            buffer << name_;
            buffer << delimiter_;
            buffer << cur_size;

            context_->delete_variable (buffer.str (), settings_);
          }
        }
      }
    }
  }
}

size_t
Madara::Knowledge_Engine::Containers::Double_Vector::size (void) const
{
  Guard guard (mutex_);
  return vector_.size ();
}

void
Madara::Knowledge_Engine::Containers::Double_Vector::set_name (
  const std::string & var_name,
  Knowledge_Base & knowledge, int size)
{
  if (context_ != &(knowledge.get_context ()) || name_ != var_name)
  {
    context_ = &(knowledge.get_context ());

    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    name_ = var_name;

    vector_.clear ();

    size_ = get_size_ref ();
    
    resize (size);
  }
}

void
Madara::Knowledge_Engine::Containers::Double_Vector::set_name (
  const std::string & var_name,
  Variables & knowledge, int size)
{
  if (context_ != knowledge.get_context () || name_ != var_name)
  {
    context_ = knowledge.get_context ();

    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    name_ = var_name;

    vector_.clear ();
    resize (size);
  }
}

void
Madara::Knowledge_Engine::Containers::Double_Vector::set_name (
  const std::string & var_name,
  Thread_Safe_Context & knowledge, int size)
{
  if (context_ != &knowledge || name_ != var_name)
  {
    context_ = &knowledge;

    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    name_ = var_name;

    vector_.clear ();
    resize (size);
  }
}

void
Madara::Knowledge_Engine::Containers::Double_Vector::set_delimiter (
const std::string & delimiter)
{
  delimiter_ = delimiter;
  if (context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    vector_.clear ();
    resize (-1);
  }
}


std::string
Madara::Knowledge_Engine::Containers::Double_Vector::get_delimiter (void)
{
  return delimiter_;
}

void
Madara::Knowledge_Engine::Containers::Double_Vector::exchange (
  Double_Vector & other, bool refresh_keys, bool delete_keys)
{
  if (context_ && other.context_)
  {
    Context_Guard context_guard (*context_);
    Context_Guard other_context_guard (*other.context_);
    Guard guard (mutex_), guard2 (other.mutex_);

    if (refresh_keys)
    {
      other.resize ();
      this->resize ();
    }

    size_t other_size = other.vector_.size ();
    size_t this_size = this->vector_.size ();

    for (size_t i = 0; i < this_size; ++i)
    {
      // temp = this[i];
      Knowledge_Record temp = context_->get (this->vector_[i], settings_);
    
      if (i < other_size)
      {
        // this[i] = other[i];
        context_->set (this->vector_[i],
          context_->get (other.vector_[i], other.settings_),
          settings_);

        // other[i] = temp;
        other.context_->set (other.vector_[i], temp, other.settings_);
      }
      else
      {
        if (delete_keys)
        {
          std::stringstream buffer;
          buffer << this->name_;
          buffer << delimiter_;
          buffer << i;
          this->context_->delete_variable (buffer.str (), other.settings_);
        }
        else
        {
          Knowledge_Record zero;
          this->context_->set (this->vector_[i], zero, this->settings_);
        }

        {
          std::stringstream buffer;
          buffer << other.name_;
          buffer << delimiter_;
          buffer << i;

          // other[i] = temp;
          other.context_->set (buffer.str (), temp, other.settings_);
        }
      }

    }

    // copy the other vector's elements to this vector's location
    for (size_t i = this_size; i < other_size; ++i)
    {
      std::stringstream buffer;
      buffer << this->name_;
      buffer << delimiter_;
      buffer << i;
      context_->set (buffer.str (),
        other.context_->get (other.vector_[i],
        other.settings_), this->settings_);
    }

    // set the size appropriately
    this->context_->set (this->size_,
      Knowledge_Record::Integer (other_size), this->settings_);
    other.context_->set (other.size_,
      Knowledge_Record::Integer (this_size), other.settings_);

    if (refresh_keys)
    {
      this->resize (-1, true);
      other.resize (-1, true);
    }
  }
}

void
Madara::Knowledge_Engine::Containers::Double_Vector::transfer_to (
  Double_Vector & other)
{
  if (context_ && other.context_)
  {
    Context_Guard context_guard (*context_);
    Context_Guard other_context_guard (*other.context_);
    Guard guard (mutex_);
    Guard guard2 (other.mutex_);

    size_t other_size = other.vector_.size ();
    size_t this_size = this->vector_.size ();

    size_t size = other_size + this_size;
    other.resize ((int)size);

    for (size_t i = 0, j = other_size; i < this_size; ++i, ++j)
    {
      other.context_->set (other.vector_[j], (*this)[i], other.settings_);
    }

    this->resize (0, true);
  }
}

void
Madara::Knowledge_Engine::Containers::Double_Vector::copy_to (
  Knowledge_Vector & target) const
{
  if (context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    target.resize (vector_.size ());
    
    for (size_t i = 0; i < vector_.size (); ++i)
    {
      target[i].deep_copy ((*this)[i]);
    }
  }
}

void
Madara::Knowledge_Engine::Containers::Double_Vector::copy_to (
std::vector <double> & target) const
{
  if (context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    target.resize (vector_.size ());

    for (size_t i = 0; i < vector_.size (); ++i)
    {
      target[i] = (*this)[i];
    }
  }
}

Madara::Knowledge_Engine::Containers::Double_Vector::type
Madara::Knowledge_Engine::Containers::Double_Vector::operator[] (
  size_t index) const
{
  Knowledge_Record result;
  Knowledge_Update_Settings keep_local (true);

  if (index < vector_.size () && context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);
    result = context_->get (vector_[index], keep_local);
  }

  return result.to_double ();
}

Madara::Knowledge_Record
Madara::Knowledge_Engine::Containers::Double_Vector::to_record (
  size_t index) const
{
  Knowledge_Record result;
  Knowledge_Update_Settings keep_local (true);

  if (index < vector_.size () && context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);
    result = context_->get (vector_[index], keep_local);
  }

  return result;
}

Madara::Knowledge_Record
Madara::Knowledge_Engine::Containers::Double_Vector::to_record (void) const
{
  Knowledge_Record result;
  Knowledge_Update_Settings keep_local (true);

  // if we have something to actually set
  if (vector_.size () > 0 && context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    // set last element first so we're not constantly resizing
    result.set_index (vector_.size () - 1,
      context_->get (vector_[vector_.size () - 1], keep_local).to_double ());

    for (size_t i = 0; i < vector_.size () - 1; ++i)
    {
      result.set_index (i,
        context_->get (vector_[i], keep_local).to_double ());
    }
  }

  return result;
}

bool
Madara::Knowledge_Engine::Containers::Double_Vector::exists (
  size_t index) const
{
  bool result (false);

  if (index < vector_.size () && context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);
    result = context_->exists (vector_[index]);
  }

  return result;
}

int
Madara::Knowledge_Engine::Containers::Double_Vector::set (
size_t index,
type value)
{
  int result = -1;

  if (index < vector_.size () && context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);
    result = context_->set (vector_[index], value, settings_);
  }

  return result;
}


int
Madara::Knowledge_Engine::Containers::Double_Vector::set (
const std::vector <type> & value)
{
  int result = -1;

  if (context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);
    if (vector_.size () < value.size ())
      resize ((int)value.size (), false);

    for (size_t i = 0; i < value.size (); ++i)
    {
      context_->set (vector_[i], value[i], settings_);
    }

    result = 0;
  }

  return result;
}

int
Madara::Knowledge_Engine::Containers::Double_Vector::set (
  size_t index,
  type value, 
  const Knowledge_Update_Settings & settings)
{
  int result = -1;
  
  if (index < vector_.size () && context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);
    result = context_->set (vector_[index], value, settings);
  }

  return result;
}


int
Madara::Knowledge_Engine::Containers::Double_Vector::set (
  const std::vector <type> & value,
  const Knowledge_Update_Settings & settings)
{
  int result = -1;
  
  if (context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);
    if (vector_.size () < value.size ())
      resize ((int)value.size (), false);

    for (size_t i = 0; i < value.size (); ++i)
    {
      context_->set (vector_[i], value[i], settings);
    }

    result = 0;
  }
  
  return result;
}

void
Madara::Knowledge_Engine::Containers::Double_Vector::set_quality (
  size_t index,
  uint32_t quality,
  const Knowledge_Reference_Settings & settings)
{
  if (index < vector_.size () && context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);
    context_->set_quality (vector_[index].get_name (), quality,
      true, settings);
  }
}

bool
Madara::Knowledge_Engine::Containers::Double_Vector::is_true (void) const
{
  bool result (false);

  madara_logger_log (context_->get_logger (), Logger::LOG_MAJOR,
    "Double_Vector::is_true: Checking for truth\n");

  if (context_)
  {
    Context_Guard context_guard (*context_);
    Guard guard (mutex_);

    result = true;

    madara_logger_log (context_->get_logger (), Logger::LOG_MINOR,
      "Double_Vector::is_true: context was not null. Result changed to %d\n",
      (int)result);

    for (size_t index = 0; index < vector_.size (); ++index)
    {

      madara_logger_log (context_->get_logger (), Logger::LOG_DETAILED,
        "Double_Vector::is_true: checking index %d, is_false of %d. \n",
        (int)result, (int)context_->get (vector_[index]).is_false ());

      if (context_->get (vector_[index]).is_false ())
      {
        madara_logger_log (context_->get_logger (), Logger::LOG_MAJOR,
          "Double_Vector::is_true: result is false, breaking\n");

        result = false;
        break;
      }
    }

    if (vector_.size () == 0)
      result = false;
  }

  madara_logger_log (context_->get_logger (), Logger::LOG_MAJOR,
    "Double_Vector::is_true: final result is %d\n", (int)result);

  return result;
}

bool
Madara::Knowledge_Engine::Containers::Double_Vector::is_false (void) const
{
  return !is_true ();
}


bool
Madara::Knowledge_Engine::Containers::Double_Vector::is_true_ (void) const
{
  return is_true ();
}

bool
Madara::Knowledge_Engine::Containers::Double_Vector::is_false_ (void) const
{
  return is_false ();
}
