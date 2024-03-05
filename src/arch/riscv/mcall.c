/* SPDX-License-Identifier: BSD-4-Clause-UC */

#include <mcall.h>
#include <string.h>
#include <vm.h>

void hls_init(uint32_t hart_id, void *fdt)
{
	memset(HLS(), 0, sizeof(*HLS()));
	HLS()->fdt = fdt;
	HLS()->hart_id = hart_id;
	HLS()->enabled = 1;

	mtime_init();
}
