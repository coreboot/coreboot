/*
 *
 * Copyright (C) 2008 Advanced Micro Devices, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <libpayload-config.h>
#include <libpayload.h>
#include <coreboot_tables.h>
#include <multiboot_tables.h>

#define CPU_KHZ_DEFAULT 200

/**
 * This is a global structure that is used through the library - we set it
 * up initially with some dummy values - hopefully they will be overridden.
 */
struct sysinfo_t lib_sysinfo = {
	.cpu_khz = CPU_KHZ_DEFAULT,
#if CONFIG(LP_SERIAL_CONSOLE)
	.ser_ioport = CONFIG_LP_SERIAL_IOBASE,
#else
	.ser_ioport = 0x3f8,
#endif
};

int lib_get_sysinfo(void)
{
	int ret;

#if CONFIG(LP_MULTIBOOT)
	/* Get the information from the multiboot tables,
	 * if they exist */
	get_multiboot_info(&lib_sysinfo);
#endif

	/* Get information from the coreboot tables,
	 * if they exist */

	ret = get_coreboot_info(&lib_sysinfo);

	/* Get the CPU speed (for delays) if not set from the default value. */
	if (lib_sysinfo.cpu_khz == CPU_KHZ_DEFAULT)
		lib_sysinfo.cpu_khz = get_cpu_speed();

	if (!lib_sysinfo.n_memranges) {
		/* If we can't get a good memory range, use the default. */
		lib_sysinfo.n_memranges = 2;

		lib_sysinfo.memrange[0].base = 0;
		lib_sysinfo.memrange[0].size = 640 * 1024;
		lib_sysinfo.memrange[0].type = CB_MEM_RAM;

		lib_sysinfo.memrange[1].base = 1024 * 1024;
		lib_sysinfo.memrange[1].size = 31 * 1024 * 1024;
		lib_sysinfo.memrange[1].type = CB_MEM_RAM;
	}

#if CONFIG(LP_PCI)
	pci_init(&lib_sysinfo.pacc);
	pci_scan_bus(&lib_sysinfo.pacc);
#endif

	return ret;
}
