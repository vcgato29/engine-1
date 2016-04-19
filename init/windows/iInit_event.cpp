/*!
 * \file windows/event.cpp
 * \brief \b Classes: \a iInit
 * \sa e_iInit.cpp e_iInit.hpp
 */
/*
 * Copyright (C) 2015 EEnginE project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "iInit.hpp"
#include "iWindow.hpp"
#include <windows.h>
#include "uLog.hpp"

namespace e_engine {


int iInit::eventLoop() {
   //! \todo Move this in windows_win32
   if ( !vEventLoopHasFinished_B )
      return -1;

   LOG.nameThread( L"EVENT" );

   iLOG( "Event thread started" );

   {
      // Make sure lLockWindow_BT will be destroyed
      std::lock_guard<std::mutex> lLockWindow_BT( vCreateWindowMutex_BT );
      vCreateWindowReturn_I = vWindow.createWindow();

      // Context created; continue with init();
      vCreateWindowCondition_BT.notify_one();
   }


   // Now wait until the main event loop is officially "started"

   MSG msg;

   while ( true ) {
      dLOG( "Event thread waiting" );
      while ( !vContinueWithEventLoop_B ) {
         std::unique_lock<std::mutex> lLockEvent_BT( vStartEventMutex_BT );
         vStartEventCondition_BT.wait( lLockEvent_BT );
      }
      dLOG( "Event thread continue" );

      vEventLoopHasFinished_B = false;
      iLOG( "Event loop started" );

      while ( vWindow.getIsWindowCreated() && vMainLoopRunning_B ) {

         while ( PeekMessageW( &msg, vWindow.getHWND_win32(), 0, 0, PM_REMOVE ) ) {
            TranslateMessage( &msg );
            DispatchMessageW( &msg );
         }

         B_SLEEP( milliseconds, 5 );
      }

      iLOG( "Event loop finished" );
      vEventLoopHasFinished_B = true;

      // Event loop finished; continue with startMainLoop();
      std::lock_guard<std::mutex> lLockWindow_BT( vStopEventLoopMutex );
      vStopEventLoopCondition.notify_one();
   }

   DestroyWindow( vWindow.getHWND_win32() );

   // Proccess the last messages
   while ( PeekMessageW( &msg, vWindow.getHWND_win32(), 0, 0, PM_REMOVE ) ) {
      TranslateMessage( &msg );
      DispatchMessageW( &msg );
   }

   while ( PeekMessageW( &msg, NULL, 0, 0, PM_REMOVE ) ) {
      TranslateMessage( &msg );
      DispatchMessageW( &msg );
   }

   iLOG( "Event thread done" );
   return 1;
}

LRESULT CALLBACK iInit::initialWndProc( HWND _hwnd, UINT _uMsg, WPARAM _wParam, LPARAM _lParam ) {
   if ( _uMsg == WM_NCCREATE ) {
      windows_win32::ClassPointers pointers;
      LPCREATESTRUCT lCreateStruct_win32 = reinterpret_cast<LPCREATESTRUCT>( _lParam );
      void *lCreateParam_win32           = lCreateStruct_win32->lpCreateParams;

      memcpy( &pointers, lCreateParam_win32, sizeof( windows_win32::ClassPointers ) );

      pointers.window->vHWND_Window_win32 = _hwnd;

      SetWindowLongPtrW( _hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>( pointers.window ) );
      SetWindowLongPtrW( _hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>( &staticWndProc ) );
      iEventInfo _tempInfo( e_engine::internal::__iInit_Pointer_OBJ.get() );
      return pointers.init->actualWndProc( _uMsg, _wParam, _lParam, _tempInfo );
   }
   // if it isn't WM_NCCREATE, do something useful and wait until
   //   WM_NCCREATE is sent
   return DefWindowProc( _hwnd, _uMsg, _wParam, _lParam );
}

LRESULT CALLBACK iInit::staticWndProc( HWND _hwnd, UINT _uMsg, WPARAM _wParam, LPARAM _lParam ) {
   windows_win32::ClassPointers pointers;
   LONG_PTR lUserData_win32 = GetWindowLongPtrW( _hwnd, GWLP_USERDATA );
   iEventInfo _tempInfo( e_engine::internal::__iInit_Pointer_OBJ.get() );

   memcpy( &pointers,
           reinterpret_cast<void *>( lUserData_win32 ),
           sizeof( windows_win32::ClassPointers ) );

   if ( !pointers.window || _hwnd != pointers.window->vHWND_Window_win32 ) {
      eLOG( "Bad Windows callback error" );
   }

   return pointers.init->actualWndProc( _uMsg, _wParam, _lParam, _tempInfo );
}

LRESULT CALLBACK iInit::actualWndProc( UINT _uMsg,
                                       WPARAM _wParam,
                                       LPARAM _lParam,
                                       iEventInfo _tempInfo ) {
   unsigned int key_state = E_PRESSED;


   if ( _tempInfo.iInitPointer == 0 ) {
      eLOG( "iInit-pointer is not yet initialized" );
      return 0;
   }

   switch ( _uMsg ) {

      case WM_SIZE:
         _tempInfo.type          = E_EVENT_RESIZE;
         _tempInfo.eResize.width = GlobConf.win.width =
               _lParam & 0xFFFF; // Get the low order word as a width
         _tempInfo.eResize.height = GlobConf.win.height =
               _lParam >> 16; // Get the high order word as a height
         _tempInfo.eResize.posX = GlobConf.win.posX;
         _tempInfo.eResize.posY = GlobConf.win.posY;
         vResize_SIG.send( _tempInfo );
         return 0;

      case WM_MOVE:
         _tempInfo.type         = E_EVENT_MOUSE;
         _tempInfo.eResize.posX = GlobConf.win.posX =
               _lParam & 0xFFFF; // Get the low order word as the x-Position
         _tempInfo.eResize.posY = GlobConf.win.posY =
               _lParam >> 16; // Get the high order word as the y-Position
         _tempInfo.eResize.width  = GlobConf.win.width;
         _tempInfo.eResize.height = GlobConf.win.height;
         vResize_SIG.send( _tempInfo );
         return 0;

      case WM_MOUSEMOVE: // Mouse moved, see
         // http://msdn.microsoft.com/en-us/library/windows/desktop/ms645616%28v=vs.85%29.aspx
         _tempInfo.type        = E_EVENT_MOUSE;
         _tempInfo.iMouse.posX = GlobConf.win.mousePosX =
               _lParam & 0xFFFF; // Get the low order word as the x-Position
         _tempInfo.iMouse.posY = GlobConf.win.mousePosY =
               _lParam >> 16; // Get the high order word as the y-Position
         _tempInfo.iMouse.button = E_MOUSE_MOVE;
         _tempInfo.iMouse.state  = E_PRESSED;

         //!\todo Check if the coords are right, see the article above

         vMouse_SIG.send( _tempInfo );
         return 0;

      case WM_CLOSE:
         _tempInfo.type = E_EVENT_WINDOWCLOSE;
         vWindowClose_SIG.send( _tempInfo );
         return 0;

      case WM_SETFOCUS:
         _tempInfo.type            = E_EVENT_FOCUS;
         _tempInfo.eFocus.hasFocus = GlobConf.win.windowHasFocus = true;
         vFocus_SIG.send( _tempInfo );
         return 0;
      case WM_KILLFOCUS:
         _tempInfo.type            = E_EVENT_FOCUS;
         _tempInfo.eFocus.hasFocus = GlobConf.win.windowHasFocus = false;
         vFocus_SIG.send( _tempInfo );
         return 0;


      case WM_LBUTTONUP: key_state = E_RELEASED;
      case WM_LBUTTONDOWN:
         _tempInfo.type          = E_EVENT_KEY;
         _tempInfo.iMouse.posX   = GlobConf.win.mousePosX;
         _tempInfo.iMouse.posY   = GlobConf.win.mousePosY;
         _tempInfo.iMouse.state  = key_state;
         _tempInfo.iMouse.button = E_MOUSE_LEFT;
         vMouse_SIG.send( _tempInfo );
         return 0;

      case WM_MBUTTONUP: key_state = E_RELEASED;
      case WM_MBUTTONDOWN:
         _tempInfo.type          = E_EVENT_KEY;
         _tempInfo.iMouse.posX   = GlobConf.win.mousePosX;
         _tempInfo.iMouse.posY   = GlobConf.win.mousePosY;
         _tempInfo.iMouse.state  = key_state;
         _tempInfo.iMouse.button = E_MOUSE_MIDDLE;
         vMouse_SIG.send( _tempInfo );
         return 0;

      case WM_RBUTTONUP: key_state = E_RELEASED;
      case WM_RBUTTONDOWN:
         _tempInfo.type          = E_EVENT_KEY;
         _tempInfo.iMouse.posX   = GlobConf.win.mousePosX;
         _tempInfo.iMouse.posY   = GlobConf.win.mousePosY;
         _tempInfo.iMouse.state  = key_state;
         _tempInfo.iMouse.button = E_MOUSE_RIGHT;
         vMouse_SIG.send( _tempInfo );
         return 0;

      case WM_XBUTTONUP: key_state = E_RELEASED;
      case WM_XBUTTONDOWN:
         _tempInfo.type         = E_EVENT_KEY;
         _tempInfo.iMouse.posX  = GlobConf.win.mousePosX;
         _tempInfo.iMouse.posY  = GlobConf.win.mousePosY;
         _tempInfo.iMouse.state = key_state;
         switch ( _wParam >> 16 ) {
            case XBUTTON1: _tempInfo.iMouse.button = E_MOUSE_1; break;
            case XBUTTON2: _tempInfo.iMouse.button = E_MOUSE_2; break;
            default:
               iLOG( "Found unhandled X-Button" );
               _tempInfo.iMouse.button = E_MOUSE_UNKNOWN;
               break;
         }
         vMouse_SIG.send( _tempInfo );
         return 0;

      case WM_MOUSEWHEEL:
         _tempInfo.type         = E_EVENT_MOUSE;
         _tempInfo.iMouse.posX  = GlobConf.win.mousePosX;
         _tempInfo.iMouse.posY  = GlobConf.win.mousePosY;
         _tempInfo.iMouse.state = E_PRESSED;

         if ( ( (short signed)( _wParam >> 16 ) ) >= 0 )
            _tempInfo.iMouse.button = E_MOUSE_WHEEL_UP;
         else
            _tempInfo.iMouse.button = E_MOUSE_WHEEL_DOWN;

         vMouse_SIG.send( _tempInfo );
         return 0;
      case WM_SYSCHAR:
      case WM_CHAR:
         // This returns the actual key the user has typed in after modifiers
         // It is only used if a key associated to a character, not a function key, was pressed
         // Unicode characters are supported by this aswell

         //!\todo Handle this output for text messages/text inputs

         if ( _wParam > 32 ) { // Check if the Char is an actual character; Enter, Backspace and
                               // others are excluded as they are already handled in WM_KEYDOWN
            _tempInfo.type       = E_EVENT_KEY;
            _tempInfo.eKey.state = E_PRESSED;
            _tempInfo.eKey.key = _wParam;
            vKey_SIG.send( _tempInfo );
         }
         return 0;
      case WM_UNICHAR: // Gives other programs the information that Unicode is supported by this
         if ( _wParam == UNICODE_NOCHAR ) {
            return TRUE;
         }
         return FALSE;
      case WM_KEYUP: key_state = E_RELEASED;
      case WM_KEYDOWN:
         _tempInfo.type       = E_EVENT_KEY;
         _tempInfo.eKey.state = key_state;
         _tempInfo.eKey.key   = vWindow.processWindowsKeyInput( _wParam, key_state );

         if ( _tempInfo.eKey.key != 0 ) // Dont send the signal if a character was found
            vKey_SIG.send( _tempInfo );

         // MapVirtualKey(_wParam, MAPVK_VK_TO_CHAR)
         // The above returns 0 when a function key is pressed, may be useful
         return 0;
      case WM_DESTROY: iLOG( "Window Destroyed WM_DESTROY" ); break;
      case WM_NCDESTROY: iLOG( "Window Destroyed WM_NCDESTROY" ); break;
      default: break;
   }

   return DefWindowProc( vWindow.getHWND_win32(), _uMsg, _wParam, _lParam );
}

} // e_engine

// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on;
