/* 2004.12 yhlu add dual core support */
/* 24 June 2005 Cleaned up dual core support Eric Biederman */

#include <console/console.h>
#include <cpu/cpu.h>
#include <cpu/x86/lapic.h>
#include <cpu/amd/dualcore.h>
#include <device/device.h>
#include <device/pci.h>
#include <pc80/mc146818rtc.h>
#include <smp/spinlock.h>
#include <cpu/x86/mtrr.h>
#include "../model_fxx/model_fxx_msr.h"
#include "../../../northbridge/amd/amdk8/cpu_rev.c"

static int first_time = 1;
static int disable_siblings = !CONFIG_LOGICAL_CPUS;

void amd_sibling_init(device_t cpu, struct node_core_id id)
{
	unsigned long i;
	unsigned siblings, max_siblings;

	/* On the bootstrap processor see if I want sibling cpus enabled */
	if (first_time) {
		first_time = 0;
		get_option(&disable_siblings, "dual_core");
	}

        siblings = cpuid_ecx(0x80000008) & 0xff;
	printk_debug("%d Sibling Cores found\n", siblings);

	/* For now assume all cpus have the same number of siblings */
	max_siblings = siblings + 1;

	/* Wishlist? make dual cores look like hyperthreading */

	/* See if I am a sibling cpu */
	if (disable_siblings && (id.coreid != 0)) {
		cpu->enabled = 0;
	}

	if (id.coreid == 0) {
		/* On the primary cpu find the siblings */
		for (i = 1; i <= siblings; i++) {
			struct device_path cpu_path;
			device_t new;
			/* Build the cpu device path */
			cpu_path.type = DEVICE_PATH_APIC;
			cpu_path.u.apic.apic_id = 
				(0x10 + i*0x10 + id.nodeid);

			new = alloc_dev(cpu->bus, &cpu_path);
			if (!new) {
				continue;
			}

                        new->path.u.apic.node_id = cpu->path.u.apic.node_id;
                        new->path.u.apic.core_id = i;
			/* Report what I have done */
			printk_debug("CPU: %s %s\n",
				dev_path(new), new->enabled?"enabled":"disabled");
		}
	}
}

struct node_core_id get_node_core_id(void)
{
        struct node_core_id id;
	unsigned siblings;
	/* Get the apicid at reset */
	id.nodeid = (cpuid_ebx(1) >> 24) & 0xff;
	id.coreid = 0;	
	/* Find out how many siblings we have */
	siblings = cpuid_ecx(0x80000008) & 0xff;
	if (siblings) {
		unsigned bits;
		msr_t msr;
		bits = 0;
		while ((1 << bits) <= siblings)
			bits++;

		msr = rdmsr(NB_CFG_MSR);
		if ((msr.hi >> (54-32)) & 1) {
		// when NB_CFG[54] is set, nodeid = ebx[27:25], coreid = ebx[24]
			id.coreid = id.nodeid & ((1 << bits) - 1);
			id.nodeid >>= bits;
		} else {
                // when NB_CFG[54] is clear, nodeid = ebx[26:24], coreid = ebx[27]
			id.coreid = id.nodeid >> 3;
			id.nodeid &= 7;
		}
	} else {
		if (!is_cpu_pre_e0()) {
			id.nodeid >>= 1;
		}
	}
	return id;
}

unsigned int read_nb_cfg_54(void)
{
        msr_t msr;
        msr = rdmsr(NB_CFG_MSR);
        return ( ( msr.hi >> (54-32)) & 1);
}

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

        int disable_siblings = !CONFIG_LOGICAL_CPUS;


        get_option(&disable_siblings, "dual_core");

        //get the nodes number
        dev = dev_find_slot(0, PCI_DEVFN(0x18,0));
        nodes = ((pci_read_config32(dev, 0x60)>>4) & 7) + 1;

	siblings = get_max_siblings(nodes);

	if(bsp_apic_id > 0) { // io apic could start from 0
		return 0;
	} else if(pci_read_config32(dev, 0x68) & ( (1<<17) | (1<<18)) )  { // enabled ext id but bsp = 0
		if(!disable_siblings) { return siblings + 1; }
		else { return 1; }
	}

	nb_cfg_54 = read_nb_cfg_54();

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
		printk_info("if the IO APIC device doesn't support 256 apic id, \r\n you need to set ENABLE_APIC_EXT_ID in auto.c so you can spare 16 id for ioapic\r\n");
		enable_apic_ext_id(nodes);
	}
	
	return apicid_base;
}

