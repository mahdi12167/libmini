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
   echo " creates a md5 file list of the directory (in <dir>.md5)"
   echo " verifies that the md5 of each file matches with the md5 list"
   echo " use the -t switch to ignore mismatches due to an updated time stamp"
   echo " uses the md5sum tool of the GNU coreutils"
   exit
endif

set dir=$dir:h

if ($dir == ".") then
   set md5=".md5"
   set tmp=".tmp.md5"
else
   set md5=$dir.md5
   set tmp=$dir.tmp.md5
endif

if (! -e $md5) then
   echo creating md5 file list
   find $dir \( -type f -a \! -name \*.md5 -a \! -path \*.svn/\* -a \! -path \*CVS/\* \) -exec md5sum {} \; > $md5
else
   echo checking integrity
   if ($stamp) then
      find $dir \( -type f -a \! -newer $md5 -a \! -name \*.md5 -a \! -path \*.svn/\* -a \! -path \*CVS/\* \) -exec egrep \{\}\$ $md5 \; > $tmp

      md5sum --check --status $tmp

      if ($status) then
         echo detected md5 mismatch
         md5sum --check $tmp | grep FAILED
         rm $tmp
         exit 1
      else
         echo verified md5 file list
      endif

      find $dir \( -type f -a -newer $md5 -a \! -name \*.md5 -a \! -path \*.svn/\* -a \! -path \*CVS/\* \) -exec md5sum {} \; >> $tmp

      mv -f $tmp $md5
   else
      md5sum --check --status $md5

      if ($status) then
         echo detected md5 mismatch
         md5sum --check $md5 | grep FAILED
         exit 1
      else
         echo verified md5 file list
      endif
   endif
endif
