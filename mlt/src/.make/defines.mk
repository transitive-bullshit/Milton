# @auth Travis Fischer
# @acct tfischer
# @date March 2008
# @site http://www.cs.brown.edu/people/tfischer/make
# @version 1.0
#
#    This file contains variable and function definitions used by the 
# Makefile subsystem
ifndef __DEFINES_MK__
__DEFINES_MK__ := # Guard against multiple includes

include $(PROJECT_BASE_LIB)/functions.mk

# Defaults used by the Makefile System
# ------------------------------------
#DEFAULT_LANGUAGE		?= C++
#DEFAULT_MODE			?= DBG
#DEFAULT_PROFILE		?= 
#DEFAULT_OUT_DIR		?= .bin
#DEFAULT_TARGET		?= $(shell basename `pwd`)
#DEFAULT_TARGET_TYPE	?= EXECUTABLE
#DEFAULT_SRC_DIRS		?= ALL
DEFAULT_IGNORE_DIRS ?= $(OUT_DIR) .svn .cvs CVS .CVS .hg .git ..


# Acceptable options for public variables which rely on specific values
# ---------------------------------------------------------------------
LANGUAGE_OPTIONS 		:= C++ C
MODE_OPTIONS 			:= DBG OPT
TARGET_TYPE_OPTIONS	:= EXECUTABLE ARCHIVE SHARED HIERARCHY


# Merge defaults with project-specific settings
# ---------------------------------------------
define defineCustomizableVariable
	$(eval DEFAULT_$(1)		?= $(2))
   $(eval PROJECT_$(1)		?= $(DEFAULT_$(1)))
   $(eval $(1)					?= $(PROJECT_$(1)))
   
   # Convert variable value to lowercase
   $(eval STR_$(1)			:= $(call CONVERT_TO_LOWERCASE,$($(1))))
   
   $(eval $(1)_OPTIONS		?= $($(1)))
   $(eval STR_$(1)_OPTIONS	:= $(subst $(SPACE),|,$($(1)_OPTIONS)))
   
   # Ensure public variables take on valid values
   $(if $(or (findstring $($(1)),$($(1)_OPTIONS)),	\
				$(call EQUAL,$($(1)_OPTIONS,))),,		\ # else
      $(error $(1) = '$($(1))' must be one of $(STR_$(1)_OPTIONS)))
endef

$(eval $(call defineCustomizableVariable,LANGUAGE,C++))
$(eval $(call defineCustomizableVariable,MODE,DBG))
$(eval $(call defineCustomizableVariable,PROFILE,))
$(eval $(call defineCustomizableVariable,OUT_DIR,.bin))
$(eval $(call defineCustomizableVariable,TARGET,$(shell basename `pwd`)))
$(eval $(call defineCustomizableVariable,TARGET_TYPE,EXECUTABLE))
$(eval $(call defineCustomizableVariable,SRC_DIRS,ALL))
$(eval $(call defineCustomizableVariable,IGNORE_DIRS,$(DEFAULT_IGNORE_DIRS)))


export MODE

ifeq ($(PROJECT_SRC_DIRS),ALL)
   SRC_DIRS = $(call BFS,GET_SUB_DIRS,.,$(IGNORE_DIRS))
#$(info "Including all subdirs: $(SRC_DIRS)")
endif

# Define Language Suffixes
# ------------------------
FIND_SUFFIXES = $(if $(or $(call STORE,RESULT_,),$(and $(foreach DIR,$2,\
   $(strip $(foreach OPT,$1,$(call STORE,RESULT2_,\
	$(if $(wildcard $(DIR)/*.$(OPT)),$(OPT)))\
	$(if $(RESULT2_),\
	$(if $(and $(RESULT_),$(call NOTEQUAL,$(RESULT2_),$(RESULT_))),\
	$(error More than one source type found: '$(RESULT2_)' '$(RESULT_)'))\
	$(call STORE,RESULT_,$(RESULT2_)))))),$(RESULT_))),$(RESULT_),$(firstword $1))

C_SUFFIXES		:= c h o
C++_SUFFIXES	:= \
   $(call FIND_SUFFIXES,C cpp cc c,$(SRC_DIRS)) $(call FIND_SUFFIXES,h H,$(SRC_DIRS)) o
.SUFFIXES		:= $($(LANGUAGE)_SUFFIXES)

#$(info $(C++_SUFFIXES))

define initSuffixes
   SOURCE_SUFFIX	:= $$(word 1,$(1))
   HEADER_SUFFIX	:= $$(word 2,$(1))
   OBJ_SUFFIX		:= $$(word 3,$(1))
endef

$(eval $(call initSuffixes,$(.SUFFIXES)))


# Common Defines
# --------------
ARCH				:= $(shell uname -m)
OBJ_DIR			:= $(OUT_DIR)/$(STR_MODE)/objects/$(ARCH)
OBJ_DIR_ESC		:= $(call ESCAPE,$(OBJ_DIR))
ALL_DIR			:= all/$(1)/

CLEAN				:= $(call CONTAINS,clean,$(MAKECMDGOALS))
ONLYCLEAN		:= $(call EQUAL,clean,$(MAKECMDGOALS))

# Note: it is important to have MODE-specific dependency files for several reasons:
#    * the source could include different files dependending on MODE-specific #ifdefs
#	  * automatically generated dependency files have to point at the correct MODE 
#	    targets (generally .o files)
DEP_SUFFIX		:= depend
DEP_DIR			:= $(OUT_DIR)/$(STR_MODE)/$(DEP_SUFFIX)
MAKEFILE			:= Makefile
DEP_MAKEFILE	:= $(DEP_DIR)/$(MAKEFILE).$(DEP_SUFFIX)

SETTINGS			:= $(MODE) $(OUT_DIR) $(DEP_DIR)
SETTINGS_FILE	:= $(OUT_DIR)/settings
SETTINGS_CHANGED := $(if $(and $(call NOT,$(CLEAN)),\
						  $(or $(call DOES_NOT_EXIST,$(SETTINGS_FILE)),\
						  $(call NOTEQUAL,$(shell cat $(SETTINGS_FILE)),$(SETTINGS)))),\
						     $(and $(shell $(RM) -f $(SETTINGS_FILE)),CHANGED))

SOURCES			:= $(strip $(call CLEAN_PATH, \
	$(foreach DIR,$(SRC_DIRS),$(wildcard $(DIR)/*.$(SOURCE_SUFFIX)))))
HEADERS			:= $(SOURCES:%.$(SOURCE_SUFFIX)=%.$(HEADER_SUFFIX))

STRIPPED_HEADERS = $(foreach HEADER,$(HEADERS),$(notdir $(HEADER)))


# Compiler-Specific Defines
# -------------------------
COMPILER_C		 = /usr/bin/gcc
COMPILER_C++	 = /usr/bin/g++
COMPILER			:= $(COMPILER_$(PROJECT_LANGUAGE))
LINKER			:= $(COMPILER)

ifneq ($(PROFILE),)
   ifeq ($(MODE),DBG)
      PROFILE_FLAGS	?= -pg
   endif
endif


# Setup default compiler flags
# ----------------------------
COMPILE_FLAGS_DBG	:= -g -O0 -ggdb -DDEBUG
COMPILE_FLAGS_OPT	:= \
	-O3 -DNDEBUG \
	$(addprefix -f,reorder-blocks omit-frame-pointer) \
	$(addprefix -m,arch=native sse2 fpmath=sse inline-all-stringops)
#fast-math

#-O2 -DNDEBUG
COMPILE_FLAGS		:= -pipe -Wall # default compiler flags
COMPILE_FLAGS		+= $(PROJECT_CFLAGS) $(PROJECT_CFLAGS_$(MODE))	\
							$(PROFILE_FLAGS) $(COMPILE_FLAGS_$(MODE))
COMPILE_FLAGS		:= $(strip $(COMPILE_FLAGS))

LINK_FLAGS_DBG		:= -g
LINK_FLAGS_OPT		:= 
LINK_FLAGS			:= # default linker flags
LINK_FLAGS			+= $(PROJECT_LFLAGS) $(PROJECT_LFLAGS_$(MODE))	\
							$(LINK_FLAGS_$(MODE))
LINK_FLAGS			:= $(strip $(LINK_FLAGS))

ifeq ($(TARGET_TYPE),SHARED)
   SHARED_OFLAGS	:= -fPIC -shared
   COMPILE_FLAGS	+= $(SHARED_OFLAGS)
   LINK_FLAGS		+= $(SHARED_OFLAGS)
endif

PROJECT_INCPATH  ?=
PROJECT_LIBPATH  ?=

PROJECT_INCPATH  := $(SRC_DIRS) $(PROJECT_INCPATH)

# Add -I to every path in $(PROJECT_INCPATH)
INCLUDE_PATH	  := $(shell echo '$(strip $(call ADD_UNIQUE_PREFIX,$(PROJECT_INCPATH),-I))' | sed 's/-I\.\/\([^/]\)/-I\1/g')

# Add -L to every path in $(PROJECT_LIBPATH)
# Add -l to every lib  in $(PROJECT_LIBS)
LIBRARIES		  := $(strip $(call ADD_UNIQUE_PREFIX,$(PROJECT_LIBPATH),-L) \
						     $(call ADD_UNIQUE_PREFIX,$(PROJECT_LIBS),-l))

ifneq ("$(PROJECT_LIBPATH)","")
ifneq ($(TARGET_TYPE),ARCHIVE)
   LIBRARIES	  += $(foreach LIB,$(PROJECT_LIBPATH),-Wl$(COMMA)-rpath$(COMMA)$(LIB))
endif
endif


# Miscellaneous Defines
# ---------------------
STR_DBG	= debug
STR_OPT	= optimized
STR_MODE	= $(STR_$(MODE))

MAKE_TARGET	:= MAKE_$(TARGET_TYPE)

CLEAN_CMD	 = $(filter-out $(BACKSLASH) $(NEWLINE) $(TAB),$1)

# $(call ENSURE_ALL_DIR_EXISTS)
#    Creates all Makefile-specific directories if they do not already exist.
#
# @returns void
define ENSURE_ALL_DIR_EXISTS
	$(foreach DIR,$(SRC_DIRS),$(call ENSURE_DIR_EXISTS_SHELL,$(OBJ_DIR)/$(DIR)) \
									  $(call ENSURE_DIR_EXISTS_SHELL,$(DEP_DIR)/$(DIR)))
endef


# Assertions pertaining to valid input from the public interface
# --------------------------------------------------------------
SUB_PROJECTS = $(GET_PROJECT_SUBDIRS)

ifeq ($(CLEAN),)
ifeq ($(PROJECT_TARGET_TYPE),HIERARCHY)
   STATUS := $(call ASSERT,$(SUB_PROJECTS),"No subprojects found in this \
$(PROJECT_TARGET_TYPE) project!")
else 
ifneq ($(PROJECT_TARGET_TYPE),ARCHIVE)
   STATUS := $(call ASSERT,$(SOURCES),"No source files found in $(CURDIR).\
$(if $(SUB_PROJECTS),$(SPACE)You probably want to change PROJECT_TARGET_TYPE \
to HIERARCHY in your Makefile)")
endif
endif
endif

#$(info SUB=$(SUB_PROJECTS))
#$$(info ADD_PROJECT: '$(call TARGET_IS_UP_TO_DATE,,$1)')

define ADD_PROJECT
# ensure all is first target!
all: $(if $(call TARGET_IS_UP_TO_DATE,,$1),,$1)

# Main rule to recursively make subdirectory
$(call RECURSIVE_MAKE,$1)

clean-$1:
	+@$$(MAKE) -C $1 clean --no-print-directory

.PHONY: $1 clean-$1
clean: clean-$1
endef

POSSIBLE_LIBS = $(if $(call NOTEQUAL,$1,$(patsubst lib%.so,%,$1)),$1 a,	\
					 $(if $(call NOTEQUAL,$1,$(patsubst lib%.a,%,$1)),$1 b,	\
					 $(if $(call NOTEQUAL,$1,$(patsubst %.so,%,$1)),$1 lib$1,\
					 $(if $(call NOTEQUAL,$1,$(patsubst %.a,%,$1)),$1 lib$1,	\
					 $1 lib$1 lib$1.so lib$1.a))))

#$(info stop_rec='$(STOP_RECURSION)')

ifeq ($(CLEAN),)
ifndef STOP_RECURSION
AVAILABLE_TARGETS = $(call GET_LIST_OF_TARGETS)

#$(info AVAILABLE='$(AVAILABLE_TARGETS)')
endif # STOP_RECURSION
endif # CLEAN

#$(info mkrestarts=$(MAKE_RESTARTS))


include $(PROJECT_BASE_LIB)/libraries.mk

# Note: it is important that these definitions come after including libraries.mk
OFILES			:= $(SOURCES:%.$(SOURCE_SUFFIX)=$(OBJ_DIR)/%.$(OBJ_SUFFIX))
DFILES			:= $(SOURCES:%.$(SOURCE_SUFFIX)=$(DEP_DIR)/%.$(DEP_SUFFIX))

$(foreach OBJ,$(OFILES),\
	$(eval $(OBJ)_INCLUDE_PATH  = $(INCLUDE_PATH)) \
	$(eval $(OBJ)_COMPILE_FLAGS = $(COMPILE_FLAGS)))

# Descriptions for each of the available TARGET_TYPEs, to be displayed 
# in the 'make help' target
EXECUTABLE_TYPE_DESCRIPTION := "This project/directory is setup to create an executable: '$(FR_COLOR_BLUE)$(TARGET)$(END_COLOR)'"
ARCHIVE_TYPE_DESCRIPTION 	 := "This project/directory is setup to create a static archive library: '$(FR_COLOR_BLUE)$(TARGET)$(END_COLOR)'"
SHARED_TYPE_DESCRIPTION 	 := "This project/directory is setup to create a shared library: '$(FR_COLOR_BLUE)$(TARGET)$(END_COLOR)'"
HIERARCHY_TYPE_DESCRIPTION	 := "This project/directory is setup to batch compile multiple subprojects: '$(FR_COLOR_BLUE)$(SUB_PROJECTS)$(END_COLOR)'"

endif # __DEFINES_MK__

