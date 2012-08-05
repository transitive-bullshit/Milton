# Windows-specific Makefile for core Milton library
# -------------------------------------------------

# comment the following line to enable optimizations and disable assertions
# (defaults to a debug build)
DEBUG        = true

GCC          = gcc
GXX          = ./g++.exe
#/cygdrive/c/MinGW/bin/g++.exe

ifeq ($(QTDIR),)
QTDIR=C:\Qt\4.5.1
endif

QT_BASE      = $(QTDIR)
#/course/cs123/qt
QT_FLAGS     = -DQT_NO_DEBUG -DQT_OPENGL_LIB -DQT_GUI_LIB -D_REENTRANT    \
               -DQT_CORE_LIB

FLAGS        = -pipe -Wall -g $(QT_FLAGS)
LINK_FLAGS   = -g

ifeq ($(DEBUG),true)
   FLAGS       += -O0 -DDEBUG
   LINK_FLAGS  += 
else
   FLAGS       += -O3 -DNDEBUG \
   $(addprefix -f,reorder-blocks omit-frame-pointer) \
   $(addprefix -m,arch=native fpmath=sse inline-all-stringops)
   LINK_FLAGS  += 
endif

PRE          = lib
POST         = .dll
OBJ_DIR      = .obj

SOURCES  = $(subst ./,,$(shell find . \( -name "*.cpp" \) | grep -v "third-party/misc\|third-party.*dist"))
HEADERS  = $(subst ./,,$(shell find . \( -name "*.h"   \) | grep -v "third-party/misc\|third-party.*dist"))
OBJECTS  = $(addprefix $(OBJ_DIR)/,$(SOURCES:%.cpp=%.o))

SRC_DIRS = $(sort $(subst / , ,$(dir $(SOURCES) $(HEADERS))))

INCLUDE_PATH = $(addprefix -I, . $(SRC_DIRS)                              \
                 $(QT_BASE)/mkspecs/default $(QT_BASE)/include            \
                 $(QT_BASE)/include/QtOpenGL $(QT_BASE)/include/QtGui     \
                 /usr/X11R6/include $(QT_BASE)/include/QtCore             \
					  C:/lib/boost_1_39_0 C:\lib\glut\include)

LINK_PATH    = $(addprefix -L, . $(QT_BASE)/lib                           \
                 /usr/X11R6/lib $(QT_BASE)/plugins/imageformats)
LIBS         = $(addprefix -l, \
                 glut QtOpenGL GLU GL QtGui jpeg tiff qjpeg    \
					  qtiff png pthread SM ICE Xi Xrender Xrandr Xfixes Xcursor\
					  Xinerama freetype fontconfig Xext X11                    \
					  QtCore z m gthread-2.0 glib-2.0 rt dl)

PWD = $(shell pwd)
TARGET_BASE  = $(shell basename '$(PWD)')
TARGET       = $(PRE)$(TARGET_BASE)$(POST)


# ----------------------------------------------------------------------------
# ----------------------------------------------------------------------------


.PHONY: all tidy clean

all: $(TARGET)

$(TARGET): $(OBJECTS)
	@echo
	$(GXX) -o $@ $(OBJECTS) $(LINK_FLAGS) $(LINK_PATH) $(LIBS)

#$(info $(foreach DIR,$(SRC_DIRS),$(OBJ_DIR)/$(DIR)%.o))

$(OBJ_DIR)/%.o : %.cpp
	@-mkdir -p $(dir $(OBJECTS))
	@echo
	$(GXX) -o $@ -c $? $(FLAGS) $(INCLUDE_PATH)

%.cpp: %.h

%.h: ;

tidy:
	-rm -rf $(OBJ_DIR)

clean: tidy
	-rm -f $(TARGET)

