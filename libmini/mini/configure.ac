#########################################################################
#  package specific values
#########################################################################
pushdef([PACKAGE_MAJOR_VERSION], [8])
pushdef([PACKAGE_MINOR_VERSION], [9])
pushdef([PACKAGE_MICRO_VERSION], [4])
pushdef([PKG_VERSION], PACKAGE_MAJOR_VERSION.PACKAGE_MINOR_VERSION.PACKAGE_MICRO_VERSION)

AC_INIT([Mini], PKG_VERSION, [stefan@stereofx.org])

[PACKAGE_MAJOR_VERSION]=PACKAGE_MAJOR_VERSION
[PACKAGE_MINOR_VERSION]=PACKAGE_MINOR_VERSION
[PACKAGE_MICRO_VERSION]=PACKAGE_MICRO_VERSION

popdef([PACKAGE_MAJOR_VERSION])
popdef([PACKAGE_MINOR_VERSION])
popdef([PACKAGE_MICRO_VERSION])
popdef([PKG_VERSION])

AC_PROG_LIBTOOL
#
# +1 :  ? : +1 == new interface that does not break old one
# +1 :  ? :  0 == new interface that breaks old one
#  ? :  ? :  0 == no new interfaces, but breaks apps
#  ? : +1 :  ? == just some internal changes, nothing breaks but might work better
# CURRENT : REVISION : AGE
LIBTOOL_SO_VERSION=0:0:0
LIBTOOL_SO_VERSION_MAJOR=0

PKGCONFIG_REQUIRES="gl glu libcurl x11 xmu"

#########################################################################
#  don't touch this
#########################################################################
PACKAGE_RELEASE=$PACKAGE_MAJOR_VERSION.$PACKAGE_MINOR_VERSION
AC_SUBST(PACKAGE_RELEASE)

AC_SUBST(LIBTOOL_SO_VERSION)
AC_SUBST(LIBTOOL_SO_VERSION_MAJOR)

AC_DEFINE_UNQUOTED(PACKAGE_MAJOR_VERSION, $PACKAGE_MAJOR_VERSION, [Major version])
AC_DEFINE_UNQUOTED(PACKAGE_MINOR_VERSION, $PACKAGE_MINOR_VERSION, [Minor version])
AC_DEFINE_UNQUOTED(PACKAGE_MICRO_VERSION, $PACKAGE_MICRO_VERSION, [Micro version])
AC_SUBST(PACKAGE_MAJOR_VERSION)
AC_SUBST(PACKAGE_MINOR_VERSION)
AC_SUBST(PACKAGE_MICRO_VERSION)

AM_INIT_AUTOMAKE([1.9 tar-ustar dist-bzip2 dist-zip])
AM_CONFIG_HEADER([config.h])
AX_PREFIX_CONFIG_H([mini-config.h],[MINI],[config.h])

AC_LANG_CPLUSPLUS
AC_PROG_CXX
AC_PROG_INSTALL

for top_builddir in . .. ../.. $ac_auxdir $ac_auxdir/..; do
test -f $top_builddir/configure && break
done

AC_HEADER_STDC

#########################################################################
#  pkgconfig package requirement checks
#########################################################################

# OpenThreads support checks
AC_ARG_WITH([openthreads],
            [AS_HELP_STRING([--with-openthreads],[Build with OpenThreads support @<:@default=no@:>@])],
            [with_openthreads=yes], [with_openthreads=no])
AS_IF([test "x${with_openthreads}" != xno],
      [ AC_CHECK_LIB(OpenThreads,OpenThreadsGetVersion,,AC_MSG_ERROR([OpenThreads library not found]))
        OPENTHREADS_CFLAGS="-DUSEOPENTH"
        PKGCONFIG_REQUIRES+=" openthreads"
      ],
      [ AC_CHECK_LIB(pthread,pthread_create,,AC_MSG_ERROR([pthreads library not found and --with-openthreads not specified])) ])

# squish support checks
AC_ARG_WITH([squish],
            [AS_HELP_STRING([--with-squish],[Build with squish support @<:@default=yes@:>@])],
            [with_squish=no], [with_squish=yes])
AS_IF([test "x${with_squish}" != xno],
      [ AX_CXX_CHECK_LIB(squish,[squish::Compress],,AC_MSG_ERROR([Squish library not found]))
        PKGCONFIG_REQUIRES+=" squish"
      ],
      [ SQUISH_CFLAGS="-DNOSQUISH" ])

# GREYCStoration support checks
AC_ARG_WITH([greyc],
            [AS_HELP_STRING([--with-greyc],[Build with greyc support @<:@default=no@:>@])],
            [with_greyc=yes], [with_greyc=no])
AS_IF([test "x${with_greyc}" != xno],
      [ GREYC_CFLAGS="-DUSEGREYC"
        AC_DEFINE([HAVE_GREYC],[],[Build with greyc support])
      ])

#PKG_CHECK_MODULES([MINI],[$PKGCONFIG_REQUIRES])
AC_SUBST(PKGCONFIG_REQUIRES)
AC_SUBST(SQUISH_CFLAGS)
MINI_CFLAGS+=" $OPENTHREADS_CFLAGS $SQUISH_CFLAGS $GREYC_CFLAGS"
#AC_SUBST(MINI_LIBS)
AC_SUBST(MINI_CFLAGS)

#########################################################################
#  other library checks
#########################################################################
AC_CHECK_LIB(GL,glBegin,,[OpenGL \(libGL\) library not found])
AC_CHECK_LIB(GLU,gluLookAt,,[OpenGL Utility \(libGLU\) library not found])
AC_CHECK_LIB(glut,glutInit,,[OpenGL Utility Toolkit \(libglut\) library not found])
AC_CHECK_LIB(jpeg,jpeg_start_compress,,[jpeg \(libjpeg\) library not found])
AC_CHECK_LIB(png,png_error,,[png \(libpng\) library not found])
AC_CHECK_LIB(curl,curl_version,,[curl \(libcurl\) library not found])
AC_CHECK_LIB(X11,XDisplayName,,[X11 \(libX11\) library not found])
AC_CHECK_LIB(Xt,XtDestroyWidget,,[Xt \(libXt\) library not found])
AC_CHECK_LIB(Xmu,XmuNewArea,,[Xmu \(libXmu\) library not found])

#########################################################################
#  The purpose of autoconf... creating makefiles et. al.
#########################################################################
AC_OUTPUT(Makefile tools/Makefile Mini.pc)
