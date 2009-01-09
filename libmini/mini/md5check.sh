#!/bin/tcsh -f

set dir=$1

set stamp=0
if ("$dir" == "-t") then
   set stamp=1
   set dir=$2
endif

if ("$dir" == "") then
   echo "usage: md5check.sh [-t] <dir>"
   echo " checks the integrity of a directory <dir>"
   echo " creates a md5 file list of the directory"
   echo " verifies that the md5 of each file matches with the md5 list"
   echo " use the -t switch to ignore mismatches due to an updated time stamp"
   echo " uses the md5sum tool of the GNU coreutils"
   exit
endif

if ($dir == ".") then
   set md5=".md5"
else
   set md5=$dir.md5
endif

if (! -e $md5) then
   find $dir \( -type f -a \! -name .md5 -a \! -path \*.svn\* \) -exec md5sum {} \; > $md5
else
   if ($stamp) then
      find $dir \( -type f -a \! -newer $md5 -a \! -name .md5 -a \! -path \*.svn\* \) -exec egrep \{\}\$ $md5 \; > $md5.tmp

      md5sum --check --status $md5.tmp

      if ($status) then
         md5sum --check $md5.tmp | grep FAILED
         rm $md5.tmp
         exit 1
      endif

      find $dir \( -type f -a -newer $md5 -a \! -name .md5 -a \! -path \*.svn\* \) -exec md5sum {} \; >> $md5.tmp

      mv -f $md5.tmp $md5
   else
      md5sum --check --status $md5

      if ($status) then
         md5sum --check $md5 | grep FAILED
         exit 1
      endif
   endif
endif
