# @auth Travis Fischer
# @acct tfischer
# @date March 2008
# @site http://www.cs.brown.edu/people/tfischer/make
# @version 1.0
# 
#    This file contains function for automatically detecting library 
# dependencies and adding them to the linking phase.
#
# NOTE: This file is based on icompile source code written by Morgan McGuire
# See http://ice.sf.net for more info
ifndef __LIBRARIES_MK__
__LIBRARIES_MK__ := # Guard against multiple includes
# Included from defines.mk

# Canonical-name Type Release Debug F.Release F.Debug Header-list Symbol-list Depends-on
LIB_NAME			= 2
LIB_TYPE			= 3
LIB_RELEASE		= 4
LIB_DEBUG		= 5
LIB_F_RELEASE	= 6
LIB_F_DEBUG		= 7
LIB_HEADERS		= 8
LIB_SYMBOLS		= 9
LIB_DEPENDS_ON	= 10

# On non-OSX UNIX systems, G3D needs X11. On OSX, GL is a framework
ifeq ($(OS),osx)
   MAYBEFRAMEWORK	= FRAMEWORK
   MAYBEG3DX11		= 
else # not osx
   MAYBEFRAMEWORK	= DYNAMIC
   MAYBEG3DX11		= X11
endif

QT_HEADERS := $(shell $(CAT) $(PROJECT_BASE_LIB)/qtIncludes)
QT_HEADERS += $(addprefix Qt/,$(QT_HEADERS))

LIBS_	:= $(shell $(GREP) '^[ 	]*[^$(POUND_SIGN)]' $(PROJECT_BASE_LIB)/libList | \
	$(SED) -e 's|QT_HEADERS|$(QT_HEADERS)|1' -e 's| |$(CONCAT_STR)|g' -e 's|\n| |')

PROCESS_LIB = $(sort $(foreach HEADER_,$(subst $(CONCAT_STR),$(SPACE),$($(LIB_HEADERS))),\
	$(if $(call EQUAL,$1,$(HEADER_)),$(eval $($(LIB_NAME))_USE_$(notdir $1) = true)$($(LIB_NAME)))))

# TODO: assure that lib dependencies are respected (library order matters!)
SORT_LIBS = $1
#$(foreach LIB1,$1,$(foreach LIB2,$1,$(if $(call NOTEQUAL,$(LIB1),$(LIB2)))))
#$(info 1'$1' 2'$2' 8'$8')

#$(info INCL='$(call BFS,EXTRACT_INCLUDES,Gui.cpp)')

$(foreach SOURCE,$(SOURCES),$(eval $$(SOURCE:%.$$(SOURCE_SUFFIX)=%)_LIBS := \
	$$(call SORT_LIBS,$$(strip \
	$$(foreach INCL,$$(call BFS,EXTRACT_INCLUDES,$$(SOURCE)),\
	$(foreach LIB_,$(LIBS_),$$(call PROCESS_LIB,$$(INCL),$(LIB_))))))))

#$(foreach SOURCE,$(SOURCES),$(foreach INCL,$(call BFS,EXTRACT_INCLUDES,$(SOURCE)),\
#	$(foreach LIB_,$(LIBS_),$(or $(info ),$(info $(LIB_))))))

# Display the detected library dependencies for each source file
#$(foreach SOURCE,$(SOURCES),$(info $(SOURCE:%.$(SOURCE_SUFFIX)=%)_LIBS = '$($(SOURCE:%.$(SOURCE_SUFFIX)=%)_LIBS)'))

define QT_ACTION
   QT_SOURCE_FILES += $1
   USE_QT  = true
endef

# Perform any library-specific actions for those libraries which are included
$(foreach SOURCE,$(SOURCES),$(if $($(SOURCE:%.$(SOURCE_SUFFIX)=%)_LIBS),\
	$(foreach LIB,$($(SOURCE:%.$(SOURCE_SUFFIX)=%)_LIBS),						\
		$(if $($(LIB)_ACTION), 															\
			$(eval $(call $(LIB)_ACTION,$(SOURCE)))))))


# TODO: only include QtOpenGL if it is referenced!
ifeq ($(USE_QT),true)
   QT_MOC_PREFIX	= moc_
   QT_OUT_DIR		= $(OBJ_DIR)
   
   QT_CONVERT_SOURCE_TO_MOC	= $(call CLEAN_PATH,$(QT_OUT_DIR)/$(dir $1)/$(QT_MOC_PREFIX)$(notdir $1))
   
   QT_BASES 	= $(patsubst %.$(HEADER_SUFFIX),%,$(call EXTRACT_QT_OBJECTS,$(HEADERS)))
   # sources which must be mocced
   QT_SOURCES	= $(addsuffix .$(SOURCE_SUFFIX),$(QT_BASES))
   # moc_*.cpp versions
   QT_MOCS		= $(strip $(if $(QT_SOURCES), \
		$(foreach SOURCE,$(QT_SOURCES),$(call QT_CONVERT_SOURCE_TO_MOC,$(SOURCE)))))
   QT_OBJECTS	= $(QT_MOCS:%.$(SOURCE_SUFFIX)=%.$(OBJ_SUFFIX))
   
   QT_OBJECT_FILES = $(addprefix $(QT_OUT_DIR)/,$(patsubst %.$(SOURCE_SUFFIX),%.$(OBJ_SUFFIX),$(QT_SOURCE_FILES)))
#$(QT_OBJECTS)
   
   QT_DEFINES			= -DQT_NO_DEBUG 
   QT_INCLUDE_PATH	= $(QT_MKSPECS)/default $(QT_INCLUDE)
   QT_LIB_PATH			= $(QT_LIB) /usr/X11R6/lib $(QT_DIR)/plugins/imageformats
   QT_LIBS				= 
   
   # Qt Module Support
   # -----------------
   QT_USE_QtCore_DEFINES 			= -DQT_CORE_LIB
   QT_USE_QtCore_INCLUDE_PATH		= $(QT_INCLUDE)/QtCore
   QT_USE_QtCore_LIBS 				= QtCore z m gthread-2.0 glib-2.0 rt dl
   
   QT_USE_QtGui_DEFINES 			= -DQT_GUI_LIB -D_REENTRANT
   QT_USE_QtGui_INCLUDE_PATH		= $(QT_INCLUDE)/QtGui /usr/X11R6/include
   QT_USE_QtGui_LIBS 				= QtGui jpeg tiff qjpeg qtiff png pthread SM ICE Xi Xrender Xrandr Xfixes Xcursor Xinerama freetype fontconfig Xext X11 
   QT_USE_QtGui_DEPEND				= QT_USE_QtCore
   
   QT_USE_QtOpenGL_DEFINES 		= -DQT_OPENGL_LIB
   QT_USE_QtOpenGL_INCLUDE_PATH	= $(QT_INCLUDE)/QtOpenGL
   QT_USE_QtOpenGL_LIBS 			= QtOpenGL GLU GL
   QT_USE_QtOpenGL_DEPEND			= QT_USE_QtGui
   
   # Common Qt Headers which require certain modules
   QT_USE_QApplication_DEPEND		= QT_USE_QtGui
   QT_USE_QImage_DEPEND				= QT_USE_QtGui
   QT_USE_QString_DEPEND			= QT_USE_QtCore
   
   # Define all Modules
   DEFINE_MODULE = $($1_DEPEND)

   # Real modules defined by Qt and supported by this Makefile system
	# Note: the order that Qt module libraries are passed to the linker is 
	# very important.  QtGui must come before QtCore!
   QT_REAL_MODULES					=	QT_USE_QtOpenGL		\
												QT_USE_QtGui			\
												QT_USE_QtCore
   
	# Phony modules which exist only to indicate the use of real modules
   QT_PHONY_MODULES					=	QT_USE_QApplication	\
												QT_USE_QImage			\
												QT_USE_QString
   
   # All real/phony Qt modules supported
   QT_ALL_MODULES						=	$(QT_REAL_MODULES)	\
												$(QT_PHONY_MODULES)
	
   # Real/Phony modules actually used directly or indirectly in this project
   QT_USED_MODULES					= \
		$(call BFS,DEFINE_MODULE,$(foreach MODULE,$(QT_ALL_MODULES),\
			$(if $(call EQUAL,true,$($(MODULE))),$(MODULE))))
   
   $(foreach MODULE,$(QT_REAL_MODULES),\
		$(foreach DEP,$($(MODULE)_DEPEND),\
			$(if $(call PRECEDES,$(DEP),$(MODULE),$(QT_USED_MODULES)),\
				$(eval QT_USED_MODULES := \
				$(call REMOVE_ALL,$(DEP),$(QT_USED_MODULES)) $(DEP)))))
   
   # Real Qt modules actually used directly or indirectly in this project
   QT_REAL_USED_MODULES				= $(patsubst QT_USE_%,%, \
		$(call REMOVE_ALL,$(QT_PHONY_MODULES),$(QT_USED_MODULES)))
   
   # Debugging info
   #$(info QT::BASES '$(QT_BASES)')
   #$(info QT::SOURCES '$(QT_SOURCES)')
   #$(info QT::MOCS '$(QT_MOCS)')
   #$(info QT::SOURCE_FILES '$(QT_SOURCE_FILES)')
   #$(info QT::OBJECT_FILES '$(QT_OBJECT_FILES)')
   #$(info QT::USED_MODULES '$(QT_USED_MODULES)')
   
   $(foreach MODULE,$(QT_USED_MODULES),							\
		$(eval QT_DEFINES 		+= $($(MODULE)_DEFINES))		\
		$(eval QT_INCLUDE_PATH 	+= $($(MODULE)_INCLUDE_PATH))	\
		$(eval QT_LIB_PATH 		+= $($(MODULE)_LIB_PATH))		\
		$(eval QT_LIBS		 		+= $($(MODULE)_LIBS)))
	
   QT_DEFINES		  := $(strip $(QT_DEFINES))
   QT_INCLUDE_PATH  := $(strip $(QT_INCLUDE_PATH))
   QT_LIB_PATH		  := $(strip $(QT_LIB_PATH))
   QT_LIBS			  := $(strip $(QT_LIBS))
   
   
   QT_INCLUDE_PATH  := $(strip $(INCLUDE_PATH) 										\
										 $(call ADD_UNIQUE_PREFIX,$(QT_INCLUDE_PATH),-I))
   
   QT_LIBRARIES     := $(strip $(LIBRARIES)											\
										 $(call ADD_UNIQUE_PREFIX,$(QT_LIB_PATH),-L) \
									    $(call ADD_UNIQUE_PREFIX,$(QT_LIBS),-l))
   
   QT_LINK_FLAGS	  := -Wl,-rpath,$(QT_LIB) $(LINK_FLAGS)
   QT_COMPILE_FLAGS := $(QT_DEFINES) $(COMPILE_FLAGS)
   
   override LINK_FLAGS := $(QT_LINK_FLAGS)
   override LIBRARIES  := $(QT_LIBRARIES)
   
# Notify the user that Qt is being used, as well as the specific modules
# Don't print notification if --question or -q flags are used to question if 
# this all targets are up-to-date
ifeq ($(call CONTAINS,q,$(MAKEFLAGS)),)
.PHONY: QT_NOTICE
all: QT_NOTICE ;
endif

   SRC_DIRS += $(QT_OUT_DIR)
   SOURCES	+= $(QT_MOCS)
   OFILES	+= $(QT_OBJECTS)
endif

endif # __LIBRARIES_MK__

