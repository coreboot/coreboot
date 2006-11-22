/* the standard linuxbios include file has constant definitions, types and so on */
#include <linuxbios.h>

#if 0
/* NOTES */
/* support library code. */
src/LinuxBIOSv2/src/cpu/x86/lapic/boot_cpu.c -- which cpu is the boot cpu
src/LinuxBIOSv2/src/northbridge/amd/amdk8/reset_test.c  -- determine if we had a reset -- hard or soft
src/LinuxBIOSv2/src/cpu/amd/mtrr/amd_earlymtrr.c -- early mtrr setup

/* this is currently used but may be replaced by properties for the CPUs */
northbridge/amd/amdk8/setup_resource_map.c -- map of 18.1 device for routing

#include "southbridge/nvidia/ck804/ck804_enable_rom.c"
#include "northbridge/amd/amdk8/early_ht.c"
#endif

/* there is a global struct used by main, that is the dtb tree */
/* it is built when the linuxbios image is built */
/* it is linked in, as it is generated as a C struct */
extern struct dtb *dtb;

/* basic ugly crud that is not at all elegant ... very mainboard specific, has to be done this way */
static void stage1_superio_setup(void)
{
	struct property *superio;
	unsigned value;
	uint32_t dword;
	uint8_t byte;

	superio = get_property(dtb, "ck804");
	/* read dev  1 , function 0, of the superio, */
	byte = pci_read_config8(superio, 1, 0, 0x7b);
	byte |= 0x20;
	pci_write_config8(superio, 1, 0, 0x7b, byte);

	dword = pci_read_config32(superio, 1, 0, 0xa0);
	dword |= (1<<0);
	pci_write_config32(superio, 1, 0, 0xa0, dword);

}

/* assumptions: when we get here, we have a small region of cache-as-ram usable as a stack. 
  * we have the DTB in flash. bist (built-in-self-test) and cpu_init_detectedx are set by CAR code. 
  * This code is common to both fallback and normal images, so we do it in pre_initram support. 
  */
/* what we have to do:
  * enable console(s)
  * make the processors sane
  * do initial hardware enable
  */
void stage1(unsigned long bist, unsigned long cpu_init_detectedx)
{
	struct property *image;
	struct property *uart;
	struct LAR_dir *dir;
	struct LAR_file *file;
	struct LAR_file *decompressor;
	void (*code)():

        int needs_reset;
        unsigned bsp_apicid = 0;

        struct mem_controller ctrl[8];
        unsigned nodes;
        unsigned last_boot_normal_x = last_boot_normal(); /* from CMOS */

        /* Is this a cpu only reset? or Is this a secondary cpu? */
	/* cpu only means we came here before, set up some basic things (e.g. hypertransport), 
	  * and found that as part of that we had to reset the CPU to get the bus set up correctly. 
	  * Secondary CPUs do less work than primary CPUs (on K8) and hence do not need to 
	  * do some of the more primitive setup operations (such as setting up routing tables)
	  */
        if ((cpu_init_detectedx) || (!boot_cpu())) {
                if (last_boot_normal_x) {
                        image = get_property(dtb, "normal");
                } else {
                        image = get_property(dtb, "fallback");
                }
        } else {
		/* we are here because we need to set up baseline hardware after a full reset or power cycle */

		/* Nothing special needs to be done to find bus 0 */
		/* Allow the HT devices to be found */
		/* note that this will be filling in the DTB! */
	
	        stage1_enumerate_ht_chain();
	
		uart = get_property(dtb, "uart");

		stage1_sio_setup(uart);
	
	        /* Setup the ck804 */
	        stage1_ck804_enable_rom();
	
	        /* Is this a deliberate reset by the bios */
	        if (bios_reset_detected() && last_boot_normal_x) {
	                image = get_property(dtb, "normal");
	        }
	        /* This is the primary cpu; is this a normal or fallback boot? Determined mostly by CMOS settings */
	        else if (do_normal_boot()) {
	                image = get_property(dtb, "normal");
	        } else {
	                image = get_property(dtb, "fallback");
	        }
	}

	/* now, using the image property as a directory name, make the LAR calls to run files in the 
	  * directory. Uncompress as needed. Names are as in the LInux dentry cache, pointer + length
	  */
	dir = LAR_lookup(image->val.val, image->val.len);
	dir = LAR_walk(dir, "stage2");
	/* LAR_walk walks from a directory to another directory or file */
	file = LAR_walk(dir, "initram");
	if (! file) {
		/*uh oh!*/
	}
	/* initram is uncompressed. */
	code = code_pointer(file);
	/* we have to chain to the rest of LinuxBIOS, since CAR will go away */
	/* The stack will be gone. Pass two parameters to the initram: 
	  * pointer to function to run when initram is done, and property for booting. 
	  */

	(*code)(stage1_run_stage2, image);
}

void
stage1_run_stage2(struct property *image){
	struct property *uart;
	struct LAR_dir *dir;
	struct LAR_file *file = NULL;
	struct LAR_file *decompressor;
	void (*code)();
	
	dir = LAR_lookup(image->val.val, image->val.len);
	dir = LAR_walk(dir, "stage2");
	/* LAR_next just walks to the next file from the current one */
	while (file = LAR_next(dir, file)) {
		if (! strcmp(file->name, "initram"))
			continue;
		decompressor = find_decompressor(file);
		/* if the decompressor is null, then the function 
		 * just returns a pointer to the start of the file in FLASH 
		 */
		code = run_decompressor(file, decompressor);
		if (! code) {
			/* it's a bad day! */
		}
		(*code)();
	}

	/* NOTREACHED -- last file runs the payload */

}
