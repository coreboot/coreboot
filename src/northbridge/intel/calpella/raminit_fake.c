static u32 gav_real (int line, u32 in)
{
  //  printf ("%d: GAV: %x\n", line, in);
  return in;
}

#define gav(x) gav_real (__LINE__, (x))

#include <parse.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void pm_wait (u16 us);

#define ARRAY_SIZE(array) (sizeof (array) / sizeof (array[0]))

#define CONFIG_SMM_TSEG_SIZE (8 << 20)

#define MTRR_TYPE_WRPROT     5
#define MTRRdefTypeEn		(1 << 11)
#define MTRRphysBase_MSR(reg) (0x200 + 2 * (reg))
#define MTRRphysMask_MSR(reg) (0x200 + 2 * (reg) + 1)

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

static void
write32 (u32 addr, u32 val)
{
	struct opcode op;
	if (!fetch_opcode (&op))
	{
		printf ("EOF\n");
		exit (1);
	}
	if (op.is_in || op.data_width != 32 || op.addr != addr || op.val != val || op.type != MEM)
	{
		printf ("Bad %d: %x, %x vs %x, %llx\n", __LINE__, addr, val, op.addr, op.val);
		exit (1);
	}
}

static void
write16 (u32 addr, u16 val)
{
	struct opcode op;
	if (!fetch_opcode (&op))
	{
		printf ("EOF\n");
		exit (1);
	}
	if (op.is_in || op.data_width != 16 || op.addr != addr || op.val != val || op.type != MEM)
	{
	  printf ("Bad %d: %x, %x vs %x, %llx\n", __LINE__, addr, val, op.addr, op.val);
		exit (1);
	}
}

static void
write8 (u32 addr, u8 val)
{
	struct opcode op;
	fetch_opcode (&op);
	if (op.is_in || op.data_width != 8 || op.addr != addr || op.val != val || op.type != MEM)
	{
		printf ("Bad %d\n", __LINE__);
		exit (1);
	}
}


static int
smbus_read_byte (u32 dev, u32 addr)
{
	struct opcode op;
	if (!fetch_opcode (&op))
	{
		printf ("EOF\n");
		exit (1);
	}
	if (!op.is_in || op.data_width != 8 || op.addr != addr || op.type != SMBUS || op.dev != dev)
	{
		printf ("Bad %d: %x, %d vs %x, %d\n", __LINE__, op.addr, SMBUS, addr, op.type);
		exit (1);
	}
	return (signed short) op.val;
}

static int
smbus_block_read (u32 dev, u32 addr, u32 len, u8 *block)
{
	struct opcode op;
	if (!fetch_opcode (&op))
	{
		printf ("EOF\n");
		exit (1);
	}
	if (!op.is_in || op.data_width != len || op.addr != addr || op.type != OPCODE_SMBUS_BLOCK || op.dev != dev)
	{
		printf ("Bad %d: %x, %d vs %x, %d\n", __LINE__, op.addr, OPCODE_SMBUS_BLOCK, addr, op.type);
		exit (1);
	}
	memcpy (block, &op.val, len);
	return 0;
}

static int
smbus_block_write (u32 dev, u32 addr, u32 len, const u8 *block)
{
	struct opcode op;
	if (!fetch_opcode (&op))
	{
		printf ("EOF\n");
		exit (1);
	}
	if (op.is_in || op.data_width != len || op.addr != addr || op.type != OPCODE_SMBUS_BLOCK || op.dev != dev || memcmp (block, &op.val, len) != 0)
	{
		printf ("Bad %d: %x, %d vs %x, %d\n", __LINE__, op.addr, OPCODE_SMBUS_BLOCK, addr, op.type);
		exit (1);
	}
	return 0;
}

static void
smbus_write_byte (u32 dev, u32 addr, u8 val)
{
	struct opcode op;
	if (!fetch_opcode (&op))
	{
		printf ("EOF\n");
		exit (1);
	}
	if (op.is_in || op.data_width != 8 || op.addr != addr || op.type != SMBUS || op.dev != dev || op.val != val)
	{
		printf ("Bad %d: %x, %d vs %x, %d\n", __LINE__, addr, SMBUS, op.addr, op.type);
		exit (1);
	}
}

static void
write_mchbar32 (u32 addr, u32 val)
{
	struct opcode op;
	if (!fetch_opcode (&op))
	{
		printf ("EOF\n");
		exit (1);
	}
	if (op.is_in || op.data_width != 32 || op.addr != addr || op.val != val || op.type != OPCODE_MCHBAR)
	{
		printf ("Bad [%x] = %x vs [%x] = %llx\n", addr, val, op.addr, op.val);
		exit (1);
	}
}

static void
write_acpi32 (u32 addr, u32 val)
{
	struct opcode op;
	if (!fetch_opcode (&op))
	{
		printf ("EOF\n");
		exit (1);
	}
	if (op.is_in || op.data_width != 32 || op.addr != addr || op.val != val || op.type != ACPI)
	{
		printf ("Bad [%x] = %x vs [%x] = %llx\n", addr, val, op.addr, op.val);
		exit (1);
	}
}

static void
write_mchbar16 (u32 addr, u16 val)
{
	struct opcode op;
	fetch_opcode (&op);
	if (op.is_in || op.data_width != 16 || op.addr != addr || op.val != val || op.type != OPCODE_MCHBAR)
	{
		printf ("Bad %d: %x, %x vs %x, %llx\n", __LINE__, addr, val, op.addr, op.val);
		exit (1);
	}
}

static void
write_acpi16 (u32 addr, u16 val)
{
	struct opcode op;
	fetch_opcode (&op);
	if (op.is_in || op.data_width != 16 || op.addr != addr || op.val != val || op.type != ACPI)
	{
		printf ("Bad %d: %x, %x vs %x, %llx\n", __LINE__, addr, val, op.addr, op.val);
		exit (1);
	}
}

static void
write_tco16 (u32 addr, u16 val)
{
	struct opcode op;
	fetch_opcode (&op);
	if (op.is_in || op.data_width != 16 || op.addr != addr || op.val != val || op.type != TCO)
	{
		printf ("Bad %d: %x, %x vs %x, %llx\n", __LINE__, addr, val, op.addr, op.val);
		exit (1);
	}
}

static void
write_tco8 (u32 addr, u8 val)
{
	struct opcode op;
	fetch_opcode (&op);
	if (op.is_in || op.data_width != 8 || op.addr != addr || op.val != val || op.type != TCO)
	{
		printf ("Bad %d: %x, %x vs %x, %llx\n", __LINE__, addr, val, op.addr, op.val);
		exit (1);
	}
}

static void
write_mchbar8 (u32 addr, u8 val)
{
	struct opcode op;
	fetch_opcode (&op);
	if (op.is_in || op.data_width != 8 || op.addr != addr || op.val != val || op.type != OPCODE_MCHBAR)
	{
		printf ("Bad %d: %x, %x vs %x, %llx\n", __LINE__, addr, val, op.addr, op.val);
		exit (1);
	}
}

static u32
read_mchbar32 (u32 addr)
{
	struct opcode op;
	if (!fetch_opcode (&op))
	{
		printf ("EOF\n");
		exit (1);
	}
	if (!op.is_in || op.data_width != 32 || op.addr != addr || op.type != OPCODE_MCHBAR)
	{
		printf ("Bad %d\n", __LINE__);
		exit (1);
	}
	return op.val;
}

static u32
read_mchbar32_bypass (u32 addr)
{
  return read_mchbar32 (addr);
}

static u32
read_acpi32 (u32 addr)
{
	struct opcode op;
	if (!fetch_opcode (&op))
	{
		printf ("EOF\n");
		exit (1);
	}
	if (!op.is_in || op.data_width != 32 || op.addr != addr || op.type != ACPI)
	{
		printf ("Bad %d\n", __LINE__);
		exit (1);
	}
	return op.val;
}

static u16
read_mchbar16 (u32 addr)
{
	struct opcode op;
	fetch_opcode (&op);
	if (!op.is_in || op.data_width != 16 || op.addr != addr || op.type != OPCODE_MCHBAR)
	{
		printf ("Bad %d\n", __LINE__);
		exit (1);
	}
	return op.val;
}

static u16
read_tco16 (u32 addr)
{
	struct opcode op;
	fetch_opcode (&op);
	if (!op.is_in || op.data_width != 16 || op.addr != addr || op.type != TCO)
	{
		printf ("Bad %d\n", __LINE__);
		exit (1);
	}
	return op.val;
}

static u16
read_acpi16 (u32 addr)
{
	struct opcode op;
	fetch_opcode (&op);
	if (!op.is_in || op.data_width != 16 || op.addr != addr || op.type != ACPI)
	{
	  printf ("Bad %d: %x, 16 vs %x, %d\n", __LINE__, addr, op.addr, op.data_width);
		exit (1);
	}
	return op.val;
}

static u8
read_mchbar8 (u32 addr)
{
	struct opcode op;
	if (!fetch_opcode (&op))
	{
		printf ("EOF\n");
		exit (1);
	}
	if (!op.is_in || op.data_width != 8 || op.addr != addr || op.type != OPCODE_MCHBAR)
	{
		printf ("Bad %d: %x vs %x\n", __LINE__, addr, op.addr);
		exit (1);
	}
	return op.val;
}

static u8
read_tco8 (u32 addr)
{
	struct opcode op;
	if (!fetch_opcode (&op))
	{
		printf ("EOF\n");
		exit (1);
	}
	if (!op.is_in || op.data_width != 8 || op.addr != addr || op.type != TCO)
	{
		printf ("Bad %d: %x vs %x\n", __LINE__, addr, op.addr);
		exit (1);
	}
	return op.val;
}

static u32
read32 (u32 addr)
{
	struct opcode op;
	fetch_opcode (&op);
	if (!op.is_in || op.data_width != 32 || op.addr != addr || op.type != MEM)
	{
		printf ("Bad %d: %x vs %x\n", __LINE__, addr, op.addr);
		exit (1);
	}
	return op.val;
}

static u64
read64 (u32 addr)
{
	struct opcode op;
	if (!fetch_opcode (&op))
	{
		printf ("EOF\n");
		exit (1);
	}
	if (!op.is_in || op.data_width != 64 || op.addr != addr || op.type != MEM)
	{
		printf ("Bad %d: %x vs %x\n", __LINE__, addr, op.addr);
		exit (1);
	}
	return op.val;
}

static void
clflush (u32 addr)
{
	struct opcode op;
	if (!fetch_opcode (&op))
	{
		printf ("EOF\n");
		exit (1);
	}

	if (op.addr != addr || op.type != CLFLUSH)
	{
		printf ("Bad %d: %x vs %x\n", __LINE__, addr, op.addr);
		exit (1);
	}
}

static void
read128 (u32 addr, u64 *out)
{
  out[0] = read64 (addr);
  out[1] = read64 (addr + 8);
}

static u16
read16 (u32 addr)
{
	struct opcode op;
	fetch_opcode (&op);
	if (!op.is_in || op.data_width != 16 || op.addr != addr || op.type != MEM)
	{
		printf ("Bad %d: %x vs %x\n", __LINE__, addr, op.addr);
		exit (1);
	}
	return op.val;
}

static u8
read8 (u32 addr)
{
	struct opcode op;
	fetch_opcode (&op);
	if (!op.is_in || op.data_width != 8 || op.addr != addr || op.type != MEM)
	{
		printf ("Bad %d\n", __LINE__);
		exit (1);
	}
	return op.val;
}

static u8
inb (u32 addr)
{
	struct opcode op;
	fetch_opcode (&op);
	if (!op.is_in || op.data_width != 8 || op.addr != addr || op.type != PCIO)
	{
		printf ("Bad %d\n", __LINE__);
		exit (1);
	}
	return op.val;
}

static void
outb (u8 val, u32 addr)
{
	struct opcode op;
	if (!fetch_opcode (&op))
	{
		printf ("EOF\n");
		exit (1);
	}
	if (op.is_in || op.data_width != 8 || op.addr != addr || op.type != PCIO || op.val != val)
	{
	  printf ("Bad %d: %x, %x, 8, %d, 0 vs %x, %llx, %d, %d, %d\n", __LINE__, addr, val, PCIO, op.addr, op.val, op.data_width, op.type,
		  op.is_in);
	  printf ("%x, %llx, %d\n", val, op.val, op.val != val);
		exit (1);
	}
}

static void
outw (u16 val, u32 addr)
{
	struct opcode op;
	fetch_opcode (&op);
	if (op.is_in || op.data_width != 16 || op.addr != addr || op.type != PCIO || op.val != val)
	{
	  printf ("Bad %d: %x, %x vs %x, %llx\n", __LINE__, addr, val, op.addr, op.val);
		exit (1);
	}
}

static void
outl (u32 val, u32 addr)
{
	struct opcode op;
	fetch_opcode (&op);
	if (op.is_in || op.data_width != 32 || op.addr != addr || op.type != PCIO || op.val != val)
	{
		printf ("Bad %d\n", __LINE__);
		exit (1);
	}
}

static u32
inl (u32 addr)
{
	struct opcode op;
	fetch_opcode (&op);
	if (!op.is_in || op.data_width != 32 || op.addr != addr || op.type != PCIO)
	{
		printf ("Bad %d\n", __LINE__);
		exit (1);
	}
	return op.val;
}

static u16
inw (u32 addr)
{
	struct opcode op;
	fetch_opcode (&op);
	if (!op.is_in || op.data_width != 16 || op.addr != addr || op.type != PCIO)
	{
		printf ("Bad %d\n", __LINE__);
		exit (1);
	}
	return op.val;
}

static void
pci_mm_write8 (int bus, int dev, int func, u32 addr, u8 val)
{
	struct opcode op;
	if (bus == 0xff)
	  {
	    write8 (DEFAULT_PCIEXBAR | (bus << 20) | (dev << 15) | (func << 12) | addr, val);
	    return;
	  }

	fetch_opcode (&op);
	if (op.is_in || op.data_width != 8 || op.addr != addr || op.type != PCIMM || op.dev != dev || op.func != func || op.bus != bus || op.val != val)
	{
		printf ("Bad %d\n", __LINE__);
		exit (1);
	}
}

static void
pci_write8 (int bus, int dev, int func, u32 addr, u8 val)
{
	struct opcode op;
	fetch_opcode (&op);
	if (op.is_in || op.data_width != 8 || op.addr != addr || op.type != PCI || op.dev != dev || op.func != func || op.bus != bus || op.val != val)
	{
		printf ("Bad %d\n", __LINE__);
		exit (1);
	}
}

static void
pci_write16 (int bus, int dev, int func, u32 addr, u16 val)
{
	struct opcode op;
	fetch_opcode (&op);
	if (op.is_in || op.data_width != 16 || op.addr != addr || op.type != PCI || op.dev != dev || op.func != func || op.bus != bus || op.val != val)
	{
		printf ("Bad %d\n", __LINE__);
		exit (1);
	}
}

static void
pci_mm_write16 (int bus, int dev, int func, u32 addr, u16 val)
{
	struct opcode op;
	fetch_opcode (&op);
	if (op.is_in || op.data_width != 16 || op.addr != addr || op.type != PCIMM || op.dev != dev || op.func != func || op.bus != bus || op.val != val)
	{
		printf ("Bad %d\n", __LINE__);
		exit (1);
	}
}

static void
pci_mm_write32 (int bus, int dev, int func, u32 addr, u32 val)
{
	struct opcode op;
	if (bus == 0xff)
	  {
	    write32 (DEFAULT_PCIEXBAR | (bus << 20) | (dev << 15) | (func << 12) | addr, val);
	    return;
	  }

	if (!fetch_opcode (&op))
	{
		printf ("EOF\n");
		exit (1);
	}

	if (op.is_in || op.data_width != 32 || op.addr != addr || op.type != PCIMM || op.dev != dev || op.func != func || op.bus != bus || op.val != val)
	{
		printf ("Bad %d\n", __LINE__);
		exit (1);
	}
}

static void
pci_write32 (int bus, int dev, int func, u32 addr, u32 val)
{
	struct opcode op;
	if (!fetch_opcode (&op))
	{
		printf ("EOF\n");
		exit (1);
	}
	if (op.is_in || op.data_width != 32 || op.addr != addr || op.type != PCI || op.dev != dev || op.func != func || op.bus != bus || op.val != val)
	{
		printf ("Bad %d\n", __LINE__);
		exit (1);
	}
}

static u8
pci_read8 (int bus, int dev, int func, u32 addr)
{
	struct opcode op;
	fetch_opcode (&op);
	if (!op.is_in || op.data_width != 8 || op.addr != addr || op.type != PCI || op.dev != dev || op.func != func || op.bus != bus)
	{
		printf ("Bad %d\n", __LINE__);
		exit (1);
	}
	return op.val;
}

static u8
nvram_read (u8 addr)
{
	struct opcode op;
	fetch_opcode (&op);
	if (!op.is_in || op.addr != addr || op.type != NVRAM)
	{
		printf ("Bad %d\n", __LINE__);
		exit (1);
	}
	return op.val;
}

static void
nvram_write (u8 addr, u8 val)
{
	struct opcode op;
	fetch_opcode (&op);
	if (op.is_in || op.addr != addr || op.type != NVRAM || op.val != val)
	{
		printf ("Bad %d\n", __LINE__);
		exit (1);
	}
}

static u8
pci_mm_read8 (int bus, int dev, int func, u32 addr)
{
	struct opcode op;
	fetch_opcode (&op);
	if (!op.is_in || op.data_width != 8 || op.addr != addr || op.type != PCIMM || op.dev != op.dev || op.func != op.func || op.bus != op.bus)
	{
		printf ("Bad %d\n", __LINE__);
		exit (1);
	}
	return op.val;
}

static u16
pci_mm_read16 (int bus, int dev, int func, u32 addr)
{
	struct opcode op;
	if (!fetch_opcode (&op))
	{
		printf ("EOF\n");
		exit (1);
	}
	if (!op.is_in || op.data_width != 16 || op.addr != addr || op.type != PCIMM || op.dev != dev || op.func != func || op.bus != bus)
	{
	  printf ("Bad %d: %x vs %x\n", __LINE__, addr, op.addr);
		exit (1);
	}
	return op.val;
}

static u16
pci_read16 (int bus, int dev, int func, u32 addr)
{
	struct opcode op;
	fetch_opcode (&op);
	if (!op.is_in || op.data_width != 16 || op.addr != addr || op.type != PCI || op.dev != op.dev || op.func != op.func || op.bus != op.bus)
	{
		printf ("Bad %d\n", __LINE__);
		exit (1);
	}
	return op.val;
}

static u32
pci_mm_read32 (int bus, int dev, int func, u32 addr)
{
	struct opcode op;

	if (bus == 0xff)
	  return read32 (DEFAULT_PCIEXBAR | (bus << 20) | (dev << 15) | (func << 12) | addr);

	fetch_opcode (&op);
	if (!op.is_in || op.data_width != 32 || op.addr != addr || op.type != PCIMM || op.dev != op.dev || op.func != op.func || op.bus != op.bus)
	{
		printf ("Bad %d\n", __LINE__);
		exit (1);
	}
	return op.val;
}

static u32
pci_read32 (int bus, int dev, int func, u32 addr)
{
	struct opcode op;
	if (!fetch_opcode (&op))
	{
		printf ("EOF\n");
		exit (1);
	}
	if (!op.is_in || op.data_width != 32 || op.addr != addr || op.type != PCI || op.dev != op.dev || op.func != op.func || op.bus != op.bus)
	{
		printf ("Bad %d\n", __LINE__);
		exit (1);
	}
	return op.val;
}

struct cpuid_result
{
  u32 eax, ebx;
};


struct cpuid_result
cpuid_ext(u32 eax, u32 ecx)
{
  struct cpuid_result ret;
  struct opcode op;
  if (!fetch_opcode (&op))
    {
      printf ("EOF\n");
      exit (1);
    }
  if (op.addr != eax || op.type != CPUID || op.ecx != ecx)
    {
      printf ("Bad %d\n", __LINE__);
      exit (1);
    }
  ret.eax = op.val;
  ret.ebx = op.val >> 32;
  return ret;
}

static u64
my_read_msr (u32 addr)
{
	struct opcode op;
	fetch_opcode (&op);
	if (!op.is_in || op.addr != addr || op.type != MSR)
	{
		printf ("Bad %d\n", __LINE__);
		exit (1);
	}
	return op.val;
}

static u64
my_write_msr (u32 addr, u64 val)
{
	struct opcode op;
	fetch_opcode (&op);
	if (op.is_in || op.addr != addr || op.type != MSR || op.val != val)
	{
		printf ("Bad %d\n", __LINE__);
		exit (1);
	}
	return op.val;
}

static void
die (const char *msg)
{
  printf ("%s\n", msg);
  exit (1);
}

static void
intel_early_me_init (void)
{
}

static unsigned
intel_early_me_uma_size (void)
{
  u32 t;
  t = pci_mm_read32 (HECIDEV, 0x44);
  if ( t & 0x10000 )
    return t & 0x3F;
  return 0;
}

static u8
read_mchbar8_bypass (u32 addr)
{
  return read_mchbar8 (addr);
}

#define printk(condition, fmt, args...) printf(fmt, ## args)

#define udelay(x)



#if 1
static const struct ram_training *
get_cached_training (void)
{
  return NULL;
#if 0
  static const struct ram_training ret =
    {
#if 1
      .lane_timings =
      {
	{
	  {
	    {
	      { 5, 5, 3, 4, 4, 3, 4, 4, 21 },
	      { 5, 4, 2, 5, 4, 3, 4, 4, 21 }
	    },
	    {
	      { 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	      { 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	    }
	  },
	  {
	    {
	      { 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	      { 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	    },
	    {
	      { 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	      { 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	    }
	  },
	},
	{
	  {
	    {
	      { 0x6e, 0x64, 0x7b, 0x56, 0xbd, 0xa0, 0xae, 0xad, 0x100 },
	      { 0x6e, 0x67, 0x7a, 0x54, 0xbd, 0x9f, 0xac, 0xac, 0x100 }
	    },
	    {
	      { 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	      { 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	    }
	  },
	  {
	    {
	      { 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	      { 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	    },
	    {
	      { 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	      { 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	    }
	  },
	},
	{
	  {
	    {
	      { 0x59, 0x55, 0x6d, 0x44, 0xa3, 0x76, 0x90, 0x81, 0x80 },
	      { 0x58, 0x51, 0x6b, 0x41, 0xa1, 0x75, 0x8e, 0x7f, 0x80 },
	    },
	    {
	      { 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	      { 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	    }
	  },
	  {
	    {
	      { 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	      { 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	    },
	    {
	      { 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	      { 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	    }
	  },
	},
	{
	  {
	    {
	      { 0x78, 0x74, 0x8b, 0x64, 0xc1, 0x94, 0xaf, 0x9d, 0x80 },
	      { 0x76, 0x6e, 0x88, 0x60, 0xbe, 0x93, 0xae, 0x9d, 0x80 },
	    },
	    {
	      { 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	      { 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	    }
	  },
	  {
	    {
	      { 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	      { 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	    },
	    {
	      { 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	      { 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	    }
	  },
	}
      },
      .reg_178 = 0x42,
      .reg_10b = 1,
      .v775 = { 19, 35 },
      .v777 = 
      {
	{
	  {
	    { 
	      { 0x01, 0x25 },
	      { 0x01, 0x25 },
	      { 0x01, 0x21 },
	      { 0x02, 0x22 },
	      { 0x00, 0x23 },
	      { 0x00, 0x21 },
	      { 0x01, 0x22 },
	      { 0x01, 0x22 },
	      { 0x00, 0x00 }
	    },
	    { 
	      { 0x01, 0x24 },
	      { 0x02, 0x23 },
	      { 0x01, 0x20 },
	      { 0x01, 0x24 },
	      { 0x00, 0x22 },
	      { 0x01, 0x21 },
	      { 0x01, 0x21 },
	      { 0x02, 0x21 },
	      { 0x00, 0x00 }
	    },
	    { 
	      { 0x00, 0x00 },
	      { 0x00, 0x00 },
	      { 0x00, 0x00 },
	      { 0x00, 0x00 },
	      { 0x00, 0x00 },
	      { 0x00, 0x00 },
	      { 0x00, 0x00 },
	      { 0x00, 0x00 },
	      { 0x00, 0x00 },
	    },
	    {
	      { 0x00, 0x00 },
	      { 0x00, 0x00 },
	      { 0x00, 0x00 },
	      { 0x00, 0x00 },
	      { 0x00, 0x00 },
	      { 0x00, 0x00 },
	      { 0x00, 0x00 },
	      { 0x00, 0x00 },
	      { 0x00, 0x00 },
	    }
	  },
	  {
	    {
	      { 0x00, 0x00 },
	      { 0x00, 0x00 },
	      { 0x00, 0x00 },
	      { 0x00, 0x00 },
	      { 0x00, 0x00 },
	      { 0x00, 0x00 },
	      { 0x00, 0x00 },
	      { 0x00, 0x00 },
	      { 0x00, 0x00 },
	    },
	    {
	      { 0x00, 0x00 },
	      { 0x00, 0x00 },
	      { 0x00, 0x00 },
	      { 0x00, 0x00 },
	      { 0x00, 0x00 },
	      { 0x00, 0x00 },
	      { 0x00, 0x00 },
	      { 0x00, 0x00 },
	      { 0x00, 0x00 },
	    },
	    {
	      { 0x00, 0x00 },
	      { 0x00, 0x00 },
	      { 0x00, 0x00 },
	      { 0x00, 0x00 },
	      { 0x00, 0x00 },
	      { 0x00, 0x00 },
	      { 0x00, 0x00 },
	      { 0x00, 0x00 },
	      { 0x00, 0x00 },
	    },
	    {
	      { 0x00, 0x00 },
	      { 0x00, 0x00 },
	      { 0x00, 0x00 },
	      { 0x00, 0x00 },
	      { 0x00, 0x00 },
	      { 0x00, 0x00 },
	      { 0x00, 0x00 },
	      { 0x00, 0x00 },
	      { 0x00, 0x00 },
	    }
	  }
	},
	{
	  {
	    { 
	      { 0x00, 0x25 },
	      { 0x01, 0x24 },
	      { 0x01, 0x20 },
	      { 0x01, 0x22 },
	      { 0x00, 0x22 },
	      { 0x00, 0x21 },
	      { 0x01, 0x23 },
	      { 0x00, 0x22 },
	      { 0x00, 0x00 }
	    },
	    { 
	      { 0x00, 0x26 },
	      { 0x01, 0x22 },
	      { 0x01, 0x20 },
	      { 0x01, 0x24 },
	      { 0x00, 0x23 },
	      { 0x00, 0x21 },
	      { 0x01, 0x22 },
	      { 0x01, 0x22 },
	      { 0x00, 0x00 }
	    },
	    {
	      { 0x00, 0x00 },
	      { 0x00, 0x00 },
	      { 0x00, 0x00 },
	      { 0x00, 0x00 },
	      { 0x00, 0x00 },
	      { 0x00, 0x00 },
	      { 0x00, 0x00 },
	      { 0x00, 0x00 },
	      { 0x00, 0x00 },
	    },
	    {
	      { 0x00, 0x00 },
	      { 0x00, 0x00 },
	      { 0x00, 0x00 },
	      { 0x00, 0x00 },
	      { 0x00, 0x00 },
	      { 0x00, 0x00 },
	      { 0x00, 0x00 },
	      { 0x00, 0x00 },
	      { 0x00, 0x00 },
	    }
	  },
	  {
	    {
	      { 0x00, 0x00 },
	      { 0x00, 0x00 },
	      { 0x00, 0x00 },
	      { 0x00, 0x00 },
	      { 0x00, 0x00 },
	      { 0x00, 0x00 },
	      { 0x00, 0x00 },
	      { 0x00, 0x00 },
	      { 0x00, 0x00 },
	    },
	    {
	      { 0x00, 0x00 },
	      { 0x00, 0x00 },
	      { 0x00, 0x00 },
	      { 0x00, 0x00 },
	      { 0x00, 0x00 },
	      { 0x00, 0x00 },
	      { 0x00, 0x00 },
	      { 0x00, 0x00 },
	      { 0x00, 0x00 },
	    },
	    {
	      { 0x00, 0x00 },
	      { 0x00, 0x00 },
	      { 0x00, 0x00 },
	      { 0x00, 0x00 },
	      { 0x00, 0x00 },
	      { 0x00, 0x00 },
	      { 0x00, 0x00 },
	      { 0x00, 0x00 },
	      { 0x00, 0x00 },
	    },
	    {
	      { 0x00, 0x00 },
	      { 0x00, 0x00 },
	      { 0x00, 0x00 },
	      { 0x00, 0x00 },
	      { 0x00, 0x00 },
	      { 0x00, 0x00 },
	      { 0x00, 0x00 },
	      { 0x00, 0x00 },
	      { 0x00, 0x00 },
	    }
	  }
	}
      },
      .v1065 = 
      {
	{
	  { 0x9c, 0x92, 0xab, 0x85, 0xec, 0xd0, 0xdd, 0xdc, 0x00 },
	  { 0x9c, 0x96, 0xab, 0x82, 0xec, 0xcf, 0xdb, 0xdb, 0x00 },
	  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }
	},
	{
	  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }
	}
      },
      .v1209 = {
	{
	  { 0x59, 0x55, 0x6d, 0x44, 0xa3, 0x76, 0x90, 0x81, 0x00 },
	  { 0x58, 0x51, 0x6b, 0x41, 0xa1, 0x75, 0x8e, 0x7f, 0x00 },
	  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	},
	{
	  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	  { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	}
      },
      .v1353 = {
	{
	  {
	    { 0x5d, 0x93 },
	    { 0x59, 0x90 },
	    { 0x70, 0xa6 },
	    { 0x49, 0x7f },
	    { 0xa5, 0xdd },
	    { 0x79, 0xb0 },
	    { 0x94, 0xca },
	    { 0x83, 0xb8 },
	    { 0x00, 0x00 },
	  },
	  {
	    { 0x5c, 0x91 },
	    { 0x53, 0x89 },
	    { 0x6d, 0xa4 },
	    { 0x45, 0x7c },
	    { 0xa3, 0xd9 },
	    { 0x77, 0xaf },
	    { 0x94, 0xc9 },
	    { 0x82, 0xb8 },
	    { 0x00, 0x00 },
	  },
	  {
	    { 0x00, 0x00 },
	    { 0x00, 0x00 },
	    { 0x00, 0x00 },
	    { 0x00, 0x00 },
	    { 0x00, 0x00 },
	    { 0x00, 0x00 },
	    { 0x00, 0x00 },
	    { 0x00, 0x00 },
	    { 0x00, 0x00 },
	  },
	  {
	    { 0x00, 0x00 },
	    { 0x00, 0x00 },
	    { 0x00, 0x00 },
	    { 0x00, 0x00 },
	    { 0x00, 0x00 },
	    { 0x00, 0x00 },
	    { 0x00, 0x00 },
	    { 0x00, 0x00 },
	    { 0x00, 0x00 },
	  }
	},
	{ 
	  {
	    { 0x00, 0x00 },
	    { 0x00, 0x00 },
	    { 0x00, 0x00 },
	    { 0x00, 0x00 },
	    { 0x00, 0x00 },
	    { 0x00, 0x00 },
	    { 0x00, 0x00 },
	    { 0x00, 0x00 },
	    { 0x00, 0x00 },
	  },
	  {
	    { 0x00, 0x00 },
	    { 0x00, 0x00 },
	    { 0x00, 0x00 },
	    { 0x00, 0x00 },
	    { 0x00, 0x00 },
	    { 0x00, 0x00 },
	    { 0x00, 0x00 },
	    { 0x00, 0x00 },
	    { 0x00, 0x00 },
	  },
	  {
	    { 0x00, 0x00 },
	    { 0x00, 0x00 },
	    { 0x00, 0x00 },
	    { 0x00, 0x00 },
	    { 0x00, 0x00 },
	    { 0x00, 0x00 },
	    { 0x00, 0x00 },
	    { 0x00, 0x00 },
	    { 0x00, 0x00 },
	  },
	  {
	    { 0x00, 0x00 },
	    { 0x00, 0x00 },
	    { 0x00, 0x00 },
	    { 0x00, 0x00 },
	    { 0x00, 0x00 },
	    { 0x00, 0x00 },
	    { 0x00, 0x00 },
	    { 0x00, 0x00 },
	    { 0x00, 0x00 },
	  }
	}
      },
#else
      .lane_timings =
      {
	{
	  {
	    {
	      { 5, 5, 3, 5, 4, 4, 5, 3, 21 },
	      { 6, 5, 4, 4, 4, 3, 4, 4, 21 }
	    },
	    {
	      { 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	      { 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	    }
	  },
	  {
	    {
	      { 3, 4, 5, 3, 2, 4, 4, 4, 21 },
	      { 3, 3, 5, 4, 2, 5, 3, 4, 21 },
	    },
	    {
	      { 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	      { 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	    }
	  },
	},
	{
	  {
	    {
	      { 0x6f, 0x66, 0x82, 0x58, 0xc5, 0xa6, 0xb4, 0xb1, 0x100 },
	      { 0x70, 0x67, 0x84, 0x59, 0xc5, 0xa3, 0xb4, 0xb2, 0x100 }
	    },
	    {
	      { 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	      { 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	    }
	  },
	  {
	    {
	      { 0x91, 0x87, 0x98, 0x76, 0xdc, 0xb7, 0xcf, 0xc5, 0x100 },
	      { 0x92, 0x8d, 0x9b, 0x76, 0xde, 0xb9, 0xce, 0xc6, 0x100 },
	    },
	    {
	      { 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	      { 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	    }
	  },
	},
	{
	  {
	    {
	      { 0x5f, 0x5b, 0x74, 0x4a, 0xa9, 0x7c, 0x95, 0x85, 0x80 },
	      { 0x5d, 0x59, 0x72, 0x49, 0xa8, 0x7a, 0x96, 0x85, 0x80 },
	    },
	    {
	      { 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	      { 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	    }
	  },
	  {
	    {
	      { 0x5b, 0x53, 0x6c, 0x49, 0xa8, 0x7a, 0x92, 0x84, 0x80 },
	      { 0x5b, 0x51, 0x6c, 0x48, 0xa7, 0x79, 0x91, 0x82, 0x80 },
	    },
	    {
	      { 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	      { 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	    }
	  },
	},
	{
	  {
	    {
	      { 0x7e, 0x7a, 0x92, 0x6a, 0xc7, 0x9b, 0xb6, 0xa4, 0x80 },
	      { 0x7d, 0x77, 0x8f, 0x69, 0xc6, 0x98, 0xb6, 0xa4, 0x80 },
	    },
	    {
	      { 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	      { 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	    }
	  },
	  {
	    {
	      { 0x78, 0x6f, 0x89, 0x65, 0xc4, 0x97, 0xaf, 0x9f, 0x80 },
	      { 0x78, 0x6b, 0x89, 0x64, 0xc2, 0x96, 0xae, 0x9d, 0x80 },
	    },
	    {
	      { 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	      { 0, 0, 0, 0, 0, 0, 0, 0, 0 },
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

static void
pre_raminit_3 (int x2ca8)
{
  u8 t;
  int i;

  gav (t = nvram_read (0x33));
  if (x2ca8 == 0)
    {
      nvram_write (0x33, t & ~0x40);
      gav (read32 (DEFAULT_RCBA | 0x3598));
      write32 (DEFAULT_RCBA | 0x3598, 0x1);
      pci_write16 (0, 0x1d, 0x0, 0x20, 0x2000);
      gav (pci_read8 (0, 0x1d, 0x0, 0x4)); // = 0xff
      pci_write8 (0, 0x1d, 0x0, 0x4, 0xff);
      pci_write16 (0, 0x1d, 0x1, 0x20, 0x2020);
      gav (pci_read8 (0, 0x1d, 0x1, 0x4)); // = 0xff
      pci_write8 (0, 0x1d, 0x1, 0x4, 0xff);
      pci_write16 (0, 0x1d, 0x2, 0x20, 0x2040);
      gav (pci_read8 (0, 0x1d, 0x2, 0x4)); // = 0xff
      pci_write8 (0, 0x1d, 0x2, 0x4, 0xff);
      pci_write16 (0, 0x1d, 0x3, 0x20, 0x2060);
      gav (pci_read8 (0, 0x1d, 0x3, 0x4)); // = 0xff
      pci_write8 (0, 0x1d, 0x3, 0x4, 0xff);
      pci_write16 (0, 0x1a, 0x0, 0x20, 0x2080);
      gav (pci_read8 (0, 0x1a, 0x0, 0x4)); // = 0xff
      pci_write8 (0, 0x1a, 0x0, 0x4, 0xff);
      pci_write16 (0, 0x1a, 0x1, 0x20, 0x20a0);
      gav (pci_read8 (0, 0x1a, 0x1, 0x4)); // = 0xff
      pci_write8 (0, 0x1a, 0x1, 0x4, 0xff);
      pci_write16 (0, 0x1a, 0x2, 0x20, 0x20e0);
      gav (pci_read8 (0, 0x1a, 0x2, 0x4)); // = 0xff
      pci_write8 (0, 0x1a, 0x2, 0x4, 0xff);
      for (i = 0; i < 15; i++)
	{
	  gav (inw (0x2010)); // = 0xff
	  gav (inw (0x2012)); // = 0xff
	  gav (inw (0x2030)); // = 0xff
	  gav (inw (0x2032)); // = 0xff
	  gav (inw (0x2050)); // = 0xff
	  gav (inw (0x2052)); // = 0xff
	  gav (inw (0x2070)); // = 0xff
	  gav (inw (0x2072)); // = 0xff
	  gav (inw (0x2090)); // = 0xff
	  gav (inw (0x2092)); // = 0xff
	  gav (inw (0x20b0)); // = 0xff
	  gav (inw (0x20b2)); // = 0xff
	  gav (inw (0x20f0)); // = 0xff
	  gav (inw (0x20f2)); // = 0xff
	  if (i != 14)
	    pm_wait (0x400); /* <10*/ 
	}
      pci_write16 (0, 0x1d, 0x0, 0x20, 0x0);
      gav (pci_read8 (0, 0x1d, 0x0, 0x4)); // = 0xff
      pci_write8 (0, 0x1d, 0x0, 0x4, 0xfe);
      pci_write16 (0, 0x1d, 0x1, 0x20, 0x0);
      gav (pci_read8 (0, 0x1d, 0x1, 0x4)); // = 0xff
      pci_write8 (0, 0x1d, 0x1, 0x4, 0xfe);
      pci_write16 (0, 0x1d, 0x2, 0x20, 0x0);
      gav (pci_read8 (0, 0x1d, 0x2, 0x4)); // = 0xff
      pci_write8 (0, 0x1d, 0x2, 0x4, 0xfe);
      pci_write16 (0, 0x1d, 0x3, 0x20, 0x0);
      gav (pci_read8 (0, 0x1d, 0x3, 0x4)); // = 0xff
      pci_write8 (0, 0x1d, 0x3, 0x4, 0xfe);
      pci_write16 (0, 0x1a, 0x0, 0x20, 0x0);
      gav (pci_read8 (0, 0x1a, 0x0, 0x4)); // = 0xff
      pci_write8 (0, 0x1a, 0x0, 0x4, 0xfe);
      pci_write16 (0, 0x1a, 0x1, 0x20, 0x0);
      gav (pci_read8 (0, 0x1a, 0x1, 0x4)); // = 0xff
      pci_write8 (0, 0x1a, 0x1, 0x4, 0xfe);
      pci_write16 (0, 0x1a, 0x2, 0x20, 0x0);
      gav (pci_read8 (0, 0x1a, 0x2, 0x4)); // = 0xff
      pci_write8 (0, 0x1a, 0x2, 0x4, 0xfe);
      write32 (DEFAULT_RCBA | 0x3598, 0x0);
    }
}


