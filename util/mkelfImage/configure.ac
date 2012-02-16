dnl
dnl configure.ac for mkelfImage
dnl
dnl

dnl ---Required
AC_INIT(Makefile.conf.in)
AC_CONFIG_AUX_DIR(./config)

dnl --Options


dnl -- Compilation platform configuration
AC_CANONICAL_HOST
#AC_CANONICAL_TARGET


dnl Compute target cpu
case $host_cpu in
	i?86 )
		target_cpu="i386"
		;;
	* )
		target_cpu="$host_cpu"
		;;
esac

dnl ---Options
targets=""

AC_ARG_WITH([i386], AC_HELP_STRING([--with-i386],[enable building i386 code]),
	[ if test "withval" = "yes" ; then with_i386="i386"; else with_i386="$withval"; fi],
	[ with_i386="no"])

AC_ARG_WITH([ia64], AC_HELP_STRING([--with-ia64],[enable building ia64 code]),
	[ if test "withval" = "yes" ; then with_ia64="ia64"; else with_ia64="$withval"; fi],
	[ with_ia64="no"])

dnl If no targets are specified use a default
with_default="no"
if	test "$with_i386" = "no" &&
	test "$with_ia64" = "no" ; then
	with_default=`echo $target_cpu | tr [a-z] [A-Z]`
fi

AC_MSG_NOTICE([with_default=$with_default])
AC_MSG_NOTICE([with_i386=$with_i386])
AC_MSG_NOTICE([with_ia64=$with_ia64])

dnl ---Programs
dnl To specify a different compiler, just 'export CC=/path/to/compiler'

AC_PROG_CC


AC_CHECK_PROG([MKDIR], mkdir, mkdir, [], [$PATH])
AC_CHECK_PROG([RM],    rm,    rm,    [], [$PATH])
AC_CHECK_PROG([CP],    cp,    cp,    [], [$PATH])
AC_CHECK_PROG([LN],    ln,    ln,    [], [$PATH])
AC_CHECK_PROG([TAR],   tar,   tar,   [], [$PATH])
AC_CHECK_PROG([RPM],   rpm,   rpm,   [], [$PATH])
AC_CHECK_PROG([SED],   sed,   sed,   [], [$PATH])
AC_CHECK_PROG([FIND],  find,  find,  [], [$PATH])

AC_CHECK_HEADER(zlib.h, AC_CHECK_LIB(z, inflateInit_, [AC_DEFINE(HAVE_ZLIB_H, 1) LIBS="$LIBS -lz"]))

dnl Find the default programs
if test "with_default" != no ; then
	if test "$CC" = no; then
		AC_MSG_ERROR([cc not found])
	fi
	eval "${with_default}_CC='$CC'"
	AC_PROG_CPP
	if test "$CPP" = no; then
		AC_MSG_ERROR([cpp not found])
	fi
	eval "${with_default}_CPP='$CPP'"
	AC_CHECK_TOOL([LD], ld, no)
	if test "$LD" = no; then
		AC_MSG_ERROR([ld not found])
	fi
	eval "${with_default}_LD='$LD'"
	AC_CHECK_TOOL([AS], as, no)
	if test "$AS" = no; then
		AC_MSG_ERROR([as not found])
	fi
	eval "${with_default}_AS='$AS'"
	AC_CHECK_TOOL([OBJCOPY], objcopy, no)
	if test "$OBJCOPY" = no; then
		AC_MSG_ERROR([objcopy not found])
	fi
	eval "${with_default}_OBJCOPY='$OBJCOPY'"
	if test "$with_default" = "X86_64" ; then
		with_i386=yes
	fi
fi

dnl Find the programs for compiling i386 targets
if test "$with_i386" != "no" ; then
	cc="gcc -m32"
	cpp=cpp
	ld=ld
	as="as -32"
	objcopy=objcopy
	if test "$with_i386" != "" -a "$with_i386" != "yes" ; then
		cc="$with_i386-$cc"
		cpp="$with_i386-$cpp"
		ld="$with_i386-$ld"
		as="$with_i386-$as"
		objcopy="$with_i386-$objcopy"
	fi
	AC_CHECK_PROG([I386_CC], [$cc], [$cc], [""], [$PATH])
	if test "$I386_CC" = no; then

		AC_MSG_ERROR([$cc not found])
	fi
	AC_CHECK_PROG([I386_CPP], [$cpp], [$cpp], [""], [$PATH])
	if test "$I386_CPP" = no; then
		AC_MSG_ERROR([$cpp not found])
	fi
	AC_CHECK_PROG([I386_LD], [$ld], [$ld], [""], [$PATH])
	if test "$I386_AS" = no; then
		AC_MSG_ERROR([$ld not found])
	fi
	AC_CHECK_PROG([I386_AS], [$as], [$as], [""], [$PATH])
	if test "$I386_AS" = no; then
		AC_MSG_ERROR([$as not found])
	fi
	AC_CHECK_PROG([I386_OBJCOPY], [$objcopy], [$objcopy],[""], [$PATH])
	if test "$I386_OBJCOPY" = no; then
		AC_MSG_ERROR([$objcopy not found])
	fi
fi

dnl Find the programs for compiling ia64 targets
if test "$with_ia64" != "no" ; then
	cc=gcc
	cpp=cpp
	ld=ld
	as=as
	objcopy=objcopy
	if test "$with_ia64" != "" -a "$with_ia64" != "yes" ; then
		cc="$with_ia64-$cc"
		cpp="$with_ia64-$cpp"
		ld="$with_ia64-$ld"
		as="$with_ia64-$as"
		objcopy="$with_ia64-$objcopy"
	fi
	AC_CHECK_PROG([IA64_CC], [$cc], [$cc], [""], [$PATH])
	if test "$IA64_CC" = no; then

		AC_MSG_ERROR([$cc not found])
	fi
	AC_CHECK_PROG([IA64_CPP], [$cpp], [$cpp], [""], [$PATH])
	if test "$IA64_CPP" = no; then
		AC_MSG_ERROR([$cpp not found])
	fi
	AC_CHECK_PROG([IA64_LD], [$ld], [$ld], [""], [$PATH])
	if test "$IA64_AS" = no; then
		AC_MSG_ERROR([$ld not found])
	fi
	AC_CHECK_PROG([IA64_AS], [$as], [$as], [""], [$PATH])
	if test "$IA64_AS" = no; then
		AC_MSG_ERROR([$as not found])
	fi
	AC_CHECK_PROG([IA64_OBJCOPY], [$objcopy], [$objcopy],[""], [$PATH])
	if test "$IA64_OBJCOPY" = no; then
		AC_MSG_ERROR([$objcopy not found])
	fi
fi


dnl ---Output variables...

HOST_CC=$CC
HOST_CFLAGS="$HOST_CFLAGS -O2 -Wall \$(HOST_CPPFLAGS)"

dnl TODO: figure out how to set these appropriately for compilers other than gcc
I386_CFLAGS="$I386_CFLAGS -Os -ffreestanding -Wall -W -Wno-format \$(I386_CPPFLAGS)"
I386_ASFLAGS=''
I386_LDFLAGS='-static --warn-multiple-gp --warn-common'

dnl TODO: figure out how to set these appropriately for compilers other than gcc
IA64_CFLAGS="$IA64_CFLAGS -Os -ffreestanding -Wall -W -Wno-format -fpic -mconstant-gp -mauto-pic -fno-unwind-tables \$(IA64_CPPFLAGS)"
IA64_ASFLAGS='-mconstant-gp -mauto-pic'
IA64_LDFLAGS='-static --warn-multiple-gp --warn-common'

AC_SUBST([HOST_CC])

AC_CACHE_CHECK(for -fno-stack-protector, libc_cv_fno_stack_protector, [
cat > conftest.c <<EOF
int foo;
main () { return 0;}
EOF
if AC_TRY_COMMAND([$I386_CC -fno-stack-protector
                           -o conftest conftest.c 1>&AS_MESSAGE_LOG_FD])
then
  libc_cv_fno_stack_protector=yes
else
  libc_cv_fno_stack_protector=no
fi
rm -f conftest*])
if test $libc_cv_fno_stack_protector = yes; then
  I386_CFLAGS="$I386_CFLAGS -fno-stack-protector"
  IA64_CFLAGS="$IA64_CFLAGS -fno-stack-protector"

fi
AC_SUBST(libc_cv_fno_stack_protector)

AC_SUBST([HOST_CFLAGS])

AC_SUBST([I386_CC])
AC_SUBST([I386_LD])
AC_SUBST([I386_AS])
AC_SUBST([I386_CPP])
AC_SUBST([I386_OBJCOPY])
AC_SUBST([I386_CFLAGS])
AC_SUBST([I386_ASFLAGS])
AC_SUBST([I386_LDFLAGS])

AC_SUBST([IA64_CC])
AC_SUBST([IA64_LD])
AC_SUBST([IA64_AS])
AC_SUBST([IA64_CPP])
AC_SUBST([IA64_OBJCOPY])
AC_SUBST([IA64_CFLAGS])
AC_SUBST([IA64_ASFLAGS])
AC_SUBST([IA64_LDFLAGS])

dnl ---Output
AC_OUTPUT([Makefile.conf])


