lib_LTLIBRARIES = libobj.la
libobj_la_SOURCES = ../obj.hpp obj.hpp obj.cpp obj_parser.hpp obj_parser.cpp config.hpp
INCLUDES = -I$(top_srcdir)
library_includedir = $(includedir)/obj
library_include_HEADERS = obj.hpp obj_parser.hpp config.hpp
libobj_la_DEPENDENCIES = ../obj.hpp
AM_CXXFLAGS = $(BOOST_CPPFLAGS) -Wall -ansi -pedantic