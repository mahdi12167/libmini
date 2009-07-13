AC_INIT([Stuttgart], 1.0.0, [stefan@stereofx.org])
AM_INIT_AUTOMAKE([1.9 tar-ustar dist-bzip2 dist-zip])
AC_LANG_CPLUSPLUS
AC_PROG_CXX
AC_PROG_INSTALL
AC_HEADER_STDC

AC_ARG_WITH([data],
            [AS_HELP_STRING([--with-data],[Default data directory @<:@default="data"@:>@])],
            [YUKON_DATA="-DSTUTTGART_DATA=\"\\\"$withval\\\"\""], [])

PKG_CHECK_MODULES([STUTTGART],[Mini])
STUTTGART_CFLAGS+=" $STUTTGART_DATA"
AC_SUBST(STUTTGART_CFLAGS)
AC_SUBST(STUTTGART_LIBS)

#########################################################################
#  The purpose of autoconf... creating makefiles et. al.
#########################################################################
AC_OUTPUT(Makefile)
