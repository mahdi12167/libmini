! Project Pong Client/Server
Copyright (c) 2014 by Stefan Roettger.

Project Pong is my work title for a client/server system which allows
the transmission of data over an ssl encrypted channel.

The main use case of the system is a central data storage server at
home, that receives data from clients at various locations, for
example while traveling abroad and sending live data back home from
multiple devices.

The server acts as a gateway in the middle, storing uploaded data in a
queue until the data is delivered to be stored in its final
destination at home. On the destination side the data ends up being
stored safely in a SQLite database.

As an example use case, think about a world-wide journey where you
shoot thousands of pictures with multiple cameras and
smartphones. After the journey you need to download all your data,
delete duplicates and bad pictures, organize your data depending on
importance and location and finally make a backup. Do you remember how
often you forgot to synchronize your data or did not remember if you
synchronized your data at all or forgot which particular location a
picture belonged to? Or just lost the camera and all the pictures were
gone forever? What a hazzle! Better store the pictures immediately by
sending them back home as early as an internet connection becomes
available.

The advantages are that there is only a single and safe central point
of storage for all data that is acquired by multiple devices from
varying locations. This allows for convenient searching and easy
backup. It also minimizes copying data around, avoids double copies,
and prevents data being forgotten or lost.

Project Pong aims to provide a platform for automating the latter use
case. So the goal of the project is to automate the task of
classifying data (by gps-location, acquisition time, importance and
user tag) and sending the classified data back to a central storage
location (that is home) in a secure and safe way.

!! Installation on Unix/Mac

For the installation of the Project Pong client/server system, the
installation of the following Unix tools is required:
* cmake
* gnu/c++
* subversion
* qt/qmake

On Ubuntu 14.04, for example, this is achieved on the command line:
 sudo apt-get install g++ cmake cmake-curses-gui subversion

To install Qt, it is recommended to build it from source, e.g. grab
the source tar ball from

 http://download.qt-project.org/official_releases/qt/5.3/5.3.0/single/qt-everywhere-opensource-src-5.3.0.tar.gz/.zip

and type the following command line on the terminal in your Qt source directory:
 ./configure -opengl -openssl -release -nomake examples -nomake tests -opensource -confirm-license && make && sudo make install

After the build process has finished (go get yourself a cup of coffee),
you will be asked to enter your root password for installation of Qt.

!! Checkout and Compilation

To install the Project Pong software, type the following on the terminal:
 svn co http://libmini.googlecode.com/svn/libmini/pong pong
 (cd pong; cmake . && make && sudo make install)

If you installed Qt5, be sure to enable the BUILD_WITH_QT5 option:
 (cd pong; cmake -DBUILD_WITH_QT5 . && make && sudo make install)

!! Usage

The client and server applications are available as "ping", "poing"
and "pong" command line tools on the command line. In the Linux
desktop manager, search for the applications with the respective name
and drag them into the side bar.

!! Example Use Case

tbd ...

That's it!

!! Postcard

If you found the software useful, please send a vacation postcard to:

 Prof. Dr. Stefan Roettger
 Wassertorstr. 10
 90489 Nuernberg
 Germany

Thanks!
