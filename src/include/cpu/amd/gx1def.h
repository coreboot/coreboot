/*
    freebios/src/northbridge/nsc/gx1/gx1def.inc

    Copyright (c) 2002 Christer Weinigel <wingel@hack.org>

    Defines for the GX1 processor
*/

#define GX_BASE 0x040000000

/**********************************************************************/
/* Display Controller Registers, offset from GX_BASE */

#define DC_UNLOCK		0x8300
#define     DC_UNLOCK_MAGIC	0x4758

#define DC_GENERAL_CFG		0x8304

/**********************************************************************/
/* Bus Controller Registers, offset from GX_BASE */

#define BC_DRAM_TOP		0x8000

#define BC_XMAP_1		0x8004
#define BC_XMAP_2		0x8008
#define BC_XMAP_3		0x800c

/**********************************************************************/
/* Memory Controller Registers, offset from GX_BASE */

#define MC_MEM_CNTRL1		0x8400
#define     SDCLKSTRT		(1<<17)
#define     RFSHRATE		(0x1ff<<8)
#define     RFSHSTAG		(0x3<<6)
#define	    X2CLKADDR		(1<<5)
#define	    RFSHTST		(1<<4)
#define	    XBUSARB		(1<<3)
#define	    SMM_MAP		(1<<2)
#define     PROGRAM_SDRAM	(1<<0)

#define MC_MEM_CNTRL2		0x8404
#define     SDCLK_MASK		0x000003c0
#define     SDCLKOUT_MASK	0x00000400

#define MC_BANK_CFG		0x8408
#define     DIMM_PG_SZ		0x00000070
#define     DIMM_SZ		0x00000700
#define     DIMM_COMP_BNK	0x00001000
#define     DIMM_MOD_BNK	0x00004000

#define MC_SYNC_TIM1		0x840c

#define MC_GBASE_ADD		0x8414

