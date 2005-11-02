/************ ppc970fx_board.h ****************/

#ifndef _ppc970_h_
#define _ppc970_h_

/*----------------------------------------------------------------------------+
| Processor Version Register (PVR) values
+----------------------------------------------------------------------------*/
#define PVR_970				0x0039		/* 970   any revision*/
#define PVR_970DD_1_0			0x00391100	/* 970   DD1.0       */
#define PVR_970FX			0x003C          /* 970FX any revision*/
#define PVR_970FX_DD_2_0		0x003C0200      /* 970FX DD2.0       */
#define PVR_970FX_DD_2_1		0x003C0201      /* 970FX DD2.1       */
#define PVR_970FX_DD_3_0		0x003C0300      /* 970FX DD3.0       */
#define PVR_RESERVED			0x000000F0	/* reserved nibble   */

/*----------------------------------------------------------------------------+
| Supported platforms.
+----------------------------------------------------------------------------*/
#define PLATFORM_EVB_LITE		3
#define PLATFORM_EVB_FINAL		4

/*----------------------------------------------------------------------------+
| When timers are running based on CPU speed this is the timer to CPU frequency
| ratio.
+----------------------------------------------------------------------------*/
#define PPC970_TB_RATIO			8

/*----------------------------------------------------------------------------+
| Cache line size.
+----------------------------------------------------------------------------*/
#define CACHE_LINE_SIZE_L1		128
#define CACHE_LINE_SIZE_L2		128

/*----------------------------------------------------------------------------+
| SLB size.
+----------------------------------------------------------------------------*/
#define SLB_SIZE			64

/*----------------------------------------------------------------------------+
| TLB size.
+----------------------------------------------------------------------------*/
#define TLB_SIZE			1024

/*----------------------------------------------------------------------------+
| Partial memory map.
+----------------------------------------------------------------------------*/
#define SDRAM_BASE                      0x0000000000000000UL
#define SDRAM_SIZE			0x0000000080000000UL
#define IO_BASE				0x0000000080000000UL
#define IO_SIZE				0x0000000080000000UL
#define PCI_BUS_MEM_BASE		0x0000000080000000UL
#define PCI_BUS_MEM_SIZE		0x0000000070000000UL
#define PCI0_BASE			0x00000000F0000000UL
#define PCI0_SIZE			0x0000000002000000UL
#define HT1_BASE			0x00000000F2000000UL
#define HT1_SIZE			0x0000000003000000UL
#define PPC925_BASE			0x00000000F8000000UL
#define PPC925_SIZE			0x0000000001000000UL
#define SB_IOAPIC_BASE			0x00000000FEC00000UL
#define BOOT_BASE                       0x00000000FF000000UL
#define BOOT_BASE_AS                    0x00000000FF000000
#define BOOT_END			0x00000000FFFFFFFFUL
#define FLASH_BASE_INTEL		0x00000000FF800000UL
#define FLASH_BASE_INTEL_AS		0x00000000FF800000
#define FLASH_BASE_AMD			0x00000000FFF00000UL
#define FLASH_BASE_AMD_AS		0x00000000FFF00000
#define SDRAM_UPPER_BASE		0x0000000100000000UL
#define SDRAM_UPPER_SIZE		0x0000000F00000000UL

/*----------------------------------------------------------------------------+
| BOOT_STACK_ADDR is data used for stack before SDRAM is available.  This data
| will be written to memory after the SDRAM is initialized.  All values here
| must be less than 32 bits.  Following 13 defines need to be changed when
| changing the location of PIBS in SDRAM (the link file also need to be
| changed in order to fully relocate PIBS.
+----------------------------------------------------------------------------*/
#define PIBS_BASE_ADDR			0x00C00000
#define BOOT_STACK_ADDR         	0x00C50000
#define BOOT_STACK_SIZE         	0x00004000
#define MEM_CHK_START_ADDR      	0x00C40000
#define MEM_CHK_SIZE            	0x00010000

/*----------------------------------------------------------------------------+
| Address of a CPU0, CPU1 shared memory structure.
+----------------------------------------------------------------------------*/
#define CPU1_DATA_STRUCT_ADDR		0x00C00040
#define CPU1_DATA_STRUCT_SRR0_OFF	0x00000000
#define CPU1_DATA_STRUCT_SRR1_OFF	0x00000008
#define CPU1_DATA_STRUCT_R3_OFF		0x00000010
#define CPU1_DATA_STRUCT_VALID_OFF	0x00000018

/*----------------------------------------------------------------------------+
| Address of the memory location used for the test and set instruction
| sequence.
+----------------------------------------------------------------------------*/
#define VM_TEST_AND_SET_ADDR		0x0000000000C000F0UL

/*----------------------------------------------------------------------------+
| Initial page table address.
+----------------------------------------------------------------------------*/
#define INITIAL_PAGE_TABLE_ADDR_CPU0	0x0000000000D00000
#define INITIAL_PAGE_TABLE_ADDR_CPU1	0x0000000000D40000
#define INITIAL_PAGE_TABLE_SIZE		0x0000000000040000

/*----------------------------------------------------------------------------+
| Initial stack size.  Must be less than 32 bits in length.
+----------------------------------------------------------------------------*/
#define MY_MAIN_STACK_SIZE              (8* 1024)

/*----------------------------------------------------------------------------+
| Serial port address.  The base address must be programmed into super I/O. The
| external time base is available only on JS20.
+----------------------------------------------------------------------------*/
#define UART1_MMIO_BASE         	0xF40002F8UL
#define UART0_MMIO_BASE         	0xF40003F8UL
#define UART1_MMIO_OFFSET		0x2F8;
#define UART0_MMIO_OFFSET		0x3F8;
#define UART_INPUT_CLOCK        	1843200
#define EXT_TIME_BASE_FREQ		0
#define DIV_HIGH_9600   		0x00
#define DIV_LOW_9600    		0x0C

#define EXT_IRQ_COM1			EXT_SB_HT4
#define EXT_IRQ_COM2			EXT_SB_HT3

/*----------------------------------------------------------------------------+
| Locations in Super I/O NVRAM where service processor stores information for
| the PPC970FX CPU.
+----------------------------------------------------------------------------*/
#define SUPER_IO_NVRAM_DATA_VALID       64
#define SUPER_IO_NVRAM_SYS_CLK		(SUPER_IO_NVRAM_DATA_VALID+ 0x04)
#define SUPER_IO_NVRAM_CLK_MULT		(SUPER_IO_NVRAM_SYS_CLK+ 0x04)
#define SUPER_IO_NVRAM_EI_RATIO		(SUPER_IO_NVRAM_CLK_MULT+ 0x01)

#define SUPER_IO_VALID_VALUE		0x426F4F6D

#define PPC970_EI_RATIO_000		2
#define PPC970_EI_RATIO_001		3
#define PPC970_EI_RATIO_010		4
#define PPC970_EI_RATIO_011		6
#define PPC970_EI_RATIO_100		8
#define PPC970_EI_RATIO_101		12
#define PPC970_EI_RATIO_110		16

/*----------------------------------------------------------------------------+
| Locations in Super I/O NVRAM where PPC970 store commands for service
| processor.  0x01 is written by PPC970 to initiate action by the service
| processor.  This value is cleared by the service processor upon receiving
| the command.
+----------------------------------------------------------------------------*/
#define SUPER_IO_NVRAM_POWER_OFF	96
#define SUPER_IO_NVRAM_RESTART		(SUPER_IO_NVRAM_POWER_OFF+ 0x2)

/*----------------------------------------------------------------------------+
| Default HID register settings.
+----------------------------------------------------------------------------*/
#define HID0_PREFEAR    		0x0011008180000000
#define HID1_PREFEAR    		0xFD3C200000000000
#define HID4_PREFEAR    		0x0000001000000000
#define HID5_PREFEAR    		0x0000000000000080

/*----------------------------------------------------------------------------+
| Power control SCOM register definitions.
+----------------------------------------------------------------------------*/
#define SCOM_ADDR_PCR_WRITE		0x000000000AA00000UL
#define SCOM_ADDR_PCR_WRITE_ASM		0x000000000AA00000
#define SCOM_ADDR_PSR_READ		0x0000000040808000UL
#define SCOM_ADDR_PSR_READ_ASM		0x0000000040808000

#define SCOM_ADDR_PCR_DATA_MASK		0x0000000080000000UL
#define SCOM_ADDR_PCR_DATA_MASK_ASM	0x0000000080000000

#define SCOM_ADDR_PCR_FREQ_VALID	0x0000000000010000UL
#define SCOM_ADDR_PCR_FREQ_FULL		0x0000000000000000UL
#define SCOM_ADDR_PCR_FREQ_HALF		0x0000000000020000UL
#define SCOM_ADDR_PCR_FREQ_QUARTER	0x0000000000040000UL

#define SCOM_PSR_FREQ_MASK		0x0300000000000000UL
#define SCOM_PSR_FREQ_FULL		0x0000000000000000UL
#define SCOM_PSR_FREQ_HALF		0x0100000000000000UL
#define SCOM_PSR_FREQ_QUARTER		0x0200000000000000UL
#define SCOM_PSR_COMM_COMPLETED    	0x1000000000000000UL
#define SCOM_PSR_COMM_COMPLETED_ASM	0x1000000000000000

/*----------------------------------------------------------------------------+
| Serial port for CPU2
+----------------------------------------------------------------------------*/
#define CPU2_SERIAL_PORT		2
#define CPU2_BAUD_RATE			115200

/*----------------------------------------------------------------------------+
| External interrupt assignments.
+----------------------------------------------------------------------------*/
#define EXT_I2C_MASTER			0
#define EXT_VSP   			1
#define EXT_HT1_BRIDGE			2
#define EXT_PCI0_AGP_BRIDGE		3
#define EXT_SLEEP0			4
#define EXT_SLEEP1			5
#define EXT_SB_HT0			6
#define EXT_SB_HT1			7
#define EXT_SB_HT2	 		8
#define EXT_SB_HT3			9
#define EXT_SB_HT4    			10
#define EXT_SB_HT5    			11
#define EXT_SB_HT6    			12
#define EXT_SB_HT7    			13
#define EXT_SB_HT8    			14
#define EXT_SB_HT9    			15
#define EXT_SB_HT10    			16
#define EXT_SB_HT11    			17
#define EXT_SB_HT12    			18
#define EXT_SB_HT13			19
#define EXT_SB_HT14			20
#define EXT_SB_HT15			21
#define EXT_SB_HT16			22
#define EXT_SB_HT17			23
#define EXT_SB_HT18			24
#define EXT_SB_HT19			25
#define EXT_SB_HT20			26
#define EXT_SB_HT21			27
#define EXT_SB_HT22			28
#define EXT_SB_HT23			29
#define EXT_SB_HT24			30
#define EXT_SB_HT25			31
#define EXT_SB_HT26			32
#define EXT_SB_HT27			33
#define EXT_SB_HT28			34
#define EXT_SB_HT29			35
#define EXT_SB_HT30			36
#define EXT_SB_HT31			37
#define EXT_SB_HT32			38
#define EXT_SB_HT33			39
#define EXT_SB_HT34			40
#define EXT_SB_HT35			41
#define EXT_SB_HT36   			42
#define EXT_SB_HT37    			43
#define EXT_SB_HT38    			44
#define EXT_SB_HT39         		45
#define EXT_SB_HT40         		46
#define EXT_SB_HT41         		47
#define EXT_SB_HT42         		48
#define EXT_SB_HT43         		49
#define EXT_SB_HT44         		50
#define EXT_SB_HT45         		51
#define EXT_SB_HT46         		52
#define EXT_SB_HT47         		53
#define EXT_SB_HT48         		54
#define EXT_SB_HT49         		55
#define EXT_SB_HT50         		56
#define EXT_SB_HT51         		57
#define EXT_SB_HT52         		58
#define EXT_SB_HT53         		59
#define EXT_SB_HT54         		60
#define EXT_SB_HT55         		61
#define EXT_SB_HT56         		62
#define EXT_SB_HT57         		63
#define EXT_SB_HT58         		64
#define EXT_SB_HT59         		65
#define EXT_SB_HT60         		66
#define EXT_SB_HT61         		67
#define EXT_SB_HT62         		68
#define EXT_SB_HT63         		69
#define EXT_SB_HT64         		70
#define EXT_SB_HT65         		71
#define EXT_SB_HT66         		72
#define EXT_SB_HT67         		73
#define EXT_SB_HT68         		74
#define EXT_SB_HT69         		75
#define EXT_SB_HT70         		76
#define EXT_SB_HT71         		77
#define EXT_SB_HT72         		78
#define EXT_SB_HT73         		79
#define EXT_SB_HT74         		80
#define EXT_SB_HT75         		81
#define EXT_SB_HT76         		82
#define EXT_SB_HT77         		83
#define EXT_SB_HT78         		84
#define EXT_SB_HT79         		85
#define EXT_SB_HT80         		86
#define EXT_SB_HT81         		87
#define EXT_SB_HT82         		88
#define EXT_SB_HT83         		89
#define EXT_SB_HT84         		90
#define EXT_SB_HT85         		91
#define EXT_SB_HT86         		92
#define EXT_SB_HT87         		93
#define EXT_SB_HT88         		94
#define EXT_SB_HT90         		95
#define EXT_SB_HT91         		96
#define EXT_SB_HT92         		97
#define EXT_SB_HT93         		98
#define EXT_SB_HT94         		99
#define EXT_SB_HT95          		100
#define EXT_SB_HT96           		101
#define EXT_SB_HT97           		102
#define EXT_SB_HT98       		103
#define EXT_SB_HT99        		104
#define EXT_SB_HT100 			105
#define EXT_SB_HT101  			106
#define EXT_SB_HT102  			107
#define EXT_SB_HT103  			108
#define EXT_SB_HT104  			109
#define EXT_SB_HT105  			110
#define EXT_SB_HT106  			111
#define EXT_SB_HT107 			112
#define EXT_SB_HT108 			113
#define EXT_SB_HT109 			114
#define EXT_SB_HT110 			115
#define EXT_SB_HT111 			116
#define EXT_SB_HT112   			117
#define EXT_SB_HT113   			118
#define EXT_SB_HT114   			119
#define EXT_SB_HT115   			120
#define EXT_SB_HT116   			121
#define EXT_SB_HT117   			122
#define EXT_SB_HT118   			123
#define EXT_IPI_0      			124
#define EXT_IPI_1      			125
#define EXT_MAX_IRQ_NUM			125

/*----------------------------------------------------------------------------+
| #     #    #    ######  #######
| #     #   # #   #     #    #
| #     #  #   #  #     #    #
| #     # #     # ######     #
| #     # ####### #   #      #
| #     # #     # #    #     #
|  #####  #     # #     #    #
+----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------+
| Interrupt Enable Register.  DLAB must be set to 0 access this register.
+----------------------------------------------------------------------------*/
#define asyncIER                    	1
#define asyncIERModem               	0x08
#define asyncIERLine                	0x04
#define asyncIERTransmit            	0x02
#define asyncIERReceive             	0x01
#define asyncIERdisableAll          	0x00

/*----------------------------------------------------------------------------+
| Interrupt Identification Register.  Read only register.
+----------------------------------------------------------------------------*/
#define asyncIIR                    	2
#define asyncIIRMask                	0x0F
#define asyncIIRFifoTimeout         	0x0C
#define asyncIIRLine                	0x06
#define asyncIIRReceive             	0x04
#define asyncIIRTransmit            	0x02
#define asyncIIRNoInterrupt         	0x01
#define asyncIIRModem               	0x00

/*----------------------------------------------------------------------------+
| FIFO Control Register.  Write only register.
+----------------------------------------------------------------------------*/
#define asyncFCR                    	2
#define asyncFCRFifoTrigger14       	0xC0
#define asyncFCRFifoTrigger8        	0x80
#define asyncFCRFifoTrigger4        	0x40
#define asyncFCRFifoTrigger1        	0x00
#define asyncFCRDmaSet              	0x08
#define asyncFCRClearXmitFifo       	0x04
#define asyncFCRClearRcvFifo        	0x02
#define asyncFCRFifoEnable          	0x01

/*----------------------------------------------------------------------------+
| Line Control Register.
+----------------------------------------------------------------------------*/
#define asyncLCR                    	3
#define asyncLCRDLAB                	0x80
#define asyncLCRSetBreak            	0x40
#define asyncLCRStickParity         	0x20
#define asyncLCREvenParity          	0x10
#define asyncLCROddParity           	0x00
#define asyncLCRParityEnable        	0x08
#define asyncLCRParityDisable       	0x00
#define asyncLCRStopBitsTwo         	0x04
#define asyncLCRStopBitsOne         	0x00
#define asyncLCRWordLengthSel       	0x03
#define asyncLCRWordLength5         	0x00
#define asyncLCRWordLength6         	0x01
#define asyncLCRWordLength7         	0x02
#define asyncLCRWordLength8         	0x03

/*----------------------------------------------------------------------------+
| Modem Control Register.
+----------------------------------------------------------------------------*/
#define asyncMCR                    	4
#define asyncMCRLoop                	0x10
#define asyncMCROut2                	0x08
#define asyncMCROut1                	0x04
#define asyncMCRRTS                 	0x02
#define asyncMCRDTR                 	0x01
#define asyncMCRdisableAll          	0x00

/*----------------------------------------------------------------------------+
| Line Status Register.
+----------------------------------------------------------------------------*/
#define asyncLSR                    	5
#define asyncLSRRxFifoError         	0x80
#define asyncLSRTxEmpty             	0x60
#define asyncLSRTxShiftEmpty        	0x40
#define asyncLSRTxHoldEmpty         	0x20
#define asyncLSRBreakInterrupt      	0x10
#define asyncLSRFramingError        	0x08
#define asyncLSRParityError         	0x04
#define asyncLSROverrunError        	0x02
#define asyncLSRDataReady           	0x01

/*----------------------------------------------------------------------------+
| Modem Status Register.  Read only register.
+----------------------------------------------------------------------------*/
#define asyncMSR                    	6
#define asyncMSRCD                  	0x80
#define asyncMSRRI                  	0x40
#define asyncMSRDSR                 	0x20
#define asyncMSRCTS                 	0x10
#define asyncMSRDeltaDCD            	0x08
#define asyncMSRDeltaRI             	0x04
#define asyncMSRDeltaDSR            	0x02
#define asyncMSRDeltaCTS            	0x01

/*----------------------------------------------------------------------------+
| Miscellanies defines.
+----------------------------------------------------------------------------*/
#define asyncScratchReg             	7
#define asyncTxBuffer               	0
#define asyncRxBuffer               	0
#define asyncDLABLsb                	0
#define asyncDLABMsb                	1

/*----------------------------------------------------------------------------+
|  #####  ######   #####   #####   #####  #######
| #     # #     # #     # #     # #     # #
| #       #     # #       #     #       # #
| #       ######  #        ######  #####  ######
| #       #       #             # #             #
| #     # #       #     # #     # #       #     #
|  #####  #        #####   #####  #######  #####
+----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------+
| When performing PCI configuration read/write the configuration address
| register must be written and then read before configuration data register is
| accessed.
| PCI type 0 Configuration address format is:
| 0-20 id. sel., 21-23 function number, 24-29 register number|00
+----------------------------------------------------------------------------*/
#define NB_PCI_CONFIGURATION_ADDR	0x0F0800000UL
#define NB_PCI_CONFIGURATION_DATA	0x0F0C00000UL

/*----------------------------------------------------------------------------+
| When performing HT configuration read/write the configuration address
| register must be written and then read before configuration data register is
| accessed.
| HT type 0 Configuration address format is:
| 0-15 == 0x0000, 16-20 device number, 21-23 function number, 24-29 reg.num|00
| HT type 1 configuration address format is
| 0-15 == 0x0000, 16-20 device number, 21-23 function number, 24-29 reg.num|01
+----------------------------------------------------------------------------*/
#define NB_HT_CONFIGURATION_ADDR	0x0F2800000UL
#define NB_HT_CONFIGURATION_DATA	0x0F2C00000UL

/*----------------------------------------------------------------------------+
| HT Configuration Address Spaces.
+----------------------------------------------------------------------------*/
#define NB_HT_CONFIG_TYPE_0_BASE	0x0F2000000UL
#define NB_HT_CONFIG_TYPE_1_BASE	0x0F3000000UL

/*----------------------------------------------------------------------------+
| HT I/O Space.  NB_HT_IO_RESERVED is reserved for Super I/O peripherals.  The
| SuperI/O utilizes subtractive decode. All PCI I/0 addresses are translated
| from 0xF4xxxxxx (CPU) to 0x00xxxxxx (PCI).
+----------------------------------------------------------------------------*/
#define NB_HT_IO_BASE_CPU		0x0F4000000UL
#define NB_HT_IO_BASE_BYTE		0xF4
#define NB_HT_IO_BASE_BYTE_SH		24
#define NB_HT_IO_BASE_PCI		0x000000000UL
#define NB_HT_IO_BASE_ASM		0xF4000000
#define NB_HT_IO_SIZE			0x000400000UL
#define NB_HT_IO_RESERVED		0x000010000UL

/*----------------------------------------------------------------------------+
| HT EOI Space.
+----------------------------------------------------------------------------*/
#define NB_HT_EOI_BASE			0x0F4400000UL
#define NB_HT_EOI_SIZE			0x000400000UL

/*----------------------------------------------------------------------------+
| HT Device Header Regs.  Big Endian.
+----------------------------------------------------------------------------*/
#define NB_HT_REG_BASE			0x0F8070000UL
#define NB_HT_DID_VID			0x0F8070000UL
#define NB_HT_STAT_CMD			0x0F8070010UL
#define NB_HT_CLASS_REV			0x0F8070020UL
#define NB_HT_BIST_HT			0x0F8070030UL
#define NB_HT_CAP_PTR			0x0F80700D0UL
#define NB_HT_INT_LINE			0x0F80700F0UL

/*----------------------------------------------------------------------------+
| HT Capabilities Block.  Big Endian.
+----------------------------------------------------------------------------*/
#define NB_HT_CMD_PTR_ID		0x0F8070100UL
#define HT_WARM_RESET			0x00010000
#define NB_HT_LINK_CFG_CONTROL		0x0F8070110UL
#define HT_CRC_ERR			0x00000F00
#define HT_END_OF_CHAIN			0x00000040
#define HT_INIT				0x00000020
#define HT_LINK_FAIL			0x00000010
#define HT_LINK_OUT_MASK		0x70000000
#define HT_LINK_IN_MASK			0x07000000
#define HT_LINK_MAX_OUT_MASK		0x00700000
#define HT_LINK_MAX_IN_MASK		0x00070000
#define HT_LINK_WIDTH_8_BIT		0x0
#define HT_LINK_WIDTH_16_BIT		0x1
#define HT_LINK_WIDTH_32_BIT		0x3
#define HT_LINK_WIDTH_2_BIT		0x4
#define HT_LINK_WIDTH_4_BIT		0x5
#define NB_HT_LINK_FREQ_ERROR		0x0F8070120UL
#define HT_LINK_FREQ_CAP_MASK		0xFFFF0000
#define HT_LINK_FREQ_MASK		0x00000F00
#define HT_LINK_FREQ_200		0x0
#define HT_LINK_FREQ_300		0x1
#define HT_LINK_FREQ_400		0x2
#define HT_LINK_FREQ_500		0x3
#define HT_LINK_FREQ_600		0x4
#define HT_LINK_FREQ_800		0x5
#define HT_LINK_FREQ_1000		0x6

/*----------------------------------------------------------------------------+
| HT Other registers.  Big Endian.
+----------------------------------------------------------------------------*/
#define NB_HT_ADDRESS_MASK		0x0F8070200UL
#define NB_HT_PROCESSOR_INT_CONTROL	0x0F8070210UL
#define NB_HT_BRIDGE_CONTROL		0x0F8070300UL
#define HT_SECBUSRESET			0x00400000
#define NB_HT_TXCTL_DATABUFALLOC	0x0F8070310UL
#define NB_HT_TXBUFCOUNTMAX		0x0F8070340UL

/*----------------------------------------------------------------------------+
| Accessed through AGP/PCI configuration space on PCI0 bus.
+----------------------------------------------------------------------------*/
#define NB_PCI_ADDRESS_MASK		0x48
#define NB_PCI_ADDRESS_MASK_RVALUE	0x00000003

/*----------------------------------------------------------------------------+
| MPIC
+----------------------------------------------------------------------------*/
#define NB_MPIC_TOGGLE			0x0F80000E0UL
#define NB_MPIC_ENABLE_OUT		0x00000004
#define NB_MPIC_RESET			0x00000002

#define NB_MPIC_BASE                    0x0F8040000UL
#define NB_MPIC_SIZE                    0x000040000UL

#define NB_MPIC_FEATURE			0x0F8041000UL
#define NB_MPIC_GLOBAL0			0x0F8041020UL
#define NB_MPIC_GLOBAL0_MPIC_RESET	0x80000000U
#define NB_MPIC_IPI0_VECT_PRIO		0x0F80410A0UL
#define NB_MPIC_IPI1_VECT_PRIO		0x0F80410B0UL
#define NB_MPIC_SPURIOUS_VECTOR 	0x0F80410E0UL

#define NB_MPIC_S0_VECT_PRIO 		0x0F8050000UL
#define NB_MPIC_VECT_PRIO_ADDER		0x00000020
#define NB_MPIC_S0_DESINATION 		0x0F8050010UL
#define NB_MPIC_DESINATION_ADDER	0x00000020

#define NB_MPIC_P0_IPI0_DISPATCH 	0x0F8060040UL
#define NB_MPIC_P0_IPI1_DISPATCH 	0x0F8060050UL
#define NB_MPIC_P0_TASK_PRIO	 	0x0F8060080UL
#define NB_MPIC_P0_INT_ACK	 	0x0F80600A0UL
#define NB_MPIC_P0_INT_ACK_AS		0x0F80600A0
#define NB_MPIC_P0_EIO    	 	0x0F80600B0UL
#define NB_MPIC_P0_EIO_AS    	 	0x0F80600B0
#define NB_MPIC_P1_IPI0_DISPATCH 	0x0F8061040UL
#define NB_MPIC_P1_IPI1_DISPATCH 	0x0F8061050UL
#define NB_MPIC_P1_TASK_PRIO	 	0x0F8061080UL
#define NB_MPIC_P1_INT_ACK	 	0x0F80610A0UL
#define NB_MPIC_P1_INT_ACK_AS	 	0x0F80610A0
#define NB_MPIC_P1_EIO    	 	0x0F80610B0UL
#define NB_MPIC_P1_EIO_AS    	 	0x0F80610B0

#define NB_MPIC_IPI_PRIO_MASK		0x000F0000
#define NB_MPIC_IPI_PRIO_SH		16
#define NB_MPIC_IPI_VECTOR_MASK		0x000000FF
#define NB_MPIC_IPI_MASK       		0x80000000U
#define NB_MPIC_IPI_ACTIVE     		0x40000000

#define NB_MPIC_EXT_PRIO_MASK		0x000F0000
#define NB_MPIC_EXT_PRIO_SH		16
#define NB_MPIC_EXT_VECTOR_MASK		0x000000FF
#define NB_MPIC_EXT_MASK       		0x80000000U
#define NB_MPIC_EXT_ACTIVE     		0x40000000
#define NB_MPIC_EXT_SENSE     		0x00400000

#define NB_MPIC_DEST_CPU0     		0x00000001
#define NB_MPIC_DEST_CPU1     		0x00000002

#define NB_MPIC_IPI_CPU0     		0x00000001
#define NB_MPIC_IPI_CPU1     		0x00000002

#define NB_MPIC_TASK_PRIO_MASK		0x0000000F

#define NB_MPIC_C0_CASCADE		0x20000000

/*----------------------------------------------------------------------------+
| I2C
+----------------------------------------------------------------------------*/
#define NB_IIC_MMIO_BASE                0xF8001000UL
#define NB_IIC_MMIO_BASE_BYTE4          0xF8
#define NB_IIC_MMIO_BASE_BYTE5          0x00
#define NB_IIC_MMIO_BASE_BYTE6          0x10
#define NB_IIC_MMIO_BASE_BYTE7          0x00
#define NB_IIC_MMIO_BASE_MASK           0xFFFFFFFF
#define NB_IIC_MMIO_SIZE                0x00001000UL
#define NB_IIC_MODE                     0x00
#define NB_IIC_CNTRL                    0x10
#define NB_IIC_STATUS                   0x20
#define NB_IIC_ISR                      0x30
#define NB_IIC_IER                      0x40
#define NB_IIC_ADDR                     0x50
#define NB_IIC_SUBADDR                  0x60
#define NB_IIC_DATA                     0x70
#define NB_IIC_REV                      0x80
#define NB_IIC_RISETTIMECNT             0x90
#define NB_IIC_BITTIMECNT               0xA0

#define IIC_MODE_PORTSEL0               0x00000000
#define IIC_MODE_PORTSEL1               0x00000010
#define IIC_MODE_APMODE_MANUAL          0x00000000
#define IIC_MODE_APMODE_STANDARD        0x00000004
#define IIC_MODE_APMODE_SUBADDR         0x00000008
#define IIC_MODE_APMODE_COMBINED        0x0000000C
#define IIC_MODE_SPEED_25               0x00000002
#define IIC_MODE_SPEED_50               0x00000001
#define IIC_MODE_SPEED_100              0x00000000

#define IIC_CNTRL_STOP                  0x00000004
#define IIC_CNTRL_XADDR                 0x00000002
#define IIC_CNTRL_AAK                   0x00000001

#define IIC_STATUS_LASTAAK              0x00000002

#define IIC_ISR_ISTOP                   0x00000004
#define IIC_ISR_IADDR                   0x00000002
#define IIC_ISR_IDATA                   0x00000001

/*----------------------------------------------------------------------------+
| DDR_SDRAM Controller
+----------------------------------------------------------------------------*/
#define NB_SDRAM_BASE             	0xF8002000UL
#define NB_SDRAM_BASE_BYTE4             0xF8
#define NB_SDRAM_BASE_BYTE5             0x00
#define NB_SDRAM_BASE_BYTE6             0x20
#define NB_SDRAM_BASE_BYTE7             0x00
#define NB_SDRAM_BASE_MASK              0xFFFFFFFF
#define NB_SDRAM_SIZE                   0x00001000UL
#define NB_SDRAM_MEMTIMINGPARAM  	0x050
#define NB_SDRAM_MEMPROGCNTL     	0x0E0
#define NB_SDRAM_MRS             	0x0F0
#define NB_SDRAM_MRSREGCNTL            	0x0F0
#define NB_SDRAM_EMRS            	0x100
#define NB_SDRAM_EMRSREGCNTL           	0x100
#define NB_SDRAM_MEMBUSCFG       	0x190
#define NB_SDRAM_MEMMODE0        	0x1C0
#define NB_SDRAM_MEMBOUNDAD0     	0x1D0
#define NB_SDRAM_MEMMODE1        	0x1E0
#define NB_SDRAM_MEMBOUNDAD1     	0x1F0
#define NB_SDRAM_MEMMODE2        	0x200
#define NB_SDRAM_MEMBOUNDAD2     	0x210
#define NB_SDRAM_MEMMODE3        	0x220
#define NB_SDRAM_MEMBOUNDAD3     	0x230
#define NB_SDRAM_MEMMODE4        	0x240
#define NB_SDRAM_MEMBOUNDAD4     	0x250
#define NB_SDRAM_MEMMODE5        	0x260
#define NB_SDRAM_MEMBOUNDAD5     	0x270
#define NB_SDRAM_MEMMODE6        	0x280
#define NB_SDRAM_MEMBOUNDAD6     	0x290
#define NB_SDRAM_MEMMODE7        	0x2A0
#define NB_SDRAM_MEMBOUNDAD7     	0x2B0
#define NB_SDRAM_MSCR                   0x400
#define NB_SDRAM_MSRSR                  0x410
#define NB_SDRAM_MSRER                  0x420
#define NB_SDRAM_MSPR                   0x430
#define NB_SDRAM_MCCR                   0x440
#define NB_SDRAM_MEMMODECNTL     	0x500
#define NB_SDRAM_DELMEASSTATE    	0x510
#define NB_SDRAM_CKDELADJ        	0x520
#define NB_SDRAM_IOMODECNTL      	0x530
#define NB_SDRAM_DQSDELADJ0      	0x600
#define NB_SDRAM_DQSDATADELADJ0         0x610

#define SDRAM_MEMORY_MODE_256M_16Mx16   0x0A000000
#define SDRAM_MEMORY_MODE_256M_32Mx8    0x0C000000
#define SDRAM_MEMORY_MODE_512M_64Mx8    0x0E000000
#define SDRAM_MEMORY_MODE_1G_64Mx16     0x10000000
#define SDRAM_MEMORY_MODE_1G_128Mx8     0x12000000

#define SDRAM_MEMMODE_BANKEN            0x40000000
#define SDRAM_MEMMODE_BASEBANKADDR      0x01000000
#define SDRAM_MEMMODE_LSSIDE            0x00800000
#define SDRAM_MEMMODE_HSSIDE            0x00400000

#define SDRAM_MEMBOUNDAD_BASEBANKADDR   0xFF000000

#define SDRAM_MEMPROGCNTL_SL      	0x80000000
#define SDRAM_MEMPROGCNTL_WDR     	0x40000000

#define SDRAM_MTP_RCD_MASK              0xE0000000
#define SDRAM_MTP_RP_MASK               0x1C000000
#define SDRAM_MTP_RAS_MASK              0x03800000
#define SDRAM_MTP_WRT                   0x00400000
#define SDRAM_MTP_RFC_MASK              0x003C0000
#define SDRAM_MTP_WRCD                  0x00020000
#define SDRAM_MTP_CAS_RR_MASK           0x0001C000
#define SDRAM_MTP_CAS_RW_MASK           0x00003800
#define SDRAM_MTP_TRFCX2                0x00000400

#define SDRAM_MTP_RCD_2                 0x20000000
#define SDRAM_MTP_RCD_3                 0x40000000
#define SDRAM_MTP_RCD_4                 0x60000000
#define SDRAM_MTP_RCD_5                 0x80000000
#define SDRAM_MTP_RCD_6                 0xA0000000
#define SDRAM_MTP_RP_2                  0x04000000
#define SDRAM_MTP_RP_3                  0x08000000
#define SDRAM_MTP_RP_4                  0x0C000000
#define SDRAM_MTP_RP_5                  0x10000000
#define SDRAM_MTP_RP_6                  0x14000000
#define SDRAM_MTP_RAS_4                 0x00000000
#define SDRAM_MTP_RAS_5                 0x00800000
#define SDRAM_MTP_RAS_6                 0x01000000
#define SDRAM_MTP_RAS_7                 0x01800000
#define SDRAM_MTP_RAS_8                 0x02000000
#define SDRAM_MTP_CAS_RR_2              0x00008000
#define SDRAM_MTP_CAS_RR_3              0x0000C000
#define SDRAM_MTP_CAS_RR_4              0x00010000
#define SDRAM_MTP_CAS_RR_5              0x00014000
#define SDRAM_MTP_CAS_RR_25             0x00018000
#define SDRAM_MTP_CAS_RW_2              0x00001000
#define SDRAM_MTP_CAS_RW_3              0x00001800
#define SDRAM_MTP_CAS_RW_4              0x00002000
#define SDRAM_MTP_CAS_RW_5              0x00002800
#define SDRAM_MTP_CAS_RW_25             0x00003000

#define SDRAM_MRS_LTMODE_MASK           0x00000070
#define SDRAM_MRS_LTMODE_20             0x00000020
#define SDRAM_MRS_LTMODE_25             0x00000060
#define SDRAM_MRS_BT                    0x00000008
#define SDRAM_MRS_BL4                   0x00000002

#define SDRAM_MMCR_REGISTERED_MASK      0x14400000

#define SDRAM_MSCR_SCRUBMODOFF          0x00000000
#define SDRAM_MSCR_SCRUBMODBACKG        0x40000000
#define SDRAM_MSCR_SCRUBMODIMMED        0x80000000
#define SDRAM_MSCR_SCRUBMODIMMEDFILL    0xC0000000
#define SDRAM_MSCR_SI_MASK              0x00FF0000

#define SDRAM_MCCR_ECC_EN               0x80000000
#define SDRAM_MCCR_ECC_APP_DIS          0x40000000
#define SDRAM_MCCR_EI_EN_H              0x20000000
#define SDRAM_MCCR_EI_EN_L              0x10000000
#define SDRAM_MCCR_ECC_UE_MASK_H        0x08000000
#define SDRAM_MCCR_ECC_CE_MASK_H        0x04000000
#define SDRAM_MCCR_ECC_UE_MASK_L        0x02000000
#define SDRAM_MCCR_ECC_CE_MASK_L        0x01000000
#define SDRAM_MCCR_EI_PAT_H             0x0000FF00
#define SDRAM_MCCR_EI_PAT_L             0x000000FF

/*----------------------------------------------------------------------------+
| Power Management
+----------------------------------------------------------------------------*/
#define NB_CLOCK_CTL			0xF8000F00UL
#define HT_LOGIC_STOP_EN		0x00000010
#define HT_CLK_EN			0x00000008
#define NB_PLL2                   	0xF8000F60UL
#define NB_PLL2_BYTE4             	0xF8
#define NB_PLL2_BYTE5             	0x00
#define NB_PLL2_BYTE6             	0x0F
#define NB_PLL2_BYTE7             	0x60
#define NB_PLL2_MASK            	0xFFFFFFFF
#define PLL2_FORCEPLLLOAD               0x40000000
#define PLL2_VALUES_MASK                0x0F01F3FF
#define PLL2_266                        0x021082B8
#define PLL2_300                        0x021092B8
#define PLL2_333                        0x0210A2B8
#define PLL2_FEEDBACK_MASK              0x0001F000
#define PLL2_FEEDBACK_SPEED_266         0x00008000
#define PLL2_FEEDBACK_SPEED_300         0x00009000
#define PLL2_FEEDBACK_SPEED_333         0x0000A000
#define PLL2_FEEDBACK_SPEED_366         0x0000B000
#define PLL2_FEEDBACK_SPEED_400         0x0000C000
#define PLL2_FEEDBACK_SPEED_433         0x0000D000
#define PLL2_FEEDBACK_SPEED_466         0x0000E000
#define PLL2_FEEDBACK_SPEED_500         0x0000F000
#define NB_PLL4				0xF8000F80UL
#define PLL4_FORCEPLLLOAD               0x40000000

/*----------------------------------------------------------------------------+
| CPC925 Control
+----------------------------------------------------------------------------*/
#define NB_REVISION			0xF8000000UL
#define CPC925_DD1_1			0x00000035
#define NB_WHOAMI			0xF8000050UL
#define NB_SEMAPHORE			0xF8000060UL
#define NB_HW_INIT_STATE		0xF8000070UL
#define NB_HW_INIT_STATE_ASM		0xF8000070

/*----------------------------------------------------------------------------+
| DART
+----------------------------------------------------------------------------*/
#define NB_DART_BASE             	0xF8033000UL
#define NB_DART_SIZE             	0x00007000UL

/*----------------------------------------------------------------------------+
|  #####  #     # ######  ####### ######            ###   #######
| #     # #     # #     # #       #     #            #    #     #
| #       #     # #     # #       #     #            #    #     #
|  #####  #     # ######  #####   ######             #    #     #
|       # #     # #       #       #   #              #    #     #
| #     # #     # #       #       #    #             #    #     #
|  #####   #####  #       ####### #     # #######   ###   #######
+----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------+
| Configuration registers.
+----------------------------------------------------------------------------*/
#define SUPER_IO_INDEX_OFF		0x2E
#define SUPER_IO_DATA_OFF		0x2F

#define SUPER_SST_IO_INDEX_OFF		0x2E
#define SUPER_SST_IO_DATA_OFF		0x2F

#define SUPER_IO_DEVICE_SEL		0x07

#define SUPER_IO_DEVICE_S1		3
#define SUPER_IO_DEVICE_S2		2
#define SUPER_IO_DEVICE_XBUS		15
#define SUPER_IO_DEVICE_RTC		16

#define SUPER_IO_ADDR_XBUS		0x800
#define SUPER_IO_ADDR_RTC		0x900
#define SUPER_IO_ADDR_NVRAM		0x902

#define SUPER_IO_DEVICE_CTRL		0x30
#define SUPER_IO_BASE_DEV_MSB		0x60
#define SUPER_IO_BASE_DEV_LSB		0x61
#define SUPER_IO_EXT_DEV_MSB		0x62
#define SUPER_IO_EXT_DEV_LSB		0x63
#define SUPER_IO_INT_NUM		0x70
#define SUPER_IO_INT_TYPE		0x71

#define SUPER_IO_SERIAL_CONFIG		0xF0

#define SUPER_IO_XBUS_CONFIG		0xF8
#define SUPER_IO_BIOS_SIZE_16M		0x06
#define SUPER_IO_BIOS_SIZE_1M		0x02

#define SUPER_IO_XBUS_HOST_ACCESS	(SUPER_IO_ADDR_XBUS+ 0x13)

#define SUPER_IO_RTC_DATE_ALARM_OFF	0xF1
#define SUPER_IO_RTC_MONTH_ALARM_OFF	0xF2
#define SUPER_IO_RTC_CENTURY_ALARM_OFF	0xF3

#define SUPER_IO_RTC_DATE_ALARM_LOC	0x0D
#define SUPER_IO_RTC_MONTH_ALARM_LOC	0x0E
#define SUPER_IO_RTC_CENTURY_ALARM_LOC	0x0F

#define SUPER_IO_DEVICE_ENABLE		0x01

#define SUPER_IO_SST_START_CONFIG	0x55
#define SUPER_IO_SST_STOP_CONFIG	0xAA

#define SUPER_IO_SST_ID_INDEX		0x20
#define SUPER_IO_SST_ID_VALUE		0x51

#define SUPER_IO_SST_DEVICE_INDEX	0x07
#define SUPER_IO_SST_DEVICE_S1		0x04
#define SUPER_IO_SST_DEVICE_S2		0x05
#define SUPER_IO_SST_DEVICE_RUNTIME	0x0A

#define SUPER_IO_INT_SELECT             0x70
#define SUPER_IO_INT_SERIAL_1           0x04
#define SUPER_IO_INT_SERIAL_2           0x03

#define SUPER_IO_SST_RUNTIME_REGS	0x100

#define SUPER_IO_BASE_CLOCKL32		0xF0

#define SUPER_IO_BASE_CLOCKL32_ALL_OFF	0x03

#define SUPER_IO_SST_GPIO_52		0x41
#define SUPER_IO_SST_GPIO_53		0x42

#define SUPER_IO_SST_GPIO_60		0x47
#define SUPER_IO_SST_GPIO_61		0x48

#define SUPER_IO_SST_GPIO_LED1		0x5D
#define SUPER_IO_SST_GPIO_LED2		0x5E

#define SEPER_IO_SST_RX			0x05
#define SEPER_IO_SST_TX			0x04

#define SEPER_IO_SST_LED1		0x06
#define SEPER_IO_SST_LED2		0x06

#define SEPER_IO_SST_LED_ONE_HZ		0x01
#define SEPER_IO_SST_LED_HALF_HZ	0x02

/*----------------------------------------------------------------------------+
|    #    #     # ######   #####     #     #####     #
|   # #   ##   ## #     # #     #   ##    #     #   ##
|  #   #  # # # # #     # #     #  # #          #  # #
| #     # #  #  # #     #  #####     #     #####     #
| ####### #     # #     # #     #    #          #    #
| #     # #     # #     # #     #    #    #     #    #
| #     # #     # ######   #####   #####   #####   #####
+----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------+
| PCI register information.
+----------------------------------------------------------------------------*/
#define HTT_BRIDGE_ID				((unsigned int)0x7450)
#define HTT_IOAPIC_ID				((unsigned int)0x7451)

#define HTT_INDEX_OFF				0xB8
#define HTT_DATA_OFF				0xBC
#define HTT_IOAPIC_CTRL				0x44
#define HTT_PREF_CONFIG_REG			0x4C
#define HTT_LINK_CFG_A				0xC4
#define HTT_LINK_CFG_B				0xC8
#define HTT_LINK_FREQ_CAP_A			0xCC
#define HTT_SEC_STATUS_REG			0xA0
#define HTT_LINK_FREQ_CAP_B			0xD0

/*----------------------------------------------------------------------------+
|    #    #     # ######   #####     #       #       #
|   # #   ##   ## #     # #     #   ##      ##      ##
|  #   #  # # # # #     # #     #  # #     # #     # #
| #     # #  #  # #     #  #####     #       #       #
| ####### #     # #     # #     #    #       #       #
| #     # #     # #     # #     #    #       #       #
| #     # #     # ######   #####   #####   #####   #####
+----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------+
| PCI register information.
+----------------------------------------------------------------------------*/
#define AMD_VENDOR_ID			((unsigned int)0x1022)
#define SB_LPCB_DEV_ID			((unsigned int)0x7468)
#define SB_SYSM_DEV_ID			((unsigned int)0x746B)
#define SB_PCIB_DEV_ID			((unsigned int)0x7460)
#define SB_USB_DEV_ID			((unsigned int)0x7464)
#define SB_EHC_DEV_ID			((unsigned int)0x7463)
#define SB_ENET_DEV_ID			((unsigned int)0x7462)
#define SB_IDE_DEV_ID			((unsigned int)0x7469)
#define SB_SMB_DEV_ID			((unsigned int)0x746A)
#define SB_AC97AUDIO_DEV_ID		((unsigned int)0x746D)
#define SB_AC97MODEM_DEV_ID		((unsigned int)0x746E)

#define SB_R_IO_CTRL1			0x40
#define SB_R_LEG_CTRL			0x42
#define SB_R_ROM_DECODE			0x43
#define SB_R_MISC_CTRL			0x47
#define SB_R_FUNC_ENABLE                0x48
#define SB_R_IOAPIC_C0			0x4A
#define SB_R_IOAPIC_C1			0x4B
#define SB_R_SCICONFIG			0x42
#define SB_R_PNP_IRQ_SEL		0x44
#define SB_R_SERIRQ_CONNF		0x4A
#define SB_R_PCI_PREF_C0		0x50
#define SB_R_PCI_PREF_C1		0x54
#define SB_R_PCI_IRQ_ROUTE		0x56
#define SB_R_NVCTRL			0x74

#define SB_LPC_ROM_W			0x01
#define SB_LPC_ROM_SIZE			0xC0
#define SB_PCI_PR_C0			0x00000000
#define SB_PCI_PR_C1			0x0000718D
#define SB_NVRAM_EN			0xDE01

#define SB_SYSM_CC_WRITE                0x60

#define SB_NVRAM_ADDR			(NB_HT_IO_BASE_CPU+ 0xDE00)

/*----------------------------------------------------------------------------+
| IDE controller
+----------------------------------------------------------------------------*/
#define SB_IDE_PRI_BASE                 (NB_HT_IO_BASE_CPU+ 0x1F0)
#define SB_IDE_SEC_BASE                 (NB_HT_IO_BASE_CPU+ 0x170)
#define SB_IDE_DATA                     (NB_HT_IO_BASE_CPU+ 0x1F0)
#define SB_IDE_CNT                      (NB_HT_IO_BASE_CPU+ 0x1F2)
#define SB_IDE_LBAL                     (NB_HT_IO_BASE_CPU+ 0x1F3)
#define SB_IDE_LBAM                     (NB_HT_IO_BASE_CPU+ 0x1F4)
#define SB_IDE_LBAH                     (NB_HT_IO_BASE_CPU+ 0x1F5)
#define SB_IDE_DEV                      (NB_HT_IO_BASE_CPU+ 0x1F6)

/*----------------------------------------------------------------------------+
| Write.
+----------------------------------------------------------------------------*/
#define SB_IDE_CMD                      (NB_HT_IO_BASE_CPU+ 0x1F7)

/*----------------------------------------------------------------------------+
| Read.
+----------------------------------------------------------------------------*/
#define SB_IDE_STAT                     (NB_HT_IO_BASE_CPU+ 0x1F7)
#define SB_IDE_CTRL                     (NB_HT_IO_BASE_CPU+ 0x3F6)

#define IDE_STAT_BSY                    0x80
#define IDE_DEV_LBA                     0x40
#define IDE_DEV_HEAD_MASK               0x0F
#define IDE_CMD_READ_RETRIES            0x20
#define IDE_CTRL_NIEN                   0x02

#define IDE_RANGE_LEGACY                0xCC00

#define SB_EIDEC_CMD                    0x04
#define SB_EIDEC_PROG                   0x08
#define SB_EIDEC_INT                    0x3C
#define SB_EIDEC_CONFIG                 0x40

#define EIDEC_CMD_BMEN                  0x00000004
#define EIDEC_CMD_IOEN                  0x00000001
#define EIDEC_PROG_PROGIF2              0x00000400
#define EIDEC_PROG_PROGIF0              0x00000100
#define EIDEC_CONFIG_PRIEN              0x00000002
#define EIDEC_CONFIG_SECEN              0x00000001

/*----------------------------------------------------------------------------+
| LPC bus.
+----------------------------------------------------------------------------*/
#define SB_LPC_FUNCENAB                 0x48
#define LPC_FUNCENAB_IDE                0x0002

#define SB_RTC_LEG_ADDR                 0x70
#define SB_RTC_LEG_DATA                 0x71

/*----------------------------------------------------------------------------+
| RTC.
+----------------------------------------------------------------------------*/
#define SB_RTC_ADDR_PORT70		(NB_HT_IO_BASE_CPU+ 0x70)
#define SB_RTC_DATA_PORT71		(NB_HT_IO_BASE_CPU+ 0x71)
#define SB_RTC_ADDR_PORT72		(NB_HT_IO_BASE_CPU+ 0x72)
#define SB_RTC_DATA_PORT73		(NB_HT_IO_BASE_CPU+ 0x73)


/************ as_archppc.h ****************/
/*----------------------------------------------------------------------------+
| Schould be included only in assemble files.
+----------------------------------------------------------------------------*/
//#ifdef __ASM__

/*----------------------------------------------------------------------------+
| Register definitions and assembler specific definitions.
+----------------------------------------------------------------------------*/
#ifdef __GNUC__
#define new_section(name,attr)		.section "name", "attr"
#define r0				0
#define r1				1
#define r2  				2
#define r3				3
#define r4				4
#define r5				5
#define r6				6
#define r7				7
#define r8				8
#define r9				9
#define r10				10
#define r11				11
#define r12				12
#define r13				13
#define r14				14
#define r15				15
#define r16				16
#define r17				17
#define r18				18
#define r19				19
#define r20				20
#define r21				21
#define r22				22
#define r23				23
#define r24				24
#define r25				25
#define r26				26
#define r27				27
#define r28				28
#define r29				29
#define r30				30
#define r31				31
#define fr0				0
#define fr1				1
#define fr2  				2
#define fr3				3
#define fr4				4
#define fr5				5
#define fr6				6
#define fr7				7
#define fr8				8
#define fr9				9
#define fr10				10
#define fr11				11
#define fr12				12
#define fr13				13
#define fr14				14
#define fr15				15
#define fr16				16
#define fr17				17
#define fr18				18
#define fr19				19
#define fr20				20
#define fr21				21
#define fr22				22
#define fr23				23
#define fr24				24
#define fr25				25
#define fr26				26
#define fr27				27
#define fr28				28
#define fr29				29
#define fr30				30
#define fr31				31
#endif
#ifdef __MW__
#define new_section(name,attr)	.section name, attr
#define r0				%r0
#define r1				%r1
#define r2  				%r2
#define r3				%r3
#define r4				%r4
#define r5				%r5
#define r6				%r6
#define r7				%r7
#define r8				%r8
#define r9				%r9
#define r10				%r10
#define r11				%r11
#define r12				%r12
#define r13				%r13
#define r14				%r14
#define r15				%r15
#define r16				%r16
#define r17				%r17
#define r18				%r18
#define r19				%r19
#define r20				%r20
#define r21				%r21
#define r22				%r22
#define r23				%r23
#define r24				%r24
#define r25				%r25
#define r26				%r26
#define r27				%r27
#define r28				%r28
#define r29				%r29
#define r30				%r30
#define r31				%r31
#define fr0				%f0
#define fr1				%f1
#define fr2  				%f2
#define fr3				%f3
#define fr4				%f4
#define fr5				%f5
#define fr6				%f6
#define fr7				%f7
#define fr8				%f8
#define fr9				%f9
#define fr10				%f10
#define fr11				%f11
#define fr12				%f12
#define fr13				%f13
#define fr14				%f14
#define fr15				%f15
#define fr16				%f16
#define fr17				%f17
#define fr18				%f18
#define fr19				%f19
#define fr20				%f20
#define fr21				%f21
#define fr22				%f22
#define fr23				%f23
#define fr24				%f24
#define fr25				%f25
#define fr26				%f26
#define fr27				%f27
#define fr28				%f28
#define fr29				%f29
#define fr30				%f30
#define fr31				%f31
#endif

/*----------------------------------------------------------------------------+
| Condition register defines.
+----------------------------------------------------------------------------*/
#define cr0				0
#define cr1				1
#define cr2				2
#define cr3				3
#define cr4				4
#define cr5				5
#define cr6				6
#define cr7				7
#define cr0_0   			0
#define cr0_1   			1
#define cr0_2   			2
#define cr0_3   			3
#define cr1_0   			4
#define cr1_1   			5
#define cr1_2   			6
#define cr1_3   			7
#define cr2_0   			8
#define cr2_1   			9
#define cr2_2   			10
#define cr2_3   			11
#define cr3_0   			12
#define cr3_1   			13
#define cr3_2   			14
#define cr3_3   			15
#define cr4_0   			16
#define cr4_1   			17
#define cr4_2   			18
#define cr4_3   			19
#define cr5_0   			20
#define cr5_1   			21
#define cr5_2   			22
#define cr5_3   			23
#define cr6_0   			24
#define cr6_1   			25
#define cr6_2   			26
#define cr6_3   			27
#define cr6_sign 			24
#define cr6_inf 			25
#define cr6_zero 			26
#define cr6_NaN 			27
#define cr7_0   			28
#define cr7_1   			29
#define cr7_2   			30
#define cr7_3   			31
#define cr7_sign 			28
#define cr7_inf 			29
#define cr7_zero 			30
#define cr7_NaN 			31

#define cr4_lt 				16 
#define cr4_eq 				18 
#define cr6_lt 				24 
#define cr7_lt 				28 

/*----------------------------------------------------------------------------+
| Basic PowerPC 32/64 bit instruction definitions and 32/64 bit defines.
+----------------------------------------------------------------------------*/
#ifdef __PPC64__
#define LOAD(rd,disp,ds)		ld	rd,disp(ds)
#define STORE(rs,disp,ra)		std	rs,disp(ra)
#define CMPI(cr,ra,si)			cmpdi	cr,ra,si
#define CMPL(cr,ra,rb)			cmpld	cr,ra,rb
#define GETTIMEBASEL(ra)		mftb	ra
#define TRACE_ENTRY_SIZE		16
#define MULL(rt,ra,rb)			mulld	rt,ra,rb
#define RFI				rfid
#else
#define LOAD(rd,disp,ds)		lwz	rd,disp(ds)
#define STORE(rs,disp,ra)		stw	rs,disp(ra)
#define CMPI(cr,ra,si)			cmpwi	cr,ra,si
#define CMPL(cr,ra,rb)			cmplw	cr,ra,rb
#define GETTIMEBASEL(ra)		mfspr   ra,tblr
#define TRACE_ENTRY_SIZE		12
#define MULL(rt,ra,rb)			mullw	rt,ra,rb
#define RFI				rfi
#endif

/*----------------------------------------------------------------------------+
| Macro for loading a 64 bit value into a register without using TOC and a 
| TOC entry that has to be used when loading symbol address using TOC.
+----------------------------------------------------------------------------*/
#ifdef __PPC64__
#define LOAD_64BIT_VAL(ra,value)	addis    ra,r0,value@highest;         \
					ori      ra,ra,value@higher;          \
					sldi     ra,ra,32;                    \
					oris     ra,ra,value@h;               \
					ori      ra,ra,value@l
#define TOC_ENTRY(name,symbol)          .section ".toc","aw";                 \
					name:    .tc symbol[TC],symbol
#define GETSYMADDR(ra,sym,name)		ld 	ra,name@toc(r2)
#else
#define TOC_ENTRY(name,symbol)           
#define GETSYMADDR(ra,sym,name)		addis	ra,r0,sym@h;ori	ra,ra,sym@l
#endif

/*----------------------------------------------------------------------------+
| Stack frame offsets.  FP register area starts after back chain and LR save. 
| Space to save non-volatile registers 16-31 starts after FP register area.
| Stack frame needs to be a multiple of 8 bytes for EABI.  This is only used
| in 32-bit mode for floating point emulation functions.
+----------------------------------------------------------------------------*/
#ifndef __PPC64__
#define NUMB_SAVEREGS 			16
#define STACKSIZE 			((NUMB_SAVEREGS* 4)+ 8)
#define SAVER_BASE 			8
#define SAVEREG(rn) 			stw r##rn,(SAVER_BASE+ (rn- 16)* 4)(r1)
#define RESTREG(rn) 			lwz r##rn,(SAVER_BASE+ (rn- 16)* 4)(r1)
#endif

/*----------------------------------------------------------------------------+
| Macros for function prolog/epilog, function call.
+----------------------------------------------------------------------------*/
#ifdef __PPC64__
#define function_prolog(fn)		.section	".text";             \
					.align 		2;                   \
					.globl 		fn;	             \
         				.section        ".opd","aw";	     \
         				.align 		3;		     \
 					fn:;				     \
					.quad   	.fn,.TOC.@tocbase,0; \
        				.previous;			     \
        				.size   	fn,24;	     	     \
        				.globl  	.fn;		     \
					.fn:

#define function_epilog(fn)		.long		0;		     \
					.byte 		0,12,0,0,0,0,0,0;    \
        				.type           .fn,@function;	     \
					.size 		.fn,.-.fn

#define data_global_prolog(dn) 		.section        ".toc","aw";	     \
        				.tc 		dn[TC],dn;	     \
					.section        ".data";	     \
					.align 		3; 		     \
					.globl 		dn; 		     \
					dn:

#define data_global_epilog(dn)	 	.type   	dn,@object; 	     \
					.size   	dn,.-dn

#define data_prolog(dn) 		.section        ".toc","aw";         \
					.tc             dn[TC],dn;           \
					.section        ".data";             \
					.align          3;                   \
					dn:

#define data_epilog(data_name) 		.type           dn,@object;          \
					.size           dn,.-dn

#define function_call(func_name)	bl  		.func_name 

#else
#define function_nos_prolog(func_name)	.align  	2; 		     \
					.globl 		func_name;           \
					func_name:

#define function_prolog(func_name)	.text; 				     \
					.align 		2;		     \
					.globl 		func_name;           \
					func_name:

#define function_epilog(func_name)	.type 		func_name,@function; \
					.size 		func_name,.-func_name

#define data_global_prolog(data_name) 	.data; 				     \
					.align 		2; 		     \
					.globl 		data_name;           \
					data_name:

#define data_global_epilog(data_name) 	.type   	data_name,@object;   \
					.size   	data_name,.-data_name

#define data_prolog(data_name) 		.data; 				     \
					.align 		2;		     \
					data_name:

#define data_epilog(data_name) 		.type   	data_name,@object;   \
					.size   	data_name,.-data_name

#define function_call(func_name)	bl 		func_name 

#endif

/*----------------------------------------------------------------------------+
| EABI and 64-bit API stack frame definition.
+----------------------------------------------------------------------------*/
#ifdef __PPC64__
#define stack_frame_min			112
#define stack_frame_bc			0
#define stack_frame_lr			16
#define stack_neg_off			288
#else
#define stack_frame_min			8
#define stack_frame_bc			0
#define stack_frame_lr			4
#define stack_neg_off			0
#endif

/*----------------------------------------------------------------------------+
| Size of the context register.
+----------------------------------------------------------------------------*/
#ifdef __PPC64__
#define stack_reg_image_size		328
#define stack_reg_img_s_new		328
#define stack_reg_s_spr   	        72
#else
#define stack_reg_image_size		160
#define stack_reg_img_s_new		168
#define stack_reg_s_spr   	        40
#endif

/*----------------------------------------------------------------------------+
| "dlmzb" instruction is not supported by the GNU assembler.  On 7XX 
| processorsdlmzb instruction does not exist.  In that case the instruction
| is emulated.  The emulated unstruction does not set CR[CR0] SO (overflow).
+----------------------------------------------------------------------------*/
#if ! defined __PPC7XX__ && ! defined __PPC64__
#define DLMZBDOT(ra,rs,rb,lab,rt)	.long 0x7c00009d|\
					(rs<<21)|(ra<<16)|(rb<<11)

#define DLMZBNDOT(ra,rs,rb,lab,rt)	.long 0x7c00009c|\
					(rs<<21)|(ra<<16)|(rb<<11)
#else
#define DLMZBDOT(ra,rs,rb,lab,rt)	addi	ra,r0,0x0001;\
					rlwinm.	rt,rs,0,0,7;\
					beq	lab##end1;\
					addi	ra,r0,0x0002;\
					rlwinm. rt,rs,0,8,15;\
					beq	lab##end1;\
					addi	ra,r0,0x0003;\
					rlwinm.	rt,rs,0,16,23;\
					beq	lab##end1;\
					addi	ra,r0,0x0004;\
					rlwinm.	rt,rs,0,24,31;\
					beq	lab##end1;\
					addi	ra,r0,0x0005;\
					rlwinm. rt,rb,0,0,7;\
					beq	lab##end2;\
					addi	ra,r0,0x0006;\
					rlwinm. rt,rb,0,8,15;\
					beq	lab##end2;\
					addi	ra,r0,0x0007;\
					rlwinm.	rt,rb,0,16,23;\
					beq	lab##end2;\
					addi	ra,r0,0x0008;\
					rlwinm.	rt,rb,0,24,31;\
					beq	lab##end2;\
					addis	rt,r0,0x2000;\
					mtcrf	0x80,rt;\
					b	lab##fin;\
					lab##end1:;\
					addis	rt,r0,0x4000;\
					mtcrf	0x80,rt;\
					b	lab##fin;\
					lab##end2:;\
					addis	rt,r0,0x8000;\
					mtcrf	0x80,rt;\
					lab##fin:;\
					mfspr   rt,xer_reg;\
					rlwinm	rt,rt,0,0,24;\
					or	rt,rt,ra;\
					mtspr	xer_reg,rt

#define DLMZBNDOT(ra,rs,rb,lab,rt)	addi	ra,r0,0x0001;\
					rlwinm.	rt,rs,0,0,7;\
					beq	lab##end;\
					addi	ra,r0,0x0002;\
					rlwinm. rt,rs,0,8,15;\
					beq	lab##end;\
					addi	ra,r0,0x0003;\
					rlwinm.	rt,rs,0,16,23;\
					beq	lab##end;\
					addi	ra,r0,0x0004;\
					rlwinm.	rt,rs,0,24,31;\
					beq	lab##end;\
					addi	ra,r0,0x0005;\
					rlwinm. rt,rb,0,0,7;\
					beq	lab##end;\
					addi	ra,r0,0x0006;\
					rlwinm. rt,rb,0,8,15;\
					beq	lab##end;\
					addi	ra,r0,0x0007;\
					rlwinm.	rt,rb,0,16,23;\
					beq	lab##end;\
					addi	ra,r0,0x0008;\
					rlwinm.	rt,rb,0,24,31;\
					lab##end:;\
					mfspr   rt,xer_reg;\
					rlwinm	rt,rt,0,0,24;\
					or	rt,rt,ra;\
					mtspr	xer_reg,rt
#endif

/*----------------------------------------------------------------------------+
| Following instructions are not available in Book E mode of the GNU assembler.
+----------------------------------------------------------------------------*/
#ifdef __PPC64__
#define TLBIEL(rb)			.long 0x7C000000|\
					(rb<<11)|(274<<1)
#define HRFID()				.long 0x4C000000|\
					(274<<1)
#else
#define DCCCI(ra,rb)			.long 0x7c000000|\
					(ra<<16)|(rb<<11)|(454<<1)

#define ICCCI(ra,rb)			.long 0x7c000000|\
					(ra<<16)|(rb<<11)|(966<<1)

#define DCREAD(rt,ra,rb)		.long 0x7c000000|\
					(rt<<21)|(ra<<16)|(rb<<11)|(486<<1)

#define ICREAD(ra,rb)			.long 0x7c000000|\
					(ra<<16)|(rb<<11)|(998<<1)

#define TLBSX(rt,ra,rb)			.long 0x7c000000|\
					(rt<<21)|(ra<<16)|(rb<<11)|(914<<1)

#define TLBWE(rs,ra,ws)			.long 0x7c000000|\
					(rs<<21)|(ra<<16)|(ws<<11)|(978<<1)

#define TLBRE(rt,ra,ws)			.long 0x7c000000|\
					(rt<<21)|(ra<<16)|(ws<<11)|(946<<1)

#define TLBSXDOT(rt,ra,rb)		.long 0x7c000001|\
					(rt<<21)|(ra<<16)|(rb<<11)|(914<<1)

#define MSYNC				.long 0x7c000000|\
					(598<<1)

#define MBAR 				.long 0x7c000000|\
					(854<<1)
#endif

/*----------------------------------------------------------------------------+
| Special Purpose Registers.  Xer, lr, ctr, srr0, srr1, sprg0, sprg1, sprg2,
| sprg3, pvr, tbl (read), tlu (read) registers.
+----------------------------------------------------------------------------*/
#define xer_reg     			0x001
#define lr_reg				0x008
#define	ctr				0x009
#define srr0	 			0x01a
#define srr1	 			0x01b
#define sprg0				0x110
#define sprg1				0x111
#define sprg2				0x112
#define sprg3				0x113
#define pvr				0x11f
#define tblr          			0x10c
#define tbur				0x10d

//#endif //__ASM__

/*----------------------------------------------------------------------------+
| Same as above for the C code.
+----------------------------------------------------------------------------*/
#define SPR_SRR0	 		0x01a
#define SPR_SRR1	 		0x01b
#define SPR_SPRG0			0x110
#define SPR_SPRG1			0x111
#define SPR_SPRG2			0x112
#define SPR_SPRG3			0x113
#define SPR_PVR				0x11f
#define SPR_TBLR          		0x10c
#define SPR_TBUR			0x10d

/*----------------------------------------------------------------------------+
| Machine State Register.  MSR_EE, MSR_PR, MSR_FP, MSR_ME, MSR_FE0, MSR_FE1.
+----------------------------------------------------------------------------*/
#define MSR_EE				0x00008000
#define MSR_PR				0x00004000
#define MSR_FP				0x00002000
#define MSR_ME				0x00001000
#define MSR_FE0				0x00000800
#define MSR_FE1				0x00000100


/************ as_archppc970.h ****************/
/*----------------------------------------------------------------------------+
| PVR value.
+----------------------------------------------------------------------------*/
#define PVR_970_DD1   		0x00391100
#define PVR_970FX_DD2   	0x003C0200
#define PVR_970FX_DD2_1 	0x003C0201
#define PVR_970FX_DD3   	0x003C0300

/*----------------------------------------------------------------------------+
| Special Purpose Registers.  Xer (64), lr (64), ctr (64), srr0 (64), srr1 (64)
| sprg0 (64), sprg1 (64), sprg2 (64), sprg3 (64), pvr (32) tblr (64), tbur (32)
| registers are defined in as_archppc.h.
+----------------------------------------------------------------------------*/
#define SPR_ACCR    	0x001D  /* 64-bit read/write                        $*/
#define SPR_ASR 	0x0118  /* 64-bit read/write, write hypervisor only  */
#define SPR_DABR	0x03F5  /* 64-bit read/write, write hypervisor only  */
#define SPR_DABRX	0x03F7  /* 64-bit read/write, write hypervisor only  */
#define SPR_DAR     	0x0013  /* 64-bit read/write	                     */
#define SPR_DEC         0x0016  /* 32-bit read/write                         */
#define SPR_DSISR   	0x0012  /* 32-bit read/write                         */
#define SPR_HDEC        0x0136	/* 64-bit read/write, write hypervisor only  */
#define SPR_HID0	0x03F0  /* 64-bit read/write, write hypervisor only  */
#define SPR_HID1    	0x03F1  /* 64-bit read/write, write hypervisor only  */
#define SPR_HID4    	0x03F4  /* 64-bit read/write, write hypervisor only  */
#define SPR_HID5        0x03F6  /* 64-bit read/write, write hypervisor only  */
#define SPR_HIOR    	0x0137  /* 64-bit read/write			     */
#define SPR_HSPRG0  	0x0130  /* 64-bit read/write, write hypervisor only  */
#define SPR_HSPRG1  	0x0131  /* 64-bit read/write, write hypervisor only  */
#define SPR_HSRR0   	0x013A  /* 64-bit read/write, write hypervisor only  */
#define SPR_HSRR1   	0x013B  /* 64-bit read/write, write hypervisor only  */
#define SPR_IMC	        0x030F  /* 64-bit read/write                         */
#define SPR_MMCR0   	0x031B  /* 64-bit read/write                         */
#define SPR_MMCR1   	0x031E  /* 64-bit read/write                         */
#define SPR_MMCRA   	0x0312  /* 64-bit read/write                         */
#define SPR_PIR 	0x03FF  /* 32-bit read                               */
#define SPR_PMC1    	0x0313  /* 32-bit read/write                         */
#define SPR_PMC2    	0x0314  /* 32-bit read/write                         */
#define SPR_PMC3    	0x0315  /* 32-bit read/write                         */
#define SPR_PMC4    	0x0316  /* 32-bit read/write                         */
#define SPR_PMC5    	0x0317  /* 32-bit read/write                         */
#define SPR_PMC6    	0x0318  /* 32-bit read/write                         */
#define SPR_PMC7    	0x0319  /* 32-bit read/write                         */
#define SPR_PMC8    	0x031A  /* 32-bit read/write                         */
#define SPR_SCOMC   	0x0114  /* 64-bit read/write, write hypervisor only  */
#define SPR_SCOMD   	0x0115  /* 64-bit read/write, write hypervisor only  */
#define SPR_SDAR        0x031D	/* 64-bit read/write                         */
#define SPR_SDR1        0x0019  /* 64-bit read/write, write hypervisor only  */
#define SPR_SIAR    	0x031C  /* 64-bit read/write                         */
#define SPR_TBL_WRITE	0x011C	/* 32-bit write				     */
#define SPR_TBU_WRITE	0x011D	/* 32-bit write                              */
#define SPR_TRACE 	0x03FE  /* 64-bit read                              $*/
#define SPR_TRIG0   	0x03D0  /* 64-bit write                              */
#define SPR_TRIG1   	0x03D1  /* 64-bit write                              */
#define SPR_TRIG2   	0x03D2  /* 64-bit write                              */
#define SPR_VRSAVE      0x0100  /* 64-bit read/write			    $*/

/*----------------------------------------------------------------------------+
| Vector status and control register is accessed using the mfvscr and mtvscr
| instructions.
+----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------+
| Machine State Register.  MSR_EE, MSR_PR, MSR_FP, MSR_ME, MSR_FE0, MSR_FE1,
| register bits are defined in as_archppc.h.  This is a 64-bit register.
+----------------------------------------------------------------------------*/
#define MSR_SF          0x8000000000000000 /* 64/32 bit mode indicator       */
#define MSR_HV		0x1000000000000000 /* hypervisor mode		     */
#define MSR_VMX		0x0000000002000000 /* vmx unit available   	     */
#define MSR_POW		0x0000000000040000 /* power management enable	     */
#define MSR_SE 		0x0000000000000400 /* single step          	     */
#define MSR_BE 		0x0000000000000200 /* branch trace      	     */
#define MSR_IS  	0x0000000000000020 /* instruction address space	     */
#define MSR_DS  	0x0000000000000010 /* data address space	     */
#define MSR_PM		0x0000000000000004 /* performance monitor	     */
#define MSR_RI		0x0000000000000002 /* recoverable interrupt          */

/*----------------------------------------------------------------------------+
| HID0 bits.
+----------------------------------------------------------------------------*/
#define HID0_ONEPPC	0x8000000000000000
#define HID0_SINGLE	0x4000000000000000
#define HID0_ISYNC_SC	0x2000000000000000
#define HID0_SERIAL_G	0x1000000000000000
#define HID0_DEEP_NAP  	0x0100000000000000
#define HID0_NAP     	0x0040000000000000
#define HID0_DPM     	0x0010000000000000
#define HID0_TR_GR   	0x0004000000000000
#define HID0_TR_DIS   	0x0002000000000000
#define HID0_NHR      	0x0001000000000000
#define HID0_INORDER    0x0000800000000000
#define HID0_ENH_TR     0x0000400000000000
#define HID0_TB_CTRL    0x0000200000000000
#define HID0_EXT_TB_EN  0x0000100000000000
#define HID0_CIABR_EN   0x0000020000000000
#define HID0_HDEC_EN    0x0000010000000000
#define HID0_EB_THERM   0x0000008000000000
#define HID0_EN_ATTN    0x0000000100000000
#define HID0_EN_MAC     0x0000000080000000

/*----------------------------------------------------------------------------+
| HID1 bits.
+----------------------------------------------------------------------------*/
#define HID1_BHT_PM	0xE000000000000000
#define HID1_BHT_STATIC 0x0000000000000000
#define HID1_BHT_GLOBAL 0x4000000000000000
#define HID1_BHT_LOCAL  0x8000000000000000
#define HID1_BHT_GL_LO  0xC000000000000000
#define HID1_BHT_GL_CO  0x6000000000000000
#define HID1_BHT_FULL   0xE000000000000000
#define HID1_EN_LS 	0x1000000000000000
#define HID1_EN_CC 	0x0800000000000000
#define HID1_EN_IC 	0x0400000000000000
#define HID1_PF_MASK	0x0180000000000000
#define HID1_PF_NSA	0x0080000000000000
#define HID1_PF_NSA_P	0x0100000000000000
#define HID1_PF_DIS  	0x0180000000000000
#define HID1_EN_ICBI 	0x0040000000000000
#define HID1_EN_IF_CACH 0x0020000000000000
#define HID1_EN_IC_REC  0x0010000000000000
#define HID1_EN_ID_REC  0x0008000000000000
#define HID1_EN_ER_REC  0x0004000000000000
#define HID1_IC_PE      0x0002000000000000
#define HID1_ICD0_PE    0x0001000000000000
#define HID1_ICD1_PE    0x0000800000000000
#define HID1_IER_PE     0x0000400000000000
#define HID1_EN_SP_ITW  0x0000200000000000
#define HID1_S_CHICKEN  0x0000100000000000

/*----------------------------------------------------------------------------+
| HID4 bits.
+----------------------------------------------------------------------------*/
#define HID4_LPES0	0x8000000000000000
#define HID4_RMLR12_MSK	0x6000000000000000
#define HID4_LPID25_MSK	0x1E00000000000000
#define HID4_RMOR_MASK 	0x01FFFE0000000000
#define HID4_RM_CI     	0x0000010000000000
#define HID4_FORCE_AI  	0x0000008000000000
#define HID4_DIS_PERF  	0x0000004000000000
#define HID4_RES_PERF  	0x0000002000000000
#define HID4_EN_SP_DTW 	0x0000001000000000
#define HID4_L1DC_FLSH 	0x0000000800000000
#define HID4_D_DERAT_P1	0x0000000400000000
#define HID4_D_DERAT_P2	0x0000000200000000
#define HID4_D_DERAT_G 	0x0000000100000000
#define HID4_D_DERAT_S1 0x0000000040000000
#define HID4_D_DERAT_S2 0x0000000080000000
#define HID4_DC_TP_S1   0x0000000020000000
#define HID4_DC_TP_S2   0x0000000010000000
#define HID4_DC_TP_GEN  0x0000000008000000
#define HID4_DC_SET1    0x0000000004000000
#define HID4_DC_SET2    0x0000000002000000
#define HID4_DC_DP_S1   0x0000000001000000
#define HID4_DC_DP_S2   0x0000000000800000
#define HID4_DC_DP_GEN  0x0000000000400000
#define HID4_R_TAG1P_CH 0x0000000000200000
#define HID4_R_TAG2P_CH 0x0000000000100000
#define HID4_TLB_PC1    0x0000000000080000
#define HID4_TLB_PC2    0x0000000000040000
#define HID4_TLB_PC3    0x0000000000020000
#define HID4_TLB_PC4    0x0000000000010000
#define HID4_TLB_P_GEN  0x0000000000008000
#define HID4_TLB_SET1   0x0000000000003800
#define HID4_TLB_SET2   0x0000000000005800
#define HID4_TLB_SET3   0x0000000000006800
#define HID4_TLB_SET4   0x0000000000007000
#define HID4_DIS_SLBPC  0x0000000000000400
#define HID4_DIS_SLBPG  0x0000000000000200
#define HID4_MCK_INJ    0x0000000000000100
#define HID4_DIS_STFWD  0x0000000000000080
#define HID4_LPES1      0x0000000000000040
#define HID4_RMLR0_MSK  0x0000000000000020
#define HID4_DIS_SPLARX 0x0000000000000008
#define HID4_LP_PG_EN   0x0000000000000004
#define HID4_LPID01_MSK 0x0000000000000003

/*----------------------------------------------------------------------------+
| HID5 bits.
+----------------------------------------------------------------------------*/
#define HID5_HRMOR_MASK 0x00000000FFFF0000
#define HID5_DC_MCK     0x0000000000002000
#define HID5_DIS_PWRSAV 0x0000000000001000
#define HID5_FORCE_G    0x0000000000000800
#define HID5_DC_REPL    0x0000000000000400
#define HID5_HWR_STMS   0x0000000000000200
#define HID5_DST_NOOP   0x0000000000000100
#define HID5_DCBZ_SIZE  0x0000000000000080
#define HID5_DCBZ32_ILL 0x0000000000000040
#define HID5_TLB_MAP    0x0000000000000020
#define HID5_IMQ_PORT   0x0000000000000010
#define HID5_LMP_SIZE0  0x0000000000000008
#define HID5_DPFLOOD    0x0000000000000004
#define HID5_TCH_NOP    0x0000000000000002
#define HID5_LMP_SIZE1  0x0000000000000001

/*----------------------------------------------------------------------------+
| Specific SRR1 bit definitions for Machine Check.
+----------------------------------------------------------------------------*/
#define SRR1_IFU_UNREC  0x0000000000200000
#define SRR1_LOAD_STORE 0x0000000000100000
#define SRR1_SLB_PARITY 0x0000000000040000
#define SRR1_TLB_PARITY 0x0000000000080000
#define SRR1_ITLB_RELOA 0x00000000000C0000
#define SRR1_RI         0x0000000000000002

#endif /* _ppc970_h_ */
