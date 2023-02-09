SUBDIRS=gtest

AM_CPPFLAGS = -I@top_builddir@ -I$(top_srcdir)/include -I$(top_srcdir)/src                   \
			   @HISPIXEL_CFLAGS@

AM_LDFLAGS = @HISPIXEL_LIBS@

AM_CXXFLAGS = -std=c++17

if DEBUG
AM_CFLAGS = -g3 -O0
AM_CXXFLAGS += -g3 -O0
else
AM_CFLAGS = -O2
AM_CXXFLAGS += -O2
endif


bin_PROGRAMS = hispixel testpixel

hispixel_SOURCES = main.cc config.cc hispixelapp.cc hisbus.c \
                  tabs.cc dbushandler.cc colormanager.cc utils.cc

testpixel_CPPFLAGS = -I$(top_srcdir)/src/gtest/include @HISPIXEL_CFLAGS@ -g3 -O0
testpixel_LDADD = gtest/libgtest_main.a gtest/libgtest.a -lpthread -lcrypt

testpixel_CFLAGS = -g3 -O0


testpixel_SOURCES = test.cc

BUILT_SOURCES = hisbus.h hisbus.c

hisbus.h: hisbus.c

hisbus.c: com.hispixel.GDBUS.xml
	gdbus-codegen --generate-c-code hisbus --c-namespace HisPixel --interface-prefix com.hispixel. $(top_srcdir)/src/com.hispixel.GDBUS.xml \
		--output-directory $(top_srcdir)/src
