/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2009-2010 iWave Systems
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef RAMINIT_H
#define RAMINIT_H

/**
 * Bit Equates
 **/
#define BIT(x)                        (1<<x)

#define   EBP_TRP_MASK               (BIT(1) | BIT(0))
#define   TRP_LOW                    3h
#define   TRP_HIGH                   5h
#define   EBP_TRP_OFFSET             0           /* Start of TRP field in EBP*/
#define   EBP_TRCD_MASK              (BIT(3) | BIT(2))
#define   TRCD_LOW                   3h
#define   TRCD_HIGH                  5h
#define   EBP_TRCD_OFFSET            2           /* Start of TRCD field in EBP*/
#define   EBP_TCL_MASK               (BIT(5) | BIT(4))
#define   TCL_LOW                    3           /* Minimum supported CL*/
#define   TCL_HIGH                   5           /* Maximum supported CL*/
#define   EBP_TCL_OFFSET             4           /* EBP bit( )for CL mask*/
#define   EBP_DDR2_CL_5_0            BIT(5)        /* CL 5.0 = 10b*/
#define   EBP_DDR2_CL_4_0            BIT(4)        /* CL 4.0 = 01b*/
#define   EBP_DDR2_CL_3_0            00h         /* CL 3.0 = 00b*/
#define   EBP_FREQ_MASK              (BIT(10)| BIT(9))
#define   EBP_FREQ_OFFSET            9           /* EBP bit( )for frequency mask*/
#define   EBP_FREQ_400               0           /* 400MHz EBP[10:9] = 00b*/
#define   EBP_FREQ_533               BIT(9)        /* 533MHz EBP[10:9] = 01b*/
#define   EBP_REFRESH_MASK           (BIT(12)| BIT(11))
#define   EBP_REFRESH_OFFSET         11          /* Bit offset of refresh field*/
#define   EBP_REF_DIS                00h         /* Mask for refresh disabled*/
#define   EBP_REF_128CLK             BIT(11)       /* Mask for 128 clks referesh rate*/
#define   EBP_REF_3_9                BIT(12)       /* Mask for 3.9us refresh rate*/
#define   EBP_REF_7_8                (BIT(12)| BIT(11))/* Mask for 7.8us refresh rate*/
#define   EBP_WIDTH_MASK             BIT(15)
#define   EBP_WIDTH_OFFSET           15          /* Bit offset of EBP width field*/
#define   EBP_SOCKET_X16             BIT(15)       /* Bit mask of x8/x16 bit*/
#define   EBP_DENSITY_MASK           (BIT(17)| BIT(16))
#define   EBP_DENSITY_OFFSET         16
#define   EBP_DENSITY_512            BIT(16)       /* 512Mbit density*/
#define   EBP_DENSITY_1024           BIT(17)       /* 1024Mbit density*/
#define   EBP_DENSITY_2048           (BIT(17)| BIT(16))/* 2048Mbit density*/
#define   EBP_RANKS_MASK             BIT(18)
#define   EBP_RANKS_OFFSET           18
#define   EBP_RANKS                  BIT(18)       /* Bit offset of # of ranks bit*/
#define   EBP_PACKAGE_TYPE             BIT(19)       /* Package type (stacked or not)*/
#define   EBP_2X_MASK                  BIT(20)
#define   EBP_2X_OFFSET              20          /* Bit offset of ebp 2x refresh field*/
#define   EBP_2X_AUTO_REFRESH        BIT(20)       /* Bit mask of 2x refresh field*/
#define   EBP_DRAM_PARM_MASK           BIT(21)
#define   EBP_DRAM_PARM_OFFSET       21
#define   EBP_DRAM_PARM_SPD          0           /* Use SPD to get DRAM parameters*/
#define   EBP_DRAM_PARM_CMC          BIT(21)       /* DRAM parameters in CMC binary*/
#define   EBP_BOOT_PATH                BIT(31)




#define HB_REG_MCR                   0xD0         /* Message Control Register              */
#define HB_REG_MCR_OP_OFFSET         24           /* Offset of the opcode field in MCR     */
#define HB_REG_MCR_PORT_OFFSET       16           /* Offset of the port field in MCR       */
#define HB_REG_MCR_REG_OFFSET        8            /* Offset of the register field in MCR   */
#define HB_REG_MDR                   0xD4         /* Message Data Register                 */

/* SCH Message OpCodes and Attributes*/
#define SCH_OPCODE_WAKEFULLON         0x2           /* SCH message bus "Wake Full On" opcode*/
#define SCH_OPCODE_DRAMINIT           0xA0          /* SCH message bus "DRAM Init" opcode   */
#define SCH_DRAMINIT_CMD_MRS          0x4000           /* MRS command                          */
#define SCH_DRAMINIT_CMD_EMRS1        0x8           /* EMRS 1 command                       */
#define SCH_DRAMINIT_CMD_EMRS2        0x10           /* EMRS 2 command                       */
#define SCH_DRAMINIT_CMD_EMRS3        0x18           /* EMRS 3 command                       */
#define SCH_DRAMINIT_CMD_CBR          0x1           /* CBR command                          */
#define SCH_DRAMINIT_CMD_AREF         0x10001        /* Refresh command, MA10=0->All         */
#define SCH_DRAMINIT_CMD_PALL         0x10002        /* Precharge command, MA10=1->All       */
#define SCH_DRAMINIT_CMD_BACT         0x3           /* Bank activate command                */
#define SCH_DRAMINIT_CMD_NOP          0x7           /* NOP command                          */
#define SCH_DRAMINIT_RANK_OFFSET      21            /* Offset of the rank selection bit     */
#define SCH_DRAMINIT_RANK_MASK        BIT(21)
#define SCH_DRAMINIT_ADDR_OFFSET      6            /* Offset of the address field in MDR    */
#define SCH_DRAMINIT_INTLV            BIT(3)         /* Interleave burst type                 */
#define SCH_DRAMINIT_BL4              2         /* Burst Length = 4                      */
#define SCH_DRAMINIT_CL_OFFSET        4            /* CAS Latency bit offset                */
#define SCH_DRAMINIT_OCD_DEFAULT      0xE000       /* OCD Default command                   */
#define SCH_DRAMINIT_DQS_DIS          BIT(16)        /* DQS Disable command                   */
#define SCH_OPCODE_READ                 0xD0         /* SCH message bus "read" opcode         */
#define SCH_OPCODE_WRITE                0xE0         /* SCH message bus "write" opcode        */

/* SCH Message Ports and Registers*/

#define  SCH_MSG_DUNIT_PORT            0x1          /* DRAM unit port                        */
#define  SCH_MSG_DUNIT_REG_DRP         0x0          /* DRAM Rank Population and Interface    */
#define  DRP_FIELDS                    0xFF         /* Pertinent fields in DRP               */
#define  DRP_RANK0_OFFSET              3            /* Rank 0 enable offset                  */
#define  DRP_RANK1_OFFSET              7            /* Rank 1 enable offset                  */
#define  DRP_DENSITY0_OFFSET           1            /* Density offset - Rank 0               */
#define  DRP_DENSITY1_OFFSET           5            /* Density offset - Rank 1               */
#define  DRP_WIDTH0_OFFSET             0            /* Width offset - Rank 0                 */
#define  DRP_WIDTH1_OFFSET             4            /* Width offset - Rank 1                 */
#define  DRP_CKE_DIS                   (BIT(14)| BIT(13))  /* CKE disable bits for both ranks       */
#define  DRP_CKE_DIS0                  BIT(13)        /* CKE disable bit - Rank 0              */
#define  DRP_CKE_DIS1                  BIT(14)        /* CKE disable bit - Rank 1              */
#define  DRP_SCK_DIS                   (BIT(11)| BIT(10))  /* SCK/SCKB disable bits                 */
#define  DRP_SCK_DIS1                  BIT(11)        /* SCK[1]/SCKB[1] disable                */
#define  DRP_SCK_DIS0                  BIT(10)        /* SCK[0]/SCKB[0] disable                */
#define  SCH_MSG_DUNIT_REG_DTR         0x01          /* DRAM Timing Register                  */
#define  DTR_FIELDS                    0x3F          /* Pertinent fields in DTR               */
#define  DTR_TCL_OFFSET                4            /* CAS latency offset                    */
#define  DTR_TRCD_OFFSET               2            /* RAS CAS Delay Offset                  */
#define  DTR_TRP_OFFSET                0            /* RAS Precharge Delay Offset            */
#define  SCH_MSG_DUNIT_REG_DCO         0x2          /* DRAM Control Register                 */
#define  DCO_FIELDS                    0xF          /* Pertinent fields in DCO               */
#define  DCO_REFRESH_OFFSET            2            /* Refresh Rate Field Offset             */
#define  DCO_FREQ_OFFSET               0            /* DRAM Frequency Field Offset           */
#define  DCO_IC                        BIT(7)         /* Initialization complete bit           */
#define  SCH_MSG_PUNIT_PORT            04h          /* Punit Port                            */
#define  SCH_MSG_PUNIT_REG_PCR         71h          /* Punit Control Register                */
#define  SCH_MSG_TEST_PORT             05h          /* Test port                             */
#define  SCH_MSG_TEST_REG_MSR          03h          /* Mode and Status Register              */


/* Jedec initialization mapping into the MDR address field for DRAM init messages*/


#define SCH_JEDEC_DLLRESET             BIT(8)             /* DLL Reset bit( )                    */
#define SCH_JEDEC_INTLV                BIT(3)             /* Interleave/NOT(Sequential) bit( )   */
#define SCH_JEDEC_CL_OFFSET            4                /* Offset of the CAS latency field   */
#define SCH_JEDEC_OCD_DEFAULT          (BIT(7)| BIT(8)| BIT(9))   /* OCD default value                 */
#define SCH_JEDEC_DQS_DIS              BIT(10)            /* DQS disable bit                   */
#define SCH_JEDEC_BL4                  BIT(1)             /* Burst length 4 value              */
/*static values used during JEDEC iniatialization.  These values are not
dependent on memory or chipset configuration.*/
#define JEDEC_STATIC_PARAM   ((SCH_JEDEC_INTLV << SCH_DRAMINIT_ADDR_OFFSET) + (SCH_JEDEC_BL4 << SCH_DRAMINIT_ADDR_OFFSET))

#define DIMM_SOCKETS 2

#define DIMM_SPD_BASE 0x50
#define DIMM_TCO_BASE 0x30

/* Burst length is always 8 */
#define BURSTLENGTH	 8
#define RAM_PARAM_SOURCE_SOFTSTRAP 1
#define RAM_PARAM_SOURCE_SPD       0
struct sys_info {

	u16 memory_frequency;	  /* 400 or 533*/
	u16 fsb_frequency;	    /* 400 or 533*/

	u8 trp;                 /*3,4,5 DRAM clocks */
	u8 trcd;                /*3,4,5 DRAM clocks */
	u8 cl;                  /*CAS Latency 3,4,5*/

	u8 refresh;             /*Refresh rate disabled,128 DRAM clocks,3.9us,7.8us */

	u8 data_width;          /*x8/x16 data width */
	u8 device_density;      /*SDRAM Device Density 512/1024/2048Mbit */
        u8 ranks;               /*Single/Double */
	u8 ram_param_source;    /*DRAM Parameter Source SPD/SoftStraps(R) Block (down memory) */
	u8 boot_path;

} __attribute__ ((packed));

void sdram_initialize(int boot_mode);

#endif				/* RAMINIT_H */
