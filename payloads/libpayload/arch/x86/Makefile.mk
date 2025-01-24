##
##
## Copyright (C) 2008 Advanced Micro Devices, Inc.
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

ifneq ($(CONFIG_LP_COMPILER_LLVM_CLANG),y)
ifeq ($(CONFIG_LP_ARCH_X86_64),y)
CFLAGS += -mpreferred-stack-boundary=4
else
CFLAGS += -mpreferred-stack-boundary=2
endif
endif

libc-$(CONFIG_LP_ARCH_X86_32)  += head.S
libc-$(CONFIG_LP_ARCH_X86_64)  += head_64.S
libc-$(CONFIG_LP_ARCH_X86_64) += pt.S
libc-y += main.c sysinfo.c
libc-y += timer.c coreboot.c util.S
libc-y += virtual.c
libc-y += cache.c
libc-y += exception.c
libc-y += delay.c
libc-$(CONFIG_LP_ARCH_X86_32) += exec.c
libc-$(CONFIG_LP_ARCH_X86_32) += exec.S
libc-$(CONFIG_LP_ARCH_X86_32) += exception_asm.S
libc-$(CONFIG_LP_ARCH_X86_64) += exception_asm_64.S

# Will fall back to default_memXXX() in libc/memory.c if GPL not allowed.
libc-$(CONFIG_LP_GPL) += string.c

libgdb-y += gdb.c

libcbfs-$(CONFIG_LP_CBFS) += boot_media.c

# Multiboot support is configurable
libc-$(CONFIG_LP_MULTIBOOT) += multiboot.c

libc-$(CONFIG_LP_ENABLE_APIC) += apic.c
