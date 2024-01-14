/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/encoding.h>
#include <cbmem.h>
#include <console/console.h>
#include <program_loading.h>
#include <romstage_common.h>
#include <soc/addressmap.h>
#include <soc/clock.h>
#include <soc/sdram.h>

void __noreturn romstage_main(void)
{
	write_csr(0x7c1, 0); // CSR_U74_FEATURE_DISABLE (enable all Features)

	clock_init();

	console_init();

	sdram_init((u64)16*GiB);

	cbmem_initialize_empty();

	run_ramstage();
}
