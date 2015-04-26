# initialize autoconf
AC_INIT([hispixel], [1.0.1], [ondrej.holecek@gmail.com])
VERSION_INFO="-version-info 0:0:0"

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
AC_CONFIG_HEADER([config.h])

dnl This forces debian autoconf to use version 2.53
AC_PREREQ(2.50)

# disable so
AC_DISABLE_SHARED

# initialize libtool
AC_PROG_LIBTOOL

# Add a test for a compiler.
AC_PROG_CC
AC_PROG_CXX

# replace autotools original defaults
CXXFLAGS="-W -Wall -Wextra -Wconversion"


AC_SUBST(MALLOC_LIB)

#SVN_REVISION="`git describe --all --dirty --long --abbrev=40`"
#AC_SUBST(SVN_REVISION)

AC_SUBST(VERSION_INFO)

PKG_PROG_PKG_CONFIG


HISPIXEL_DEPS="gtk+-3.0 >= 3.0.0, vte-2.90"
PKG_CHECK_MODULES([HISPIXEL], [${HISPIXEL_DEPS}])

#PKG_CHECK_MODULES([GTK], [gtk+-3.0 >= 3.0.0])

AX_BOOST_BASE([1.50], , [AC_MSG_ERROR([boost libraries missing])])
AX_BOOST_IOSTREAMS
#AX_BOOST_THREAD


#check boost system
AC_LANG_CPLUSPLUS
AC_CHECK_HEADER([boost/system/error_code.hpp], [], AC_MSG_ERROR([boost system headers is missing], 1))                                     
AC_CHECK_LIB(boost_system-mt, main, , [
    AC_CHECK_LIB(boost_system, main, , [                                                                                                   
        AC_MSG_ERROR([boost system library is missing], 1)])])  

# check for boost threads
AC_LANG_CPLUSPLUS
AC_CHECK_HEADER([boost/thread.hpp], [], AC_MSG_ERROR([boost thread headers missing], 1))
AC_CHECK_LIB(boost_thread-mt, main, , [ 
    AC_CHECK_LIB(boost_thread, main, , [ 
        AC_MSG_ERROR([boost thread library missing], 1)])])



# Write config.status and the Makefile
AC_CONFIG_FILES([Makefile src/Makefile version])

AC_OUTPUT()
