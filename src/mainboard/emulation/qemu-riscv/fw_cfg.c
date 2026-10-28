/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/mmio.h>
#include <endian.h>
#include <commonlib/endian.h>
#include <device/fw_cfg.h>
#include <mainboard/addressmap.h>

#define FW_CFG_PORT_CTL       0x08
#define FW_CFG_PORT_DATA      0x00
#define FW_CFG_DMA_ADDR_HIGH  0x10
#define FW_CFG_DMA_ADDR_LOW   0x14

void mb_fw_cfg_set_dma(uint32_t dma_desc_addr_hi, uint32_t dma_desc_addr_lo)
{
	write32p(QEMU_VIRT_FW_CFG + FW_CFG_DMA_ADDR_HIGH, dma_desc_addr_hi);
	write32p(QEMU_VIRT_FW_CFG + FW_CFG_DMA_ADDR_LOW, dma_desc_addr_lo);
}

void mb_fw_cfg_read_data(void *dst, int dstlen)
{
	for (size_t i = 0; i < dstlen; i++)
		((uint8_t *)dst)[i] = read8p(QEMU_VIRT_FW_CFG + FW_CFG_PORT_DATA);
}

void mb_fw_cfg_write_control(uint16_t entry)
{
	write16p(QEMU_VIRT_FW_CFG + FW_CFG_PORT_CTL, cpu_to_be16(entry));
}
