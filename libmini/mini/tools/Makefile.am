METASOURCES = AUTO

INCLUDES = -I$(top_builddir) -I$(top_builddir)/.. $(MINI_CFLAGS)

noinst_PROGRAMS = eval test mpfptest

eval_SOURCES = eval.cpp
eval_LDADD = -L$(top_builddir) -lMini -lMiniSFX $(MINI_LIBS)

raw2db_SOURCES = raw2db.cpp
raw2db_LDADD = -L$(top_builddir) -lMini -lMiniSFX $(MINI_LIBS)

db2db_SOURCES = db2db.cpp
db2db_LDADD = -L$(top_builddir) -lMini -lMiniSFX $(MINI_LIBS)

dbgzip_SOURCES = dbgzip.cpp
dbgzip_LDADD = -L$(top_builddir) -lMini -lMiniSFX $(MINI_LIBS)

dbdump_SOURCES = dbdump.cpp
dbdump_LDADD = -L$(top_builddir) -lMini -lMiniSFX $(MINI_LIBS)

test_SOURCES = test.cpp
test_LDADD = -L$(top_builddir) -lMini -lMiniSFX $(MINI_LIBS)

mpfptest_SOURCES = mpfptest.cpp
mpfptest_LDADD = -L$(top_builddir) -lMini -lMiniSFX $(MINI_LIBS)
