METASOURCES = AUTO

INCLUDES = -I$(top_builddir) -I$(top_builddir)/.. $(MINI_CFLAGS)

noinst_PROGRAMS = db2db eval mpfptest test

EXTRA_DIST = modblack.sh modgeotiff.sh

db2db_SOURCES = db2db.cpp
db2db_LDADD = -L$(top_builddir) -lMini -lMiniSFX $(MINI_LIBS)

eval_SOURCES = eval.cpp
eval_LDADD = -L$(top_builddir) -lMini -lMiniSFX $(MINI_LIBS)

mpfptest_SOURCES = mpfptest.cpp
mpfptest_LDADD = -L$(top_builddir) -lMini -lMiniSFX $(MINI_LIBS)

test_SOURCES = test.cpp
test_LDADD = -L$(top_builddir) -lMini -lMiniSFX $(MINI_LIBS)
