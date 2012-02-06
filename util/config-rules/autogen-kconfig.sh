#! /bin/sh
#
# Auto-generates Kconfig sections for coreboot directory layout
#
# For mainboard vendor and board directories the necessary choice
# entries are created with texts extracted from Kconfig files
# placed under the child directories.
#
# For the mainboard/vendor/board directory, devicetree.cb is parsed to
# generate select directives to include sources for all chip components. 

sedpath=`dirname $0`
choice_field=$sedpath/extract_choice

autogen_sub()
{
  kconfig_in=$1/$2/Kconfig

  if test -s $kconfig_in ; then
    echo 'source "'${kconfig_in}'"' >> $sub_source
  fi

  if test "$spath" = "mainboard" -o "$spath" = "mainboard_vendor" ; then
    sed -f $choice_field $kconfig_in >> $sub_choice
  fi
}

autogen_cfg()
{
#  bootblock generation triggered from Makefile
#  bcfg="bootblock_autogen.h"
#  build/util/sconfig/sconfig $2/$3 $1 -b $bcfg

  kcfg="Kconfig.sdep"
  build/util/sconfig/sconfig $2/$3 $1 -k $kcfg
  cat $1/$kcfg >> $this_sconfig
  rm -f $1/$kcfg

}


output_sub_choice()
{
  if test ! -s $sub_choice ; then return ; fi

  if test "$spath" = "mainboard" ; then
	echo '\nchoice\n\tprompt "Mainboard vendor"\n\tdefault VENDOR_EMULATION\n' >> $Kconfig_out
        cat $sub_choice >> $Kconfig_out
	echo 'endchoice\n' >> $Kconfig_out
  elif test "$spath" = "mainboard_vendor" ; then
	echo 'choice\n\tprompt "Mainboard model"\n' >> $Kconfig_out
        cat $sub_choice >> $Kconfig_out
	echo 'endchoice\n' >> $Kconfig_out
  else
        cat $sub_choice >> $Kconfig_out
	echo >> $Kconfig_out
  fi
}

output_sub_source()
{
  if test ! -s $sub_source ; then return ; fi

  cat $sub_source >> $Kconfig_out
  echo >> $Kconfig_out
}

output_this_sconfig()
{
  if test ! -s $this_sconfig ; then return ; fi

  cat $this_sconfig >> $Kconfig_out
  echo >> $Kconfig_out
}


# splits $root to $spath/$vendor/$part
# sets $subirs to list of $root subdirectories
split_parent_directory()
{
  sdir=$(echo $parent | tr "/" " ")
  lvl=-1
  spath=none
  
  for i in $sdir ; do
    if test $lvl -eq -1 -a $i = src ; then
       lvl=0
       subdirs="cpu mainboard northbridge southbridge superio"
    elif test $lvl -eq 0 ; then
       spath=${i}
       lvl=1
    elif test $lvl -eq 1 ; then
       spath=${spath}_vendor
       vendor=${i}
       lvl=2
    else
       spath=${spath}_part
       part=${i}
    fi
  done
  if test $lvl -gt 0 ; then
    subdirs=`find $parent -mindepth 1 -maxdepth 1 -type d -printf "%f\n" | sort`
  fi
}

output_parent()
{
  Kconfig_out=$objd/Kconfig.autogen
  sub_source=`mktemp -p $objd`
  sub_choice=`mktemp -p $objd`
  this_sconfig=`mktemp -p $objd`

  # sets spath, vendor, part, subdirs
  split_parent_directory

  # generate sub-directory includes
  for subd in $subdirs ; do
    echo $parent/$subd >> nextdirs.tmp
    autogen_sub $parent $subd
  done
  
  # generate mainboard devicetree.cb configs
  if test "$spath" = "mainboard_vendor_part" ; then
    autogen_cfg $objd $vendor $part
  fi

  if test -s $sub_source -o -s $sub_choice -o -s $this_sconfig ; then 
    echo '## begin Kconfig.autogen\n' > $Kconfig_out
    output_this_sconfig
    output_sub_choice
    output_sub_source
    echo '## end Kconfig.autogen' >> $Kconfig_out
  fi
  rm -f $sub_source $sub_choice $this_sconfig
}

# main

find ./build -name Kconfig.autogen -delete
find ./build -name bootblock_autogen.h -delete

roots=src
while test -n "$roots" ; do
  rm -f nextdirs.tmp

  for parent in $roots ; do
    objd=`echo $parent | sed -e "s/^src/build/g"`
    mkdir -p $objd
    output_parent
  done

  if test -s nextdirs.tmp ; then
    roots=`cat nextdirs.tmp`
  else
    roots=
  fi
  
done

