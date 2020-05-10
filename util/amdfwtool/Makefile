#*****************************************************************************
#
# All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause

#*****************************************************************************
HOSTCC ?= cc

amdfwtool_exe : amdfwtool.c
	$(HOSTCC) amdfwtool.c -o amdfwtool

amdfwtool : amdfwtool_exe

clean:
	@rm -f amdfwtool.o amdfwtool amdfwtool.exe
