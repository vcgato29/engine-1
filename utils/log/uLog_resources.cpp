/*!
 * \file log_resources.cpp
 * \brief \b Classes: \a uLogType, \a __uLogStore, \a uLogEntry, \a __uLogStoreHelper
 */

#include "uLog_resources.hpp"
#include "uLog.hpp"
#include "defines.hpp"
#include <iomanip>
#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>

#if UNIX
#define E_COLOR_NO_TERMTEST
#include "eCMDColor.hpp"
#endif

namespace e_engine {
namespace internal {


__uLogStoreHelper::__uLogStoreHelper( wchar_t _type, std::string _rawFilename, int _logLine, std::string _functionName, bool _wait )  {
   std::time( &vTime_lI );
   vType_C              = _type;
   vRawFilename_STR     = std::wstring( _rawFilename.begin(), _rawFilename.end() );
   vLogLine_I           = _logLine;

   vFunctionName_STR    = std::wstring( _functionName.begin(), _functionName.end() );

   vComplete_B          = false;

   vAttrib_C            = '-';
   vFG_C                = '-';
   vBG_C                = '-';
   vIsPrinted_B         = false;
   vEndFinished_B       = false;
   vWaitForLogPrinted_B = _wait;
}


unsigned int __uLogStoreHelper::getLogEntry( std::vector< e_engine::internal::uLogType > &_vLogTypes_V_eLT, e_engine::uLogEntry &_entry ) {
   _entry.data.vLogEntries_V_eLS = vElements_V_eLS;
   _entry.data.vFilename_STR     = vRawFilename_STR;
   _entry.data.vFunctionName_STR = vFunctionName_STR;
   _entry.data.vLine_I           = vLogLine_I;
   _entry.data.vTime_lI          = vTime_lI;
   _entry.data.vType_STR         = L"UNKNOWN";
   _entry.data.vNewColor_STR     = testNewColor();

   if ( _vLogTypes_V_eLT.empty() ) {
      eLOG "No Log type found!! Please add at least one manually or run 'uLog.devInit();', which will be run now to prevent further Errors" END
      LOG.devInit();
   }

   for ( unsigned int i = 0; i < _vLogTypes_V_eLT.size(); ++i ) {
      if ( _vLogTypes_V_eLT[i].getType() == vType_C ) {
         _entry.data.vType_STR     = _vLogTypes_V_eLT[i].getString();
         _entry.data.vBasicColor_C = _vLogTypes_V_eLT[i].getColor();
         _entry.data.vBold_B       = _vLogTypes_V_eLT[i].getBold();
         return i;
      }
   }

   std::wstring ltemp_STR = L"WARNING!! Log type '";
   ltemp_STR += vType_C;
   ltemp_STR += L"' not Found";

   vElements_V_eLS.push_back( internal::__uLogStore( ltemp_STR, 'B', 'R', '-' ) );

   return 0;
}

std::wstring __uLogStoreHelper::testNewColor()  {
#if UNIX
   if ( vFG_C != L'-' ) {
      if ( vAttrib_C != L'-' ) {
         if ( vBG_C != L'-' ) {
            return eCMDColor::color( vAttrib_C, vFG_C, vBG_C );
         }
         return eCMDColor::color( vAttrib_C, vFG_C );
      }
      return eCMDColor::color( vFG_C );
   }
#endif
   return L"";
}


bool __uLogStore::hasColor()  {
#if UNIX
   if ( vFG_C != '-' ) {
      if ( vAttrib_C != '-' ) {
         if ( vBG_C != '-' ) {
            vColor_STR = eCMDColor::color( vAttrib_C, vFG_C, vBG_C );
            return true;
         }
         vColor_STR = eCMDColor::color( vAttrib_C, vFG_C );
         return true;
      }
      vColor_STR = eCMDColor::color( vFG_C );
      return true;
   }
#endif
   return false;
}


}

void uLogEntry::configure( e_engine::LOG_COLOR_TYPE _color, e_engine::LOG_PRINT_TYPE _time, e_engine::LOG_PRINT_TYPE _file, e_engine::LOG_PRINT_TYPE _errorType, int _columns ) {
   config.vColor_LCT     = _color;
   config.vTime_LPT      = _time;
   config.vFile_LPT      = _file;
   config.vErrorType_LPT = _errorType;
   config.vColumns_uI    = _columns;
}


}
// kate: indent-mode cstyle; indent-width 3; replace-tabs on; 
