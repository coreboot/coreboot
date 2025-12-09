/*
 *
 * Copyright (C) 2008 Jordan Crouse <jordan@cosmicpenguin.net>
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

#include <libpayload.h>
#include <sysinfo.h>

int sysinfo_have_multiboot(unsigned long *addr)
{
	*addr = (unsigned long) lib_sysinfo.mbtable;
	return (lib_sysinfo.mbtable == 0) ? 0 : 1;
}

size_t sysinfo_get_physical_memory_size_mib(void)
{
	const struct mem_chip_info *mc = phys_to_virt(lib_sysinfo.mem_chip_base);

	if (mc)
		return mem_chip_info_total_density_bytes(mc) / MiB;

#if CONFIG(LP_GPL)
	const struct memory_info *memory_info = phys_to_virt(lib_sysinfo.memory_info);
	size_t memory_size_mib = 0;
	if (memory_info) {
		for (int i = 0; i < memory_info->dimm_cnt; ++i)
			memory_size_mib += memory_info->dimm[i].dimm_size;
		return memory_size_mib;
	}
#endif

	return 0;
}
