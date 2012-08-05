# @auth Travis Fischer
# @acct tfischer
# @date March 2008
# @site http://www.cs.brown.edu/people/tfischer/make
# @version 1.0
#
#    This file contains common variable declarations used by the 
# Makefile subsytem.
ifndef __COMMON_MK__
__COMMON_MK__ := # Guard against multiple includes


# Abstract out common UNIX programs
# ---------------------------------
SHELL				= sh
COPY				= cp -f
MKDIR		 		= /bin/mkdir -p
RM					= /bin/rm
CAT				= /bin/cat
SED				= sed
GREP				= grep -s
CHK_DIR_EXISTS = test -d
TR					= /usr/bin/tr
GDB				= /usr/bin/gdb
ARCHIVE			= /usr/bin/ar
ARCHIVE_FLAGS	= -cr


# Abstract out Qt Support
# -----------------------
PROJECT_QT_DIR?= /usr/local/qt
QT_DIR			= $(PROJECT_QT_DIR)
QT_BIN			= $(QT_DIR)/bin
QT_MKSPECS		= $(QT_DIR)/mkspecs
QT_LIB			= $(QT_DIR)/lib
QT_INCLUDE		= $(QT_DIR)/include


# Common Characters which must be encapsulated in variables to not confuse make
# -----------------------------------------------------------------------------
NULLSTRING 	  := 
SPACE			  := $(NULLSTRING) # Note: this extra space has to be here
TAB			  := $(NULLSTRING)	# Note: this extra tab has to be here
BACKSLASH	  := \\
NEWLINE		  := \n
COMMA			  := ,
OPEN_PAREN    := (
END_PAREN     := )
CONCAT_STR    := ____
POUND_SIGN	  := \#
EQUALS_SIGN	  := \=

TEMP_FILE		= $(PROJECT_BASE_LIB)/.tempfile
UNAME				= $(shell uname)

ifeq ($(UNAME),Linux)
   OS = linux
else
   ifeq ($(UNAME),Darwin)
      OS = osx
   else
      ifneq ($(OS),wince)
         OS = win32
      endif
   endif
endif

endif # __COMMON_MK__

