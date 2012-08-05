# @auth Travis Fischer
# @proj .make library Makefile
# @acct tfischer
# @date Spring 2008
# @site http://www.cs.brown.edu/people/tfischer/make
# @version 1.0

UNAME = $(shell uname)

ifeq ($(UNAME),Linux)
   OS = linux
else
   ifeq ($(UNAME),Darwin)
      OS = osx
   else
      ifneq ($(OS),wince)
         OS = win
      endif
   endif
endif

ifeq ($(OS),win)
include Makefile.windows.mk
else
include Makefile.linux.mk
endif

