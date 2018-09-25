/**********************************************************************************************
*
*   rglfw - raylib GLFW single file compilation
*
*   This file includes latest GLFW sources (https://github.com/glfw/glfw) to be compiled together 
*   with raylib for all supported platforms, this way, no external dependencies are required.
*
*   LICENSE: zlib/libpng
*
*   Copyright (c) 2017-2018 Ramon Santamaria (@raysan5)
*
*   This software is provided "as-is", without any express or implied warranty. In no event
*   will the authors be held liable for any damages arising from the use of this software.
*
*   Permission is granted to anyone to use this software for any purpose, including commercial
*   applications, and to alter it and redistribute it freely, subject to the following restrictions:
*
*     1. The origin of this software must not be misrepresented; you must not claim that you
*     wrote the original software. If you use this software in a product, an acknowledgment
*     in the product documentation would be appreciated but is not required.
*
*     2. Altered source versions must be plainly marked as such, and must not be misrepresented
*     as being the original software.
*
*     3. This notice may not be removed or altered from any source distribution.
*
**********************************************************************************************/

//#define _GLFW_BUILD_DLL           // To build shared version
//http://www.glfw.org/docs/latest/compile.html#compile_manual

#if defined(_WIN32)
    #define _GLFW_WIN32
#endif
#if defined(__linux__)
    #if !defined(_GLFW_WAYLAND)     // Required for Wayland windowing
        #define _GLFW_X11
    #endif
#endif
#if defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__NetBSD__) || defined(__DragonFly__)
    #define _GLFW_X11
#endif
#if defined(__APPLE__)
    #define _GLFW_COCOA
    #define _GLFW_USE_MENUBAR       // To create and populate the menu bar when the first window is created
    #define _GLFW_USE_RETINA        // To have windows use the full resolution of Retina displays
#endif

// NOTE: _GLFW_MIR experimental platform not supported at this moment

#include "glfw/src/context.c"
#include "glfw/src/init.c"
#include "glfw/src/input.c"
#include "glfw/src/monitor.c"
#include "glfw/src/vulkan.c"
#include "glfw/src/window.c"

#if defined(_WIN32)
    #include "glfw/src/win32_init.c"
    #include "glfw/src/win32_joystick.c"
    #include "glfw/src/win32_monitor.c"
    #include "glfw/src/win32_time.c"
    #include "glfw/src/win32_thread.c"
    #include "glfw/src/win32_window.c"
    #include "glfw/src/wgl_context.c"
    #include "glfw/src/egl_context.c"
    #include "glfw/src/osmesa_context.c"
#endif

#if defined(__linux__)
    #if defined(_GLFW_WAYLAND)
        #include "glfw/src/wl_init.c"
        #include "glfw/src/wl_monitor.c"
        #include "glfw/src/wl_window.c"
        #include "glfw/src/wayland-pointer-constraints-unstable-v1-client-protocol.c"
        #include "glfw/src/wayland-relative-pointer-unstable-v1-client-protocol.c"
        #endif
    #if defined(_GLFW_X11)
        #include "glfw/src/x11_init.c"
        #include "glfw/src/x11_monitor.c"
        #include "glfw/src/x11_window.c"
        #include "glfw/src/glx_context.c"
    #endif

    #include "glfw/src/linux_joystick.c"
    #include "glfw/src/posix_thread.c"
    #include "glfw/src/posix_time.c"
    #include "glfw/src/xkb_unicode.c"
    #include "glfw/src/egl_context.c"
    #include "glfw/src/osmesa_context.c"
#endif

#if defined(__FreeBSD__) || defined(__OpenBSD__) || defined( __NetBSD__) || defined(__DragonFly__)
    #include "glfw/src/x11_init.c"
    #include "glfw/src/x11_monitor.c"
    #include "glfw/src/x11_window.c"
    #include "glfw/src/xkb_unicode.c"
    // TODO: Joystick implementation
    #include "glfw/src/null_joystick.c"
    #include "glfw/src/posix_time.c"
    #include "glfw/src/posix_thread.c"
    #include "glfw/src/glx_context.c"
    #include "glfw/src/egl_context.c"
    #include "glfw/src/osmesa_context.c"
#endif

#if defined(__APPLE__)
    #include "glfw/src/cocoa_init.m"
    #include "glfw/src/cocoa_joystick.m"
    #include "glfw/src/cocoa_monitor.m"
    #include "glfw/src/cocoa_window.m"
    #include "glfw/src/cocoa_time.c"
    #include "glfw/src/posix_thread.c"
    #include "glfw/src/nsgl_context.m"
    #include "glfw/src/egl_context.c"
    #include "glfw/src/osmesa_context.c"
#endif
