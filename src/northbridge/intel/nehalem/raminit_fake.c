static u32 gav_real(int line, u32 in)
{
	//  printf ("%d: GAV: %x\n", line, in);
	return in;
}

#define gav(x) gav_real (__LINE__, (x))

#include <parse.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void pm_wait(u16 us);

#define ARRAY_SIZE(array) (sizeof (array) / sizeof (array[0]))

#define CONFIG_SMM_TSEG_SIZE (8 << 20)

#define MTRR_TYPE_WRPROT     5
#define MTRRdefTypeEn		(1 << 11)
#define MTRRphysBase_MSR(reg) (0x200 + 2 * (reg))
#define MTRRphysMask_MSR(reg) (0x200 + 2 * (reg) + 1)
#define MTRRcap_MSR     0x0fe

#include "include/cpu/intel/speedstep.h"
#include "include/cpu/intel/turbo.h"

#define PCI_VENDOR_ID		0x00	/* 16 bits */
#define PCI_COMMAND		0x04	/* 16 bits */
#define  PCI_COMMAND_IO		0x1	/* Enable response in I/O space */
#define  PCI_COMMAND_MEMORY	0x2	/* Enable response in Memory space */
#define  PCI_COMMAND_MASTER	0x4	/* Enable bus mastering */
#define  PCI_COMMAND_SPECIAL	0x8	/* Enable response to special cycles */
#define  PCI_COMMAND_INVALIDATE	0x10	/* Use memory write and invalidate */
#define  PCI_COMMAND_VGA_PALETTE 0x20	/* Enable palette snooping */
#define  PCI_COMMAND_PARITY	0x40	/* Enable parity checking */
#define  PCI_COMMAND_WAIT 	0x80	/* Enable address/data stepping */
#define  PCI_COMMAND_SERR	0x100	/* Enable SERR */
#define  PCI_COMMAND_FAST_BACK	0x200	/* Enable back-to-back writes */
#define PCI_REVISION_ID 8
#define PCI_DEVICE_ID 2

#define CONFIG_MMCONF_BASE_ADDRESS 0xe0000000

#define PCI_SUBSYSTEM_VENDOR_ID 0x2c

static void write32(u32 addr, u32 val)
{
	struct opcode op;
	if (!fetch_opcode(&op)) {
		printf("EOF\n");
		exit(1);
	}
	if (op.is_in || op.data_width != 32 || op.addr != addr || op.val != val
	    || op.type != MEM) {
		printf("Bad %d: %x, %x vs %x, %llx\n", __LINE__, addr, val,
		       op.addr, op.val);
		exit(1);
	}
}

static void write16(u32 addr, u16 val)
{
	struct opcode op;
	if (!fetch_opcode(&op)) {
		printf("EOF\n");
		exit(1);
	}
	if (op.is_in || op.data_width != 16 || op.addr != addr || op.val != val
	    || op.type != MEM) {
		printf("Bad %d: %x, %x vs %x, %llx\n", __LINE__, addr, val,
		       op.addr, op.val);
		exit(1);
	}
}

static void write8(u32 addr, u8 val)
{
	struct opcode op;
	fetch_opcode(&op);
	if (op.is_in || op.data_width != 8 || op.addr != addr || op.val != val
	    || op.type != MEM) {
		printf("Bad %d\n", __LINE__);
		exit(1);
	}
}

static int smbus_read_byte(u32 dev, u32 addr)
{
	struct opcode op;
	if (!fetch_opcode(&op)) {
		printf("EOF\n");
		exit(1);
	}
	if (!op.is_in || op.data_width != 8 || op.addr != addr
	    || op.type != SMBUS || op.dev != dev) {
		printf("Bad %d: %x, %d vs %x, %d\n", __LINE__, op.addr, SMBUS,
		       addr, op.type);
		exit(1);
	}
	return (signed short)op.val;
}

static int smbus_block_read(u32 dev, u32 addr, u32 len, u8 * block)
{
	struct opcode op;
	if (!fetch_opcode(&op)) {
		printf("EOF\n");
		exit(1);
	}
	if (!op.is_in || op.data_width != len || op.addr != addr
	    || op.type != OPCODE_SMBUS_BLOCK || op.dev != dev) {
		printf("Bad %d: %x, %d vs %x, %d\n", __LINE__, op.addr,
		       OPCODE_SMBUS_BLOCK, addr, op.type);
		exit(1);
	}
	memcpy(block, &op.val, len);
	return 0;
}

static int smbus_block_write(u32 dev, u32 addr, u32 len, const u8 * block)
{
	struct opcode op;
	if (!fetch_opcode(&op)) {
		printf("EOF\n");
		exit(1);
	}
	if (op.is_in || op.data_width != len || op.addr != addr
	    || op.type != OPCODE_SMBUS_BLOCK || op.dev != dev
	    || memcmp(block, &op.val, len) != 0) {
		printf("Bad %d: %x, %d vs %x, %d\n", __LINE__, op.addr,
		       OPCODE_SMBUS_BLOCK, addr, op.type);
		exit(1);
	}
	return 0;
}

static void smbus_write_byte(u32 dev, u32 addr, u8 val)
{
	struct opcode op;
	if (!fetch_opcode(&op)) {
		printf("EOF\n");
		exit(1);
	}
	if (op.is_in || op.data_width != 8 || op.addr != addr
	    || op.type != SMBUS || op.dev != dev || op.val != val) {
		printf("Bad %d: %x, %d vs %x, %d\n", __LINE__, addr, SMBUS,
		       op.addr, op.type);
		exit(1);
	}
}

static void write_mchbar32(u32 addr, u32 val)
{
	struct opcode op;
	if (!fetch_opcode(&op)) {
		printf("EOF\n");
		exit(1);
	}
	if (op.is_in || op.data_width != 32 || op.addr != addr || op.val != val
	    || op.type != OPCODE_MCHBAR) {
		printf("Bad [%x] = %x vs [%x] = %llx\n", addr, val, op.addr,
		       op.val);
		exit(1);
	}
}

static void write_acpi32(u32 addr, u32 val)
{
	struct opcode op;
	if (!fetch_opcode(&op)) {
		printf("EOF\n");
		exit(1);
	}
	if (op.is_in || op.data_width != 32 || op.addr != addr || op.val != val
	    || op.type != ACPI) {
		printf("Bad [%x] = %x vs [%x] = %llx\n", addr, val, op.addr,
		       op.val);
		exit(1);
	}
}

static void write_mchbar16(u32 addr, u16 val)
{
	struct opcode op;
	fetch_opcode(&op);
	if (op.is_in || op.data_width != 16 || op.addr != addr || op.val != val
	    || op.type != OPCODE_MCHBAR) {
		printf("Bad %d: %x, %x vs %x, %llx\n", __LINE__, addr, val,
		       op.addr, op.val);
		exit(1);
	}
}

static void write_acpi16(u32 addr, u16 val)
{
	struct opcode op;
	fetch_opcode(&op);
	if (op.is_in || op.data_width != 16 || op.addr != addr || op.val != val
	    || op.type != ACPI) {
		printf("Bad %d: %x, %x vs %x, %llx\n", __LINE__, addr, val,
		       op.addr, op.val);
		exit(1);
	}
}

static void write_tco16(u32 addr, u16 val)
{
	struct opcode op;
	fetch_opcode(&op);
	if (op.is_in || op.data_width != 16 || op.addr != addr || op.val != val
	    || op.type != TCO) {
		printf("Bad %d: %x, %x vs %x, %llx\n", __LINE__, addr, val,
		       op.addr, op.val);
		exit(1);
	}
}

static void write_tco8(u32 addr, u8 val)
{
	struct opcode op;
	fetch_opcode(&op);
	if (op.is_in || op.data_width != 8 || op.addr != addr || op.val != val
	    || op.type != TCO) {
		printf("Bad %d: %x, %x vs %x, %llx\n", __LINE__, addr, val,
		       op.addr, op.val);
		exit(1);
	}
}

static void write_mchbar8(u32 addr, u8 val)
{
	struct opcode op;
	fetch_opcode(&op);
	if (op.is_in || op.data_width != 8 || op.addr != addr || op.val != val
	    || op.type != OPCODE_MCHBAR) {
		printf("Bad %d: %x, %x vs %x, %llx\n", __LINE__, addr, val,
		       op.addr, op.val);
		exit(1);
	}
}

static u32 read_mchbar32(u32 addr)
{
	struct opcode op;
	if (!fetch_opcode(&op)) {
		printf("EOF\n");
		exit(1);
	}
	if (!op.is_in || op.data_width != 32 || op.addr != addr
	    || op.type != OPCODE_MCHBAR) {
		printf("Bad %d\n", __LINE__);
		exit(1);
	}
	return op.val;
}

static u32 read_mchbar32_bypass(u32 addr)
{
	return read_mchbar32(addr);
}

static u32 read_acpi32(u32 addr)
{
	struct opcode op;
	if (!fetch_opcode(&op)) {
		printf("EOF\n");
		exit(1);
	}
	if (!op.is_in || op.data_width != 32 || op.addr != addr
	    || op.type != ACPI) {
		printf("Bad %d\n", __LINE__);
		exit(1);
	}
	return op.val;
}

static u16 read_mchbar16(u32 addr)
{
	struct opcode op;
	fetch_opcode(&op);
	if (!op.is_in || op.data_width != 16 || op.addr != addr
	    || op.type != OPCODE_MCHBAR) {
		printf("Bad %d\n", __LINE__);
		exit(1);
	}
	return op.val;
}

static u16 read_tco16(u32 addr)
{
	struct opcode op;
	fetch_opcode(&op);
	if (!op.is_in || op.data_width != 16 || op.addr != addr
	    || op.type != TCO) {
		printf("Bad %d\n", __LINE__);
		exit(1);
	}
	return op.val;
}

static u16 read_acpi16(u32 addr)
{
	struct opcode op;
	fetch_opcode(&op);
	if (!op.is_in || op.data_width != 16 || op.addr != addr
	    || op.type != ACPI) {
		printf("Bad %d: %x, 16 vs %x, %d\n", __LINE__, addr, op.addr,
		       op.data_width);
		exit(1);
	}
	return op.val;
}

static u8 read_mchbar8(u32 addr)
{
	struct opcode op;
	if (!fetch_opcode(&op)) {
		printf("EOF\n");
		exit(1);
	}
	if (!op.is_in || op.data_width != 8 || op.addr != addr
	    || op.type != OPCODE_MCHBAR) {
		printf("Bad %d: %x vs %x\n", __LINE__, addr, op.addr);
		exit(1);
	}
	return op.val;
}

static u8 read_tco8(u32 addr)
{
	struct opcode op;
	if (!fetch_opcode(&op)) {
		printf("EOF\n");
		exit(1);
	}
	if (!op.is_in || op.data_width != 8 || op.addr != addr
	    || op.type != TCO) {
		printf("Bad %d: %x vs %x\n", __LINE__, addr, op.addr);
		exit(1);
	}
	return op.val;
}

static u32 read32(u32 addr)
{
	struct opcode op;
	fetch_opcode(&op);
	if (!op.is_in || op.data_width != 32 || op.addr != addr
	    || op.type != MEM) {
		printf("Bad %d: %x vs %x\n", __LINE__, addr, op.addr);
		exit(1);
	}
	return op.val;
}

static u64 read64(u32 addr)
{
	struct opcode op;
	if (!fetch_opcode(&op)) {
		printf("EOF\n");
		exit(1);
	}
	if (!op.is_in || op.data_width != 64 || op.addr != addr
	    || op.type != MEM) {
		printf("Bad %d: %x vs %x\n", __LINE__, addr, op.addr);
		exit(1);
	}
	return op.val;
}

static void clflush(u32 addr)
{
	struct opcode op;
	if (!fetch_opcode(&op)) {
		printf("EOF\n");
		exit(1);
	}

	if (op.addr != addr || op.type != CLFLUSH) {
		printf("Bad %d: %x vs %x\n", __LINE__, addr, op.addr);
		exit(1);
	}
}

static void read128(u32 addr, u64 * out)
{
	out[0] = read64(addr);
	out[1] = read64(addr + 8);
}

static u16 read16(u32 addr)
{
	struct opcode op;
	fetch_opcode(&op);
	if (!op.is_in || op.data_width != 16 || op.addr != addr
	    || op.type != MEM) {
		printf("Bad %d: %x vs %x\n", __LINE__, addr, op.addr);
		exit(1);
	}
	return op.val;
}

static u8 read8(u32 addr)
{
	struct opcode op;
	fetch_opcode(&op);
	if (!op.is_in || op.data_width != 8 || op.addr != addr
	    || op.type != MEM) {
		printf("Bad %d\n", __LINE__);
		exit(1);
	}
	return op.val;
}

static u8 inb(u32 addr)
{
	struct opcode op;
	fetch_opcode(&op);
	if (!op.is_in || op.data_width != 8 || op.addr != addr
	    || op.type != PCIO) {
		printf("Bad %d\n", __LINE__);
		exit(1);
	}
	return op.val;
}

static void outb(u8 val, u32 addr)
{
	struct opcode op;
	if (!fetch_opcode(&op)) {
		printf("EOF\n");
		exit(1);
	}
	if (op.is_in || op.data_width != 8 || op.addr != addr || op.type != PCIO
	    || op.val != val) {
		printf("Bad %d: %x, %x, 8, %d, 0 vs %x, %llx, %d, %d, %d\n",
		       __LINE__, addr, val, PCIO, op.addr, op.val,
		       op.data_width, op.type, op.is_in);
		printf("%x, %llx, %d\n", val, op.val, op.val != val);
		exit(1);
	}
}

static void outw(u16 val, u32 addr)
{
	struct opcode op;
	fetch_opcode(&op);
	if (op.is_in || op.data_width != 16 || op.addr != addr
	    || op.type != PCIO || op.val != val) {
		printf("Bad %d: %x, %x vs %x, %llx\n", __LINE__, addr, val,
		       op.addr, op.val);
		exit(1);
	}
}

static void outl(u32 val, u32 addr)
{
	struct opcode op;
	fetch_opcode(&op);
	if (op.is_in || op.data_width != 32 || op.addr != addr
	    || op.type != PCIO || op.val != val) {
		printf("Bad %d\n", __LINE__);
		exit(1);
	}
}

static u32 inl(u32 addr)
{
	struct opcode op;
	fetch_opcode(&op);
	if (!op.is_in || op.data_width != 32 || op.addr != addr
	    || op.type != PCIO) {
		printf("Bad %d\n", __LINE__);
		exit(1);
	}
	return op.val;
}

static u16 inw(u32 addr)
{
	struct opcode op;
	fetch_opcode(&op);
	if (!op.is_in || op.data_width != 16 || op.addr != addr
	    || op.type != PCIO) {
		printf("Bad %d\n", __LINE__);
		exit(1);
	}
	return op.val;
}

static void pci_mm_write8(int bus, int dev, int func, u32 addr, u8 val)
{
	struct opcode op;
	if (bus == 0xff) {
		write8(DEFAULT_PCIEXBAR | (bus << 20) | (dev << 15) |
		       (func << 12) | addr, val);
		return;
	}

	fetch_opcode(&op);
	if (op.is_in || op.data_width != 8 || op.addr != addr
	    || op.type != PCIMM || op.dev != dev || op.func != func
	    || op.bus != bus || op.val != val) {
		printf("Bad %d\n", __LINE__);
		exit(1);
	}
}

static void pci_write8(int bus, int dev, int func, u32 addr, u8 val)
{
	struct opcode op;
	fetch_opcode(&op);
	if (op.is_in || op.data_width != 8 || op.addr != addr || op.type != PCI
	    || op.dev != dev || op.func != func || op.bus != bus
	    || op.val != val) {
		printf("Bad %d\n", __LINE__);
		exit(1);
	}
}

static void pci_write16(int bus, int dev, int func, u32 addr, u16 val)
{
	struct opcode op;
	fetch_opcode(&op);
	if (op.is_in || op.data_width != 16 || op.addr != addr || op.type != PCI
	    || op.dev != dev || op.func != func || op.bus != bus
	    || op.val != val) {
		printf("Bad %d\n", __LINE__);
		exit(1);
	}
}

static void pci_mm_write16(int bus, int dev, int func, u32 addr, u16 val)
{
	struct opcode op;
	fetch_opcode(&op);
	if (op.is_in || op.data_width != 16 || op.addr != addr
	    || op.type != PCIMM || op.dev != dev || op.func != func
	    || op.bus != bus || op.val != val) {
		printf("Bad %d\n", __LINE__);
		exit(1);
	}
}

static void pci_mm_write32(int bus, int dev, int func, u32 addr, u32 val)
{
	struct opcode op;
	if (bus == 0xff) {
		write32(DEFAULT_PCIEXBAR | (bus << 20) | (dev << 15) |
			(func << 12) | addr, val);
		return;
	}

	if (!fetch_opcode(&op)) {
		printf("EOF\n");
		exit(1);
	}

	if (op.is_in || op.data_width != 32 || op.addr != addr
	    || op.type != PCIMM || op.dev != dev || op.func != func
	    || op.bus != bus || op.val != val) {
		printf("Bad %d\n", __LINE__);
		exit(1);
	}
}

static void pci_write32(int bus, int dev, int func, u32 addr, u32 val)
{
	struct opcode op;
	if (!fetch_opcode(&op)) {
		printf("EOF\n");
		exit(1);
	}
	if (op.is_in || op.data_width != 32 || op.addr != addr || op.type != PCI
	    || op.dev != dev || op.func != func || op.bus != bus
	    || op.val != val) {
		printf("Bad %d\n", __LINE__);
		exit(1);
	}
}

static u8 pci_read8(int bus, int dev, int func, u32 addr)
{
	struct opcode op;
	fetch_opcode(&op);
	if (!op.is_in || op.data_width != 8 || op.addr != addr || op.type != PCI
	    || op.dev != dev || op.func != func || op.bus != bus) {
		printf("Bad %d\n", __LINE__);
		exit(1);
	}
	return op.val;
}

static u8 nvram_read(u8 addr)
{
	struct opcode op;
	fetch_opcode(&op);
	if (!op.is_in || op.addr != addr || op.type != NVRAM) {
		printf("Bad %d\n", __LINE__);
		exit(1);
	}
	return op.val;
}

static void nvram_write(u8 addr, u8 val)
{
	struct opcode op;
	fetch_opcode(&op);
	if (op.is_in || op.addr != addr || op.type != NVRAM || op.val != val) {
		printf("Bad %d\n", __LINE__);
		exit(1);
	}
}

static u8 pci_mm_read8(int bus, int dev, int func, u32 addr)
{
	struct opcode op;
	fetch_opcode(&op);
	if (!op.is_in || op.data_width != 8 || op.addr != addr
	    || op.type != PCIMM || op.dev != op.dev || op.func != op.func
	    || op.bus != op.bus) {
		printf("Bad %d\n", __LINE__);
		exit(1);
	}
	return op.val;
}

static u16 pci_mm_read16(int bus, int dev, int func, u32 addr)
{
	struct opcode op;
	if (!fetch_opcode(&op)) {
		printf("EOF\n");
		exit(1);
	}
	if (!op.is_in || op.data_width != 16 || op.addr != addr
	    || op.type != PCIMM || op.dev != dev || op.func != func
	    || op.bus != bus) {
		printf("Bad %d: %x vs %x\n", __LINE__, addr, op.addr);
		exit(1);
	}
	return op.val;
}

static u16 pci_read16(int bus, int dev, int func, u32 addr)
{
	struct opcode op;
	fetch_opcode(&op);
	if (!op.is_in || op.data_width != 16 || op.addr != addr
	    || op.type != PCI || op.dev != op.dev || op.func != op.func
	    || op.bus != op.bus) {
		printf("Bad %d\n", __LINE__);
		exit(1);
	}
	return op.val;
}

static u32 pci_mm_read32(int bus, int dev, int func, u32 addr)
{
	struct opcode op;

	if (bus == 0xff)
		return read32(DEFAULT_PCIEXBAR | (bus << 20) | (dev << 15) |
			      (func << 12) | addr);

	fetch_opcode(&op);
	if (!op.is_in || op.data_width != 32 || op.addr != addr
	    || op.type != PCIMM || op.dev != op.dev || op.func != op.func
	    || op.bus != op.bus) {
		printf("Bad %d\n", __LINE__);
		exit(1);
	}
	return op.val;
}

static u32 pci_read32(int bus, int dev, int func, u32 addr)
{
	struct opcode op;
	if (!fetch_opcode(&op)) {
		printf("EOF\n");
		exit(1);
	}
	if (!op.is_in || op.data_width != 32 || op.addr != addr
	    || op.type != PCI || op.dev != op.dev || op.func != op.func
	    || op.bus != op.bus) {
		printf("Bad %d\n", __LINE__);
		exit(1);
	}
	return op.val;
}

struct cpuid_result {
	u32 eax, ebx;
};

struct cpuid_result cpuid_ext(u32 eax, u32 ecx)
{
	struct cpuid_result ret;
	struct opcode op;
	if (!fetch_opcode(&op)) {
		printf("EOF\n");
		exit(1);
	}
	if (op.addr != eax || op.type != CPUID || op.ecx != ecx) {
		printf("Bad %d\n", __LINE__);
		exit(1);
	}
	ret.eax = op.val;
	ret.ebx = op.val >> 32;
	return ret;
}

static u64 my_read_msr(u32 addr)
{
	struct opcode op;
	fetch_opcode(&op);
	if (!op.is_in || op.addr != addr || op.type != MSR) {
		printf("Bad %d\n", __LINE__);
		exit(1);
	}
	return op.val;
}

static u64 my_write_msr(u32 addr, u64 val)
{
	struct opcode op;
	fetch_opcode(&op);
	if (op.is_in || op.addr != addr || op.type != MSR || op.val != val) {
		printf("Bad %d\n", __LINE__);
		exit(1);
	}
	return op.val;
}

static void die(const char *msg)
{
	printf("%s\n", msg);
	exit(1);
}

static void intel_early_me_init(void)
{
}

static unsigned intel_early_me_uma_size(void)
{
	u32 t;
	t = pci_mm_read32(HECIDEV, 0x44);
	if (t & 0x10000)
		return t & 0x3F;
	return 0;
}

static u8 read_mchbar8_bypass(u32 addr)
{
	return read_mchbar8(addr);
}

#define printk(condition, fmt, args...) printf(fmt, ## args)

#define udelay(x)

#if 1
static const struct ram_training *get_cached_training(void)
{
	return NULL;
#if 0
	static const struct ram_training ret = {
#if 1
		.lane_timings = {
				 {
				  {
				   {
				    {5, 5, 3, 4, 4, 3, 4, 4, 21},
				    {5, 4, 2, 5, 4, 3, 4, 4, 21}
				    },
				   {
				    {0, 0, 0, 0, 0, 0, 0, 0, 0},
				    {0, 0, 0, 0, 0, 0, 0, 0, 0},
				    }
				   },
				  {
				   {
				    {0, 0, 0, 0, 0, 0, 0, 0, 0},
				    {0, 0, 0, 0, 0, 0, 0, 0, 0},
				    },
				   {
				    {0, 0, 0, 0, 0, 0, 0, 0, 0},
				    {0, 0, 0, 0, 0, 0, 0, 0, 0},
				    }
				   },
				  },
				 {
				  {
				   {
				    {0x6e, 0x64, 0x7b, 0x56, 0xbd, 0xa0, 0xae,
				     0xad, 0x100},
				    {0x6e, 0x67, 0x7a, 0x54, 0xbd, 0x9f, 0xac,
				     0xac, 0x100}
				    },
				   {
				    {0, 0, 0, 0, 0, 0, 0, 0, 0},
				    {0, 0, 0, 0, 0, 0, 0, 0, 0},
				    }
				   },
				  {
				   {
				    {0, 0, 0, 0, 0, 0, 0, 0, 0},
				    {0, 0, 0, 0, 0, 0, 0, 0, 0},
				    },
				   {
				    {0, 0, 0, 0, 0, 0, 0, 0, 0},
				    {0, 0, 0, 0, 0, 0, 0, 0, 0},
				    }
				   },
				  },
				 {
				  {
				   {
				    {0x59, 0x55, 0x6d, 0x44, 0xa3, 0x76, 0x90,
				     0x81, 0x80},
				    {0x58, 0x51, 0x6b, 0x41, 0xa1, 0x75, 0x8e,
				     0x7f, 0x80},
				    },
				   {
				    {0, 0, 0, 0, 0, 0, 0, 0, 0},
				    {0, 0, 0, 0, 0, 0, 0, 0, 0},
				    }
				   },
				  {
				   {
				    {0, 0, 0, 0, 0, 0, 0, 0, 0},
				    {0, 0, 0, 0, 0, 0, 0, 0, 0},
				    },
				   {
				    {0, 0, 0, 0, 0, 0, 0, 0, 0},
				    {0, 0, 0, 0, 0, 0, 0, 0, 0},
				    }
				   },
				  },
				 {
				  {
				   {
				    {0x78, 0x74, 0x8b, 0x64, 0xc1, 0x94, 0xaf,
				     0x9d, 0x80},
				    {0x76, 0x6e, 0x88, 0x60, 0xbe, 0x93, 0xae,
				     0x9d, 0x80},
				    },
				   {
				    {0, 0, 0, 0, 0, 0, 0, 0, 0},
				    {0, 0, 0, 0, 0, 0, 0, 0, 0},
				    }
				   },
				  {
				   {
				    {0, 0, 0, 0, 0, 0, 0, 0, 0},
				    {0, 0, 0, 0, 0, 0, 0, 0, 0},
				    },
				   {
				    {0, 0, 0, 0, 0, 0, 0, 0, 0},
				    {0, 0, 0, 0, 0, 0, 0, 0, 0},
				    }
				   },
				  }
				 },
		.reg_178 = 0x42,
		.reg_10b = 1,
		.v775 = {19, 35},
		.v777 = {
			 {
			  {
			   {
			    {0x01, 0x25},
			    {0x01, 0x25},
			    {0x01, 0x21},
			    {0x02, 0x22},
			    {0x00, 0x23},
			    {0x00, 0x21},
			    {0x01, 0x22},
			    {0x01, 0x22},
			    {0x00, 0x00}
			    },
			   {
			    {0x01, 0x24},
			    {0x02, 0x23},
			    {0x01, 0x20},
			    {0x01, 0x24},
			    {0x00, 0x22},
			    {0x01, 0x21},
			    {0x01, 0x21},
			    {0x02, 0x21},
			    {0x00, 0x00}
			    },
			   {
			    {0x00, 0x00},
			    {0x00, 0x00},
			    {0x00, 0x00},
			    {0x00, 0x00},
			    {0x00, 0x00},
			    {0x00, 0x00},
			    {0x00, 0x00},
			    {0x00, 0x00},
			    {0x00, 0x00},
			    },
			   {
			    {0x00, 0x00},
			    {0x00, 0x00},
			    {0x00, 0x00},
			    {0x00, 0x00},
			    {0x00, 0x00},
			    {0x00, 0x00},
			    {0x00, 0x00},
			    {0x00, 0x00},
			    {0x00, 0x00},
			    }
			   },
			  {
			   {
			    {0x00, 0x00},
			    {0x00, 0x00},
			    {0x00, 0x00},
			    {0x00, 0x00},
			    {0x00, 0x00},
			    {0x00, 0x00},
			    {0x00, 0x00},
			    {0x00, 0x00},
			    {0x00, 0x00},
			    },
			   {
			    {0x00, 0x00},
			    {0x00, 0x00},
			    {0x00, 0x00},
			    {0x00, 0x00},
			    {0x00, 0x00},
			    {0x00, 0x00},
			    {0x00, 0x00},
			    {0x00, 0x00},
			    {0x00, 0x00},
			    },
			   {
			    {0x00, 0x00},
			    {0x00, 0x00},
			    {0x00, 0x00},
			    {0x00, 0x00},
			    {0x00, 0x00},
			    {0x00, 0x00},
			    {0x00, 0x00},
			    {0x00, 0x00},
			    {0x00, 0x00},
			    },
			   {
			    {0x00, 0x00},
			    {0x00, 0x00},
			    {0x00, 0x00},
			    {0x00, 0x00},
			    {0x00, 0x00},
			    {0x00, 0x00},
			    {0x00, 0x00},
			    {0x00, 0x00},
			    {0x00, 0x00},
			    }
			   }
			  },
			 {
			  {
			   {
			    {0x00, 0x25},
			    {0x01, 0x24},
			    {0x01, 0x20},
			    {0x01, 0x22},
			    {0x00, 0x22},
			    {0x00, 0x21},
			    {0x01, 0x23},
			    {0x00, 0x22},
			    {0x00, 0x00}
			    },
			   {
			    {0x00, 0x26},
			    {0x01, 0x22},
			    {0x01, 0x20},
			    {0x01, 0x24},
			    {0x00, 0x23},
			    {0x00, 0x21},
			    {0x01, 0x22},
			    {0x01, 0x22},
			    {0x00, 0x00}
			    },
			   {
			    {0x00, 0x00},
			    {0x00, 0x00},
			    {0x00, 0x00},
			    {0x00, 0x00},
			    {0x00, 0x00},
			    {0x00, 0x00},
			    {0x00, 0x00},
			    {0x00, 0x00},
			    {0x00, 0x00},
			    },
			   {
			    {0x00, 0x00},
			    {0x00, 0x00},
			    {0x00, 0x00},
			    {0x00, 0x00},
			    {0x00, 0x00},
			    {0x00, 0x00},
			    {0x00, 0x00},
			    {0x00, 0x00},
			    {0x00, 0x00},
			    }
			   },
			  {
			   {
			    {0x00, 0x00},
			    {0x00, 0x00},
			    {0x00, 0x00},
			    {0x00, 0x00},
			    {0x00, 0x00},
			    {0x00, 0x00},
			    {0x00, 0x00},
			    {0x00, 0x00},
			    {0x00, 0x00},
			    },
			   {
			    {0x00, 0x00},
			    {0x00, 0x00},
			    {0x00, 0x00},
			    {0x00, 0x00},
			    {0x00, 0x00},
			    {0x00, 0x00},
			    {0x00, 0x00},
			    {0x00, 0x00},
			    {0x00, 0x00},
			    },
			   {
			    {0x00, 0x00},
			    {0x00, 0x00},
			    {0x00, 0x00},
			    {0x00, 0x00},
			    {0x00, 0x00},
			    {0x00, 0x00},
			    {0x00, 0x00},
			    {0x00, 0x00},
			    {0x00, 0x00},
			    },
			   {
			    {0x00, 0x00},
			    {0x00, 0x00},
			    {0x00, 0x00},
			    {0x00, 0x00},
			    {0x00, 0x00},
			    {0x00, 0x00},
			    {0x00, 0x00},
			    {0x00, 0x00},
			    {0x00, 0x00},
			    }
			   }
			  }
			 },
		.v1065 = {
			  {
			   {0x9c, 0x92, 0xab, 0x85, 0xec, 0xd0, 0xdd, 0xdc,
			    0x00},
			   {0x9c, 0x96, 0xab, 0x82, 0xec, 0xcf, 0xdb, 0xdb,
			    0x00},
			   {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			    0x00},
			   {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
			   },
			  {
			   {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			    0x00},
			   {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			    0x00},
			   {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			    0x00},
			   {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
			   }
			  },
		.v1209 = {
			  {
			   {0x59, 0x55, 0x6d, 0x44, 0xa3, 0x76, 0x90, 0x81,
			    0x00},
			   {0x58, 0x51, 0x6b, 0x41, 0xa1, 0x75, 0x8e, 0x7f,
			    0x00},
			   {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			    0x00},
			   {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			    0x00},
			   },
			  {
			   {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			    0x00},
			   {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			    0x00},
			   {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			    0x00},
			   {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			    0x00},
			   }
			  },
		.v1353 = {
			  {
			   {
			    {0x5d, 0x93},
			    {0x59, 0x90},
			    {0x70, 0xa6},
			    {0x49, 0x7f},
			    {0xa5, 0xdd},
			    {0x79, 0xb0},
			    {0x94, 0xca},
			    {0x83, 0xb8},
			    {0x00, 0x00},
			    },
			   {
			    {0x5c, 0x91},
			    {0x53, 0x89},
			    {0x6d, 0xa4},
			    {0x45, 0x7c},
			    {0xa3, 0xd9},
			    {0x77, 0xaf},
			    {0x94, 0xc9},
			    {0x82, 0xb8},
			    {0x00, 0x00},
			    },
			   {
			    {0x00, 0x00},
			    {0x00, 0x00},
			    {0x00, 0x00},
			    {0x00, 0x00},
			    {0x00, 0x00},
			    {0x00, 0x00},
			    {0x00, 0x00},
			    {0x00, 0x00},
			    {0x00, 0x00},
			    },
			   {
			    {0x00, 0x00},
			    {0x00, 0x00},
			    {0x00, 0x00},
			    {0x00, 0x00},
			    {0x00, 0x00},
			    {0x00, 0x00},
			    {0x00, 0x00},
			    {0x00, 0x00},
			    {0x00, 0x00},
			    }
			   },
			  {
			   {
			    {0x00, 0x00},
			    {0x00, 0x00},
			    {0x00, 0x00},
			    {0x00, 0x00},
			    {0x00, 0x00},
			    {0x00, 0x00},
			    {0x00, 0x00},
			    {0x00, 0x00},
			    {0x00, 0x00},
			    },
			   {
			    {0x00, 0x00},
			    {0x00, 0x00},
			    {0x00, 0x00},
			    {0x00, 0x00},
			    {0x00, 0x00},
			    {0x00, 0x00},
			    {0x00, 0x00},
			    {0x00, 0x00},
			    {0x00, 0x00},
			    },
			   {
			    {0x00, 0x00},
			    {0x00, 0x00},
			    {0x00, 0x00},
			    {0x00, 0x00},
			    {0x00, 0x00},
			    {0x00, 0x00},
			    {0x00, 0x00},
			    {0x00, 0x00},
			    {0x00, 0x00},
			    },
			   {
			    {0x00, 0x00},
			    {0x00, 0x00},
			    {0x00, 0x00},
			    {0x00, 0x00},
			    {0x00, 0x00},
			    {0x00, 0x00},
			    {0x00, 0x00},
			    {0x00, 0x00},
			    {0x00, 0x00},
			    }
			   }
			  },
#else
		.lane_timings = {
				 {
				  {
				   {
				    {5, 5, 3, 5, 4, 4, 5, 3, 21},
				    {6, 5, 4, 4, 4, 3, 4, 4, 21}
				    },
				   {
				    {0, 0, 0, 0, 0, 0, 0, 0, 0},
				    {0, 0, 0, 0, 0, 0, 0, 0, 0},
				    }
				   },
				  {
				   {
				    {3, 4, 5, 3, 2, 4, 4, 4, 21},
				    {3, 3, 5, 4, 2, 5, 3, 4, 21},
				    },
				   {
				    {0, 0, 0, 0, 0, 0, 0, 0, 0},
				    {0, 0, 0, 0, 0, 0, 0, 0, 0},
				    }
				   },
				  },
				 {
				  {
				   {
				    {0x6f, 0x66, 0x82, 0x58, 0xc5, 0xa6, 0xb4,
				     0xb1, 0x100},
				    {0x70, 0x67, 0x84, 0x59, 0xc5, 0xa3, 0xb4,
				     0xb2, 0x100}
				    },
				   {
				    {0, 0, 0, 0, 0, 0, 0, 0, 0},
				    {0, 0, 0, 0, 0, 0, 0, 0, 0},
				    }
				   },
				  {
				   {
				    {0x91, 0x87, 0x98, 0x76, 0xdc, 0xb7, 0xcf,
				     0xc5, 0x100},
				    {0x92, 0x8d, 0x9b, 0x76, 0xde, 0xb9, 0xce,
				     0xc6, 0x100},
				    },
				   {
				    {0, 0, 0, 0, 0, 0, 0, 0, 0},
				    {0, 0, 0, 0, 0, 0, 0, 0, 0},
				    }
				   },
				  },
				 {
				  {
				   {
				    {0x5f, 0x5b, 0x74, 0x4a, 0xa9, 0x7c, 0x95,
				     0x85, 0x80},
				    {0x5d, 0x59, 0x72, 0x49, 0xa8, 0x7a, 0x96,
				     0x85, 0x80},
				    },
				   {
				    {0, 0, 0, 0, 0, 0, 0, 0, 0},
				    {0, 0, 0, 0, 0, 0, 0, 0, 0},
				    }
				   },
				  {
				   {
				    {0x5b, 0x53, 0x6c, 0x49, 0xa8, 0x7a, 0x92,
				     0x84, 0x80},
				    {0x5b, 0x51, 0x6c, 0x48, 0xa7, 0x79, 0x91,
				     0x82, 0x80},
				    },
				   {
				    {0, 0, 0, 0, 0, 0, 0, 0, 0},
				    {0, 0, 0, 0, 0, 0, 0, 0, 0},
				    }
				   },
				  },
				 {
				  {
				   {
				    {0x7e, 0x7a, 0x92, 0x6a, 0xc7, 0x9b, 0xb6,
				     0xa4, 0x80},
				    {0x7d, 0x77, 0x8f, 0x69, 0xc6, 0x98, 0xb6,
				     0xa4, 0x80},
				    },
				   {
				    {0, 0, 0, 0, 0, 0, 0, 0, 0},
				    {0, 0, 0, 0, 0, 0, 0, 0, 0},
				    }
				   },
				  {
				   {
				    {0x78, 0x6f, 0x89, 0x65, 0xc4, 0x97, 0xaf,
				     0x9f, 0x80},
				    {0x78, 0x6b, 0x89, 0x64, 0xc2, 0x96, 0xae,
				     0x9d, 0x80},
				    },
				   {
				    {0, 0, 0, 0, 0, 0, 0, 0, 0},
				    {0, 0, 0, 0, 0, 0, 0, 0, 0},
				    }
				   },
				  }
				 },
		.reg_178 = 0,
		.reg_10b = 1,
#endif
	};
	return &ret;
#endif
}
#endif

static void pre_raminit_3(int x2ca8)
{
	u8 t;
	int i;

	gav(t = nvram_read(0x33));
	if (x2ca8 == 0) {
		nvram_write(0x33, t & ~0x40);
		gav(read32(DEFAULT_RCBA | 0x3598));
		write32(DEFAULT_RCBA | 0x3598, 0x1);
		pci_write16(0, 0x1d, 0x0, 0x20, 0x2000);
		gav(pci_read8(0, 0x1d, 0x0, 0x4));	// = 0xff
		pci_write8(0, 0x1d, 0x0, 0x4, 0xff);
		pci_write16(0, 0x1d, 0x1, 0x20, 0x2020);
		gav(pci_read8(0, 0x1d, 0x1, 0x4));	// = 0xff
		pci_write8(0, 0x1d, 0x1, 0x4, 0xff);
		pci_write16(0, 0x1d, 0x2, 0x20, 0x2040);
		gav(pci_read8(0, 0x1d, 0x2, 0x4));	// = 0xff
		pci_write8(0, 0x1d, 0x2, 0x4, 0xff);
		pci_write16(0, 0x1d, 0x3, 0x20, 0x2060);
		gav(pci_read8(0, 0x1d, 0x3, 0x4));	// = 0xff
		pci_write8(0, 0x1d, 0x3, 0x4, 0xff);
		pci_write16(0, 0x1a, 0x0, 0x20, 0x2080);
		gav(pci_read8(0, 0x1a, 0x0, 0x4));	// = 0xff
		pci_write8(0, 0x1a, 0x0, 0x4, 0xff);
		pci_write16(0, 0x1a, 0x1, 0x20, 0x20a0);
		gav(pci_read8(0, 0x1a, 0x1, 0x4));	// = 0xff
		pci_write8(0, 0x1a, 0x1, 0x4, 0xff);
		pci_write16(0, 0x1a, 0x2, 0x20, 0x20e0);
		gav(pci_read8(0, 0x1a, 0x2, 0x4));	// = 0xff
		pci_write8(0, 0x1a, 0x2, 0x4, 0xff);
		for (i = 0; i < 15; i++) {
			gav(inw(0x2010));	// = 0xff
			gav(inw(0x2012));	// = 0xff
			gav(inw(0x2030));	// = 0xff
			gav(inw(0x2032));	// = 0xff
			gav(inw(0x2050));	// = 0xff
			gav(inw(0x2052));	// = 0xff
			gav(inw(0x2070));	// = 0xff
			gav(inw(0x2072));	// = 0xff
			gav(inw(0x2090));	// = 0xff
			gav(inw(0x2092));	// = 0xff
			gav(inw(0x20b0));	// = 0xff
			gav(inw(0x20b2));	// = 0xff
			gav(inw(0x20f0));	// = 0xff
			gav(inw(0x20f2));	// = 0xff
			if (i != 14)
				pm_wait(0x400);	/* <10 */
		}
		pci_write16(0, 0x1d, 0x0, 0x20, 0x0);
		gav(pci_read8(0, 0x1d, 0x0, 0x4));	// = 0xff
		pci_write8(0, 0x1d, 0x0, 0x4, 0xfe);
		pci_write16(0, 0x1d, 0x1, 0x20, 0x0);
		gav(pci_read8(0, 0x1d, 0x1, 0x4));	// = 0xff
		pci_write8(0, 0x1d, 0x1, 0x4, 0xfe);
		pci_write16(0, 0x1d, 0x2, 0x20, 0x0);
		gav(pci_read8(0, 0x1d, 0x2, 0x4));	// = 0xff
		pci_write8(0, 0x1d, 0x2, 0x4, 0xfe);
		pci_write16(0, 0x1d, 0x3, 0x20, 0x0);
		gav(pci_read8(0, 0x1d, 0x3, 0x4));	// = 0xff
		pci_write8(0, 0x1d, 0x3, 0x4, 0xfe);
		pci_write16(0, 0x1a, 0x0, 0x20, 0x0);
		gav(pci_read8(0, 0x1a, 0x0, 0x4));	// = 0xff
		pci_write8(0, 0x1a, 0x0, 0x4, 0xfe);
		pci_write16(0, 0x1a, 0x1, 0x20, 0x0);
		gav(pci_read8(0, 0x1a, 0x1, 0x4));	// = 0xff
		pci_write8(0, 0x1a, 0x1, 0x4, 0xfe);
		pci_write16(0, 0x1a, 0x2, 0x20, 0x0);
		gav(pci_read8(0, 0x1a, 0x2, 0x4));	// = 0xff
		pci_write8(0, 0x1a, 0x2, 0x4, 0xfe);
		write32(DEFAULT_RCBA | 0x3598, 0x0);
	}
}

static void pm_wait(u16 us)
{
	u32 base = read_acpi32(8);
	u32 ticks = (us * 358) / 100;
	while (((read_acpi32(8) - base) & 0xffffff) < ticks) ;
}

static void pre_raminit1(void)
{
	u16 si;

	outb(0x0, 0x62);

	unsigned number_cores;
	struct cpuid_result result;
	unsigned threads_per_package, threads_per_core;

	/* Logical processors (threads) per core */
	result = cpuid_ext(0xb, 0);
	threads_per_core = result.ebx & 0xffff;

	/* Logical processors (threads) per package */
	result = cpuid_ext(0xb, 1);
	threads_per_package = result.ebx & 0xffff;

	if (threads_per_package == 0 || threads_per_core == 0
	    || threads_per_package % threads_per_core)
		number_cores = 1;
	else
		number_cores = threads_per_package / threads_per_core;

	u8 al = nvram_read(0x4c);
	if (number_cores <= 1)
		si = 0;
	else if (!(al & 1))
		si = 1;
	else if (number_cores <= 2)
		si = 0;
	else if (!(al & 2))
		si = 2;
	else
		si = 0;
	if (!(nvram_read(0x55) & 2))
		si |= 0x100;
	/* bit 0 = disable multicore,
	   bit 1 = disable quadcore,
	   bit 8 = disable hyperthreading.  */
	pci_write32(QUICKPATH_BUS, 0x0, 0x0, 0x80,
		    (pci_read32(0xff, 0x0, 0x0, 0x80) & 0xfffffefc) | 0x10000 |
		    si);

	outb(0x1, 0x62);
	outb(0x4, 0x62);
	pci_write32(SOUTHBRIDGE, RCBA, DEFAULT_RCBA | 1);
	gav(read32(DEFAULT_RCBA | 0x3410));
	write32(DEFAULT_RCBA | 0x3410, 0xc61);
	gav(read32(DEFAULT_RCBA | 0x3410));
	pci_write32(SOUTHBRIDGE, PMBASE, 0x400);
	pci_write8(SOUTHBRIDGE, ACPI_CNTL, 0x80);

	u16 t4041 = read_tco16(0x8);
	gav(t4041);
	write_tco16(0x8, t4041);

	pci_write32(SOUTHBRIDGE, 0xd0, 0x0);

	pci_write16(SOUTHBRIDGE, LPC_EN,
		    CNF2_LPC_EN | CNF1_LPC_EN | MC_LPC_EN | KBC_LPC_EN |
		    COMA_LPC_EN);

	u32 t4046 = pci_read32(SOUTHBRIDGE, 0xdc);
	gav(t4046);
	pci_write32(SOUTHBRIDGE, BIOS_CNTL, t4046);
	pci_write32(QUICKPATH_BUS, 0x0, 0x1, 0x50, DEFAULT_PCIEXBAR | 1);
	pci_write32(SOUTHBRIDGE, RCBA, DEFAULT_RCBA | 1);
	gav(read32(DEFAULT_RCBA | 0x3410));
	write32(DEFAULT_RCBA | 0x3410, 0xc61);

	pci_write32(SOUTHBRIDGE, LPC_GEN1_DEC, 0x7c1601);
	pci_write32(SOUTHBRIDGE, LPC_GEN3_DEC, 0x1c1681);
	outb(0x2, 0x62);

	my_write_msr(0x79, 0xffec1410);
	//Unrecognised: [ffff000:fc22] 00c0.00c1   Microcode Update: ERROR: Cannot fake write in a post-hook.

	//Unrecognised: [ffff000:fb6a] 00c8.00c9   CPUID: eax: 00000006; ecx: 0000008b => 00000005.00000002.00000001.00000000

	{
		u8 reg8;
		struct cpuid_result result;
		result = cpuid_ext(0x6, 0x8b);
		if (!(result.eax & 0x2)) {
			reg8 = ((my_read_msr(MSR_FSB_CLOCK_VCC) & 0xff00) >> 8) + 1;
			my_write_msr(IA32_PERF_CTL,
				     (my_read_msr(IA32_PERF_CTL) & ~0xffULL) | reg8);
			my_write_msr(MSR_IA32_MISC_ENABLES,
				     (my_read_msr(MSR_IA32_MISC_ENABLES) &
				      ~0x0000004000000000ULL) | 0x10000);
		}

		reg8 = ((my_read_msr(MSR_FSB_CLOCK_VCC) & 0xff00) >> 8);
		my_write_msr(IA32_PERF_CTL, (my_read_msr(IA32_PERF_CTL) & ~0xffULL) | reg8);
		my_write_msr(MSR_IA32_MISC_ENABLES, my_read_msr(MSR_IA32_MISC_ENABLES) | 0x10000);
		my_write_msr(0x1f1, my_read_msr(0x1f1) | 1);
	}

	outb(0x5, 0x62);

	/*Unrecognised: [ffff000:fc9f] 00ed.00ee   LAPIC: [00000300] <= 000c4500

	   Unrecognised: [ffff000:fc9f] 00ed.00ef   LAPIC: [00000300] => 000c0500
	 */
	outb(0x3, 0x62);

	outb(0x0, 0x62);
	outb(0x2, 0x62);
	outb(0x2c, 0x62);
	outb(0x12, 0x62);
	outb(0x30, 0x62);
	/*Unrecognised: addr ff7ff7da val ff7ff856 */

	outb(0x13, 0x62);
	outb(0x28, 0x62);
	outb(0x29, 0x62);
	outb(0x17, 0x62);
	outb(0x27, 0x62);
	outb(0x4a, 0x62);
	/*Unrecognised: addr ff7ff7da val ff7ff856 */

	gav(pci_mm_read16(SOUTHBRIDGE, PMBASE));	// = 0x1001

	outb(0x11, 0x62);
	outb(0x40, 0x62);

	pci_write32(NORTHBRIDGE, D0F0_MCHBAR_LO, DEFAULT_MCHBAR | 1);

	pci_write32(0, 0x1f, 0x3, SMB_BASE, SMBUS_IO_BASE);
	pci_write32(0, 0x1f, 0x3, HOSTC, 0x1);
	gav(pci_read16(0, 0x1f, 0x3, 0x4));	// = 0x1
	pci_write16(0, 0x1f, 0x3, 0x4, 0x1);

	pci_write32(SOUTHBRIDGE, RCBA, DEFAULT_RCBA | 1);
	pci_mm_write32(NORTHBRIDGE, D0F0_MCHBAR_LO, DEFAULT_MCHBAR | 1);
	pci_mm_write32(NORTHBRIDGE, D0F0_DMIBAR_LO, DEFAULT_DMIBAR | 1);
	gav(pci_mm_read8(HECIDEV, PCI_VENDOR_ID));	// = 0x86
	pci_write32(SOUTHBRIDGE, RCBA, DEFAULT_RCBA | 1);
	gav(pci_read32(SOUTHBRIDGE, PMBASE));	// = 0x1001
	pci_write32(SOUTHBRIDGE, PMBASE, 0x1001);
	gav(pci_read8(SOUTHBRIDGE, ACPI_CNTL));	// = 0x80
	pci_write8(SOUTHBRIDGE, ACPI_CNTL, 0x80);
	gav(pci_read8(SOUTHBRIDGE, 0xa6));	// = 0x2
	pci_write8(SOUTHBRIDGE, 0xa6, 0x2);
	gav(pci_read32(SOUTHBRIDGE, GPIOBASE));	// = DEFAULT_GPIOBASE | 1
	pci_write32(SOUTHBRIDGE, GPIOBASE, DEFAULT_GPIOBASE | 1);
	gav(pci_read8(SOUTHBRIDGE, GPIO_CNTL));	// = 0x10
	pci_write8(SOUTHBRIDGE, GPIO_CNTL, 0x10);
	gav(pci_read8(SOUTHBRIDGE, 0x8));	// = 0x6
	gav(pci_read8(SOUTHBRIDGE, 0x8));	// = 0x6
	gav(read32(DEFAULT_RCBA | 0x3598));
	gav(pci_read32(0, 0x1d, 0x0, 0xfc));	// = 0x20191708
	pci_write32(0, 0x1d, 0x0, 0xfc, 0x20191708);
	gav(pci_read8(SOUTHBRIDGE, 0x8));	// = 0x6
	gav(pci_read8(SOUTHBRIDGE, 0x8));	// = 0x6
	gav(pci_read8(0, 0x1d, 0x0, 0x88));	// = 0xa0
	pci_write8(0, 0x1d, 0x0, 0x88, 0xa0);
	gav(pci_read32(0, 0x1a, 0x0, 0xfc));	// = 0x20191708
	pci_write32(0, 0x1a, 0x0, 0xfc, 0x20191708);
	gav(pci_read8(SOUTHBRIDGE, 0x8));	// = 0x6
	gav(pci_read8(SOUTHBRIDGE, 0x8));	// = 0x6
	gav(pci_read8(0, 0x1a, 0x0, 0x88));	// = 0xa0
	pci_write8(0, 0x1a, 0x0, 0x88, 0xa0);
	write8(DEFAULT_RCBA | 0x14, 0x11);
	write16(DEFAULT_RCBA | 0x50, 0x7654);
	enable_hpet();
	u32 t4143 = read32(0xfed00010);
	gav(t4143);
	write32(0xfed00010, t4143 | 1);


	gav(read8(DEFAULT_RCBA | 0x3428));
	write8(DEFAULT_RCBA | 0x3428, 0x1d);
	pci_mm_write32(0, 0x1f, 0x6, 0x40, 0x40000000);
	pci_mm_write32(0, 0x1f, 0x6, 0x44, 0x0);
	gav(pci_mm_read32(0, 0x1f, 0x6, 0x40));	// = 0x40000004
	pci_mm_write32(0, 0x1f, 0x6, 0x40, 0x40000005);
	u16 t4;
	t4 = read16(0x4000001a);
	gav(t4);
	write16(0x4000001a, (t4 & ~0xf) | 0x10f0);
	gav(pci_mm_read32(0, 0x1f, 0x6, 0x40));	// = 0x40000005
	pci_mm_write32(0, 0x1f, 0x6, 0x40, 0x40000004);
	pci_mm_write32(0, 0x1f, 0x6, 0x40, 0x0);

	pci_read16(SOUTHBRIDGE, PMBASE);	// = 0x1001
	pci_read16(SOUTHBRIDGE, GPIOBASE);	// = DEFAULT_GPIOBASE | 1
	gav(read8(DEFAULT_RCBA | 0x3414));
	gav(read_acpi16(0x0));

	u16 pm1cnt;
	gav(pm1cnt = read_acpi16(0x4));
	s3resume = ((pm1cnt >> 10) & 7) == 5;
	if (s3resume) {
		u8 ra2, ra4;
		gav(ra2 = pci_mm_read8(SOUTHBRIDGE, GEN_PMCON_2));	// = 0xa0
		gav(ra4 = pci_mm_read8(SOUTHBRIDGE, GEN_PMCON_3));	// = 0x9
		if (!(ra2 & 0x20)) {
			s3resume = 0;
			write_acpi16(0x4, 0);
		}
	}

	pci_mm_write8(SOUTHBRIDGE, GEN_PMCON_3,
		      (gav(pci_mm_read8(SOUTHBRIDGE, GEN_PMCON_3)) & ~2) | 1);

	pci_mm_write8(0, 0x3, 0x0, 0x4, 0x0);
	write16(DEFAULT_RCBA | 0x3124, 0x2321);
	pci_mm_write8(SOUTHBRIDGE, 0xdc, 0x0);

	pci_mm_write32(SOUTHBRIDGE, RCBA, DEFAULT_RCBA | 1);
	gav(pci_mm_read16(SOUTHBRIDGE, LPC_IO_DEC));	// = 0x10
	pci_mm_write16(SOUTHBRIDGE, LPC_IO_DEC, 0x10);
	pci_mm_write16(SOUTHBRIDGE, LPC_EN,
		       CNF2_LPC_EN | CNF1_LPC_EN | MC_LPC_EN | KBC_LPC_EN |
		       GAMEH_LPC_EN | GAMEL_LPC_EN | LPT_LPC_EN | COMB_LPC_EN |
		       COMA_LPC_EN);
}

static void pre_raminit_2(void)
{
	pci_mm_write16(SOUTHBRIDGE, GPIOBASE, DEFAULT_GPIOBASE);
	pci_mm_read8(SOUTHBRIDGE, GPIO_CNTL);	// = 0x10
	pci_mm_write8(SOUTHBRIDGE, GPIO_CNTL, 0x10);
	gav(inw(DEFAULT_GPIOBASE | 0x38));	// = 0x10

	outl(0x1963a5ff, DEFAULT_GPIOBASE);
	outl(0xffffffff, DEFAULT_GPIOBASE | 0xc);
	outl(0x87bf6aff, DEFAULT_GPIOBASE | 0x4);
	outl(0x0, DEFAULT_GPIOBASE | 0x18);
	outl(0x120c6, DEFAULT_GPIOBASE | 0x2c);
	outl(0x27706fe, DEFAULT_GPIOBASE | 0x30);
	outl(0x29fffff, DEFAULT_GPIOBASE | 0x38);
	outl(0x1b01f9f4, DEFAULT_GPIOBASE | 0x34);
	outl(0x0, DEFAULT_GPIOBASE | 0x40);
	outl(0x0, DEFAULT_GPIOBASE | 0x48);
	outl(0xf00, DEFAULT_GPIOBASE | 0x44);

	pci_mm_write16(SOUTHBRIDGE, PMBASE, DEFAULT_PMBASE);
	gav(pci_mm_read8(SOUTHBRIDGE, ACPI_CNTL));	// = 0x80
	pci_mm_write8(SOUTHBRIDGE, ACPI_CNTL, 0x80);
	pci_mm_write32(SOUTHBRIDGE, ETR3, gav(pci_mm_read32(SOUTHBRIDGE, ETR3)) & ~ETR3_CF9GR);	// OK
	pci_mm_write32(SOUTHBRIDGE, LPC_GEN1_DEC, 0xc0681);

	gav(read32(DEFAULT_RCBA | 0x3400));
	write32(DEFAULT_RCBA | 0x3400, 0x1c);
	gav(read32(DEFAULT_RCBA | 0x3410));
	write32(DEFAULT_RCBA | 0x3410, 0xc61);

	gav(read_tco16(0x8));
	write_tco16(0x8, 0x800);
	write_tco8(0x6, gav(read_tco8(0x6)) | 0x2);

	gav(inb(0x61));		// = 0x2
	outb(0x3c, 0x61);

	enable_hpet();

	if (pci_mm_read8(SOUTHBRIDGE, GEN_PMCON_3) & 4) {
		nvram_write(0xa, nvram_read(0xa) | 0x70);
		nvram_write(0xb, nvram_read(0xb) | 0x80);
		nvram_write(0xa, nvram_read(0xa) & ~0x50);
		nvram_write(0xb, nvram_read(0xb) & ~0x80);
	}
	gav(read32(DEFAULT_RCBA | 0x3410));
	write32(DEFAULT_RCBA | 0x3410, 0xc61);
	gav(pci_read8(SOUTHBRIDGE, 0x8));	// = 0x6
	gav(inl(DEFAULT_GPIOBASE));	// = 0x6
	outl(0x7963a5ff, DEFAULT_GPIOBASE);
	gav(inl(DEFAULT_GPIOBASE | 0x4));	// = 0x7963a5ff
	outl(0x87bf6aff, DEFAULT_GPIOBASE | 0x4);
	outl(gav(inl(DEFAULT_GPIOBASE | 0xc)) | 0x40000000,
	     DEFAULT_GPIOBASE | 0xc);
	gav(inl(DEFAULT_GPIOBASE | 0x60));	// = 0xfffbfffb
	outl(0x41000000, DEFAULT_GPIOBASE | 0x60);
	pci_write32(SOUTHBRIDGE, LPC_GEN3_DEC, 0x1c1681);
	pci_write32(SOUTHBRIDGE, LPC_GEN2_DEC, 0xc15e1);
	pci_write32(SOUTHBRIDGE, LPC_GEN1_DEC, 0x7c1601);
	gav(inl(DEFAULT_GPIOBASE | 0xc));	// = 0x7c1601
	outb(0x15, 0x62);
	outb(0x16, 0x62);


	gav(pci_read32(NORTHBRIDGE, D0F0_MCHBAR_LO));	// = DEFAULT_MCHBAR | 1
}

static void enable_hpet(void)
{
#if REAL
	u32 reg32;

	/* Move HPET to default address 0xfed00000 and enable it */
	reg32 = RCBA32(HPTC);
	reg32 |= (1 << 7);	// HPET Address Enable
	reg32 &= ~(3 << 0);
	RCBA32(HPTC) = reg32;
#else
	write32(DEFAULT_RCBA | HPTC, 0x80);
#endif
}

static void pre_raminit_4a(void)
{
#if !REAL
	int i;

	outb(0x55, 0x62);

	outb(0x32, 0x62);
	/*Unrecognised: addr ff7ff7da val ff7ff856 */

	gav(pci_read32(0, 0x1f, 0x3, 0x0));	// = 0x3b308086
	pci_write32(0, 0x1f, 0x3, 0x20, 0x1100);
	gav(pci_read8(0, 0x1f, 0x3, 0x4));	// = 0x1
	pci_write8(0, 0x1f, 0x3, 0x4, 0x1);
	gav(pci_read8(0, 0x1f, 0x3, 0x40));	// = 0x1
	pci_write8(0, 0x1f, 0x3, 0x40, 0x9);
	gav(pci_read8(0, 0x1f, 0x3, 0x40));	// = 0x1
	pci_write8(0, 0x1f, 0x3, 0x40, 0x1);

	outb(0x4f, 0x62);
	outb(0x50, 0x62);
	/*Unrecognised: addr ff7ff7da val ff7ff856 */

	gav(pci_read8(NORTHBRIDGE, D0F0_CAPID0 + 8));	// = 0x88
	my_read_msr(0x17);	// !!!
	/*Unrecognised: [0000:fffaf715] 1a183.1a184   Microcode Update: ERROR: Cannot fake write in a post-hook. */

	my_read_msr(0x17);	// !!!
	/*Unrecognised: [0000:fffaf715] 1a25d.1a25e   Microcode Update: ERROR: Cannot fake write in a post-hook. */

	outb(0x48, 0x62);
	if (x2ca8 != 0) {
		outb(0x42, 0x15ec);
		gav(inb(0x15ee));	// = 0x42
	}
#endif
	u16 t3;

	if (x2ca8 == 0) {
		gav(t3 = inw(DEFAULT_GPIOBASE | 0x38));
		outw(t3 & ~0x400, DEFAULT_GPIOBASE | 0x38);
		gav(smbus_read_byte(0x5c, 0x06));
		smbus_write_byte(0x5c, 0x06, 0x8f);

#if !REAL
		for (i = 0; i < 5; i++)
			pm_wait(0x3e8);
#endif

		gav(smbus_read_byte(0x5c, 0x07));
		smbus_write_byte(0x5c, 0x07, 0x8f);

#if !REAL
		for (i = 0; i < 5; i++)
			pm_wait(0x3e8);
#endif
		gav(pci_mm_read16(SOUTHBRIDGE, GPIOBASE));	// = DEFAULT_GPIOBASE | 1
		outw(t3 | 0x400, DEFAULT_GPIOBASE | 0x38);
#if !REAL
		outb(0x42, 0x15ec);
		gav(inb(0x15ee));	// = 0x42
		gav(pci_mm_read16(SOUTHBRIDGE, GPIOBASE));	// = DEFAULT_GPIOBASE | 1
#endif
	}

	gav(t3 = inw(DEFAULT_GPIOBASE | 0x38));
	outw(t3 & ~0x400, DEFAULT_GPIOBASE | 0x38);

	gav(smbus_read_byte(0x57, 0x55));
	gav(pci_mm_read16(SOUTHBRIDGE, GPIOBASE));	// = DEFAULT_GPIOBASE | 1
	outw(t3 | 0x400, DEFAULT_GPIOBASE | 0x38);

#if !REAL
	outb(0x42, 0x62);
	gav(read_tco16(0x6));
	pci_mm_write32(NORTHBRIDGE, PCI_SUBSYSTEM_VENDOR_ID, 0x219317aa);
	pci_mm_write32(0, 0x1, 0x0, 0x8c, 0x219417aa);
	pci_mm_write32(0xff, 0, 0, PCI_SUBSYSTEM_VENDOR_ID, 0x219617aa);
	pci_mm_write32(0xff, 0, 1, PCI_SUBSYSTEM_VENDOR_ID, 0x219617aa);
	pci_mm_write32(0xff, 2, 0, PCI_SUBSYSTEM_VENDOR_ID, 0x219617aa);
	pci_mm_write32(0xff, 2, 1, PCI_SUBSYSTEM_VENDOR_ID, 0x219617aa);
	pci_mm_write32(NORTHBRIDGE, D0F0_MCHBAR_LO, DEFAULT_MCHBAR | 1);
	pci_mm_write32(NORTHBRIDGE, D0F0_MCHBAR_HI, 0x0);
	pci_mm_write32(NORTHBRIDGE, D0F0_DMIBAR_LO, DEFAULT_DMIBAR | 1);
	pci_mm_write32(NORTHBRIDGE, D0F0_DMIBAR_HI, 0x0);
	pci_mm_write32(NORTHBRIDGE, D0F0_EPBAR_LO, DEFAULT_EPBAR | 1);
	pci_mm_write32(NORTHBRIDGE, D0F0_EPBAR_HI, 0x0);
	pci_mm_read32(NORTHBRIDGE, D0F0_MCHBAR_LO);	// = DEFAULT_MCHBAR | 1
	pci_mm_read32(NORTHBRIDGE, D0F0_MCHBAR_HI);	// = 0x0
#endif
}

static void dmi_setup(void)
{
	gav(read8(DEFAULT_DMIBAR | 0x254));
	write8(DEFAULT_DMIBAR | 0x254, 0x1);
	gav(pci_mm_read32(SOUTHBRIDGE, 0xec));	// = 0x2b83806
	write16(DEFAULT_DMIBAR | 0x1b8, 0x18f2);
	pci_mm_write16(NORTHBRIDGE, D0F0_DEVEN,
		       pci_mm_read16(NORTHBRIDGE, D0F0_DEVEN) & 0xfeff);
	read_mchbar16(0x48);
	write_mchbar16(0x48, 0x2);
	my_read_msr(0x17);

	pci_mm_read32(NORTHBRIDGE, D0F0_DMIBAR_LO);	// = DEFAULT_DMIBAR | 1
	if (pci_mm_read16(0, 0x1, 0x0, 0x0) != 0xffff) {
		gav(pci_mm_read16(0, 0x1, 0x0, 0xac));
		pci_mm_write32(0, 0x1, 0x0, 0x200,
			       pci_mm_read32(0, 0x1, 0x0, 0x200) & ~0x100);
		pci_mm_write8(0, 0x1, 0x0, 0x1f8,
			      (pci_mm_read8(0, 0x1, 0x0, 0x1f8) & ~1) | 4);
		u32 t4431 = read32(DEFAULT_DMIBAR | 0xd68);
		gav(t4431);
		write32(DEFAULT_DMIBAR | 0xd68, t4431 | 0x08000000);
		pci_mm_write32(0, 0x1, 0x0, 0x200,
			       pci_mm_read32(0, 0x1, 0x0, 0x200) & ~0x00200000);
		gav(pci_mm_read8(0, 0x1, 0x0, 0xd60));	// = 0x0
		gav(pci_mm_read8(0, 0x1, 0x0, 0xd60));	// = 0x0
		pci_mm_write8(0, 0x1, 0x0, 0xd60, 0x3);
		gav(pci_mm_read16(0, 0x1, 0x0, 0xda8));	// = 0xbf9
		gav(pci_mm_read16(0, 0x1, 0x0, 0xda8));	// = 0xbf9
		pci_mm_write16(0, 0x1, 0x0, 0xda8, 0xf9);
		pci_mm_read16(0, 0x1, 0x0, 0xda8);	// = 0xf9
		pci_mm_read16(0, 0x1, 0x0, 0xda8);	// = 0xf9
		pci_mm_write16(0, 0x1, 0x0, 0xda8, 0x79);
		pci_mm_read8(0, 0x1, 0x0, 0xd0);	// = 0x2
		pci_mm_read8(0, 0x1, 0x0, 0xd0);	// = 0x2
		pci_mm_write8(0, 0x1, 0x0, 0xd0, 0x1);
		pci_mm_read16(0, 0x1, 0x0, 0x224);	// = 0xd
		pci_mm_read32(NORTHBRIDGE, D0F0_CAPID0);	// = 0x10c0009
		pci_mm_read32(NORTHBRIDGE, D0F0_CAPID0 + 4);	// = 0x316126
		pci_mm_read16(0, 0x1, 0x0, 0x224);	// = 0xd
		pci_mm_write16(0, 0x1, 0x0, 0x224, 0x1d);
		pci_mm_read16(0, 0x6, 0x0, 0x0);	// = 0xffff
		pci_mm_read16(0, 0x1, 0x0, 0x224);	// = 0x1d
		pci_mm_read16(0, 0x6, 0x0, 0x0);	// = 0xffff
		pci_mm_write16(0, 0x1, 0x0, 0xac, 0x4d01);
		pci_mm_read16(0, 0x1, 0x0, 0x224);	// = 0x1d
		pci_mm_read8(0, 0x1, 0x0, 0xba);	// = 0x0
		pci_mm_read16(0, 0x1, 0x0, 0x0);	// = 0x8086
		pci_mm_read32(0, 0x1, 0x0, 0xc00);	// = 0xffffffff
		pci_mm_write32(0, 0x1, 0x0, 0xc00, 0xffffc0fc);
		pci_mm_read32(0, 0x1, 0x0, 0xc04);	// = 0x9600000f
		pci_mm_write32(0, 0x1, 0x0, 0xc04, 0x96000000);
		pci_mm_read32(0, 0x1, 0x0, 0xc04);	// = 0x96000000
		pci_mm_write32(0, 0x1, 0x0, 0xc04, 0x16000000);
		pci_mm_write32(0, 0x1, 0x0, 0xc08, 0x0);
	} else
		pci_mm_read16(0, 0x1, 0x0, 0x0);	// = 0xffff


	pci_mm_read32(NORTHBRIDGE, D0F0_DMIBAR_LO);	// = DEFAULT_DMIBAR | 1
	pci_mm_read16(0, 0x6, 0x0, 0x0);	// = 0xffff
	pci_mm_read16(0, 0x6, 0x0, 0x0);	// = 0xffff
	pci_mm_write32(HECIDEV, HECIBAR, DEFAULT_HECIBAR);
	pci_mm_write32(HECIDEV, PCI_COMMAND,
		       PCI_COMMAND_MASTER | PCI_COMMAND_MEMORY);

	outl((gav(inl(DEFAULT_GPIOBASE | 0x38)) & ~0x140000) | 0x400000,
	     DEFAULT_GPIOBASE | 0x38);
	gav(inb(DEFAULT_GPIOBASE | 0xe));	// = 0xfdcaff6e

	pci_mm_write32(0, 0x2, 0x0, PCI_SUBSYSTEM_VENDOR_ID, 0x215a17aa);
	pci_mm_write8(NORTHBRIDGE, D0F0_DEVEN,
		      pci_mm_read8(NORTHBRIDGE, D0F0_DEVEN) | 2);

	pci_mm_write16(0, 0x1, 0x0, 0x224,
		       pci_mm_read16(0, 0x1, 0x0, 0x224) | 1);
	pci_mm_write16(NORTHBRIDGE, D0F0_GGC,
		       pci_mm_read16(NORTHBRIDGE, D0F0_GGC) | 2);

	pci_mm_read32(NORTHBRIDGE, D0F0_MCHBAR_LO);	// = DEFAULT_MCHBAR | 1
	pci_mm_read32(NORTHBRIDGE, D0F0_MCHBAR_HI);	// = 0x0
	pci_mm_read32(NORTHBRIDGE, D0F0_DMIBAR_LO);	// = DEFAULT_DMIBAR | 1
	pci_mm_read32(NORTHBRIDGE, D0F0_DMIBAR_HI);	// = 0x0

	const struct {
		int dev, func;
	} bridges[] = { {
	0x1e, 0}, {
	0x1c, 0}, {
	0x1c, 1}, {
	0x1c, 2}, {
	0x1c, 3}, {
	0x1c, 4}, {
	0x1c, 5}, {
	0x1c, 6}, {
	0x1c, 7}};
	for (i = 0; i < sizeof(bridges) / sizeof(bridges[0]); i++) {
		u16 dev;
		pci_mm_write32(0, bridges[i].dev, bridges[i].func, 0x18,
			       0x20200);
		for (dev = 0; dev < 0x20; dev++) {
			u16 vendor = pci_mm_read16(2, dev, 0x0, 0x0);
			if (vendor == 0xffff)
				continue;
			pci_mm_read16(2, dev, 0x0, 0xa);
		}
		pci_mm_write32(0, bridges[i].dev, bridges[i].func, 0x18, 0x0);
	}

	pci_mm_read16(0, 0x1, 0x0, 0x0);	// = 0x8086
	pci_mm_read8(NORTHBRIDGE, D0F0_CAPID0 + 6);	// = 0x31
	pci_mm_read8(0, 0x1, 0x0, 0xba);	// = 0x0
	pci_mm_read16(0, 0x6, 0x0, 0x0);	// = 0xffff
	pci_mm_read8(NORTHBRIDGE, D0F0_CAPID0 + 6);	// = 0x31
	pci_mm_read8(0, 0x6, 0x0, 0xba);	// = 0xff
	pci_mm_read16(0, 0x1, 0x0, 0x11a);	// = 0x2
	pci_mm_read16(0, 0x6, 0x0, 0x11a);	// = 0xffff
	pci_mm_read16(0, 0x1, 0x0, 0x0);	// = 0x8086
	pci_mm_read32(0, 0x1, 0x0, 0x18);	// = 0x0
	pci_mm_write32(0, 0x1, 0x0, 0x18, 0x0);
	pci_mm_read16(0, 0x6, 0x0, 0x0);	// = 0xffff
}

static void pre_raminit_5(int s3resume)
{
	int i;

	for (i = 0x10; i < 0x28;) {
		u32 s;
		pci_mm_read32(0, 0x2, 0x0, i);	// = 0xffffffff
		pci_mm_read32(0, 0x2, 0x0, i);	// = 0xffffffff
		pci_mm_write32(0, 0x2, 0x0, i, 0x0);
		pci_mm_read32(0, 0x2, 0x0, i);	// = 0xffffffff
		pci_mm_write32(0, 0x2, 0x0, i, 0xffffffff);
		s = pci_mm_read32(0, 0x2, 0x0, i);
		if (s != 0xffffffff && s != 0) {
			if (s & 1) {
				pci_mm_write32(0, 0x2, 0x0, i, s & 0x7);
				i += 4;
			} else {
				pci_mm_read32(0, 0x2, 0x0, i);	// = 0xffffffff
				pci_mm_write32(0, 0x2, 0x0, i, s & 0xf);
				i += 8;
			}
		} else
			i += 4;
	}
	pci_mm_read8(0, 0x1d, 0x0, 0x80);	// = 0x0
	pci_mm_write8(0, 0x1d, 0x0, 0x80, 0x1);
	pci_mm_read8(0, 0x1a, 0x0, 0x80);	// = 0x0
	pci_mm_write8(0, 0x1a, 0x0, 0x80, 0x1);
	pci_mm_write32(HECIDEV, PCI_SUBSYSTEM_VENDOR_ID, 0x215f17aa);
	pci_mm_write32(0, 0x16, 0x2, PCI_SUBSYSTEM_VENDOR_ID, 0x216117aa);
	pci_mm_write32(0, 0x16, 0x3, PCI_SUBSYSTEM_VENDOR_ID, 0x216217aa);
	pci_mm_write32(0, 0x1a, 0x0, PCI_SUBSYSTEM_VENDOR_ID, 0x216317aa);
	pci_mm_write32(0, 0x1b, 0x0, PCI_SUBSYSTEM_VENDOR_ID, 0x215e17aa);
	pci_mm_write32(0, 0x1c, 0x0, 0x94, 0x216417aa);
	pci_mm_write32(0, 0x1c, 0x1, 0x94, 0x216417aa);
	pci_mm_write32(0, 0x1c, 0x2, 0x94, 0x216417aa);
	pci_mm_write32(0, 0x1c, 0x3, 0x94, 0x216417aa);
	pci_mm_write32(0, 0x1c, 0x4, 0x94, 0x216417aa);
	pci_mm_write32(0, 0x1d, 0x0, PCI_SUBSYSTEM_VENDOR_ID, 0x216317aa);
	pci_mm_write32(0, 0x1e, 0x0, 0x54, 0x216517aa);
	pci_mm_write32(SOUTHBRIDGE, PCI_SUBSYSTEM_VENDOR_ID, 0x216617aa);
	pci_mm_write32(0, 0x1f, 0x3, PCI_SUBSYSTEM_VENDOR_ID, 0x216717aa);
	pci_mm_write32(0, 0x1f, 0x5, PCI_SUBSYSTEM_VENDOR_ID, 0x216a17aa);
	pci_mm_write32(0, 0x1f, 0x6, PCI_SUBSYSTEM_VENDOR_ID, 0x219017aa);
	pci_mm_read8(0, 0x1d, 0x0, 0x80);	// = 0x1
	pci_mm_write8(0, 0x1d, 0x0, 0x80, 0x0);
	pci_mm_read8(0, 0x1a, 0x0, 0x80);	// = 0x1
	pci_mm_write8(0, 0x1a, 0x0, 0x80, 0x0);
	pci_mm_write32(13, 0x0, 0x0, PCI_SUBSYSTEM_VENDOR_ID, 0x213317aa);
	pci_mm_write32(13, 0x0, 0x1, PCI_SUBSYSTEM_VENDOR_ID, 0x213417aa);
	pci_mm_write32(13, 0x0, 0x3, PCI_SUBSYSTEM_VENDOR_ID, 0x213617aa);

	pci_mm_write32(HECIDEV, HECIBAR, DEFAULT_HECIBAR);
	pci_mm_write32(HECIDEV, PCI_COMMAND,
		       PCI_COMMAND_MASTER | PCI_COMMAND_MEMORY);

	write_acpi16(0x2, 0x0);
	write_acpi32(0x28, 0x0);
	write_acpi32(0x2c, 0x0);
	if (!s3resume) {
		gav(read_acpi32(0x4));
		gav(read_acpi32(0x20));
		gav(read_acpi32(0x34));
		write_acpi16(0x0, 0x900);
		write_acpi32(0x20, 0xffff7ffe);
		write_acpi32(0x34, 0x56974);
		pci_mm_write8(SOUTHBRIDGE, GEN_PMCON_3,
			      pci_mm_read8(SOUTHBRIDGE, GEN_PMCON_3) | 2);
	}

	gav(read32(DEFAULT_RCBA | 0x3410));

	if (gav(read32(DEFAULT_RCBA | 0x3804)) & 0x4000) {
		u32 v;

		write8(DEFAULT_RCBA | 0x3894, 0x6);
		write8(DEFAULT_RCBA | 0x3895, 0x50);
		write8(DEFAULT_RCBA | 0x3896, 0x3b);
		write8(DEFAULT_RCBA | 0x3897, 0x14);
		write8(DEFAULT_RCBA | 0x3898, 0x2);
		write8(DEFAULT_RCBA | 0x3899, 0x3);
		write8(DEFAULT_RCBA | 0x389a, 0x20);
		write8(DEFAULT_RCBA | 0x389b, 0x5);
		write8(DEFAULT_RCBA | 0x389c, 0x9f);
		write8(DEFAULT_RCBA | 0x389d, 0x20);
		write8(DEFAULT_RCBA | 0x389e, 0x1);
		write8(DEFAULT_RCBA | 0x389f, 0x6);
		write8(DEFAULT_RCBA | 0x3890, 0xc);
		gav(read8(DEFAULT_RCBA | 0x3890));
		write32(DEFAULT_RCBA | 0x3808, 0x0);
		gav(read32(DEFAULT_RCBA | 0x3808));
		write16(DEFAULT_RCBA | 0x3891, 0x4242);
		gav(read16(DEFAULT_RCBA | 0x3891));
		gav(read8(DEFAULT_RCBA | 0x3890));
		write8(DEFAULT_RCBA | 0x3890, 0xc);
		gav(read8(DEFAULT_RCBA | 0x3890));
		if ((gav(read32(DEFAULT_RCBA | 0x3810)) & 0x20) || WTF1)
			v = 0x2005;
		else
			v = 0x2015;
		write32(DEFAULT_RCBA | 0x38c8, v);
		write32(DEFAULT_RCBA | 0x38c4, 0x800000 | v);
		gav(read32(DEFAULT_RCBA | 0x38b0));
		write32(DEFAULT_RCBA | 0x38b0, 0x1000);
		gav(read32(DEFAULT_RCBA | 0x38b4));
		gav(read32(DEFAULT_RCBA | 0x38b0));
		write32(DEFAULT_RCBA | 0x38b0, 0x4);
		gav(read32(DEFAULT_RCBA | 0x38b4));
		write32(DEFAULT_RCBA | 0x3874, 0x1fff07d0);
	}
	gav(inb(DEFAULT_GPIOBASE | 0xe));	// = 0x1fff07d0

	set_fsb_frequency ();

	outb(0x44, 0x62);
	outb(0x3c, 0x62);
	nvram_read(0x71);
	my_read_msr (MTRRcap_MSR);
	my_read_msr (MTRRphysMask_MSR (0));
	my_read_msr (MTRRphysMask_MSR (1));
	my_read_msr (MTRRphysMask_MSR (2));
	my_read_msr (MTRRphysMask_MSR (3));

	gav(read32(DEFAULT_RCBA | 0x3410));
	write32(DEFAULT_RCBA | 0x3410, 0xc61);

	gav(read8(0xfed40000));
	pci_mm_read32(0xff, 0, 0, 0x88);
	read_mchbar32(0x28);
	gav(read8(0xfed30008));
}
