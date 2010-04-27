#include "linux_syscall.h"
#include "linux_console.h"

inline int log2(int value)
{
	/* __builtin_bsr is a exactly equivalent to the x86 machine
	 * instruction with the exception that it returns -1
	 * when the value presented to it is zero.
	 * Otherwise __builtin_bsr returns the zero based index of
	 * the highest bit set.
	 */
	return __builtin_bsr(value);
}


static int smbus_read_byte(unsigned device, unsigned address)
{
	static const unsigned char dimm[] = {
0x80, 0x08, 0x07, 0x0d, 0x0a, 0x02, 0x48, 0x00, 0x04, 0x60, 0x70, 0x02, 0x82, 0x08, 0x08, 0x01,
0x0e, 0x04, 0x0c, 0x01, 0x02, 0x20, 0x00, 0x75, 0x70, 0x00, 0x00, 0x48, 0x30, 0x48, 0x2a, 0x40,
0x80, 0x80, 0x45, 0x45, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x33,
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,

0x80, 0x08, 0x07, 0x0d, 0x0a, 0x02, 0x48, 0x00, 0x04, 0x60, 0x70, 0x02, 0x82, 0x08, 0x08, 0x01,
0x0e, 0x04, 0x0c, 0x01, 0x02, 0x20, 0x00, 0x75, 0x70, 0x00, 0x00, 0x48, 0x30, 0x48, 0x2a, 0x40,
0x80, 0x80, 0x45, 0x45, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x33,
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	};
	return dimm[(device << 8) + address];
}

#define SMBUS_MEM_DEVICE_START 0x00
#define SMBUS_MEM_DEVICE_END   0x01
#define SMBUS_MEM_DEVICE_INC   1

/* Function 2 */
#define DRAM_CONFIG_HIGH   0x94
#define  DCH_MEMCLK_SHIFT  20
#define  DCH_MEMCLK_MASK   7
#define  DCH_MEMCLK_100MHZ 0
#define  DCH_MEMCLK_133MHZ 2
#define  DCH_MEMCLK_166MHZ 5
#define  DCH_MEMCLK_200MHZ 7

/* Function 3 */
#define NORTHBRIDGE_CAP    0xE8
#define  NBCAP_128Bit         0x0001
#define  NBCAP_MP             0x0002
#define  NBCAP_BIG_MP         0x0004
#define  NBCAP_ECC            0x0004
#define  NBCAP_CHIPKILL_ECC   0x0010
#define  NBCAP_MEMCLK_SHIFT   5
#define  NBCAP_MEMCLK_MASK    3
#define  NBCAP_MEMCLK_100MHZ  3
#define  NBCAP_MEMCLK_133MHZ  2
#define  NBCAP_MEMCLK_166MHZ  1
#define  NBCAP_MEMCLK_200MHZ  0
#define  NBCAP_MEMCTRL        0x0100

typedef unsigned char uint8_t;
typedef unsigned int uint32_t;

static unsigned spd_to_dimm(unsigned device)
{
	return (device - SMBUS_MEM_DEVICE_START);
}

static void disable_dimm(unsigned index)
{
	print_debug("disabling dimm");
	print_debug_hex8(index);
	print_debug("\r\n");
#if 0
	pci_write_config32(PCI_DEV(0, 0x18, 2), DRAM_CSBASE + (((index << 1)+0)<<2), 0);
	pci_write_config32(PCI_DEV(0, 0x18, 2), DRAM_CSBASE + (((index << 1)+1)<<2), 0);
#endif
}


struct mem_param {
	uint8_t cycle_time;
	uint32_t dch_memclk;
};

static const struct mem_param *get_mem_param(unsigned min_cycle_time)
{
	static const struct mem_param speed[] = {
		{
			.cycle_time = 0xa0,
			.dch_memclk = DCH_MEMCLK_100MHZ << DCH_MEMCLK_SHIFT,
		},
		{
			.cycle_time = 0x75,
			.dch_memclk = DCH_MEMCLK_133MHZ << DCH_MEMCLK_SHIFT,
		},
		{
			.cycle_time = 0x60,
			.dch_memclk = DCH_MEMCLK_166MHZ << DCH_MEMCLK_SHIFT,
		},
		{
			.cycle_time = 0x50,
			.dch_memclk = DCH_MEMCLK_200MHZ << DCH_MEMCLK_SHIFT,
		},
		{
			.cycle_time = 0x00,
		},
	};
	const struct mem_param *param;
	for(param = &speed[0]; param->cycle_time ; param++) {
		if (min_cycle_time > (param+1)->cycle_time) {
			break;
		}
	}
	if (!param->cycle_time) {
		die("min_cycle_time to low");
	}
	return param;
}

#if 1
static void debug(int c)
{
	print_debug_char(c);
	print_debug_char('\r');
	print_debug_char('\n');
}
#endif
static const struct mem_param *spd_set_memclk(void)
{
	/* Compute the minimum cycle time for these dimms */
	const struct mem_param *param;
	unsigned min_cycle_time, min_latency;
	unsigned device;
	uint32_t value;

	static const int latency_indicies[] = { 26, 23, 9 };
	static const unsigned char min_cycle_times[] = {
		[NBCAP_MEMCLK_200MHZ] = 0x50, /* 5ns */
		[NBCAP_MEMCLK_166MHZ] = 0x60, /* 6ns */
		[NBCAP_MEMCLK_133MHZ] = 0x75, /* 7.5ns */
		[NBCAP_MEMCLK_100MHZ] = 0xa0, /* 10ns */
	};


#if 0
	value = pci_read_config32(PCI_DEV(0, 0x18, 3), NORTHBRIDGE_CAP);
#else
	value = 0x50;
#endif
	min_cycle_time = min_cycle_times[(value >> NBCAP_MEMCLK_SHIFT) & NBCAP_MEMCLK_MASK];
	min_latency = 2;

#if 1
	print_debug("min_cycle_time: ");
	print_debug_hex8(min_cycle_time);
	print_debug(" min_latency: ");
	print_debug_hex8(min_latency);
	print_debug("\r\n");
#endif

	/* Compute the least latency with the fastest clock supported
	 * by both the memory controller and the dimms.
	 */
	for(device = SMBUS_MEM_DEVICE_START;
		device <= SMBUS_MEM_DEVICE_END;
		device += SMBUS_MEM_DEVICE_INC)
	{
		int new_cycle_time, new_latency;
		int index;
		int latencies;
		int latency;

		debug('A');
		/* First find the supported CAS latencies
		 * Byte 18 for DDR SDRAM is interpreted:
		 * bit 0 == CAS Latency = 1.0
		 * bit 1 == CAS Latency = 1.5
		 * bit 2 == CAS Latency = 2.0
		 * bit 3 == CAS Latency = 2.5
		 * bit 4 == CAS Latency = 3.0
		 * bit 5 == CAS Latency = 3.5
		 * bit 6 == TBD
		 * bit 7 == TBD
		 */
		new_cycle_time = 0xa0;
		new_latency = 5;

		latencies = smbus_read_byte(device, 18);
		if (latencies <= 0) continue;

		debug('B');
		/* Compute the lowest cas latency supported */
		latency = log2(latencies) -2;

		/* Loop through and find a fast clock with a low latency */
		for(index = 0; index < 3; index++, latency++) {
			int value;
			debug('C');
			if ((latency < 2) || (latency > 4) ||
				(!(latencies & (1 << latency)))) {
				continue;
			}
			debug('D');
			value = smbus_read_byte(device, latency_indicies[index]);
			if (value < 0) continue;

			debug('E');
			/* Only increase the latency if we decreas the clock */
			if ((value >= min_cycle_time) && (value < new_cycle_time)) {
				new_cycle_time = value;
				new_latency = latency;
#if 1
				print_debug("device: ");
				print_debug_hex8(device);
				print_debug(" new_cycle_time: ");
				print_debug_hex8(new_cycle_time);
				print_debug(" new_latency: ");
				print_debug_hex8(new_latency);
				print_debug("\r\n");
#endif
			}
			debug('G');
		}
		debug('H');
#if 1
		print_debug("device: ");
		print_debug_hex8(device);
		print_debug(" new_cycle_time: ");
		print_debug_hex8(new_cycle_time);
		print_debug(" new_latency: ");
		print_debug_hex8(new_latency);
		print_debug("\r\n");
#endif
		if (new_latency > 4){
			continue;
		}
		debug('I');
		/* Does min_latency need to be increased? */
		if (new_cycle_time > min_cycle_time) {
			min_cycle_time = new_cycle_time;
		}
		/* Does min_cycle_time need to be increased? */
		if (new_latency > min_latency) {
			min_latency = new_latency;
		}
#if 1
		print_debug("device: ");
		print_debug_hex8(device);
		print_debug(" min_cycle_time: ");
		print_debug_hex8(min_cycle_time);
		print_debug(" min_latency: ");
		print_debug_hex8(min_latency);
		print_debug("\r\n");
#endif
	}
	/* Make a second pass through the dimms and disable
	 * any that cannot support the selected memclk and cas latency.
	 */
	for(device = SMBUS_MEM_DEVICE_START;
		device <= SMBUS_MEM_DEVICE_END;
		device += SMBUS_MEM_DEVICE_INC)
	{
		int latencies;
		int latency;
		int index;
		int value;
		int dimm;
		latencies = smbus_read_byte(device, 18);
		if (latencies <= 0) {
			goto dimm_err;
		}

		/* Compute the lowest cas latency supported */
		latency = log2(latencies) -2;

		/* Walk through searching for the selected latency */
		for(index = 0; index < 3; index++, latency++) {
			if (!(latencies & (1 << latency))) {
				continue;
			}
			if (latency == min_latency)
				break;
		}
		/* If I can't find the latency or my index is bad error */
		if ((latency != min_latency) || (index >= 3)) {
			goto dimm_err;
		}

		/* Read the min_cycle_time for this latency */
		value = smbus_read_byte(device, latency_indicies[index]);

		/* All is good if the selected clock speed
		 * is what I need or slower.
		 */
		if (value <= min_cycle_time) {
			continue;
		}
		/* Otherwise I have an error, disable the dimm */
	dimm_err:
		disable_dimm(spd_to_dimm(device));
	}
#if 1
	print_debug("min_cycle_time: ");
	print_debug_hex8(min_cycle_time);
	print_debug(" min_latency: ");
	print_debug_hex8(min_latency);
	print_debug("\r\n");
#endif
	/* Now that I know the minimum cycle time lookup the memory parameters */
	param = get_mem_param(min_cycle_time);

#if 0
	/* Update DRAM Config High with our selected memory speed */
	value = pci_read_config32(PCI_DEV(0, 0x18, 2), DRAM_CONFIG_HIGH);
	value &= ~(DCH_MEMCLK_MASK << DCH_MEMCLK_SHIFT);
	value |= param->dch_memclk;
	pci_write_config32(PCI_DEV(0, 0x18, 2), DRAM_CONFIG_HIGH, value);

	static const unsigned latencies[] = { 1, 5, 2 };
	/* Update DRAM Timing Low wiht our selected cas latency */
	value = pci_read_config32(PCI_DEV(0, 0x18, 2), DRAM_CONFIG_LOW);
	value &= ~7;
	value |= latencies[min_latency - 2];
	pci_write_config32(PCI_DEV(0, 0x18, 2), DRAM_CONFIG_LOW, value);
#endif

	return param;
}

static void main(void)
{
	const struct mem_param *param;
	param = spd_set_memclk();
	_exit(0);
}
