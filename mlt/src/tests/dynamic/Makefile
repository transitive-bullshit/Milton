
GCC          = gcc
GXX          = g++

MILTON_BASE  = ../..
QT_BASE      = /course/cs123/qt

QT_FLAGS     = -DQT_NO_DEBUG -DQT_OPENGL_LIB -DQT_GUI_LIB -D_REENTRANT    \
               -DQT_CORE_LIB
FLAGS        = -pipe -Wall -march=native -msse2 -g -O2 -DDEBUG $(QT_FLAGS)
LINK_FLAGS   = -g -Wl,-rpath,$(QT_BASE)/lib -Wl,-rpath,$(MILTON_BASE)/milton

INCLUDE_PATH = $(addprefix -I, . $(MILTON_BASE)/milton $(MILTON_BASE)/gui \
                 $(QT_BASE)/mkspecs/default $(QT_BASE)/include            \
                 $(QT_BASE)/include/QtOpenGL $(QT_BASE)/include/QtGui     \
					  /usr/X11R6/include $(QT_BASE)/include/QtCore)

LINK_PATH    = $(addprefix -L, . $(MILTON_BASE)/milton $(MILTON_BASE)/gui \
                 $(QT_BASE)/lib /usr/X11R6/lib $(QT_BASE)/plugins/imageformats)
LIBS         = $(addprefix -l, \
                 gui milton glut QtOpenGL GLU GL QtGui jpeg tiff qjpeg    \
					  qtiff png pthread SM ICE Xi Xrender Xrandr Xfixes Xcursor\
					  Xinerama freetype fontconfig Xext X11                    \
					  QtCore z m gthread-2.0 glib-2.0 rt dl)

PRE          = 
POST         = 

TARGET_BASE  = $(shell basename `pwd`)
TARGET       = $(PRE)$(TARGET_BASE)$(POST)
OBJ_DIR      = .obj

SOURCES = $(wildcard *.cpp)
HEADERS = $(SOURCES:%.cpp=%.h)
OBJECTS = $(addprefix $(OBJ_DIR)/,$(SOURCES:%.cpp=%.o))

# ----------------------------------------------------------------------------
# ----------------------------------------------------------------------------

.PHONY: all tidy clean install

all: $(TARGET)

$(TARGET): $(OBJECTS)
	@echo
	$(GXX) -o $@ $(OBJECTS) $(LINK_FLAGS) $(LINK_PATH) $(LIBS)

$(OBJ_DIR)/%.o: %.cpp
	@-mkdir -p $(OBJ_DIR)
	$(GXX) -o $@ -c $? \
	$(FLAGS) \
	$(INCLUDE_PATH)

%.cpp: %.h ;

tidy:
	-rm -rf $(OBJ_DIR)

clean: tidy
	-rm -f $(TARGET)

