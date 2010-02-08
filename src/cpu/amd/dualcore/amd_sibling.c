/* 2004.12 yhlu add dual core support */

#include <console/console.h>
#include <cpu/cpu.h>
#include <cpu/x86/lapic.h>
#include <cpu/amd/dualcore.h>
#include <device/device.h>
#include <device/pci.h>
#include <pc80/mc146818rtc.h>
#include <smp/spinlock.h>
#include <cpu/x86/mtrr.h>
#include <cpu/amd/model_fxx_msr.h>
#include <cpu/amd/model_fxx_rev.h>
#include <cpu/amd/amdk8_sysconf.h>

static int disable_siblings = !CONFIG_LOGICAL_CPUS;

#include "dualcore_id.c"

static int get_max_siblings(int nodes)
{
	device_t dev;
	int nodeid;
	int siblings=0;

	//get max siblings from all the nodes
	for(nodeid=0; nodeid<nodes; nodeid++){
		int j;
		dev = dev_find_slot(0, PCI_DEVFN(0x18+nodeid, 3));
		j = (pci_read_config32(dev, 0xe8) >> 12) & 3; 
		if(siblings < j) {
			siblings = j;
		}
	}
	
	return siblings;
}

static void enable_apic_ext_id(int nodes)
{
        device_t dev;
        int nodeid;

        //enable APIC_EXIT_ID all the nodes
        for(nodeid=0; nodeid<nodes; nodeid++){
                uint32_t val;
                dev = dev_find_slot(0, PCI_DEVFN(0x18+nodeid, 0));
                val = pci_read_config32(dev, 0x68);
		val |= (1<<17)|(1<<18);
		pci_write_config32(dev, 0x68, val); 
        }
}


unsigned get_apicid_base(unsigned ioapic_num)
{
	device_t dev;
	int nodes;
	unsigned apicid_base;
	int siblings;
	unsigned nb_cfg_54;
        int bsp_apic_id = lapicid(); // bsp apicid

        get_option(&disable_siblings, "dual_core");

        //get the nodes number
        dev = dev_find_slot(0, PCI_DEVFN(0x18,0));
        nodes = ((pci_read_config32(dev, 0x60)>>4) & 7) + 1;

	siblings = get_max_siblings(nodes);

	if(bsp_apic_id > 0) { // io apic could start from 0
		return 0;  
	} else if(pci_read_config32(dev, 0x68) & ( (1<<17) | (1<<18)) )  { // enabled ext id but bsp = 0
		return 1; 
	}

	nb_cfg_54 = read_nb_cfg_54();

#if 0
	//it is for all e0 single core and nc_cfg_54 low is set, but in the romstage.c stage we do not set that bit for it.
	if(nb_cfg_54 && (!disable_siblings) && (siblings == 0)) {
		//we need to check if e0 single core is there
		int i;
		for(i=0; i<nodes; i++) {
			if(is_e0_later_in_bsp(i)) {
				siblings = 1;
				break;
			}
		}
	}
#endif

	//contruct apicid_base

	if((!disable_siblings) && (siblings>0) ) {
		/* for 8 way dual core, we will used up apicid 16:16, actualy 16 is not allowed by current kernel
		and the kernel will try to get one that is small than 16 to make io apic work.
		I don't know when the kernel can support 256 apic id. (APIC_EXT_ID is enabled) */

	        //4:10 for two way  8:12 for four way 16:16 for eight way
		//Use CONFIG_MAX_PHYSICAL_CPUS instead of nodes for better consistency?
	        apicid_base = nb_cfg_54 ? (siblings+1) * nodes :  8 * siblings + nodes; 

	}
	else {
		apicid_base = nodes;
	}

	if((apicid_base+ioapic_num-1)>0xf) {
		// We need to enable APIC EXT ID
		printk_info("if the IO APIC device doesn't support 256 apic id, \r\n you need to set CONFIG_ENABLE_APIC_EXT_ID in romstage.c so you can spare 16 id for ioapic\r\n");
		enable_apic_ext_id(nodes);
	}
	
	return apicid_base;
}

#if 0
static int first_time = 1;

void amd_sibling_init(device_t cpu)
{
	unsigned i, siblings;
	struct cpuid_result result;
	unsigned nb_cfg_54;
	struct node_core_id id;

	/* On the bootstrap processor see if I want sibling cpus enabled */
	if (first_time) {
		first_time = 0;
		get_option(&disable_siblings, "dual_core");
	}
	result = cpuid(0x80000008);
	/* See how many sibling cpus we have */
	/* Is dualcore supported */
	siblings = (result.ecx & 0xff);
	if ( siblings < 1) {
		return;
	}

#if 1
	printk_debug("CPU: %u %d siblings\n",
		cpu->path.apic.apic_id,
		siblings);
#endif

	nb_cfg_54 = read_nb_cfg_54(); 
#if 1
	id = get_node_core_id(nb_cfg_54); // pre e0 nb_cfg_54 can not be set

	/* See if I am a sibling cpu */
	//if ((cpu->path.apic.apic_id>>(nb_cfg_54?0:3)) & siblings ) { // siblings = 1, 3, 7, 15,....
	//if ( ( (cpu->path.apic.apic_id>>(nb_cfg_54?0:3)) % (siblings+1) ) != 0 ) {
	if(id.coreid != 0) {
	        if (disable_siblings) {
                        cpu->enabled = 0;
                }
                return;
	}
#endif
		
	/* I am the primary cpu start up my siblings */

	for(i = 1; i <= siblings; i++) {
		struct device_path cpu_path;
		device_t new;
		/* Build the cpu device path */
		cpu_path.type = DEVICE_PATH_APIC;
		cpu_path.apic.apic_id = cpu->path.apic.apic_id + i * (nb_cfg_54?1:8);
                if(id.nodeid == 0) {
                        // need some special processing, because may the bsp is not lifted, but the core1 is lifted
			//defined in northbridge.c
                        if(sysconf.enabled_apic_ext_id && (!sysconf.lift_bsp_apicid)) {
                        	cpu->path.apic.apic_id += sysconf.apicid_offset;
                        }

                }


                /* See if I can find the cpu */
                new = find_dev_path(cpu->bus, &cpu_path);
		/* Allocate the new cpu device structure */
		if(!new) {
			new = alloc_dev(cpu->bus, &cpu_path);
			new->enabled = 1;
			new->initialized = 0;
		}

                new->path.apic.node_id = cpu->path.apic.node_id;
                new->path.apic.core_id = i;

#if 1
		printk_debug("CPU: %u has sibling %u\n", 
			cpu->path.apic.apic_id,
			new->path.apic.apic_id);
#endif

		if(new->enabled && !new->initialized)
			start_cpu(new);
	}
}
#endif

