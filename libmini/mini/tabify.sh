#!/bin/tcsh -f

set app=$0

set option=$1
set file=$2

if ("$option" == "") then
   echo "usage: tabify -x|u|t|X|U|T [<file>]"
   echo " replaces all tabs with 4-space (-x) or 8-space (-X)"
   echo " or replaces all tabs at the beginning of each line with 4-space (-u) or 8-space (-U) and the other way round (-t|T)"
   echo " additionally removes trailing white space, all spaces before a tab, and gets rid of all ctrl-m"
   echo " whithout a specified file argument all .cpp and .h files are treated accordingly"
   exit
endif

set tabify=0

if ("$option" == "-x") set tabify=1
if ("$option" == "-u") set tabify=2
if ("$option" == "-t") set tabify=3

if ("$option" == "-X") set tabify=4
if ("$option" == "-U") set tabify=5
if ("$option" == "-T") set tabify=6

if ($tabify == 0) exit 1

set spc=' '
set spc4='    '
set spc8='        '
set tab='	'

set rmtab4="s/$tab/$spc4/"
set rmtab8="s/$tab/$spc8/"

set rmspc="s/[$spc$tab]*"'$'"//"
set rmspctab="s/$spc*$tab/$tab/g"

set ctrlm='\0x0a'
set rmctrlm="s/$ctrlm//"

if ($file == "") then
   if ($tabify == 1) then
      find . \( -name \*.cpp -o -name \*.h \) -exec $app -x {} \;
   endif
   if ($tabify == 2) then
      find . \( -name \*.cpp -o -name \*.h \) -exec $app -t {} \; -exec $app -u {} \;
   endif
   if ($tabify == 3) then
      find . \( -name \*.cpp -o -name \*.h \) -exec $app -u {} \; -exec $app -t {} \;
   endif
   if ($tabify == 4) then
      find . \( -name \*.cpp -o -name \*.h \) -exec $app -X {} \;
   endif
   if ($tabify == 5) then
      find . \( -name \*.cpp -o -name \*.h \) -exec $app -T {} \; -exec $app -U {} \;
   endif
   if ($tabify == 6) then
      find . \( -name \*.cpp -o -name \*.h \) -exec $app -U {} \; -exec $app -T {} \;
   endif
else
   if ($tabify == 1) then
      echo detabifying $file
      sed -e "$rmctrlm" -e "$rmspc" -e "$rmspctab" -e "$rmtab4" <$file >$file.tmp
   endif
   if ($tabify == 2) then
      echo untabifying $file
      sed -e "$rmctrlm" -e "$rmspc" -e "$rmspctab" -e ":l\
                                                       s/^\( *\)$tab/\1    /\
                                                       t l" <$file >$file.tmp
   endif
   if ($tabify == 3) then
      echo tabifying $file
      sed -e "$rmctrlm" -e "$rmspc" -e ":l\
                                        s/^\($tab*\)    /\1$tab/\
                                        t l" <$file >$file.tmp
   endif
   if ($tabify == 4) then
      echo detabifying $file
      sed -e "$rmctrlm" -e "$rmspc" -e "$rmspctab" -e "$rmtab8" <$file >$file.tmp
   endif
   if ($tabify == 5) then
      echo untabifying $file
      sed -e "$rmctrlm" -e "$rmspc" -e "$rmspctab" -e ":l\
                                                       s/^\( *\)$tab/\1        /\
                                                       t l" <$file >$file.tmp
   endif
   if ($tabify == 6) then
      echo tabifying $file
      sed -e "$rmctrlm" -e "$rmspc" -e ":l\
                                        s/^\($tab*\)        /\1$tab/\
                                        t l" <$file >$file.tmp
   endif
   mv -f $file.tmp $file
endif
