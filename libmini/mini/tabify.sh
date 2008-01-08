#!/bin/tcsh -f

set option=$1
set file=$2

if ("$option" == "") then
   echo "usage: tabify ( \"-t\" | \"-u\" ) [ <file> ]"
   echo " replaces all tabs at the begin of each line with 4-space (-u) and the other way round (-t)"
   echo " additionally removes trailing white space, all spaces before a tab, and gets rid of all Ctrl-M"
   echo " whithout a file being specified all .cpp and .h files are treated accordingly"
   exit
endif

set tabify=1

if ("$option" == "-t") set tabify=1
if ("$option" == "-u") set tabify=0

set rmctrlm='s/\\0x0d\\0x0a$/\\0x0a/'
if ($HOSTTYPE == "i386-cygwin") set rmctrlm=''

set rmspc='s/[ \t][ \t]*$//'
set rmspctab='s/ *\t/\t/g'

if ($file == "") then
   if ($tabify != 0) then
      find . \( -name \*.cpp -o -name \*.h \) -exec tabify.sh -u {} \; -exec tabify.sh -t {} \;
   else
      find . \( -name \*.cpp -o -name \*.h \) -exec tabify.sh -t {} \; -exec tabify.sh -u {} \;
   endif
else
   if ($tabify != 0) then
      echo tabifying $file

      sed -e "$rmctrlm" -e "$rmspc" -e ':l\
                                        s/^\(\t*\)    /\1\t/\
                                        t l' <$file >$file.tmp
   else
      echo untabifying $file

      sed -e "$rmctrlm" -e "$rmspc" -e "$rmspctab" -e ':l\
                                                       s/^\( *\)\t/\1    /\
                                                       t l' <$file >$file.tmp
   endif

   mv -f $file.tmp $file
endif
