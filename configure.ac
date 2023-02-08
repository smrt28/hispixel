# initialize autoconf
AC_INIT([hispixel],[1.1.0],[ondrej.holecek@gmail.com])
#VERSION_INFO="-version-info 0:0:0"

# install autotools scripts to build-aux dir
AC_CONFIG_AUX_DIR([build-aux])

# macros from http://www.gnu.org/software/autoconf-archive/The-Macros.html
AC_CONFIG_MACRO_DIR([m4])

# initialize automake(use AC_INIT's arguments)
AM_INIT_AUTOMAKE([-Wall foreign])

# Enable silent builds (disable with --disable-silent-rules)
# or simply do "make V=1"
AM_SILENT_RULES([yes])

# tell autoconf where to find sources
AC_CONFIG_SRCDIR([src/main.cc])
AC_CONFIG_HEADERS([config.h])

dnl This forces debian autoconf to use version 2.53
AC_PREREQ([2.71])

# disable so
AC_DISABLE_SHARED

# initialize libtool
LT_INIT

# Add a test for a compiler.
AC_PROG_CC
AC_PROG_CXX

# replace autotools original defaults
CXXFLAGS="-W -Wall -Wextra -Wconversion"


AC_SUBST(MALLOC_LIB)

PKG_PROG_PKG_CONFIG


HISPIXEL_DEPS="gtk+-3.0 >= 3.0.0, vte-2.91, gio-2.0"
PKG_CHECK_MODULES([HISPIXEL], [${HISPIXEL_DEPS}])

AX_BOOST_BASE([1.50], , [AC_MSG_ERROR([boost libraries missing])])
AX_BOOST_IOSTREAMS


#check boost system
AC_LANG([C++])
AC_CHECK_HEADER([boost/system/error_code.hpp], [], AC_MSG_ERROR([boost system headers is missing], 1))                                     
AC_CHECK_LIB(boost_system-mt, main, , [
    AC_CHECK_LIB(boost_system, main, , [                                                                                                   
        AC_MSG_ERROR([boost system library is missing], 1)])])

# check for boost threads
AC_LANG([C++])
AC_CHECK_HEADER([boost/thread.hpp], [], AC_MSG_ERROR([boost thread headers missing], 1))
AC_CHECK_LIB(boost_thread-mt, main, , [ 
    AC_CHECK_LIB(boost_thread, main, , [ 
        AC_MSG_ERROR([boost thread library missing], 1)])])

# check for boost threads
AC_LANG([C++])
AC_CHECK_HEADER([boost/program_options.hpp], [], AC_MSG_ERROR([boost program_options missing], 1))
AC_CHECK_LIB(boost_program_options-mt, main, , [ 
    AC_CHECK_LIB(boost_program_options, main, , [ 
        AC_MSG_ERROR([boost program_options library missing], 1)])])



AC_CHECK_PROG(GDBUSCODEGEN, gdbus-codegen, gdbus-codegen)

# Write config.status and the Makefile
AC_CONFIG_FILES([Makefile src/Makefile src/gtest/Makefile version])

AC_OUTPUT
