#include <parse.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ARRAY_SIZE(array) (sizeof (array) / sizeof (array[0]))

#define CONFIG_SMM_TSEG_SIZE (8 << 20)

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

#if 0
static u32
cpu_model (void)
{
	struct opcode op;
	if (!fetch_opcode (&op))
	{
		printf ("EOF\n");
		exit (1);
	}
	if (op.type != CPU_MODEL)
	{
		printf ("Bad %d\n", __LINE__);
		exit (1);
	}
	return 0x20655;
}
#endif

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
	fetch_opcode (&op);
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

static u64
rdmsr (u32 addr)
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

static const struct ram_training *
get_cached_training (void)
{
  static const struct ram_training ret =
    {
#if 0
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
#else
      .lane_timings =
      {
	{
	  {
	    {
	      { 5, 5, 3, 5, 4, 4, 5, 3, 21 },
	      { 6, 6, 3, 4, 4, 4, 4, 4, 21 }
	    },
	    {
	      { 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	      { 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	    }
	  },
	  {
	    {
	      { 3, 4, 5, 3, 2, 4, 3, 4, 21 },
	      { 4, 3, 5, 4, 2, 5, 4, 4, 21 },
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
	      { 0x6f, 0x66, 0x82, 0x58, 0xc5, 0xa6, 0xb4, 0xb2, 0x100 },
	      { 0x70, 0x68, 0x83, 0x59, 0xc5, 0xa5, 0xb4, 0xb2, 0x100 }
	    },
	    {
	      { 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	      { 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	    }
	  },
	  {
	    {
	      { 0x91, 0x88, 0x99, 0x76, 0xdc, 0xb8, 0xcf, 0xc5, 0x100 },
	      { 0x94, 0x8d, 0x9c, 0x76, 0xde, 0xba, 0xce, 0xc6, 0x100 },
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
	      { 0x60, 0x5b, 0x74, 0x4a, 0xa9, 0x7d, 0x96, 0x86, 0x80 },
	      { 0x5d, 0x59, 0x72, 0x49, 0xa8, 0x7a, 0x97, 0x86, 0x80 },
	    },
	    {
	      { 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	      { 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	    }
	  },
	  {
	    {
	      { 0x5b, 0x53, 0x6c, 0x49, 0xa8, 0x79, 0x92, 0x84, 0x80 },
	      { 0x5b, 0x51, 0x6d, 0x48, 0xa7, 0x7a, 0x92, 0x82, 0x80 },
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
	      { 0x7f, 0x7a, 0x92, 0x6a, 0xc7, 0x9c, 0xb7, 0xa5, 0x80 },
	      { 0x7d, 0x77, 0x8f, 0x69, 0xc6, 0x98, 0xb7, 0xa5, 0x80 },
	    },
	    {
	      { 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	      { 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	    }
	  },
	  {
	    {
	      { 0x79, 0x6f, 0x89, 0x65, 0xc4, 0x96, 0xaf, 0x9f, 0x80 },
	      { 0x78, 0x6b, 0x89, 0x64, 0xc2, 0x98, 0xaf, 0x9d, 0x80 },
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
}

#define udelay(x)
