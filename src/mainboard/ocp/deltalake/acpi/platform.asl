/* SPDX-License-Identifier: GPL-2.0-only */

/* Enable ACPI _SWS methods */
#include <soc/intel/common/acpi/acpi_wake_source.asl>

Name (_S0, Package () // mandatory system state
{
	0x00, 0x00, 0x00, 0x00
})

Name (_S5, Package ()  // mandatory system state
{
	0x07, 0x00, 0x00, 0x00
})

/* The APM port can be used for generating software SMIs */
OperationRegion (APMP, SystemIO, 0xb2, 2)
Field (APMP, ByteAcc, NoLock, Preserve)
{
	APMC, 8,	// APM command
	APMS, 8		// APM status
}

#include <arch/x86/acpi/post.asl>

OperationRegion (PSYS, SystemMemory, 0x6D081000, 0x0400)
Field (PSYS, ByteAcc, NoLock, Preserve)
{
	PLAT,   32,  // Platform ID

	// IOAPIC
	APC0,   1,   // PCH IOAPIC Enable
	AP00,   1,   // PC00 IOAPIC Enable
	AP01,   1,   // PC01 IOAPIC Enable
	AP02,   1,   // PC02 IOAPIC Enable
	AP03,   1,   // PC03 IOAPIC Enable
	AP04,   1,   // PC04 IOAPIC Enable
	AP05,   1,   // PC05 IOAPIC Enable
	AP06,   1,   // PC06 IOAPIC Enable
	AP07,   1,   // PC07 IOAPIC Enable
	AP08,   1,   // PC08 IOAPIC Enable
	AP09,   1,   // PC09 IOAPIC Enable
	AP10,   1,   // PC10 IOAPIC Enable
	AP11,   1,   // PC11 IOAPIC Enable
	AP12,   1,   // PC12 IOAPIC Enable
	AP13,   1,   // PC13 IOAPIC Enable
	AP14,   1,   // PC14 IOAPIC Enable
	AP15,   1,   // PC15 IOAPIC Enable
	AP16,   1,   // PC16 IOAPIC Enable
	AP17,   1,   // PC17 IOAPIC Enable
	AP18,   1,   // PC18 IOAPIC Enable
	AP19,   1,   // PC19 IOAPIC Enable
	AP20,   1,   // PC20 IOAPIC Enable
	AP21,   1,   // PC21 IOAPIC Enable
	AP22,   1,   // PC22 IOAPIC Enable
	AP23,   1,   // PC23 IOAPIC Enable
	RESA,   7,
	SKOV,   1,   // Override Socket APIC Id
	RES0,   7,

	// Power Management
	TPME,   1,
	CSEN,   1,
	C3EN,   1,
	C6EN,   1,
	C7EN,   1,
	MWOS,   1,
	PSEN,   1,
	EMCA,   1,
	HWAL,   2,
	KPRS,   1,
	MPRS,   1,
	TSEN,   1,
	FGTS,   1,
	OSCX,   1,
	RESX,   1,

	// RAS
	CPHP,   8,
	IIOP,   8,
	IIOH,   64,
	PRBM,   32,
	P0ID,   32,
	P1ID,   32,
	P2ID,   32,
	P3ID,   32,
	P4ID,   32,
	P5ID,   32,
	P6ID,   32,
	P7ID,   32,
	P0BM,   64,
	P1BM,   64,
	P2BM,   64,
	P3BM,   64,
	P4BM,   64,
	P5BM,   64,
	P6BM,   64,
	P7BM,   64,
	MEBM,   16,
	MEBC,   16,
	CFMM,   32,
	TSSY,   32,  // TODO: This is TSSZ in system booted from production FW
	M0BS,   64,
	M1BS,   64,
	M2BS,   64,
	M3BS,   64,
	M4BS,   64,
	M5BS,   64,
	M6BS,   64,
	M7BS,   64,
	M0RN,   64,
	M1RN,   64,
	M2RN,   64,
	M3RN,   64,
	M4RN,   64,
	M5RN,   64,
	M6RN,   64,
	M7RN,   64,
	SMI0,   32,
	SMI1,   32,
	SMI2,   32,
	SMI3,   32,
	SCI0,   32,
	SCI1,   32,
	SCI2,   32,
	SCI3,   32,
	MADD,   64,
	CUU0,   128,
	CUU1,   128,
	CUU2,   128,
	CUU3,   128,
	CUU4,   128,
	CUU5,   128,
	CUU6,   128,
	CUU7,   128,
	CPSP,   8,
	ME00,   128,
	ME01,   128,
	ME10,   128,
	ME11,   128,
	ME20,   128,
	ME21,   128,
	ME30,   128,
	ME31,   128,
	ME40,   128,
	ME41,   128,
	ME50,   128,
	ME51,   128,
	ME60,   128,
	ME61,   128,
	ME70,   128,
	ME71,   128,
	MESP,   16,
	LDIR,   64,
	PRID,   32,
	AHPE,   8,

	// VTD
	DHRD,   192,
	ATSR,   192,
	RHSA,   192,

	// SR-IOV
	WSIC,   8,
	WSIS,   16,
	WSIB,   8,
	WSID,   8,
	WSIF,   8,
	WSTS,   8,
	WHEA,   8,

	// BIOS Guard
	BGMA,   64,
	BGMS,   8,
	BGIO,   16,
	BGIL,   8,
	CNBS,   8,

	// USB3
	XHMD,   8,
	SBV1,   8,
	SBV2,   8,

	// HWPM
	HWEN,   2,
	ACEN,   1,
	HWPI,   1,
	RES1,   4,

	// IIO
	BB00,   8,
	BB01,   8,
	BB02,   8,
	BB03,   8,
	BB04,   8,
	BB05,   8,
	BB06,   8,
	BB07,   8,
	BB08,   8,
	BB09,   8,
	BB10,   8,
	BB11,   8,
	BB12,   8,
	BB13,   8,
	BB14,   8,
	BB15,   8,
	BB16,   8,
	BB17,   8,
	BB18,   8,
	BB19,   8,
	BB20,   8,
	BB21,   8,
	BB22,   8,
	BB23,   8,
	BB24,   8,
	BB25,   8,
	BB26,   8,
	BB27,   8,
	BB28,   8,
	BB29,   8,
	BB30,   8,
	BB31,   8,
	BB32,   8,
	BB33,   8,
	BB34,   8,
	BB35,   8,
	BB36,   8,
	BB37,   8,
	BB38,   8,
	BB39,   8,
	BB40,   8,
	BB41,   8,
	BB42,   8,
	BB43,   8,
	BB44,   8,
	BB45,   8,
	BB46,   8,
	BB47,   8,
	SGEN,   8,
	SG00,   8,
	SG01,   8,
	SG02,   8,
	SG03,   8,
	SG04,   8,
	SG05,   8,
	SG06,   8,
	SG07,   8,

	// Performance
	CLOD,   8,

	// XTU
	XTUB,   32,
	XTUS,   32,
	XMBA,   32,
	DDRF,   8,
	RT3S,   8,
	RTP0,   8,
	RTP3,   8,

	// FPGA
	FBB0,   8,
	FBB1,   8,
	FBB2,   8,
	FBB3,   8,
	FBB4,   8,
	FBB5,   8,
	FBB6,   8,
	FBB7,   8,
	FBL0,   8,
	FBL1,   8,
	FBL2,   8,
	FBL3,   8,
	FBL4,   8,
	FBL5,   8,
	FBL6,   8,
	FBL7,   8,
	P0FB,   8,
	P1FB,   8,
	P2FB,   8,
	P3FB,   8,
	P4FB,   8,
	P5FB,   8,
	P6FB,   8,
	P7FB,   8,
	FMB0,   32,
	FMB1,   32,
	FMB2,   32,
	FMB3,   32,
	FMB4,   32,
	FMB5,   32,
	FMB6,   32,
	FMB7,   32,
	FML0,   32,
	FML1,   32,
	FML2,   32,
	FML3,   32,
	FML4,   32,
	FML5,   32,
	FML6,   32,
	FML7,   32,
	FKPB,   32,
	FKB0,   8,
	FKB1,   8,
	FKB2,   8,
	FKB3,   8,
	FKB4,   8,
	FKB5,   8,
	FKB6,   8,
	FKB7,   8
}

/*
 * The _PTS method (Prepare To Sleep) is called before the OS is
 * entering a sleep state. The sleep state number is passed in Arg0
 */

Method (_PTS, 1)
{
}

/* The _WAK method is called on system wakeup */

Method (_WAK, 1)
{
	Return (Package (){ 0, 0 })
}
