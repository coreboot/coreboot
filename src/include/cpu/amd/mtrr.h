#ifndef CPU_AMD_MTRR_H
#define CPU_AMD_MTRR_H

#define IORR_FIRST 0xC0010016
#define IORR_LAST  0xC0010019

#define MTRR_READ_MEM			(1 << 4)
#define MTRR_WRITE_MEM			(1 << 3)

#define SYSCFG_MSR			0xC0010010
#define SYSCFG_MSR_TOM2WB		(1 << 22)
#define SYSCFG_MSR_TOM2En		(1 << 21)
#define SYSCFG_MSR_MtrrVarDramEn	(1 << 20)
#define SYSCFG_MSR_MtrrFixDramModEn	(1 << 19)
#define SYSCFG_MSR_MtrrFixDramEn	(1 << 18)
#define SYSCFG_MSR_UcLockEn		(1 << 17)
#define SYSCFG_MSR_ChxToDirtyDis	(1 << 16)
#define SYSCFG_MSR_ClVicBlkEn		(1 << 11)
#define SYSCFG_MSR_SetDirtyEnO		(1 << 10)
#define SYSCFG_MSR_SetDirtyEnS		(1 <<  9)
#define SYSCFG_MSR_SetDirtyEnE		(1 <<  8)
#define SYSCFG_MSR_SysVicLimitMask	((1 << 8) - (1 << 5))
#define SYSCFG_MSR_SysAckLimitMask	((1 << 5) - (1 << 0))

#define IORRBase_MSR(reg) (0xC0010016 + 2 * (reg))
#define IORRMask_MSR(reg) (0xC0010016 + 2 * (reg) + 1)

#define TOP_MEM_MSR			0xC001001A
#define TOP_MEM2_MSR			0xC001001D
#ifndef TOP_MEM
  #define TOP_MEM			TOP_MEM_MSR
#endif
#ifndef TOP_MEM2
  #define TOP_MEM2			TOP_MEM2_MSR
#endif

#define TOP_MEM_MASK			0x007fffff
#define TOP_MEM_MASK_KB			(TOP_MEM_MASK >> 10)

#if !defined(__PRE_RAM__) && !defined(__ASSEMBLER__)
void amd_setup_mtrrs(void);

/* To distribute topmem MSRs to APs. */
void setup_bsp_ramtop(void);
uint64_t bsp_topmem(void);
uint64_t bsp_topmem2(void);
#endif

#endif /* CPU_AMD_MTRR_H */
