SUBDIRS=gtest

AM_CXXFLAGS = -std=c++11
AM_CPPFLAGS = -I@top_builddir@ -I$(top_srcdir)/include -I$(top_srcdir)/src                   \
			   @HISPIXEL_CFLAGS@ -g

AM_LDFLAGS = @HISPIXEL_LIBS@

bin_PROGRAMS = hispixel testpixel

hispixel_SOURCES = main.cc config.cc hispixelapp.cc hisbus.c

testpixel_CPPFLAGS = -I$(top_srcdir)/src/gtest/include @HISPIXEL_CFLAGS@
testpixel_LDADD = gtest/libgtest_main.a gtest/libgtest.a -lpthread -lcrypt

testpixel_SOURCES = test.cc

BUILT_SOURCES = hisbus.h hisbus.c

hisbus.h: hisbus.c

hisbus.c: com.hispixel.GDBUS.xml
	gdbus-codegen --generate-c-code hisbus --c-namespace HisPixel --interface-prefix com.hispixel. $(top_srcdir)/src/com.hispixel.GDBUS.xml \
		--output-directory $(top_srcdir)/src
