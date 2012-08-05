/**<!-------------------------------------------------------------------->
   @file   MiltonAppWindows.inl
   @author Andrew Tucker  (ast@halcyon.com)
   @author Travis Fischer (fisch0920@gmail.com)
   @date   Fall 2009
   
   @brief
      Provides console output on Windows.
    
   @note 
      Win32 applications invoked from the commandline apparently do not 
   have stdin, stdout, and stderr "properly" initialized by default, where 
   by properly, I mean in a sane, sensible manner.. this hack gets around 
   this absurdity by opening a secondary console to pipe output to.
   
   @note 
      Adopted from http://www.halcyon.com/~ast/dload/guicon.htm
   <!-------------------------------------------------------------------->**/

#include <common/common.h>
#ifndef MILTON_APP_WINDOWS_INL_
#define MILTON_APP_WINDOWS_INL_

#if (defined(MILTON_ARCH_WINDOWS) && defined(DEBUG))

#include <stdio.h>
#include <fcntl.h>
#include <io.h>
#include <iostream>
#include <fstream>

#ifndef _USE_OLD_IOSTREAMS
using namespace std;
#endif

// maximum mumber of lines the output console should have
static const WORD MAX_CONSOLE_LINES = 500;

static void s_redirect_windows_io_to_console()
{
   CONSOLE_SCREEN_BUFFER_INFO coninfo;
   long lStdHandle;
   int hConHandle;
   FILE *fp;
   
   // allocate a console for this app
   AllocConsole();
   
   // set the screen buffer to be big enough to let us scroll text
   GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &coninfo);
   
   coninfo.dwSize.Y = MAX_CONSOLE_LINES;
   SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), coninfo.dwSize);

   // redirect unbuffered STDOUT to the console
   lStdHandle = (long)GetStdHandle(STD_OUTPUT_HANDLE);
   hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
   
   fp = _fdopen(hConHandle, "w");
   *stdout = *fp;
   setvbuf(stdout, NULL, _IONBF, 0);
   
   // redirect unbuffered STDIN to the console
   lStdHandle = (long)GetStdHandle(STD_INPUT_HANDLE);
   hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
   
   fp = _fdopen(hConHandle, "r");
   *stdin = *fp;
   setvbuf(stdin, NULL, _IONBF, 0);
   
   // redirect unbuffered STDERR to the console
   lStdHandle = (long)GetStdHandle(STD_ERROR_HANDLE);
   hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
   fp = _fdopen(hConHandle, "w");
   
   *stderr = *fp;
   setvbuf(stderr, NULL, _IONBF, 0);
   
   // make cout, wcout, cin, wcin, wcerr, cerr, wclog and clog
   // point to console as well
   ios::sync_with_stdio();
}

#endif // MILTON_ARCH_WINDOWS && debug

#endif // MILTON_APP_WINDOWS_INL_

