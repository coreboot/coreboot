/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/io.h>
#include <device/fw_cfg.h>

#define FW_CFG_PORT_CTL       0x0510
#define FW_CFG_PORT_DATA      0x0511
#define FW_CFG_DMA_ADDR_HIGH  0x0514
#define FW_CFG_DMA_ADDR_LOW   0x0518

void mb_fw_cfg_set_dma(uint32_t dma_desc_addr_hi, uint32_t dma_desc_addr_lo)
{
	// Skip writing high half if unnecessary.
	if (dma_desc_addr_hi)
		outl(dma_desc_addr_hi, FW_CFG_DMA_ADDR_HIGH);
	outl(dma_desc_addr_lo, FW_CFG_DMA_ADDR_LOW);
}

void mb_fw_cfg_read_data(void *dst, int dstlen)
{
	insb(FW_CFG_PORT_DATA, dst, dstlen);
}

void mb_fw_cfg_write_control(uint16_t entry)
{
	outw(entry, FW_CFG_PORT_CTL);
}
