#ifndef PCI_DEF_H
#define PCI_DEF_H

/*
 * Under PCI, each device has 256 bytes of configuration address space,
 * of which the first 64 bytes are standardized as follows:
 */
#define PCI_VENDOR_ID		0x00	/* 16 bits */
#define PCI_DEVICE_ID		0x02	/* 16 bits */
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

#define PCI_STATUS		0x06	/* 16 bits */
#define  PCI_STATUS_CAP_LIST	0x10	/* Support Capability List */
#define  PCI_STATUS_66MHZ	0x20	/* Support 66 Mhz PCI 2.1 bus */
#define  PCI_STATUS_UDF		0x40	/* Support User Definable Features [obsolete] */
#define  PCI_STATUS_FAST_BACK	0x80	/* Accept fast-back to back */
#define  PCI_STATUS_PARITY	0x100	/* Detected parity error */
#define  PCI_STATUS_DEVSEL_MASK	0x600	/* DEVSEL timing */
#define  PCI_STATUS_DEVSEL_FAST	0x000
#define  PCI_STATUS_DEVSEL_MEDIUM 0x200
#define  PCI_STATUS_DEVSEL_SLOW 0x400
#define  PCI_STATUS_SIG_TARGET_ABORT 0x800 /* Set on target abort */
#define  PCI_STATUS_REC_TARGET_ABORT 0x1000 /* Master ack of " */
#define  PCI_STATUS_REC_MASTER_ABORT 0x2000 /* Set on master abort */
#define  PCI_STATUS_SIG_SYSTEM_ERROR 0x4000 /* Set when we drive SERR */
#define  PCI_STATUS_DETECTED_PARITY 0x8000 /* Set on parity error */

#define PCI_CLASS_REVISION	0x08	/* High 24 bits are class, low 8
					   revision */
#define PCI_REVISION_ID         0x08    /* Revision ID */
#define PCI_CLASS_PROG          0x09    /* Reg. Level Programming Interface */
#define PCI_CLASS_DEVICE        0x0a    /* Device class */

#define PCI_CACHE_LINE_SIZE	0x0c	/* 8 bits */
#define PCI_LATENCY_TIMER	0x0d	/* 8 bits */
#define PCI_HEADER_TYPE		0x0e	/* 8 bits */
#define  PCI_HEADER_TYPE_NORMAL	0
#define  PCI_HEADER_TYPE_BRIDGE 1
#define  PCI_HEADER_TYPE_CARDBUS 2

#define PCI_BIST		0x0f	/* 8 bits */
#define PCI_BIST_CODE_MASK	0x0f	/* Return result */
#define PCI_BIST_START		0x40	/* 1 to start BIST, 2 secs or less */
#define PCI_BIST_CAPABLE	0x80	/* 1 if BIST capable */

/*
 * Base addresses specify locations in memory or I/O space.
 * Decoded size can be determined by writing a value of
 * 0xffffffff to the register, and reading it back.  Only
 * 1 bits are decoded.
 */
#define PCI_BASE_ADDRESS_0	0x10	/* 32 bits */
#define PCI_BASE_ADDRESS_1	0x14	/* 32 bits [htype 0,1 only] */
#define PCI_BASE_ADDRESS_2	0x18	/* 32 bits [htype 0 only] */
#define PCI_BASE_ADDRESS_3	0x1c	/* 32 bits */
#define PCI_BASE_ADDRESS_4	0x20	/* 32 bits */
#define PCI_BASE_ADDRESS_5	0x24	/* 32 bits */
#define  PCI_BASE_ADDRESS_SPACE	0x01	/* 0 = memory, 1 = I/O */
#define  PCI_BASE_ADDRESS_SPACE_IO 0x01
#define  PCI_BASE_ADDRESS_SPACE_MEMORY 0x00
#define  PCI_BASE_ADDRESS_MEM_LIMIT_MASK 0x06
#define  PCI_BASE_ADDRESS_MEM_LIMIT_32	0x00	/* 32 bit address */
#define  PCI_BASE_ADDRESS_MEM_LIMIT_1M	0x02	/* Below 1M [obsolete] */
#define  PCI_BASE_ADDRESS_MEM_LIMIT_64	0x04	/* 64 bit address */
#define  PCI_BASE_ADDRESS_MEM_PREFETCH	0x08	/* prefetchable? */
#define  PCI_BASE_ADDRESS_MEM_ATTR_MASK	0x0f
#define  PCI_BASE_ADDRESS_IO_ATTR_MASK	0x03
/* bit 1 is reserved if address_space = 1 */

/* Header type 0 (normal devices) */
#define PCI_CARDBUS_CIS		0x28
#define PCI_SUBSYSTEM_VENDOR_ID	0x2c
#define PCI_SUBSYSTEM_ID	0x2e
#define PCI_ROM_ADDRESS		0x30	/* Bits 31..11 are address, 10..1 reserved */
#define  PCI_ROM_ADDRESS_ENABLE	0x01
#define PCI_ROM_ADDRESS_MASK	(~0x7ffUL)

#define PCI_CAPABILITY_LIST	0x34	/* Offset of first capability list entry */

/* 0x35-0x3b are reserved */
#define PCI_INTERRUPT_LINE	0x3c	/* 8 bits */
#define PCI_INTERRUPT_PIN	0x3d	/* 8 bits */
#define PCI_MIN_GNT		0x3e	/* 8 bits */
#define PCI_MAX_LAT		0x3f	/* 8 bits */

/* Header type 1 (PCI-to-PCI bridges) */
#define PCI_PRIMARY_BUS		0x18	/* Primary bus number */
#define PCI_SECONDARY_BUS	0x19	/* Secondary bus number */
#define PCI_SUBORDINATE_BUS	0x1a	/* Highest bus number behind the bridge */
#define PCI_SEC_LATENCY_TIMER	0x1b	/* Latency timer for secondary interface */
#define PCI_IO_BASE		0x1c	/* I/O range behind the bridge */
#define PCI_IO_LIMIT		0x1d
#define  PCI_IO_RANGE_TYPE_MASK	0x0f	/* I/O bridging type */
#define  PCI_IO_RANGE_TYPE_16	0x00
#define  PCI_IO_RANGE_TYPE_32	0x01
#define  PCI_IO_RANGE_MASK	~0x0f
#define PCI_SEC_STATUS		0x1e	/* Secondary status register, only bit 14 used */
#define PCI_MEMORY_BASE		0x20	/* Memory range behind */
#define PCI_MEMORY_LIMIT	0x22
#define  PCI_MEMORY_RANGE_TYPE_MASK 0x0f
#define  PCI_MEMORY_RANGE_MASK	~0x0f
#define PCI_PREF_MEMORY_BASE	0x24	/* Prefetchable memory range behind */
#define PCI_PREF_MEMORY_LIMIT	0x26
#define  PCI_PREF_RANGE_TYPE_MASK 0x0f
#define  PCI_PREF_RANGE_TYPE_32	0x00
#define  PCI_PREF_RANGE_TYPE_64	0x01
#define  PCI_PREF_RANGE_MASK	~0x0f
#define PCI_PREF_BASE_UPPER32	0x28	/* Upper half of prefetchable memory range */
#define PCI_PREF_LIMIT_UPPER32	0x2c
#define PCI_IO_BASE_UPPER16	0x30	/* Upper half of I/O addresses */
#define PCI_IO_LIMIT_UPPER16	0x32
/* 0x34 same as for htype 0 */
/* 0x35-0x3b is reserved */
#define PCI_ROM_ADDRESS1	0x38	/* Same as PCI_ROM_ADDRESS, but for htype 1 */
/* 0x3c-0x3d are same as for htype 0 */
#define PCI_BRIDGE_CONTROL	0x3e
#define  PCI_BRIDGE_CTL_PARITY	0x01	/* Enable parity detection on secondary interface */
#define  PCI_BRIDGE_CTL_SERR	0x02	/* The same for SERR forwarding */
#define  PCI_BRIDGE_CTL_NO_ISA	0x04	/* Disable bridging of ISA ports */
#define  PCI_BRIDGE_CTL_VGA	0x08	/* Forward VGA addresses */
#define  PCI_BRIDGE_CTL_MASTER_ABORT 0x20  /* Report master aborts */
#define  PCI_BRIDGE_CTL_BUS_RESET 0x40	/* Secondary bus reset */
#define  PCI_BRIDGE_CTL_FAST_BACK 0x80	/* Fast Back2Back enabled on secondary interface */

/* Header type 2 (CardBus bridges) */
#define PCI_CB_CAPABILITY_LIST	0x14
/* 0x15 reserved */
#define PCI_CB_SEC_STATUS	0x16	/* Secondary status */
#define PCI_CB_PRIMARY_BUS	0x18	/* PCI bus number */
#define PCI_CB_CARD_BUS		0x19	/* CardBus bus number */
#define PCI_CB_SUBORDINATE_BUS	0x1a	/* Subordinate bus number */
#define PCI_CB_LATENCY_TIMER	0x1b	/* CardBus latency timer */
#define PCI_CB_MEMORY_BASE_0	0x1c
#define PCI_CB_MEMORY_LIMIT_0	0x20
#define PCI_CB_MEMORY_BASE_1	0x24
#define PCI_CB_MEMORY_LIMIT_1	0x28
#define PCI_CB_IO_BASE_0	0x2c
#define PCI_CB_IO_BASE_0_HI	0x2e
#define PCI_CB_IO_LIMIT_0	0x30
#define PCI_CB_IO_LIMIT_0_HI	0x32
#define PCI_CB_IO_BASE_1	0x34
#define PCI_CB_IO_BASE_1_HI	0x36
#define PCI_CB_IO_LIMIT_1	0x38
#define PCI_CB_IO_LIMIT_1_HI	0x3a
#define  PCI_CB_IO_RANGE_MASK	~0x03
/* 0x3c-0x3d are same as for htype 0 */
#define PCI_CB_BRIDGE_CONTROL	0x3e
#define  PCI_CB_BRIDGE_CTL_PARITY	0x01	/* Similar to standard bridge control register */
#define  PCI_CB_BRIDGE_CTL_SERR		0x02
#define  PCI_CB_BRIDGE_CTL_ISA		0x04
#define  PCI_CB_BRIDGE_CTL_VGA		0x08
#define  PCI_CB_BRIDGE_CTL_MASTER_ABORT	0x20
#define  PCI_CB_BRIDGE_CTL_CB_RESET	0x40	/* CardBus reset */
#define  PCI_CB_BRIDGE_CTL_16BIT_INT	0x80	/* Enable interrupt for 16-bit cards */
#define  PCI_CB_BRIDGE_CTL_PREFETCH_MEM0 0x100	/* Prefetch enable for both memory regions */
#define  PCI_CB_BRIDGE_CTL_PREFETCH_MEM1 0x200
#define  PCI_CB_BRIDGE_CTL_POST_WRITES	0x400
#define PCI_CB_SUBSYSTEM_VENDOR_ID 0x40
#define PCI_CB_SUBSYSTEM_ID	0x42
#define PCI_CB_LEGACY_MODE_BASE	0x44	/* 16-bit PC Card legacy mode base address (ExCa) */
/* 0x48-0x7f reserved */

/* Capability lists */

#define PCI_CAP_LIST_ID		0	/* Capability ID */
#define  PCI_CAP_ID_PM		0x01	/* Power Management */
#define  PCI_CAP_ID_AGP		0x02	/* Accelerated Graphics Port */
#define  PCI_CAP_ID_VPD		0x03	/* Vital Product Data */
#define  PCI_CAP_ID_SLOTID	0x04	/* Slot Identification */
#define  PCI_CAP_ID_MSI		0x05	/* Message Signaled Interrupts */
#define  PCI_CAP_ID_CHSWP	0x06	/* CompactPCI HotSwap */
#define  PCI_CAP_ID_PCIX	0x07	/* PCIX  */
#define  PCI_CAP_ID_HT          0x08	/* Hypertransport */
#define  PCI_CAP_ID_SHPC 	0x0C	/* PCI Standard Hot-Plug Controller */
#define  PCI_CAP_ID_PCIE	0x10	/* PCI Express */
#define  PCI_CAP_ID_MSIX	0x11	/* MSI-X */
#define PCI_CAP_LIST_NEXT	1	/* Next capability in the list */
#define PCI_CAP_FLAGS		2	/* Capability defined flags (16 bits) */

/* Hypertransport Registers */
#define PCI_HT_CAP_SIZEOF	   4
#define PCI_HT_CAP_HOST_CTRL       4	/* Host link control */
#define PCI_HT_CAP_HOST_WIDTH	   6	/* width value & capability  */
#define PCI_HT_CAP_HOST_FREQ	   0x09	/* Host frequency */
#define PCI_HT_CAP_HOST_FREQ_CAP   0x0a	/* Host Frequency capability  */
#define PCI_HT_CAP_SLAVE_CTRL0	   4	/* link control */
#define PCI_HT_CAP_SLAVE_CTRL1	   8	/* link control to */
#define PCI_HT_CAP_SLAVE_WIDTH0	   6	/* width value & capability  */
#define PCI_HT_CAP_SLAVE_WIDTH1	   0x0a	/* width value & capability  to */
#define PCI_HT_CAP_SLAVE_FREQ0	   0x0d	/* Slave frequency from */
#define PCI_HT_CAP_SLAVE_FREQ1	   0x011	/* Slave frequency to */
#define PCI_HT_CAP_SLAVE_FREQ_CAP0 0x0e	/* Frequency capability from */
#define PCI_HT_CAP_SLAVE_FREQ_CAP1 0x12	/* Frequency capability to */
#define PCI_HT_CAP_SLAVE_LINK_ENUM   0x14 /* Link Enumeration Scratchpad */

/* Power Management Registers */

#define PCI_PM_PMC              2       /* PM Capabilities Register */
#define  PCI_PM_CAP_VER_MASK	0x0007	/* Version */
#define  PCI_PM_CAP_PME_CLOCK	0x0008	/* PME clock required */
#define  PCI_PM_CAP_AUX_POWER	0x0010	/* Auxiliary power support */
#define  PCI_PM_CAP_DSI		0x0020	/* Device specific initialization */
#define  PCI_PM_CAP_D1		0x0200	/* D1 power state support */
#define  PCI_PM_CAP_D2		0x0400	/* D2 power state support */
#define  PCI_PM_CAP_PME		0x0800	/* PME pin supported */
#define PCI_PM_CTRL		4	/* PM control and status register */
#define  PCI_PM_CTRL_STATE_MASK	0x0003	/* Current power state (D0 to D3) */
#define  PCI_PM_CTRL_PME_ENABLE	0x0100	/* PME pin enable */
#define  PCI_PM_CTRL_DATA_SEL_MASK	0x1e00	/* Data select (??) */
#define  PCI_PM_CTRL_DATA_SCALE_MASK	0x6000	/* Data scale (??) */
#define  PCI_PM_CTRL_PME_STATUS	0x8000	/* PME pin status */
#define PCI_PM_PPB_EXTENSIONS	6	/* PPB support extensions (??) */
#define  PCI_PM_PPB_B2_B3	0x40	/* Stop clock when in D3hot (??) */
#define  PCI_PM_BPCC_ENABLE	0x80	/* Bus power/clock control enable (??) */
#define PCI_PM_DATA_REGISTER	7	/* (??) */
#define PCI_PM_SIZEOF		8

/* AGP registers */

#define PCI_AGP_VERSION		2	/* BCD version number */
#define PCI_AGP_RFU		3	/* Rest of capability flags */
#define PCI_AGP_STATUS		4	/* Status register */
#define  PCI_AGP_STATUS_RQ_MASK	0xff000000	/* Maximum number of requests - 1 */
#define  PCI_AGP_STATUS_SBA	0x0200	/* Sideband addressing supported */
#define  PCI_AGP_STATUS_64BIT	0x0020	/* 64-bit addressing supported */
#define  PCI_AGP_STATUS_FW	0x0010	/* FW transfers supported */
#define  PCI_AGP_STATUS_RATE4	0x0004	/* 4x transfer rate supported */
#define  PCI_AGP_STATUS_RATE2	0x0002	/* 2x transfer rate supported */
#define  PCI_AGP_STATUS_RATE1	0x0001	/* 1x transfer rate supported */
#define PCI_AGP_COMMAND		8	/* Control register */
#define  PCI_AGP_COMMAND_RQ_MASK 0xff000000  /* Master: Maximum number of requests */
#define  PCI_AGP_COMMAND_SBA	0x0200	/* Sideband addressing enabled */
#define  PCI_AGP_COMMAND_AGP	0x0100	/* Allow processing of AGP transactions */
#define  PCI_AGP_COMMAND_64BIT	0x0020 	/* Allow processing of 64-bit addresses */
#define  PCI_AGP_COMMAND_FW	0x0010 	/* Force FW transfers */
#define  PCI_AGP_COMMAND_RATE4	0x0004	/* Use 4x rate */
#define  PCI_AGP_COMMAND_RATE2	0x0002	/* Use 4x rate */
#define  PCI_AGP_COMMAND_RATE1	0x0001	/* Use 4x rate */
#define PCI_AGP_SIZEOF		12

/* Slot Identification */

#define PCI_SID_ESR		2	/* Expansion Slot Register */
#define  PCI_SID_ESR_NSLOTS	0x1f	/* Number of expansion slots available */
#define  PCI_SID_ESR_FIC	0x20	/* First In Chassis Flag */
#define PCI_SID_CHASSIS_NR	3	/* Chassis Number */

/* Message Signaled Interrupts registers */

#define PCI_MSI_FLAGS		2	/* Various flags */
#define  PCI_MSI_FLAGS_64BIT	0x80	/* 64-bit addresses allowed */
#define  PCI_MSI_FLAGS_QSIZE	0x70	/* Message queue size configured */
#define  PCI_MSI_FLAGS_QMASK	0x0e	/* Maximum queue size available */
#define  PCI_MSI_FLAGS_ENABLE	0x01	/* MSI feature enabled */
#define PCI_MSI_RFU		3	/* Rest of capability flags */
#define PCI_MSI_ADDRESS_LO	4	/* Lower 32 bits */
#define PCI_MSI_ADDRESS_HI	8	/* Upper 32 bits (if PCI_MSI_FLAGS_64BIT set) */
#define PCI_MSI_DATA_32		8	/* 16 bits of data for 32-bit devices */
#define PCI_MSI_DATA_64		12	/* 16 bits of data for 64-bit devices */
#define PCI_MSI_MASK_BIT	16	/* Mask bits register */

/* CompactPCI Hotswap Register */

#define PCI_CHSWP_CSR		2	/* Control and Status Register */
#define  PCI_CHSWP_DHA		0x01	/* Device Hiding Arm */
#define  PCI_CHSWP_EIM		0x02	/* ENUM# Signal Mask */
#define  PCI_CHSWP_PIE		0x04	/* Pending Insert or Extract */
#define  PCI_CHSWP_LOO		0x08	/* LED On / Off */
#define  PCI_CHSWP_PI		0x30	/* Programming Interface */
#define  PCI_CHSWP_EXT		0x40	/* ENUM# status - extraction */
#define  PCI_CHSWP_INS		0x80	/* ENUM# status - insertion */

/* PCI-X registers */

#define PCI_X_CMD		2	/* Modes & Features */
#define  PCI_X_CMD_DPERR_E	0x0001	/* Data Parity Error Recovery Enable */
#define  PCI_X_CMD_ERO		0x0002	/* Enable Relaxed Ordering */
#define  PCI_X_CMD_MAX_READ	0x000c	/* Max Memory Read Byte Count */
#define  PCI_X_CMD_MAX_SPLIT	0x0070	/* Max Outstanding Split Transactions */
#define  PCI_X_CMD_VERSION(x) 	(((x) >> 12) & 3) /* Version */
#define PCI_X_STATUS		4	/* PCI-X capabilities */
#define  PCI_X_STATUS_DEVFN	0x000000ff	/* A copy of devfn */
#define  PCI_X_STATUS_BUS	0x0000ff00	/* A copy of bus nr */
#define  PCI_X_STATUS_64BIT	0x00010000	/* 64-bit device */
#define  PCI_X_STATUS_133MHZ	0x00020000	/* 133 MHz capable */
#define  PCI_X_STATUS_SPL_DISC	0x00040000	/* Split Completion Discarded */
#define  PCI_X_STATUS_UNX_SPL	0x00080000	/* Unexpected Split Completion */
#define  PCI_X_STATUS_COMPLEX	0x00100000	/* Device Complexity */
#define  PCI_X_STATUS_MAX_READ	0x00600000	/* Designed Max Memory Read Count */
#define  PCI_X_STATUS_MAX_SPLIT	0x03800000	/* Designed Max Outstanding Split Transactions */
#define  PCI_X_STATUS_MAX_CUM	0x1c000000	/* Designed Max Cumulative Read Size */
#define  PCI_X_STATUS_SPL_ERR	0x20000000	/* Rcvd Split Completion Error Msg */
#define  PCI_X_STATUS_266MHZ	0x40000000	/* 266 MHz capable */
#define  PCI_X_STATUS_533MHZ	0x80000000	/* 533 MHz capable */

/* PCI-X bridge registers */
#define PCI_X_SEC_STATUS	2	/* Secondary status */
#define  PCI_X_SSTATUS_64BIT	0x0001	/* The bus behind the bridge is 64bits wide */
#define  PCI_X_SSTATUS_133MHZ	0x0002	/* The bus behind the bridge is 133Mhz Capable */
#define  PCI_X_SSTATUS_SPL_DISC 0x0004	/* Split Completion Discarded */
#define  PCI_X_SSTATUS_UNX_SPL	0x0008	/* Unexpected Split Completion */
#define  PCI_X_SSTATUS_SPL_OVR	0x0010	/* Split Completion Overrun */
#define  PCI_X_SSTATUS_SPL_DLY	0x0020	/* Split Completion Delayed */
#define  PCI_X_SSTATUS_MFREQ(x) (((x) & 0x03c0) >> 6)	/* PCI-X mode and frequency */
#define   PCI_X_SSTATUS_CONVENTIONAL_PCI	0x0
#define   PCI_X_SSTATUS_MODE1_66MHZ	0x1
#define   PCI_X_SSTATUS_MODE1_100MHZ	0x2
#define   PCI_X_SSTATUS_MODE1_133MHZ	0x3
#define   PCI_X_SSTATUS_MODE2_266MHZ_REF_66MHZ	0x9
#define   PCI_X_SSTATUS_MODE2_266MHZ_REF_100MHZ	0xa
#define   PCI_X_SSTATUS_MODE2_266MHZ_REF_133MHZ	0xb
#define   PCI_X_SSTATUS_MODE2_533MHZ_REF_66MHZ	0xd
#define   PCI_X_SSTATUS_MODE2_533MHZ_REF_100MHZ	0xe
#define   PCI_X_SSTATUS_MODE2_533MHZ_REF_133MHZ	0xf
#define  PCI_X_SSTATUS_VERSION(x)	(((x) >> 12) & 3) /* Version */
#define  PCI_X_SSTATUS_266MHZ	0x4000	/* The bus behind the bridge is 266Mhz Capable */
#define  PCI_X_SSTAUTS_533MHZ	0x8000	/* The bus behind the bridge is 533Mhz Capable */

/* PCI Express capability registers */

#define PCI_EXP_FLAGS		2	/* Capabilities register */
#define PCI_EXP_FLAGS_VERS	0x000f	/* Capability version */
#define PCI_EXP_FLAGS_TYPE	0x00f0	/* Device/Port type */
#define  PCI_EXP_TYPE_ENDPOINT	0x0	/* Express Endpoint */
#define  PCI_EXP_TYPE_LEG_END	0x1	/* Legacy Endpoint */
#define  PCI_EXP_TYPE_ROOT_PORT 0x4	/* Root Port */
#define  PCI_EXP_TYPE_UPSTREAM	0x5	/* Upstream Port */
#define  PCI_EXP_TYPE_DOWNSTREAM 0x6	/* Downstream Port */
#define  PCI_EXP_TYPE_PCI_BRIDGE 0x7	/* PCI/PCI-X Bridge */
#define PCI_EXP_FLAGS_SLOT	0x0100	/* Slot implemented */
#define PCI_EXP_FLAGS_IRQ	0x3e00	/* Interrupt message number */
#define PCI_EXP_DEVCAP		4	/* Device capabilities */
#define  PCI_EXP_DEVCAP_PAYLOAD	0x07	/* Max_Payload_Size */
#define  PCI_EXP_DEVCAP_PHANTOM	0x18	/* Phantom functions */
#define  PCI_EXP_DEVCAP_EXT_TAG	0x20	/* Extended tags */
#define  PCI_EXP_DEVCAP_L0S	0x1c0	/* L0s Acceptable Latency */
#define  PCI_EXP_DEVCAP_L1	0xe00	/* L1 Acceptable Latency */
#define  PCI_EXP_DEVCAP_ATN_BUT	0x1000	/* Attention Button Present */
#define  PCI_EXP_DEVCAP_ATN_IND	0x2000	/* Attention Indicator Present */
#define  PCI_EXP_DEVCAP_PWR_IND	0x4000	/* Power Indicator Present */
#define  PCI_EXP_DEVCAP_RBER	0x8000  /* Role-Based Error Reporting */
#define  PCI_EXP_DEVCAP_PWR_VAL	0x3fc0000 /* Slot Power Limit Value */
#define  PCI_EXP_DEVCAP_PWR_SCL	0xc000000 /* Slot Power Limit Scale */
#define PCI_EXP_DEVCTL		8	/* Device Control */
#define  PCI_EXP_DEVCTL_CERE	0x0001	/* Correctable Error Reporting En. */
#define  PCI_EXP_DEVCTL_NFERE	0x0002	/* Non-Fatal Error Reporting Enable */
#define  PCI_EXP_DEVCTL_FERE	0x0004	/* Fatal Error Reporting Enable */
#define  PCI_EXP_DEVCTL_URRE	0x0008	/* Unsupported Request Reporting En. */
#define  PCI_EXP_DEVCTL_RELAX_EN 0x0010 /* Enable relaxed ordering */
#define  PCI_EXP_DEVCTL_PAYLOAD	0x00e0	/* Max_Payload_Size */
#define  PCI_EXP_DEVCTL_EXT_TAG	0x0100	/* Extended Tag Field Enable */
#define  PCI_EXP_DEVCTL_PHANTOM	0x0200	/* Phantom Functions Enable */
#define  PCI_EXP_DEVCTL_AUX_PME	0x0400	/* Auxiliary Power PM Enable */
#define  PCI_EXP_DEVCTL_NOSNOOP_EN 0x0800  /* Enable No Snoop */
#define  PCI_EXP_DEVCTL_READRQ	0x7000	/* Max_Read_Request_Size */
#define PCI_EXP_DEVSTA		10	/* Device Status */
#define  PCI_EXP_DEVSTA_CED	0x01	/* Correctable Error Detected */
#define  PCI_EXP_DEVSTA_NFED	0x02	/* Non-Fatal Error Detected */
#define  PCI_EXP_DEVSTA_FED	0x04	/* Fatal Error Detected */
#define  PCI_EXP_DEVSTA_URD	0x08	/* Unsupported Request Detected */
#define  PCI_EXP_DEVSTA_AUXPD	0x10	/* AUX Power Detected */
#define  PCI_EXP_DEVSTA_TRPND	0x20	/* Transactions Pending */
#define PCI_EXP_LNKCAP		12	/* Link Capabilities */
#define  PCI_EXP_LNKCAP_ASPMS	0xc00	/* ASPM Support */
#define  PCI_EXP_LNKCAP_L0SEL	0x7000	/* L0s Exit Latency */
#define  PCI_EXP_LNKCAP_L1EL	0x38000	/* L1 Exit Latency */
#define PCI_EXP_LNKCTL		16	/* Link Control */
#define  PCI_EXP_LNKCTL_RL	0x20	/* Retrain Link */
#define  PCI_EXP_LNKCTL_CCC	0x40	/* Common Clock COnfiguration */
#define PCI_EXP_LNKSTA		18	/* Link Status */
#define  PCI_EXP_LNKSTA_LT	0x800	/* Link Training */
#define  PCI_EXP_LNKSTA_SLC	0x1000	/* Slot Clock Configuration */
#define PCI_EXP_SLTCAP		20	/* Slot Capabilities */
#define PCI_EXP_SLTCTL		24	/* Slot Control */
#define PCI_EXP_SLTSTA		26	/* Slot Status */
#define PCI_EXP_RTCTL		28	/* Root Control */
#define  PCI_EXP_RTCTL_SECEE	0x01	/* System Error on Correctable Error */
#define  PCI_EXP_RTCTL_SENFEE	0x02	/* System Error on Non-Fatal Error */
#define  PCI_EXP_RTCTL_SEFEE	0x04	/* System Error on Fatal Error */
#define  PCI_EXP_RTCTL_PMEIE	0x08	/* PME Interrupt Enable */
#define  PCI_EXP_RTCTL_CRSSVE	0x10	/* CRS Software Visibility Enable */
#define PCI_EXP_RTCAP		30	/* Root Capabilities */
#define PCI_EXP_RTSTA		32	/* Root Status */

/* Extended Capabilities (PCI-X 2.0 and Express) */
#define PCI_EXT_CAP_ID(header)		(header & 0x0000ffff)
#define PCI_EXT_CAP_VER(header)		((header >> 16) & 0xf)
#define PCI_EXT_CAP_NEXT(header)	((header >> 20) & 0xffc)

#define PCI_EXT_CAP_ID_ERR	1
#define PCI_EXT_CAP_ID_VC	2
#define PCI_EXT_CAP_ID_DSN	3
#define PCI_EXT_CAP_ID_PWR	4

/* Advanced Error Reporting */
#define PCI_ERR_UNCOR_STATUS	4	/* Uncorrectable Error Status */
#define  PCI_ERR_UNC_TRAIN	0x00000001	/* Training */
#define  PCI_ERR_UNC_DLP	0x00000010	/* Data Link Protocol */
#define  PCI_ERR_UNC_POISON_TLP	0x00001000	/* Poisoned TLP */
#define  PCI_ERR_UNC_FCP	0x00002000	/* Flow Control Protocol */
#define  PCI_ERR_UNC_COMP_TIME	0x00004000	/* Completion Timeout */
#define  PCI_ERR_UNC_COMP_ABORT	0x00008000	/* Completer Abort */
#define  PCI_ERR_UNC_UNX_COMP	0x00010000	/* Unexpected Completion */
#define  PCI_ERR_UNC_RX_OVER	0x00020000	/* Receiver Overflow */
#define  PCI_ERR_UNC_MALF_TLP	0x00040000	/* Malformed TLP */
#define  PCI_ERR_UNC_ECRC	0x00080000	/* ECRC Error Status */
#define  PCI_ERR_UNC_UNSUP	0x00100000	/* Unsupported Request */
#define PCI_ERR_UNCOR_MASK	8	/* Uncorrectable Error Mask */
	/* Same bits as above */
#define PCI_ERR_UNCOR_SEVER	12	/* Uncorrectable Error Severity */
	/* Same bits as above */
#define PCI_ERR_COR_STATUS	16	/* Correctable Error Status */
#define  PCI_ERR_COR_RCVR	0x00000001	/* Receiver Error Status */
#define  PCI_ERR_COR_BAD_TLP	0x00000040	/* Bad TLP Status */
#define  PCI_ERR_COR_BAD_DLLP	0x00000080	/* Bad DLLP Status */
#define  PCI_ERR_COR_REP_ROLL	0x00000100	/* REPLAY_NUM Rollover */
#define  PCI_ERR_COR_REP_TIMER	0x00001000	/* Replay Timer Timeout */
#define PCI_ERR_COR_MASK	20	/* Correctable Error Mask */
	/* Same bits as above */
#define PCI_ERR_CAP		24	/* Advanced Error Capabilities */
#define  PCI_ERR_CAP_FEP(x)	((x) & 31)	/* First Error Pointer */
#define  PCI_ERR_CAP_ECRC_GENC	0x00000020	/* ECRC Generation Capable */
#define  PCI_ERR_CAP_ECRC_GENE	0x00000040	/* ECRC Generation Enable */
#define  PCI_ERR_CAP_ECRC_CHKC	0x00000080	/* ECRC Check Capable */
#define  PCI_ERR_CAP_ECRC_CHKE	0x00000100	/* ECRC Check Enable */
#define PCI_ERR_HEADER_LOG	28	/* Header Log Register (16 bytes) */
#define PCI_ERR_ROOT_COMMAND	44	/* Root Error Command */
#define PCI_ERR_ROOT_STATUS	48
#define PCI_ERR_ROOT_COR_SRC	52
#define PCI_ERR_ROOT_SRC	54

/* Virtual Channel */
#define PCI_VC_PORT_REG1	4
#define PCI_VC_PORT_REG2	8
#define PCI_VC_PORT_CTRL	12
#define PCI_VC_PORT_STATUS	14
#define PCI_VC_RES_CAP		16
#define PCI_VC_RES_CTRL		20
#define PCI_VC_RES_STATUS	26

/* Power Budgeting */
#define PCI_PWR_DSR		4	/* Data Select Register */
#define PCI_PWR_DATA		8	/* Data Register */
#define  PCI_PWR_DATA_BASE(x)	((x) & 0xff)	    /* Base Power */
#define  PCI_PWR_DATA_SCALE(x)	(((x) >> 8) & 3)    /* Data Scale */
#define  PCI_PWR_DATA_PM_SUB(x)	(((x) >> 10) & 7)   /* PM Sub State */
#define  PCI_PWR_DATA_PM_STATE(x) (((x) >> 13) & 3) /* PM State */
#define  PCI_PWR_DATA_TYPE(x)	(((x) >> 15) & 7)   /* Type */
#define  PCI_PWR_DATA_RAIL(x)	(((x) >> 18) & 7)   /* Power Rail */
#define PCI_PWR_CAP		12	/* Capability */
#define  PCI_PWR_CAP_BUDGET(x)	((x) & 1)	/* Included in system budget */


/*
 * The PCI interface treats multi-function devices as independent
 * devices.  The slot/function address of each device is encoded
 * in a single byte as follows:
 *
 *	7:3 = slot
 *	2:0 = function
 */
#define PCI_DEVFN(slot,func)	((((slot) & 0x1f) << 3) | ((func) & 0x07))
#define PCI_SLOT(devfn)		(((devfn) >> 3) & 0x1f)
#define PCI_FUNC(devfn)		((devfn) & 0x07)
#define PCI_BDF(bus,dev,func)	((bus) << 16 | (dev) << 11 | (func) << 8)

/* Translation from PCI_DEV() to devicetree bus and path.pci.devfn. */
#define PCI_DEV2DEVFN(sdev)		(((sdev)>>12) & 0xff)
#define PCI_DEV2SEGBUS(sdev)	(((sdev)>>20) & 0xfff)

#endif /* PCI_DEF_H */
