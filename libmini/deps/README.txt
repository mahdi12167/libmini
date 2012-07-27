LIBMINI INSTALL SCRIPT

To pull and install all required libMini dependencies, run the libmini install script:

 ./build.sh

It downloads the source code of the dependencies from the according svn, git, cvs or http repositories.
Then the respective source code is configured, compiled and installed the usual unix way using autotools or cmake.

The default installation path is ../.. from here.
Supply a path argument for the build script to override.

Tested on Ubuntu, SuSe and MacOS X.
Please report problems to stefan@stereofx.org.

