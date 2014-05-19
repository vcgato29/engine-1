/// \file eInit.hpp
/// \brief \b Classes: \a eInit
/// \sa e_eInit.cpp e_event.cpp
/*
 *  E Engine
 *  Copyright (C) 2013 Daniel Mensinger
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */


#ifndef E_INIT_HPP
#define E_INIT_HPP

#if defined __linux__
#include <GL/glxew.h>
#endif

#if defined WIN32 || defined _WIN32
#include <GL/glew.h>
#endif

#include "context.hpp"
#include "keyboard.hpp"
#include "signal_slot.hpp"
#include "eWindowData.hpp"
#include <boost/thread.hpp>



namespace e_engine {

typedef GLvoid( *RENDER_FUNC )( eWinInfo info );


/*!
 * \class e_engine::eInit
 * \brief Init the <b>E Engine</b>
 *
 * This class handels the context with the eContext class. It
 * sets the Crt-C handle function and starts the main and eventLoop
 * loop. This should be after eWindowData the 1st class you generate
 * and you will do all window and curser config with it.
 *
 * \note Until now ther is no MS Windows or Mac support
 *
 * \note Curser function are not implementet yet
 *
 * \sa eContext eWindowData e_eInit.cpp e_event.cpp
 */
class eInit : public eContext, public eKeyboard {
      typedef eSignal<void, eWinInfo>      _SIGNAL_;
      typedef eSlot<void, eInit, eWinInfo> _SLOT_;
   private:
      RENDER_FUNC       fRender;

      _SIGNAL_          vWindowClose_SIG;  //!< The signal for Window close
      _SIGNAL_          vResize_SIG;       //!< The signal for Resize
      _SIGNAL_          vKey_SIG;          //!< The signal for Key
      _SIGNAL_          vMouse_SIG;        //!< The signal for Mouse

      _SLOT_            vWindowClose_SLOT;    //!< The standard slot for Window close
      _SLOT_            vResize_SLOT;         //!< The standard slot for Resize
      _SLOT_            vKey_SLOT;            //!< The standard slot for Key
      _SLOT_            vMouse_SLOT;          //!< The standard slot for Mouse

      bool              vMainLoopRunning_B;
      bool              vBoolCloseWindow_B;

      bool              vHaveGLEW_B;

      bool              vEventLoopHasFinished_B;
      bool              vRenderLoopHasFinished_B;

      boost::thread     vEventLoop_BT;
      boost::thread     vRenderLoop_BT;
      boost::thread     vQuitMainLoop_BT;

      // Thread Functions --------------------------------------------------------- ###
      int  renderLoop();       //!< The render loop function
      int  eventLoop();        //!< The event loop function ( In PLATFORM/e_event.cpp )
      int  quitMainLoopCall(); //!< The real function to quit the main loop

      // Signal handling ---------------------------------------------------------- ###
      static GLvoid handleSignal( int _signal ); //!< The signal handle function
      GLvoid _setThisForHandleSignal();          //!< Unfortunately you can not pas this with signal()

      // Standart callbacks NEW --------------------------------------------------- ###

      //! The standard render function
      static inline GLvoid standardRender( eWinInfo _info ) {
         glClearColor( 0, 0, 0, 1 );
         glClear( GL_COLOR_BUFFER_BIT );
         _info.eInitPointer->swapBuffers();
      }

      //! The standard Resize function
      GLvoid s_standardResize( eWinInfo _info ) {}

      //! The standard Window close function
      GLvoid s_standardWindowClose( eWinInfo _info );

      //! The standard Key function
      GLvoid s_standardKey( eWinInfo _info ) {}

      //! The standard Mouse function
      GLvoid s_standardMouse( eWinInfo _info ) {}

   public:
      eInit();
      ~eInit() {closeWindow();shutdown();}

      int    init();
      int    shutdown();
      int    startMainLoop( bool _wait = true );

      void   quitMainLoop();

      /*!
       * \brief Quit the main loop and close the window
       * \param waitUntilClosed Wait until window is closed  \c DEFAULT: \b false
       * \returns \c SUCCESS: \a 1 -- \C FAIL: \a 0
       */
      int    closeWindow( bool _waitUntilClosed = false );


      // Standart callbacks NEW --------------------------------------------------- ###
      template<class __C>
      void addWindowCloseSlot( eSlot<void, __C, eWinInfo> *_slot ) {
         vWindowClose_SLOT.disconnectAll();
         _slot->connectWith( &vWindowClose_SIG );
      }
      template<class __C>
      void addResizeSlot( eSlot<void, __C, eWinInfo> *_slot ) {
         vResize_SLOT.disconnectAll();
         _slot->connectWith( &vResize_SIG );
      }
      template<class __C>
      void addKeySlot( eSlot<void, __C, eWinInfo> *_slot ) {
         vKey_SLOT.disconnectAll();
         _slot->connectWith( &vKey_SIG );
      }
      template<class __C>
      void addMouseSlot( eSlot<void, __C, eWinInfo> *_slot ) {
         vMouse_SLOT.disconnectAll();
         _slot->connectWith( &vMouse_SIG );
      }
      
      void removeAllSlots();

      template<class __C> inline bool removeWindowCloseSlot( eSlot<void, __C, eWinInfo> *_slot ) {return vWindowClose_SIG.disconnect( _slot );}
      template<class __C> inline bool removeResizeSlot( eSlot<void, __C, eWinInfo> *_slot )      {return vResize_SIG.disconnect( _slot );}
      template<class __C> inline bool removeKeySlot( eSlot<void, __C, eWinInfo> *_slot )         {return vKey_SIG.disconnect( _slot );}
      template<class __C> inline bool removeMouseSlot( eSlot<void, __C, eWinInfo> *_slot )       {return vMouse_SIG.disconnect( _slot );}

      GLvoid setRenderFunc( RENDER_FUNC _f )            {fRender = _f;}
};

//    #########################
// ######## BEGIN DOXYGEN ########
//    #########################



namespace e_engine_internal {

/*!
 * \class __eInit_Pointer
 * \brief Need this vor the static signal Callback
 *
 * \todo remove this workaround / hack
 *
 */
class __eInit_Pointer {
   private:
      eInit *pointer;
      bool  is_set;
   public:
      __eInit_Pointer() {
         pointer = 0;
         is_set  = false;
      }
      ~__eInit_Pointer() {pointer = 0;}
      bool set( eInit *_THIS ) {
         if ( is_set == true ) {return false;}
         pointer = _THIS;
         is_set = true;
         return true;
      }
      eInit *get() {return pointer;}
};
extern __eInit_Pointer __eInit_Pointer_OBJ;
}

}

#endif // E_INIT_HPP

// kate: indent-mode cstyle; indent-width 3; replace-tabs on; 