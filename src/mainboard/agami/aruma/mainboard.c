#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <cpu/x86/msr.h>
#include <part/hard_reset.h>
#include <device/smbus.h>
#include <delay.h>

#include <arch/io.h>
#include "../../../northbridge/amd/amdk8/northbridge.h"
#include <cpu/amd/model_fxx_rev.h>
#include "chip.h"

#include "pc80/mc146818rtc.h"


#undef DEBUG
#define DEBUG 0
#if DEBUG 
static void debug_init(device_t dev)
{
	unsigned bus;
	unsigned devfn;
#if 0
	for(bus = 0; bus < 256; bus++) {
		for(devfn = 0; devfn < 256; devfn++) {
			int i;
			dev = dev_find_slot(bus, devfn);
			if (!dev) {
				continue;
			}
			if (!dev->enabled) {
				continue;
			}
			printk_info("%02x:%02x.%0x aka %s\n", 
				bus, devfn >> 3, devfn & 7, dev_path(dev));
			for(i = 0; i < 256; i++) {
				if ((i & 0x0f) == 0) {
					printk_info("%02x:", i);
				}
				printk_info(" %02x", pci_read_config8(dev, i));
				if ((i & 0x0f) == 0xf) {
					printk_info("\n");
				}
			}
			printk_info("\n");
		}
	}
#endif
#if 0
	msr_t msr;
	unsigned index;
	unsigned eax, ebx, ecx, edx;
	index = 0x80000007;
	printk_debug("calling cpuid 0x%08x\n", index);
	asm volatile(
		"cpuid"
		: "=a" (eax), "=b" (ebx), "=c" (ecx), "=d" (edx)
		: "a" (index)
		);
	printk_debug("cpuid[%08x]: %08x %08x %08x %08x\n",
		index, eax, ebx, ecx, edx);
	if (edx & (3 << 1)) {
		index = 0xC0010042;
		printk_debug("Reading msr: 0x%08x\n", index);
		msr = rdmsr(index);
		printk_debug("msr[0x%08x]: 0x%08x%08x\n",
			index, msr.hi, msr.hi);
	}
#endif
}

static void debug_noop(device_t dummy)
{
}

static struct device_operations debug_operations = {
	.read_resources   = debug_noop,
	.set_resources    = debug_noop,
	.enable_resources = debug_noop,
	.init             = debug_init,
};

static unsigned int scan_root_bus(device_t root, unsigned int max)
{
	struct device_path path;
	device_t debug;
	max = root_dev_scan_bus(root, max);
	path.type = DEVICE_PATH_PNP;
	path.u.pnp.port   = 0;
	path.u.pnp.device = 0;
	debug = alloc_dev(&root->link[1], &path);
	debug->ops = &debug_operations;
	return max;
}
#endif

#if 1
static void handle_smbus_error(int value, const char *msg)
{
	if (value >= 0) {
		return;
	}
	switch(value) {
	case SMBUS_WAIT_UNTIL_READY_TIMEOUT:
		printk_emerg("SMBUS wait until ready timed out - resetting...");
		hard_reset();
		break;
	case SMBUS_WAIT_UNTIL_DONE_TIMEOUT:
		printk_emerg("SMBUS wait until done timed out - resetting...");
		hard_reset();
		break;
	default:
		die(msg);
		break;
	}
}

#define ADM1026_DEVICE 0x2d /* Either 0x2c, 0x2d or 0x2e. */
#define ADM1026_REG_CONFIG1 0x00
#define CFG1_MONITOR     0x01
#define CFG1_INT_ENABLE  0x02
#define CFG1_INT_CLEAR   0x04
#define CFG1_AIN8_9      0x08
#define CFG1_THERM_HOT   0x10
#define CFT1_DAC_AFC     0x20
#define CFG1_PWM_AFC     0x40
#define CFG1_RESET       0x80
#define ADM1026_REG_CONFIG2 0x01
#define ADM1026_REG_CONFIG3 0x07



#define BILLION 1000000000UL

static void  verify_cpu_voltage(const char *name, 
	device_t dev, unsigned int reg, 
	unsigned factor, unsigned cpu_volts, unsigned delta)
{
	unsigned nvolts_lo, nvolts_hi;
	unsigned cpuvolts_hi, cpuvolts_lo;
	int value;
	int loops;

	loops = 1000;
	do {
		value = smbus_read_byte(dev, reg);
		handle_smbus_error(value, "SMBUS read byte failed");
	} while ((--loops > 0) && value == 0);
	/* Convert the byte value to nanoVolts.
	 * My accuracy is nowhere near that good but I don't
	 * have to round so the math is simple. 
	 * I can only go up to about 4.2 Volts this way so my range is
	 * limited.
	 */
	nvolts_lo = ((unsigned)value * factor);
	nvolts_hi = nvolts_lo + factor - 1;
	/* Get the range of acceptable cpu voltage values */
	cpuvolts_lo = cpu_volts - delta;
	cpuvolts_hi = cpu_volts + delta;
	if ((nvolts_lo < cpuvolts_lo) || (nvolts_hi > cpuvolts_hi)) {
		printk_emerg("%s at (%u.%09u-%u.%09u)Volts expected %u.%09u+/-%u.%09uVolts\n",
			name,
			nvolts_lo/BILLION, nvolts_lo%BILLION,
			nvolts_hi/BILLION, nvolts_hi%BILLION,
			cpu_volts/BILLION, cpu_volts%BILLION,
			delta/BILLION, delta%BILLION);
		die("");
	}
	printk_info("%s at (%u.%09u-%u.%09u)Volts\n",
		name,
		nvolts_lo/BILLION, nvolts_lo%BILLION,
		nvolts_hi/BILLION, nvolts_hi%BILLION);
		
}

static void adm1026_enable_monitoring(device_t dev)
{
	int result;
	result = smbus_read_byte(dev, ADM1026_REG_CONFIG1);
	handle_smbus_error(result, "ADM1026: cannot read config1");

	result = (result | CFG1_MONITOR) & ~(CFG1_INT_CLEAR | CFG1_RESET);
	result = smbus_write_byte(dev, ADM1026_REG_CONFIG1, result);
	handle_smbus_error(result, "ADM1026: cannot write to config1");

	result = smbus_read_byte(dev, ADM1026_REG_CONFIG1);
	handle_smbus_error(result, "ADM1026: cannot reread config1");
	if (!(result & CFG1_MONITOR)) {
		die("ADM1026: monitoring would not enable");
	}
}


static unsigned k8_cpu_volts(void)
{
	unsigned volts = ~0;
	if (is_cpu_c0()) {
		volts = 1500000000;
	}
	if (is_cpu_b3()) {
		volts = 1550000000;
	}
	return volts;
}

static void verify_cpu_voltages(device_t dev)
{
	unsigned cpu_volts;
	unsigned delta;
#if 0
	delta =  50000000;
#else
	delta =  75000000;
#endif
	cpu_volts = k8_cpu_volts();
	if (cpu_volts == ~0) {
		printk_info("Required cpu voltage unknwon not checking\n");
		return; 
	}
	/* I need to read registers 0x37 == Ain7CPU1 core 0x2d == VcppCPU0 core */
	/* CPU1 core 
	 * The sensor has a range of 0-2.5V and reports in
	 * 256 distinct steps.
	 */
	verify_cpu_voltage("CPU1 Vcore", dev, 0x37, 9765625, 
		cpu_volts, delta);
	/* CPU0 core 
	 * The sensor has range of 0-3.0V and reports in 
	 * 256 distinct steps.
	 */
	verify_cpu_voltage("CPU0 Vcore", dev, 0x2d, 11718750, 
		cpu_volts, delta);
}

#define SMBUS_MUX 0x71

static void do_verify_cpu_voltages(void)
{
	device_t smbus_dev;
	device_t mux, sensor;
	struct device_path mux_path, sensor_path;
	int result;
	int mux_setting;
	
	/* Find the smbus controller */
	smbus_dev = dev_find_device(0x1022, 0x746b, 0);
	if (!smbus_dev) {
		die("SMBUS controller not found\n");
	}

	/* Find the smbus mux */
	mux_path.type         = DEVICE_PATH_I2C;
	mux_path.u.i2c.device = SMBUS_MUX;
	mux = find_dev_path(smbus_dev, &mux_path);
	if (!mux) {
		die("SMBUS mux not found\n");
	}

	/* Find the adm1026 sensor */
	sensor_path.type         = DEVICE_PATH_I2C;
	sensor_path.u.i2c.device = ADM1026_DEVICE;
	sensor = find_dev_path(mux, &sensor_path);
	if (!sensor) {
		die("ADM1026 not found\n");
	}
	
	/* Set the mux to see the temperature sensors */
	mux_setting = 1;
	result = smbus_send_byte(mux, mux_setting);
	handle_smbus_error(result, "SMBUS send byte failed\n");

	result = smbus_recv_byte(mux);
	handle_smbus_error(result, "SMBUS recv byte failed\n");
	if (result != mux_setting) {
		printk_emerg("SMBUS mux would not set to %d\n", mux_setting);
		die("");
	}

	adm1026_enable_monitoring(sensor);

	/* It takes 11.38ms to read a new voltage sensor value */
	mdelay(12);

	/* Read the cpu voltages and make certain everything looks sane */
	verify_cpu_voltages(sensor);
}
#else
#define do_verify_cpu_voltages() do {} while(0)
#endif


static void fixup_aruma(void)
{
	msr_t msr;

	/* bit 6 (0x40) in MSR 0xC0010015 
	 * disables the TLB cache flush filter
	 */
	msr=rdmsr(0xC0010015);
	msr.lo |= 0x40;
	wrmsr(0xC0010015, msr);
}


static void mainboard_init(device_t dev)
{
	root_dev_init(dev);

	printk_info("Initializing mainboard components... ");
	// do_verify_cpu_voltages();
	printk_info("ok\n");

	printk_info("Initializing mainboard specific functions... ");
	fixup_aruma();
	printk_info("ok\n");
}

static void enable_dev(struct device *dev)
{
	dev->ops->init = mainboard_init;
}

struct chip_operations mainboard_agami_aruma_ops = {
	CHIP_NAME("AGAMI Aruma Mainboard")
	.enable_dev = enable_dev, 
};

