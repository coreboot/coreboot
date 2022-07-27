/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef CPU_PPC64_SPR_H
#define CPU_PPC64_SPR_H

#include <arch/byteorder.h>	// PPC_BIT()

#define SPR_TB					0x10C

#define SPR_PVR					0x11F
#define SPR_PVR_REV_MASK			(PPC_BITMASK(52, 55) | PPC_BITMASK(60, 63))
#define SPR_PVR_REV(maj, min)			(PPC_SHIFT((maj), 55) | PPC_SHIFT((min), 63))

#define SPR_HSPRG0				0x130
#define SPR_HSPRG1				0x131

#define SPR_HRMOR				0x139

#define SPR_HMER				0x150
/* Bits in HMER/HMEER */
#define SPR_HMER_MALFUNCTION_ALERT		PPC_BIT(0)
#define SPR_HMER_PROC_RECV_DONE			PPC_BIT(2)
#define SPR_HMER_PROC_RECV_ERROR_MASKED		PPC_BIT(3)
#define SPR_HMER_TFAC_ERROR			PPC_BIT(4)
#define SPR_HMER_TFMR_PARITY_ERROR		PPC_BIT(5)
#define SPR_HMER_XSCOM_FAIL			PPC_BIT(8)
#define SPR_HMER_XSCOM_DONE			PPC_BIT(9)
#define SPR_HMER_PROC_RECV_AGAIN		PPC_BIT(11)
#define SPR_HMER_WARN_RISE			PPC_BIT(14)
#define SPR_HMER_WARN_FALL			PPC_BIT(15)
#define SPR_HMER_SCOM_FIR_HMI			PPC_BIT(16)
#define SPR_HMER_TRIG_FIR_HMI			PPC_BIT(17)
#define SPR_HMER_HYP_RESOURCE_ERR		PPC_BIT(20)
#define SPR_HMER_XSCOM_STATUS			PPC_BITMASK(21, 23)
#define SPR_HMER_XSCOM_OCCUPIED			PPC_BIT(23)

#ifndef __ASSEMBLER__
#include <types.h>

static inline uint64_t read_spr(int spr)
{
	uint64_t val;
	asm volatile("mfspr %0,%1" : "=r"(val) : "i"(spr) : "memory");
	return val;
}

static inline void write_spr(int spr, uint64_t val)
{
	asm volatile("mtspr %0, %1" :: "i"(spr), "r"(val) : "memory");
}

static inline uint64_t read_hmer(void)
{
	return read_spr(SPR_HMER);
}

static inline void clear_hmer(void)
{
	write_spr(SPR_HMER, 0);
}

static inline uint64_t read_msr(void)
{
	uint64_t val;
	asm volatile("mfmsr %0" : "=r"(val) :: "memory");
	return val;
}

static inline uint64_t pvr_revision(void)
{
	return read_spr(SPR_PVR) & SPR_PVR_REV_MASK;
}

#endif /* __ASSEMBLER__ */
#endif /* CPU_PPC64_SPR_H */
