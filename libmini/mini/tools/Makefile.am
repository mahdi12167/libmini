METASOURCES = AUTO

INCLUDES = -I$(top_builddir) -I$(top_builddir)/.. $(MINI_CFLAGS)

noinst_PROGRAMS = eval expr test mpfptest

eval_SOURCES = eval.cpp
eval_LDADD = -L$(top_builddir) -lMini -lMiniSFX $(MINI_LIBS)

expr_SOURCES = expr.cpp
expr_LDADD = -L$(top_builddir) -lMini -lMiniSFX $(MINI_LIBS)

raw2db_SOURCES = raw2db.cpp
raw2db_LDADD = -L$(top_builddir) -lMini -lMiniSFX $(MINI_LIBS)

db2db_SOURCES = db2db.cpp
db2db_LDADD = -L$(top_builddir) -lMini -lMiniSFX $(MINI_LIBS)

dbgzip_SOURCES = dbgzip.cpp
dbgzip_LDADD = -L$(top_builddir) -lMini -lMiniSFX $(MINI_LIBS)

dbdump_SOURCES = dbdump.cpp
dbdump_LDADD = -L$(top_builddir) -lMini -lMiniSFX $(MINI_LIBS)

dds_SOURCES = dds.cpp
dds_LDADD = -L$(top_builddir) -lMini -lMiniSFX $(MINI_LIBS)

rek2raw_SOURCES = rek2raw.cpp
rek2raw_LDADD = -L$(top_builddir) -lMini -lMiniSFX $(MINI_LIBS)

rawquant_SOURCES = rawquant.cpp
rawquant_LDADD = -L$(top_builddir) -lMini -lMiniSFX $(MINI_LIBS)

raw2iso_SOURCES = raw2iso.cpp
raw2iso_LDADD = -L$(top_builddir) -lMini -lMiniSFX $(MINI_LIBS)

geo2ply_SOURCES = geo2ply.cpp
geo2ply_LDADD = -L$(top_builddir) -lMini -lMiniSFX $(MINI_LIBS)

test_SOURCES = test.cpp
test_LDADD = -L$(top_builddir) -lMini -lMiniSFX $(MINI_LIBS)

mpfptest_SOURCES = mpfptest.cpp
mpfptest_LDADD = -L$(top_builddir) -lMini -lMiniSFX $(MINI_LIBS)
