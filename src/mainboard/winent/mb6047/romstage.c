#include <stdint.h>
#include <string.h>
#include <device/pci_def.h>
#include <arch/io.h>
#include <device/pnp_def.h>
#include <cpu/x86/lapic.h>
#include <pc80/mc146818rtc.h>
#include <console/console.h>
#include <lib.h>
#include <spd.h>
#include <cpu/amd/model_fxx_rev.h>
#include "northbridge/amd/amdk8/incoherent_ht.c"
#include "southbridge/nvidia/ck804/early_smbus.h"
#include "northbridge/amd/amdk8/raminit.h"
#include "lib/delay.c"
#include "cpu/x86/lapic.h"
#include "northbridge/amd/amdk8/reset_test.c"
#include "northbridge/amd/amdk8/debug.c"
#include "superio/winbond/w83627thg/early_serial.c"
#include "cpu/x86/mtrr/earlymtrr.c"
#include "cpu/x86/bist.h"
#include "northbridge/amd/amdk8/setup_resource_map.c"

#define SERIAL_DEV PNP_DEV(0x2e, W83627THG_SP1)

static void memreset_setup(void) { }
static void memreset(int controllers, const struct mem_controller *ctrl) { }
static void activate_spd_rom(const struct mem_controller *ctrl) { }

static inline int spd_read_byte(unsigned device, unsigned address)
{
	return smbus_read_byte(device, address);
}

#include "northbridge/amd/amdk8/raminit.c"
#include "northbridge/amd/amdk8/coherent_ht.c"
#include "lib/generic_sdram.c"
#include "cpu/amd/dualcore/dualcore.c"
#include "southbridge/nvidia/ck804/early_setup_ss.h"
#include "southbridge/nvidia/ck804/early_setup.c"
#include "cpu/amd/car/post_cache_as_ram.c"
#include "cpu/amd/model_fxx/init_cpus.c"
#if CONFIG_SET_FIDVID
#include "cpu/amd/model_fxx/fidvid.c"
#endif
#include "northbridge/amd/amdk8/early_ht.c"

static void sio_setup(void)
{
	uint32_t dword;
	uint8_t byte;

	/* subject decoding*/
	byte = pci_read_config8(PCI_DEV(0, CK804_DEVN_BASE+1 , 0), 0x7b);
	byte |= 0x20;
	pci_write_config8(PCI_DEV(0, CK804_DEVN_BASE+1 , 0), 0x7b, byte);

	/* LPC Positive Decode 0 */
	dword = pci_read_config32(PCI_DEV(0, CK804_DEVN_BASE+1 , 0), 0xa0);
	/* Serial 0, Serial 1 */
	dword |= (1<<0) | (1<<1);
	pci_write_config32(PCI_DEV(0, CK804_DEVN_BASE+1 , 0), 0xa0, dword);

}

void cache_as_ram_main(unsigned long bist, unsigned long cpu_init_detectedx)
{
	static const uint16_t spd_addr [] = {
		DIMM0, 0, 0, 0,
		DIMM1, 0, 0, 0,
	};

	int needs_reset;
	unsigned bsp_apicid = 0, nodes;
	struct mem_controller ctrl[8];

	if (!cpu_init_detectedx && boot_cpu()) {
		/* Nothing special needs to be done to find bus 0 */
		/* Allow the HT devices to be found */
		enumerate_ht_chain();
		sio_setup();
	}

	if (bist == 0)
		bsp_apicid = init_cpus(cpu_init_detectedx);

//	post_code(0x32);

	w83627thg_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);
	console_init();

	/* Halt if there was a built in self test failure */
	report_bist_failure(bist);

#if 0
	dump_pci_device(PCI_DEV(0, 0x18, 0));
#endif

	needs_reset = setup_coherent_ht_domain();

	wait_all_core0_started();
	// It is said that we should start core1 after all core0 launched
	start_other_cores();
	wait_all_other_cores_started(bsp_apicid);

#if CONFIG_SET_FIDVID
	/* Check to see if processor is capable of changing FIDVID  */
	/* otherwise it will throw a GP# when reading FIDVID_STATUS */
	if ((cpuid_edx(0x80000007) & 0x6) == 0x6) {
		msr_t msr;
		/* Read FIDVID_STATUS */
		msr = rdmsr(0xc0010042);
		printk(BIOS_DEBUG, "begin msr fid, vid: hi=0x%x, lo=0x%x\n", msr.hi, msr.lo);

		enable_fid_change();
		init_fidvid_bsp(bsp_apicid);

		msr = rdmsr(0xc0010042);
		printk(BIOS_DEBUG, "end msr fid, vid: hi=0x%x, lo=0x%x\n", msr.hi, msr.lo);
	}
#endif

	needs_reset |= ht_setup_chains_x();
	needs_reset |= ck804_early_setup_x();
	if (needs_reset) {
		printk(BIOS_INFO, "ht reset -\n");
		soft_reset();
	}

	allow_all_aps_stop(bsp_apicid);

	nodes = get_nodes();
	//It's the time to set ctrl now;
	fill_mem_ctrl(nodes, ctrl, spd_addr);

	enable_smbus();
#if 0
	dump_spd_registers(&cpu[0]);
	dump_smbus_registers();
#endif

	memreset_setup();
	sdram_initialize(nodes, ctrl);

#if 0
	print_pci_devices();
	dump_pci_devices();
#endif

	post_cache_as_ram();
}
