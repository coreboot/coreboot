/* SPDX-License-Identifier: GPL-2.0-only */

#define __SIMPLE_DEVICE__

#include <assert.h>
#include <commonlib/bsd/helpers.h>
#include <console/console.h>
#include <device/mmio.h>
#include <device/pci_def.h>
#include <device/pci_ops.h>
#include <device/pci_type.h>
#include <intelblocks/pcr.h>
#include <soc/pci_devs.h>
#include <timer.h>
#include <types.h>

#if (CONFIG_PCR_BASE_ADDRESS == 0)
#error "PCR_BASE_ADDRESS need to be non-zero!"
#endif

#if !CONFIG(PCR_COMMON_IOSF_1_0)

#define PCR_SBI_CMD_TIMEOUT	10 /* 10ms */

/* P2SB PCI configuration register */
#define P2SB_CR_SBI_ADDR	0xd0
#define  P2SB_CR_SBI_DESTID	24
#define P2SB_CR_SBI_DATA	0xd4
#define P2SB_CR_SBI_STATUS	0xd8
/* Bit 15:8 */
#define  P2SB_CR_SBI_OPCODE	8
#define  P2SB_CR_SBI_OPCODE_MASK	0xFF00
/* Bit 7 */
#define  P2SB_CR_SBI_POSTED	7
#define  P2SB_CR_SBI_POSTED_MASK	0x0080
/* Bit 2-1 */
#define  P2SB_CR_SBI_STATUS_MASK	0x0006
#define  P2SB_CR_SBI_STATUS_SUCCESS	0
#define  P2SB_CR_SBI_STATUS_NOT_SUPPORTED	1
#define  P2SB_CR_SBI_STATUS_POWERED_DOWN	2
#define  P2SB_CR_SBI_STATUS_MULTI_CAST_MIXED	3
/* Bit 0 */
#define  P2SB_CR_SBI_STATUS_READY	0
#define  P2SB_CR_SBI_STATUS_BUSY	1
#define P2SB_CR_SBI_ROUTE_IDEN	0xda
/* Bit 15-12 */
#define  P2SB_CR_SBI_FBE	12
#define  P2SB_CR_SBI_FBE_MASK	0xF
/* Bit 10-8 */
#define  P2SB_CR_SBI_BAR	8
#define  P2SB_CR_SBI_MASK	0x7
/* Bit 7-0 */
#define  P2SB_CR_SBI_FID	0
#define  P2SB_CR_SBI_FID_MASK	0xFF
#define P2SB_CR_SBI_EXT_ADDR	0xdc
#endif

static void *__pcr_reg_address(uint8_t pid, uint16_t offset)
{
	uintptr_t reg_addr;

	/* Create an address based off of port id and offset. */
	reg_addr = CONFIG_PCR_BASE_ADDRESS;
	reg_addr += ((uintptr_t)pid) << PCR_PORTID_SHIFT;
	reg_addr += (uintptr_t)offset;

	return (void *)reg_addr;
}

void *pcr_reg_address(uint8_t pid, uint16_t offset)
{
	if (CONFIG(PCR_COMMON_IOSF_1_0))
		assert(IS_ALIGNED(offset, sizeof(uint32_t)));

	return __pcr_reg_address(pid, offset);
}

/*
 * The mapping of addresses via the SBREG_BAR assumes the IOSF-SB
 * agents are using 32-bit aligned accesses for their configuration
 * registers. For IOSF versions greater than 1_0, IOSF-SB
 * agents can use any access (8/16/32 bit aligned) for their
 * configuration registers
 */
static inline void check_pcr_offset_align(uint16_t offset, size_t size)
{
	const size_t align = CONFIG(PCR_COMMON_IOSF_1_0) ?
					sizeof(uint32_t) : size;

	assert(IS_ALIGNED(offset, align));
}

uint32_t pcr_read32(uint8_t pid, uint16_t offset)
{
	/* Ensure the PCR offset is correctly aligned. */
	assert(IS_ALIGNED(offset, sizeof(uint32_t)));

	return read32(__pcr_reg_address(pid, offset));
}

uint16_t pcr_read16(uint8_t pid, uint16_t offset)
{
	/* Ensure the PCR offset is correctly aligned. */
	check_pcr_offset_align(offset, sizeof(uint16_t));

	return read16(__pcr_reg_address(pid, offset));
}

uint8_t pcr_read8(uint8_t pid, uint16_t offset)
{
	/* Ensure the PCR offset is correctly aligned. */
	check_pcr_offset_align(offset, sizeof(uint8_t));

	return read8(__pcr_reg_address(pid, offset));
}

/*
 * After every write one needs to perform a read an innocuous register to
 * ensure the writes are completed for certain ports. This is done for
 * all ports so that the callers don't need the per-port knowledge for
 * each transaction.
 */
static inline void write_completion(uint8_t pid, uint16_t offset)
{
	read32(__pcr_reg_address(pid, ALIGN_DOWN(offset, sizeof(uint32_t))));
}

void pcr_write32(uint8_t pid, uint16_t offset, uint32_t indata)
{
	/* Ensure the PCR offset is correctly aligned. */
	assert(IS_ALIGNED(offset, sizeof(indata)));

	write32(__pcr_reg_address(pid, offset), indata);
	/* Ensure the writes complete. */
	write_completion(pid, offset);
}

void pcr_write16(uint8_t pid, uint16_t offset, uint16_t indata)
{
	/* Ensure the PCR offset is correctly aligned. */
	check_pcr_offset_align(offset, sizeof(uint16_t));

	write16(__pcr_reg_address(pid, offset), indata);
	/* Ensure the writes complete. */
	write_completion(pid, offset);
}

void pcr_write8(uint8_t pid, uint16_t offset, uint8_t indata)
{
	/* Ensure the PCR offset is correctly aligned. */
	check_pcr_offset_align(offset, sizeof(uint8_t));

	write8(__pcr_reg_address(pid, offset), indata);
	/* Ensure the writes complete. */
	write_completion(pid, offset);
}

void pcr_rmw32(uint8_t pid, uint16_t offset, uint32_t anddata, uint32_t ordata)
{
	uint32_t data32;

	data32 = pcr_read32(pid, offset);
	data32 &= anddata;
	data32 |= ordata;
	pcr_write32(pid, offset, data32);
}

void pcr_rmw16(uint8_t pid, uint16_t offset, uint16_t anddata, uint16_t ordata)
{
	uint16_t data16;

	data16 = pcr_read16(pid, offset);
	data16 &= anddata;
	data16 |= ordata;
	pcr_write16(pid, offset, data16);
}

void pcr_rmw8(uint8_t pid, uint16_t offset, uint8_t anddata, uint8_t ordata)
{
	uint8_t data8;

	data8 = pcr_read8(pid, offset);
	data8 &= anddata;
	data8 |= ordata;
	pcr_write8(pid, offset, data8);
}

void pcr_or32(uint8_t pid, uint16_t offset, uint32_t ordata)
{
	uint32_t data32;

	data32 = pcr_read32(pid, offset);
	data32 |= ordata;
	pcr_write32(pid, offset, data32);
}

void pcr_or16(uint8_t pid, uint16_t offset, uint16_t ordata)
{
	uint16_t data16;

	data16 = pcr_read16(pid, offset);
	data16 |= ordata;
	pcr_write16(pid, offset, data16);
}

void pcr_or8(uint8_t pid, uint16_t offset, uint8_t ordata)
{
	uint8_t data8;

	data8 = pcr_read8(pid, offset);
	data8 |= ordata;
	pcr_write8(pid, offset, data8);
}

#if !CONFIG(PCR_COMMON_IOSF_1_0)

static int pcr_wait_for_completion(const pci_devfn_t dev)
{
	struct stopwatch sw;

	stopwatch_init_msecs_expire(&sw, PCR_SBI_CMD_TIMEOUT);
	do {
		if ((pci_read_config16(dev, P2SB_CR_SBI_STATUS) &
			P2SB_CR_SBI_STATUS_BUSY) == 0)
			return 0;
	} while (!stopwatch_expired(&sw));

	return -1;
}

/*
 * API to perform sideband communication
 *
 * Input:
 * struct pcr_sbi_msg
 * data - read/write for sbi message
 * response -
 * 0 - successful
 * 1 - unsuccessful
 * 2 - powered down
 * 3 - multi-cast mixed
 *
 * Output:
 * 0: SBI message is successfully completed
 * -1: SBI message failure
 */
int pcr_execute_sideband_msg(pci_devfn_t dev, struct pcr_sbi_msg *msg, uint32_t *data,
		uint8_t *response)
{
	uint32_t sbi_data;
	uint16_t sbi_status;
	uint16_t sbi_rid;

	if (!msg || !data || !response) {
		printk(BIOS_ERR, "Pointer checked for NULL Fail! "
		       "msg = %p \t data = %p \t response = %p\n",
		       msg, data, response);
		return -1;
	}

	switch (msg->opcode) {
		case MEM_READ:
		case MEM_WRITE:
		case PCI_CONFIG_READ:
		case PCI_CONFIG_WRITE:
		case PCR_READ:
		case PCR_WRITE:
		case GPIO_LOCK_UNLOCK:
			break;
		default:
			printk(BIOS_ERR, "SBI Failure: Wrong Input = %x!\n",
					msg->opcode);
			return -1;
			break;
	}

	if (pci_read_config16(dev, PCI_VENDOR_ID) == 0xffff) {
		printk(BIOS_ERR, "SBI Failure: P2SB device Hidden!\n");
		return -1;
	}

	/*
	 * BWG Section 2.2.1
	 * 1. Poll P2SB PCI offset D8h[0] = 0b
	 * Make sure the previous operation is completed.
	 */
	if (pcr_wait_for_completion(dev)) {
		printk(BIOS_ERR, "SBI Failure: Time Out!\n");
		return -1;
	}

	/* Initial Response status */
	*response = P2SB_CR_SBI_STATUS_NOT_SUPPORTED;

	/*
	 * 2. Write P2SB PCI offset D0h[31:0] with Address
	 * and Destination Port ID
	 */
	pci_write_config32(dev, P2SB_CR_SBI_ADDR,
		(msg->pid << P2SB_CR_SBI_DESTID) | msg->offset);

	/*
	 * 3. Write P2SB PCI offset DCh[31:0] with extended address,
	 * which is expected to be 0
	 */
	pci_write_config32(dev, P2SB_CR_SBI_EXT_ADDR, msg->offset >> 16);

	/*
	 * 4. Set P2SB PCI offset D8h[15:8] = 00000110b for read
	 *    Set P2SB PCI offset D8h[15:8] = 00000111b for write
	 *
	 * Set SBISTAT[15:8] to the opcode passed in
	 * Set SBISTAT[7] to the posted passed in
	 */
	sbi_status = pci_read_config16(dev, P2SB_CR_SBI_STATUS);
	sbi_status &= ~(P2SB_CR_SBI_OPCODE_MASK | P2SB_CR_SBI_POSTED_MASK);
	sbi_status |= (msg->opcode << P2SB_CR_SBI_OPCODE) |
				(msg->is_posted << P2SB_CR_SBI_POSTED);
	pci_write_config16(dev, P2SB_CR_SBI_STATUS, sbi_status);

	/*
	 * 5. Write P2SB PCI offset DAh[15:0] = F000h
	 *
	 * Set RID[15:0] = Fbe << 12 | Bar << 8 | Fid
	 */
	sbi_rid = ((msg->fast_byte_enable & P2SB_CR_SBI_FBE_MASK)
					<< P2SB_CR_SBI_FBE) |
			((msg->bar & P2SB_CR_SBI_MASK) << P2SB_CR_SBI_BAR) |
			(msg->fid & P2SB_CR_SBI_FID_MASK);
	pci_write_config16(dev, P2SB_CR_SBI_ROUTE_IDEN, sbi_rid);

	switch (msg->opcode) {
		case MEM_WRITE:
		case PCI_CONFIG_WRITE:
		case PCR_WRITE:
		case GPIO_LOCK_UNLOCK:
			/*
			 * 6. Write P2SB PCI offset D4h[31:0] with the
			 * intended data accordingly
			 */
			sbi_data = *data;
			pci_write_config32(dev, P2SB_CR_SBI_DATA, sbi_data);
			break;
		default:
			/* 6. Write P2SB PCI offset D4h[31:0] with dummy data */
			pci_write_config32(dev, P2SB_CR_SBI_DATA, 0);
			break;
	}

	/*
	 * 7. Set P2SB PCI offset D8h[0] = 1b, Poll P2SB PCI offset D8h[0] = 0b
	 *
	 * Set SBISTAT[0] = 1b, trigger the SBI operation
	 */
	sbi_status = pci_read_config16(dev, P2SB_CR_SBI_STATUS);
	sbi_status |= P2SB_CR_SBI_STATUS_BUSY;
	pci_write_config16(dev, P2SB_CR_SBI_STATUS, sbi_status);

	/* Poll SBISTAT[0] = 0b, Polling for Busy bit */
	if (pcr_wait_for_completion(dev)) {
		printk(BIOS_ERR, "SBI Failure: Time Out!\n");
		return -1;
	}

	/*
	 * 8. Check if P2SB PCI offset D8h[2:1] = 00b for
	 * successful transaction
	 */
	*response = (sbi_status & P2SB_CR_SBI_STATUS_MASK) >> 1;
	if (*response == P2SB_CR_SBI_STATUS_SUCCESS) {
		switch (msg->opcode) {
		case MEM_READ:
		case PCI_CONFIG_READ:
		case PCR_READ:
			sbi_data = pci_read_config32(dev, P2SB_CR_SBI_DATA);
			*data = sbi_data;
			break;
		default:
			break;
		}
		return 0;
	}
	printk(BIOS_ERR, "SBI Failure: Transaction Status = %x\n",
			*response);
	return -1;
}
#endif
