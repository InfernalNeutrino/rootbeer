//! \file rb_import_data.h
//! \brief Defines the RB_IMPORT_DATA macro.
//! \details The actual definition will depend on the location from which
//!  this file is #included, basically this allows the macro to be put in just
//!  one place (ImportData.h) but used differently in multiple others.
#ifndef __MAKECINT__

//! Allocate Data<T> classes on heap or stack?
#define RB_DATA_ON_HEAP 1



//! \def RB_IMPORT_DATA(CLASS, SYMBOL, NAME, VISIBLE, ARGS)
//! \brief Adds a global instance of a user's class to rootbeer.
//! \details Users should invoke this macro in ImportData.hh for every
//! instance of their classes they want to include globally in ROOTBEER. Global
//! inclusion is needed to make histograms or use class data in calculations.
//! The function of the macro is to create a global instance of rb::data::Wrapper<T> wrapping
//! the user's class, and to ensure that it's visible in the source files where it's needed.
//! \param CLASS The typename of the user's class.
//! \param SYMBOL The c++ variable name to be used for the global instance.
//! \param NAME String specifying the top-level branch name you want to be used for this instance.
//! \param VISIBLE Boolean specifying whether or not the class should be visible in CINT (using
//! rb::MData::SetValue() and rb::data::Wrapper::GetValue(). Generally, set this to true for variables and
//! false for parameters.
//! \param ARGS String literal specifying constructor arguments to be used for this instance of the
//! user's class. A null string (<tt>""</tt>) will call the default constructor.
#define RB_IMPORT_DATA(CLASS, SYMBOL, NAME, VISIBLE, ARGS)
#undef RB_IMPORT_DATA // Doxygen trick //

#if defined RB_DECLARE
#if RB_DATA_ON_HEAP
#define RB_IMPORT_DATA(CLASS, SYMBOL, NAME, VISIBLE, ARGS)	\
  std::auto_ptr<rb::data::Wrapper<CLASS> > SYMBOL;
#else
#define RB_IMPORT_DATA(CLASS, SYMBOL, NAME, VISIBLE, ARGS)	\
  rb::data::Wrapper<CLASS> SYMBOL;
#endif
#undef RB_DECLARE

#elif defined RB_INIT
#if RB_DATA_ON_HEAP
#define RB_IMPORT_DATA(CLASS, SYMBOL, NAME, VISIBLE, ARGS)	\
  SYMBOL(new rb::data::Wrapper<CLASS>(NAME, VISIBLE, ARGS)),
#else
#define RB_IMPORT_DATA(CLASS, SYMBOL, NAME, VISIBLE, ARGS)	\
  SYMBOL(NAME, VISIBLE, ARGS),
#endif
#undef RB_INIT

#elif defined RB_ADD_BRANCH
#if RB_DATA_ON_HEAP
#define RB_IMPORT_DATA(CLASS, SYMBOL, NAME, VISIBLE, ARGS)	\
  {								\
    CountedLockingPointer<CLASS> p = SYMBOL->GetPointer();	\
    void * v = reinterpret_cast<void*>(p.Get());		\
    LockFreePointer<TTree>(fTree)->Branch(NAME, #CLASS, &v);	\
  }
#else
#define RB_IMPORT_DATA(CLASS, SYMBOL, NAME, VISIBLE, ARGS)	\
  {								\
    CountedLockingPointer<CLASS> p = *SYMBOL;			\
    void * v = reinterpret_cast<void*>(p.Get());		\
    LockFreePointer<TTree>(fTree)->Branch(NAME, #CLASS, &v);	\
  }
#endif
#undef RB_ADD_BRANCH

#elif defined RB_REFERENCE_DECLARE // [rb::BufferSource]
#define RB_IMPORT_DATA(CLASS, SYMBOL, NAME, VISIBLE, ARGS)	\
  rb::data::Wrapper<CLASS>& SYMBOL;
#undef RB_REFERENCE_DECLARE

#elif defined RB_REFERENCE_INIT // [rb::BufferSource::BufferSource]
#if RB_DATA_ON_HEAP
#define RB_IMPORT_DATA(CLASS, SYMBOL, NAME, VISIBLE, ARGS)	\
  SYMBOL(*(rb::gApp()->fDataGlobals.SYMBOL.get())),
#else
#define RB_IMPORT_DATA(CLASS, SYMBOL, NAME, VISIBLE, ARGS)	\
  SYMBOL(rb::gApp()->fDataGlobals.SYMBOL),
#endif
#undef RB_REFERENCE_INIT

#else
#define RB_IMPORT_DATA(CLASS, SYMBOL, NAME, VISIBLE, ARGS) //
#if RB_DATA_ON_HEAP
#include <memory>
#endif

#endif

#include "../user/ImportData.h"
#undef RB_IMPORT_DATA

#endif // #ifndef __MAKECINT__
