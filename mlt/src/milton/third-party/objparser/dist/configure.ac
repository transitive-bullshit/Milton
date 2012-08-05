AC_PREREQ(2.60)
AC_INIT([obj],[0.1],[ares.lagae@cs.kuleuven.be])
AC_CONFIG_SRCDIR([obj])
AC_CONFIG_AUX_DIR(config)
AC_CONFIG_HEADER([config.h])
AX_PREFIX_CONFIG_H(obj/config.hpp)
AC_DEFINE([PACKAGE_AUTHOR], ["Ares Lagae"], [Author of package])
AM_INIT_AUTOMAKE
AM_SANITY_CHECK
AC_LANG([C++])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL
AM_PROG_LIBTOOL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_RANLIB

# Checks for libraries.



# Checks for header files.

AC_CHECK_HEADER([tr1/functional], [AC_DEFINE([HAVE_TR1_FUNCTIONAL], [1], [Define to 1 if you have <tr1/functional>.])], [AC_MSG_ERROR([Sorry, can't do anything for you])])
AC_CHECK_HEADER([tr1/tuple], [AC_DEFINE([HAVE_TR1_TUPLE], [1], [Define to 1 if you have <tr1/tuple>.])], [AC_MSG_ERROR([Sorry, can't do anything for you])])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_ERROR_AT_LINE

AC_CONFIG_FILES([Makefile obj/Makefile tools/Makefile doc/Makefile doc/Doxyfile obj.pc])
AC_OUTPUT
