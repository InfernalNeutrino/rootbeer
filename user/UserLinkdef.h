//! \file UserLinkdef.h
//! \brief User-defined portion of the CINT Linkdef file.
//!
//! \full This file is where you can tell CINT about the classes you've written to store and analyze
//! your data.  At the least, you will need to do this for any class that stores parameters that you
//! wish to view or variables used in calculating those parameters.
//! Usually the easiest option is link everything in the header files which define your classes. \n
//! Example:
//! \code
//! #pragma link C++ defined_in /path/to/my/header.hxx;
//! \endcode
//! (note that you need to specify the fill path to the header file).
//!
//! If the above doesn't work, you might have to specify each class (or namespace) individually
//! (or figure out why not). \n
//! Example:
//! \code
//! #pragma link C++ namespace my;
//! #pragma link C++ class my::Class+;
//! // -- OR -- //
//! #pragma link C++ class MyClass+;
//! // -- ETC -- //
//! \endcode
//! (note that the '+' at the end is required except in special cases).


#pragma link C++ defined_in /user/gchristian/soft/develop/dragon/analyzer/src/Dragon.hxx;
#pragma link C++ defined_in /user/gchristian/soft/develop/dragon/analyzer/src/Gamma.hxx;
#pragma link C++ defined_in /user/gchristian/soft/develop/dragon/analyzer/src/HeavyIon.hxx;
#pragma link C++ defined_in /user/gchristian/soft/develop/dragon/analyzer/src/Modules.hxx;
#pragma link C++ defined_in /user/gchristian/soft/develop/dragon/analyzer/src/vme/Vme.hxx;
#pragma link C++ class vme::caen::Adc<32>+;
#pragma link C++ class vme::caen::V1190b+;
#pragma link C++ class vme::IO32+;
