METASOURCES = AUTO

SUBDIRS = . tools

HDRS	= minibase minicore\
	  minicoreP.h miniOGLP.h\
	  minicomplex minivec minimtx\
	  minidyna minigeom minisort ministring\
	  miniref mininode

MODS	= mini miniOGL\
	  miniv3f miniv3d miniv4f miniv4d\
	  minimath mininoise minimpfp\
	  minitime minirgb minicrs\
	  miniio minidir minidds\
	  ministub minitile miniload\
	  minicoord miniwarp minicam minianim\
	  minilayer miniterrain miniearth\
	  minicache minishader\
	  miniray ministrip\
	  minipoint minitext minisky miniglobe\
	  minitree minibrick minilod\
	  minimesh minibspt miniproj\
	  mininodes\
	  pnmbase pnmsample\
	  database datafill\
	  datacloud datacache datagrid\
	  datacalc dataparse\
	  lunascan lunaparse lunacode

VMODS	= miniscene miniview\
	  threadbase curlbase\
	  jpegbase pngbase zlibbase squishbase greycbase\
	  dataconv miniimg\
	  minipano

INCLUDES = -I$(top_builddir)/.. $(MINI_CFLAGS)

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = Mini.pc

lib_LTLIBRARIES = libMini.la libMiniSFX.la
library_includedir = $(prefix)/include/mini

noinst_PROGRAMS = example stubtest viewer

libMini_la_LIBADD = $(MINI_LIBS)
libMini_la_SOURCES = $(MODS:=.cpp)

libMiniSFX_la_LIBADD = $(MINI_LIBS) -lMini
libMiniSFX_la_SOURCES = $(VMODS:=.cpp)

library_include_HEADERS = $(patsubst %,%.h,${HDRS}) $(patsubst %,%.h,${MODS}) $(patsubst %,%.h,${VMODS})

EXTRA_DIST = minicore.cpp\
	     build.sh build.bat\
	     data/SkyDome.ppm data/EarthDay.db data/EarthNight.db data/Cone.db\
	     GL/glext.h\
	     libMini.jpg libMini.ppm libMini.ico\
	     README.html libMini.css\
	     LICENSE.txt

example_SOURCES = example.cpp
example_LDADD = -lMini $(MINI_LIBS)

stubtest_SOURCES = stubtest.cpp
stubtest_LDADD = -lMini $(MINI_LIBS)

viewer_SOURCES = viewer.cpp
viewer_LDADD = -lMini -lMiniSFX $(MINI_LIBS)
