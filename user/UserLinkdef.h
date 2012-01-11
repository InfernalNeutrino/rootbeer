//! \file UserLinkdef.h
//! \brief User-defined portion of the CINT Linkdef file.

/// This file is where you can tell CINT about the classes you've written to store and analyze
/// your data.  At the least, you will need to do this for any class that stores parameters that you
/// wish to view or variables used in calculating those parameters.
/// Usually the easiest option is link everything in the header files which define your classes.
/// Example:
/// #pragma link C++ defined_in /path/to/my/header.hxx;
/// (note that you need to specify the fill path to the header file).
///
/// If the above doesn't work, you might have to specify each class (or namespace) individually.
/// (or figure out why not...)
/// Example:
/// \code
/// #pragma link C++ namespace my;
/// #pragma link C++ class my::Class+;
/// \endcode
/// -- OR --
/// #pragma link C++ class MyClass+;
/// -- ETC -- 
/// (note that the '+' at the end is required except in spacial cases).



#pragma link C++ namespace raw;
#pragma link C++ namespace cal;
#pragma link C++ namespace var;
#pragma link C++ defined_in /user/gchristian/soft/n2analysis/src/mona.hh;
#pragma link C++ defined_in /user/gchristian/soft/develop/dragon/analyzer/src/Dragon.hxx;
#pragma link C++ class TMidasEvent+;
#pragma link C++ class TimeStamp+;

#pragma link C++ class raw::detector+;

#pragma link C++ class ExampleData+;
#pragma link C++ class ExampleVariables+;
#pragma link C++ class InnerVariables+;
