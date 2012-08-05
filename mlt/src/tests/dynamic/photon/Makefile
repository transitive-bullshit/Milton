# @auth Travis Fischer
# @proj .make library Makefile
# @acct tfischer
# @date Spring 2008
# @site http://www.cs.brown.edu/people/tfischer/make
# @version 1.0

# README:
#    This main Makefile defines project-specific settings in order 
# to override the defaults contained in the .make Makefile subsystem.
# Take note of lines beginning with ## which may be uncommented and 
# changed. 
# 
# Note:  all project-specific variables are prefixed by PROJECT_


# Where to find the makefile sybsystem
# Note: you will need to change PROJECT_BASE_DIR if this Makefile is not 
# in the same folder as the '.make' library folder.
override PROJECT_BASE_DIR	= ../../..
override PROJECT_BASE_LIB	= $(PROJECT_BASE_DIR)/.make


# PROJECT_LANGUAGE
#    The language this project should use.
# 
# Supported Options: C|C++
# Default: C++
##PROJECT_LANGUAGE		= C++


# PROJECT_DEFAULT_MODE
#    The type of build to create (optimized or debug), when no override 
# is specified on the commandline via 'make MODE=DBG' or 'make MODE=OPT'.
# 
# Supported Options: DBG|OPT
# Default: DBG
##PROJECT_DEFAULT_MODE	= DBG


# PROJECT_PROFILE
#    Any non-empty value denotes that profiling should be enabled by default.
# 
# Supported Options: empty or non-empty
# Default: empty
##PROJECT_PROFILE			= 


# PROJECT_OUT_DIR
#    Path to a scratch directory where all intermediate files will be stored, 
# including object and dependency files.
# 
# Default: .bin
##PROJECT_OUT_DIR			= .bin


# PROJECT_TARGET
#    Main project target to produce (differs depending on PROJECT_TARGET_TYPE).
# 
#    If the project's target type is EXECUTABLE, PROJECT_TARGET refers to the 
# name of an executable binary file to be produced.
#    If the target type is ARCHIVE, PROJECT_TARGET refers to the name of the 
# archive to produce (generally of the form lib*.a).
#    If the target type is SHARED, PROJECT_TARGET refers to the name of the 
# shared library to produce (generally of the form lib*.so).
#    If the target type is HIERARCHY, PROJECT_TARGET is irrelevant and will be 
# ignored.
# 
# Default: the name of the current directory
##PROJECT_TARGET			=    $(shell basename `pwd`)# name of current directory
##PROJECT_TARGET			= lib$(shell basename `pwd`).a# example of static archive
PROJECT_TARGET			= lib$(shell basename `pwd`).so# example of shared obj library


# PROJECT_TARGET_TYPE
#    Describes the type of project this directory contains:
# 
# * EXECUTABLE : generate a binary executable file (default)
# * ARCHIVE    : generate a static archive 
# * SHARED     : generate a shared object library
# * HIERARCHY  : automatically define targets for and compile all 
#                subdirectories containing valid Makefiles
# 
# Note: HIERARCHY projects will search for files called 'Makefile' in all 
# subdirectories and recursively descend and compile those it finds (if 'all'
# is the implied or explicit target).  This includes Makefiles which are not 
# part of this build system.  It is perfectly fine and expected that you may 
# wish to use a different build system for some parts of a project.  To do so, 
# just create a subdirectory containing a valid Makefile like normal, and it 
# will be recognized and incorporated into the usual build system if a parent 
# HIERARCHY PROJECT_TARGET_TYPE exists.
# 
# Supported options: EXECUTABLE|ARCHIVE|SHARED|HIERARCHY
# Default: EXECUTABLE
PROJECT_TARGET_TYPE	= SHARED


# PROJECT_SRC_DIRS
#    List of directories to search for source files. Separate entries by 
# whitespace.
# 
# If 'ALL' is specified, all subdirectories (excluding those listed in 
# PROJECT_IGNORE_DIRS) will be searched.  This is typically the behavior that 
# you'll want.
# 
# Note: all sources found must have consistent endings (whether they 
# be h/H for headers or c/C/cpp/cc/etc for sources, they must be consistent 
# throughout) a project.
# 
# Default: ALL
##PROJECT_SRC_DIRS      = ALL


# PROJECT_IGNORE_DIRS
#    List of directories to exclude while searching for sources.
# 
# Default: $(PROJECT_OUT_DIR) .svn .cvs CVS .CVS .hg .git ..
##PROJECT_IGNORE_DIRS	= $(PROJECT_OUT_DIR) .svn .cvs CVS .CVS .hg .git ..


# Project-Specific Compilation Flags
PROJECT_CFLAGS	= -march=native -msse2

# Project-Specific Linking Flags
##PROJECT_LFLAGS	= 

# Debug/Optimized Mode specific Compilation Flags
##PROJECT_CFLAGS_DBG = 
##PROJECT_CFLAGS_OPT = 

# Debug/Optimized Mode specific Linking Flags
##PROJECT_LFLAGS_DBG = 
##PROJECT_LFLAGS_OPT = 


# PROJECT_INCPATH
#    Project-Specific Include Paths during compilation.
#
# Default: PROJECT_SRC_DIRS
PROJECT_INCPATH	= $(PROJECT_BASE_DIR)/milton


# PROJECT_LIBPATH
#    Project-Specific Library Paths during linking.
# 
# Note: the order of paths you specify will match the order in which the linker 
# will search for libraries.
# 
# Default: .
##PROJECT_LIBPATH	= $(PROJECT_BASE_DIR)/milton


# PROJECT_LIBS
#    Project-Specific Libraries.  '-l' will automatically be prepended onto 
# each library which doesn't already start with a '-l' before passing them to 
# the linker.
#
# Ex:  jpeg zip
# Default: none
##PROJECT_LIBS		= 


# PROJECT_QT_DIR
#    Should point to the directory where Qt was installed to.
# (containing the Qt 'bin', 'lib', and 'include' subdirectories)
# 
# Note: this variable is only relevant if you intend to use Qt.
# Default: none
PROJECT_QT_DIR	= /course/cs123/qt/


# Sanity-check PROJECT_BASE_DIR and PROJECT_BASE_LIB
$(if $(shell [ -d $(PROJECT_BASE_LIB) ] && echo "exists"),, 											  \
   $(shell "Could not find PROJECT_BASE_LIB '$(PROJECT_BASE_LIB)'") 									  \
   $(shell "You need to point PROJECT_BASE_DIR to the directory containing the .make library") \
   $(error "Invalid PROJECT_BASE_LIB"))

# Include the .make Makefile library (do not modify this)
include $(PROJECT_BASE_LIB)/defines.mk
include $(PROJECT_BASE_LIB)/targets.mk


# EXTRA_TARGETS
#    Extra rules dependent on PROJECT_TARGET, meant to allow for customized 
# manipulation of the main target after it has been generated.  You could, 
# for example, declare an 'install' target which is dependent on 
# PROJECT_TARGET and would get called every time PROJECT_TARGET was remade.
#
# Example:
#    EXTRA_TARGETS = install
#    
#    install:
#       mkdir release
#       tar -cvf release/$(PROJECT_TARGET).tar $(PROJECT_TARGET) $(PROJECT_SRC_DIRS)
#       cp $(PROJECT_TARGET) /usr/lib
# 
# Note: it is recommended that extra targets come at the end of this file, 
# specifically after including the .make library in order to assure that 'all'
# will still be the default target (since GNU make assigns the first target 
# it sees to be the default target).
# 
# Default: no extra targets defined
##EXTRA_TARGETS = 

