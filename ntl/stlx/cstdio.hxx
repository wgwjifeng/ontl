/**\file*********************************************************************
 *                                                                     \brief
 *  C Library files utilities [c.files]
 *
 ****************************************************************************
 */

#ifndef NTL__STLX_CSTDIO
#define NTL__STLX_CSTDIO

#include "cstddef.hxx"
#include "cstdarg.hxx"
using std::va_list;




#ifndef _INC_STDLIB// MSVC compatibility

NTL__EXTERNAPI size_t _cdecl
  _snprintf(char *buffer, size_t count, const char *format, ...);

NTL__EXTERNAPI size_t _cdecl
  _vsnprintf(char *buffer, size_t count, const char *format, va_list argptr);

#endif



namespace std {

/**\addtogroup  lib_language_support *** Language support library [18] ******
 *@{*/

/**\addtogroup  lib_general_utilities ** C Library filees [c.files  ] *******
 *@{*/

  using ::_snprintf; using ::_vsnprintf;


/**@} lib_general_utilities
 */
/**@} lib_language_support */

}//namespace std

#endif //#ifndef NTL__STLX_CSTDIO
