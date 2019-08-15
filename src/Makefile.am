SUBDIRS=gtest

svnver = "svnver"

AM_CXXFLAGS = -std=c++11
AM_CPPFLAGS = -I@top_builddir@ -I$(top_srcdir)/include                         \
			   @HISPIXEL_CFLAGS@ -g



AM_LDFLAGS = @HISPIXEL_LIBS@

bin_PROGRAMS = hispixel testpixel

hispixel_SOURCES = main.cc config.cc hispixelapp.cc hisbus.c

testpixel_CPPFLAGS = -I$(top_srcdir)/src/gtest/include @HISPIXEL_CFLAGS@
testpixel_LDADD = gtest/libgtest_main.a gtest/libgtest.a -lpthread -lcrypt
testpixel_SOURCES = test.cc

$(top_srcdir)/src/hisbus.h: hisbus.c

$(top_srcdir)/src/hisbus.c: com.hispixel.GDBUS.xml
	gdbus-codegen --generate-c-code hisbus --c-namespace HisPixel --interface-prefix com.hispixel. $(top_srcdir)/src/com.hispixel.GDBUS.xml \
		--output-directory $(top_srcdir)/src
