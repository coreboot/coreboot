##
##
## Copyright (C) 2008 Advanced Micro Devices, Inc.
## Copyright (C) 2008 coresystems GmbH
##
## Redistribution and use in source and binary forms, with or without
## modification, are permitted provided that the following conditions
## are met:
## 1. Redistributions of source code must retain the above copyright
##    notice, this list of conditions and the following disclaimer.
## 2. Redistributions in binary form must reproduce the above copyright
##    notice, this list of conditions and the following disclaimer in the
##    documentation and/or other materials provided with the distribution.
## 3. The name of the author may not be used to endorse or promote products
##    derived from this software without specific prior written permission.
##
## THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
## ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
## IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
## ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
## FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
## DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
## OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
## HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
## LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
## OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
## SUCH DAMAGE.
##

libc-$(CONFIG_LP_LIBC) += malloc.c printf.c console.c string.c
libc-$(CONFIG_LP_LIBC) += memory.c ctype.c ipchecksum.c lib.c libgcc.c
libc-$(CONFIG_LP_LIBC) += rand.c time.c exec.c
libc-$(CONFIG_LP_LIBC) += readline.c getopt_long.c sysinfo.c
libc-$(CONFIG_LP_LIBC) += args.c
libc-$(CONFIG_LP_LIBC) += strlcpy.c
libc-$(CONFIG_LP_LIBC) += qsort.c
libc-$(CONFIG_LP_LIBC) += hexdump.c
libc-$(CONFIG_LP_LIBC) += die.c
libc-$(CONFIG_LP_LIBC) += coreboot.c
libc-$(CONFIG_LP_LIBC) += fmap.c
libc-$(CONFIG_LP_LIBC) += fpmath.c

ifeq ($(CONFIG_LP_LIBC),y)
libc-srcs += $(coreboottop)/src/commonlib/bsd/elog.c
endif
