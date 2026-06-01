/* inteltool - dump all registers on an Intel CPU + chipset based system */
/* SPDX-License-Identifier: GPL-2.0-only */

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "inteltool.h"

/* 320766 */
static const io_register_t nehalem_dmi_registers[] = {
	{ 0x00, 4, "DMIVCH" },		// DMI Virtual Channel Capability Header
	{ 0x04, 4, "DMIVCCAP1" },	// DMI Port VC Capability Register 1
	{ 0x08, 4, "DMIVCCAP2" },	// DMI Port VC Capability Register 2
	{ 0x0C, 4, "DMIVCCTL" },	// DMI Port VC Control
	{ 0x10, 4, "DMIVC0RCAP" },	// DMI VC0 Resource Capability
	{ 0x14, 4, "DMIVC0RCTL" },	// DMI VC0 Resource Control
/*	{ 0x18, 2, "RSVD" }, // Reserved */
	{ 0x1A, 2, "DMIVC0RSTS" },	// DMI VC0 Resource Status
	{ 0x1C, 4, "DMIVC1RCAP" },	// DMI VC1 Resource Capability
	{ 0x20, 4, "DMIVC1RCTL" },	// DMI VC1 Resource Control
/*	{ 0x24, 2, "RSVD" }, // Reserved */
	{ 0x26, 2, "DMIVC1RSTS" },	// DMI VC1 Resource Status
/*	... - Reserved */
	{ 0x84, 4, "DMILCAP" },		// DMI Link Capabilities
	{ 0x88, 2, "DMILCTL" },		// DMI Link Control
	{ 0x8A, 2, "DMILSTS" },		// DMI Link Status
/*	... - Reserved */
};

/* 322812 */
static const io_register_t westmere_dmi_registers[] = {
	{ 0x00, 4, "DMIVCECH" },	// DMI Virtual Channel Enhanced Capability
	{ 0x04, 4, "DMIPVCCAP1" },	// DMI Port VC Capability Register 1
	{ 0x08, 4, "DMIPVCCAP2" },	// DMI Port VC Capability Register 2
	{ 0x0C, 2, "DMIPVCCTL" },	// DMI Port VC Control
/*	{ 0x0E, 2, "RSVD" }, // Reserved */
	{ 0x10, 4, "DMIVC0RCAP" },	// DMI VC0 Resource Capability
	{ 0x14, 4, "DMIVC0RCTL" },	// DMI VC0 Resource Control
/*	{ 0x18, 2, "RSVD" }, // Reserved */
	{ 0x1A, 2, "DMIVC0RSTS" },	// DMI VC0 Resource Status
	{ 0x1C, 4, "DMIVC1RCAP" },	// DMI VC1 Resource Capability
	{ 0x20, 4, "DMIVC1RCTL1" },	// DMI VC1 Resource Control
/*	{ 0x24, 2, "RSVD" },	// Reserved */
	{ 0x26, 2, "DMIC1RSTS" },	// DMI VC1 Resource Status
/*	... - Reserved */
	{ 0x84, 4, "DMILCAP" },		// DMI Link Capabilities
	{ 0x88, 2, "DMILCTL" },		// DMI Link Control
	{ 0x8A, 2, "DMILSTS" },		// DMI Link Status
/*	... - Reserved */
};

static const io_register_t sandybridge_dmi_registers[] = {
	{ 0x00, 4, "DMI VCECH" }, // DMI Virtual Channel Enhanced Capability
	{ 0x04, 4, "DMI PVCCAP1" }, // DMI Port VC Capability Register 1
	{ 0x08, 4, "DMI PVVAP2" }, // DMI Port VC Capability Register 2
	{ 0x0C, 2, "DMI PVCCTL" }, // DMI Port VC Control
/*	{ 0x0E, 2, "RSVD" }, // Reserved */
	{ 0x10, 4, "DMI VC0RCAP" }, // DMI VC0 Resource Capability
	{ 0x14, 4, "DMI VC0RCTL" }, // DMI VC0 Resource Control
/*	{ 0x18, 2, "RSVD" }, // Reserved */
	{ 0x1A, 2, "DMI VC0RSTS" }, // DMI VC0 Resource Status
	{ 0x1C, 4, "DMI VC1RCAP" }, // DMI VC1 Resource Capability
	{ 0x20, 4, "DMI VC1RCTL" }, // DMI VC1 Resource Control
/*	{ 0x24, 2, "RSVD" }, // Reserved */
	{ 0x26, 2, "DMI VC1RSTS" }, // DMI VC1 Resource Status
	{ 0x28, 4, "DMI VCPRCAP" }, // DMI VCp Resource Capability
	{ 0x2C, 4, "DMI VCPRCTL" }, // DMI VCp Resource Control
/*	{ 0x30, 2, "RSVD" }, // Reserved */
	{ 0x32, 2, "DMI VCPRSTS" }, // DMI VCp Resource Status
	{ 0x34, 4, "DMI VCMRCAP" }, // DMI VCm Resource Capability
	{ 0x38, 4, "DMI VCMRCTL" }, // DMI VCm Resource Control
/*	{ 0x3C, 2, "RSVD" }, // Reserved */
	{ 0x3E, 2, "DMI VCMRSTS" }, // DMI VCm Resource Status
/*	{ 0x40, 4, "RSVD" }, // Reserved */
	{ 0x44, 4, "DMI ESC" }, // DMI Element Self Description
/*	{ 0x48, 8, "RSVD" }, // Reserved */
	{ 0x50, 4, "DMI LE1D" }, // DMI Link Entry 1 Description
/*	{ 0x54, 4, "RSVD" }, // Reserved */
	{ 0x58, 4, "DMI LE1A" }, // DMI Link Entry 1 Address
	{ 0x5C, 4, "DMI LUE1A" }, // DMI Link Upper Entry 1 Address
	{ 0x60, 4, "DMI LE2D" }, // DMI Link Entry 2 Description
/*	{ 0x64, 4, "RSVD" }, // Reserved */
	{ 0x68, 4, "DMI LE2A" }, // DMI Link Entry 2 Address
/*	{ 0x6C, 4, "RSVD" }, // Reserved
	{ 0x70, 8, "RSVD" }, // Reserved
	{ 0x78, 8, "RSVD" }, // Reserved
	{ 0x80, 4, "RSVD" }, // Reserved */
	{ 0x84, 4, "LCAP" }, // Link Capabilities
	{ 0x88, 2, "LCTL" }, // Link Control
	{ 0x8A, 2, "LSTS" }, // Link Status
/*	{ 0x8C, 4, "RSVD" }, // Reserved
	{ 0x90, 4, "RSVD" }, // Reserved
	{ 0x94, 4, "RSVD" }, // Reserved */
	{ 0x98, 2, "LCTL2" }, // Link Control 2
	{ 0x9A, 2, "LSTS2" }, // Link Status 2
/*	... - Reserved */
	{ 0xBC0, 4, "AFE_BMUF0" }, // AFE BMU Configuration Function 0
	{ 0xBC4, 4, "RSVD" }, // Reserved
	{ 0xBC8, 4, "RSVD" }, // Reserved
	{ 0xBCC, 4, "AFE_BMUT0" }, // AFE BMU Configuration Test 0
/*	... - Reserved */
};

/*
 * All Haswell DMI Registers per
 *
 * Mobile 4th Generation Intel Core TM Processor Family, Mobile Intel Pentium Processor Family,
 * and Mobile Intel Celeron Processor Family
 * Datasheet Volume 2
 * 329002-002
 */
static const io_register_t haswell_ult_dmi_registers[] = {
	{ 0x00, 4, "DMIVCECH" }, // DMI Virtual Channel Enhanced Capability
	{ 0x04, 4, "DMIPVCCAP1" }, // DMI Port VC Capability Register 1
	{ 0x08, 4, "DMIPVCCAP2" }, // DMI Port VC Capability Register 2
	{ 0x0C, 2, "DMI PVCCTL" }, // DMI Port VC Control
/*	{ 0x0E, 2, "RSVD" }, // Reserved */
	{ 0x10, 4, "DMIVC0RCAP" }, // DMI VC0 Resource Capability
	{ 0x14, 4, "DMIVC0RCTL" }, // DMI VC0 Resource Control
/*	{ 0x18, 2, "RSVD" }, // Reserved */
	{ 0x1A, 2, "DMIVC0RSTS" }, // DMI VC0 Resource Status
	{ 0x1C, 4, "DMIVC1RCAP" }, // DMI VC1 Resource Capability
	{ 0x20, 4, "DMIVC1RCTL" }, // DMI VC1 Resource Control
/*	{ 0x24, 2, "RSVD" }, // Reserved */
	{ 0x26, 2, "DMIVC1RSTS" }, // DMI VC1 Resource Status
	{ 0x28, 4, "DMIVCPRCAP" }, // DMI VCp Resource Capability
	{ 0x2C, 4, "DMIVCPRCTL" }, // DMI VCp Resource Control
/*	{ 0x30, 2, "RSVD" }, // Reserved */
	{ 0x32, 2, "DMIVCPRSTS" }, // DMI VCp Resource Status
	{ 0x34, 4, "DMIVCMRCAP" }, // DMI VCm Resource Capability
	{ 0x38, 4, "DMIVCMRCTL" }, // DMI VCm Resource Control
/*	{ 0x3C, 2, "RSVD" }, // Reserved */
	{ 0x3E, 2, "DMIVCMRSTS" }, // DMI VCm Resource Status
	{ 0x40, 4, "DMIRCLDECH" }, // DMI Root Complex Link Declaration */
	{ 0x44, 4, "DMIESD" }, // DMI Element Self Description
/*	{ 0x48, 4, "RSVD" }, // Reserved */
/*	{ 0x4C, 4, "RSVD" }, // Reserved */
	{ 0x50, 4, "DMILE1D" }, // DMI Link Entry 1 Description
/*	{ 0x54, 4, "RSVD" }, // Reserved */
	{ 0x58, 4, "DMILE1A" }, // DMI Link Entry 1 Address
	{ 0x5C, 4, "DMILUE1A" }, // DMI Link Upper Entry 1 Address
	{ 0x60, 4, "DMILE2D" }, // DMI Link Entry 2 Description
/*	{ 0x64, 4, "RSVD" }, // Reserved */
	{ 0x68, 4, "DMILE2A" }, // DMI Link Entry 2 Address
/*	{ 0x6C, 4, "RSVD" }, // Reserved */
/*	{ 0x70, 4, "RSVD" }, // Reserved */
/*	{ 0x74, 4, "RSVD" }, // Reserved */
/*	{ 0x78, 4, "RSVD" }, // Reserved */
/*	{ 0x7C, 4, "RSVD" }, // Reserved */
/*	{ 0x80, 4, "RSVD" }, // Reserved */
/*	{ 0x84, 4, "RSVD" }, // Reserved */
	{ 0x88, 2, "LCTL" }, // Link Control
	/*  ... - Reserved */
	{ 0x1C4, 4, "DMIUESTS" }, // DMI Uncorrectable Error Status
	{ 0x1C8, 4, "DMIUEMSK" }, // DMI Uncorrectable Error Mask
	{ 0x1D0, 4, "DMICESTS" }, // DMI Correctable Error Status
	{ 0x1D4, 4, "DMICEMSK" }, // DMI Correctable Error Mask
/*  ... - Reserved */
};

/*
 * All Skylake-S/H DMI Registers per
 *
 * 6th Generation Intel Processor Families for S-Platform Volume 2 of 2
 * Page 117
 * 332688-003E
 *
 * 6th Generation Intel Processor Families for H-Platform Volume 2 of 2
 * Page 117
 * 332987-002EN
 */
static const io_register_t skylake_dmi_registers[] = {
	{ 0x00, 4, "DMIVCECH"   }, // DMI Virtual Channel Enhanced Capability
	{ 0x04, 4, "DMIPVCCAP1" }, // DMI Port VC Capability Register 1
	{ 0x08, 4, "DMIPVCCAP2" }, // DMI Port VC Capability Register 2
	{ 0x0C, 2, "DMIPVCCTL"  }, // DMI Port VC Control
	{ 0x10, 4, "DMIVC0RCAP" }, // DMI VC0 Resource Capability
	{ 0x14, 4, "DMIVC0RCTL" }, // DMI VC0 Resource Control
	{ 0x1A, 2, "DMIVC0RSTS" }, // DMI VC0 Resource Status
	{ 0x1C, 4, "DMIVC1RCAP" }, // DMI VC1 Resource Capability
	{ 0x20, 4, "DMIVC1RCTL" }, // DMI VC1 Resource Control
	{ 0x26, 2, "DMIVC1RSTS" }, // DMI VC1 Resource Status
	{ 0x34, 4, "DMIVCMRCAP" }, // DMI VCm Resource Capability
	{ 0x38, 4, "DMIVCMRCTL" }, // DMI VCm Resource Control
	{ 0x3E, 2, "DMIVCMRSTS" }, // DMI VCm Resource Status
	{ 0x40, 4, "DMIRCLDECH" }, // DMI Root Complex Link Declaration */
	{ 0x44, 4, "DMIESD"     }, // DMI Element Self Description
	{ 0x50, 4, "DMILE1D"    }, // DMI Link Entry 1 Description
	{ 0x58, 4, "DMILE1A"    }, // DMI Link Entry 1 Address
	{ 0x5C, 4, "DMILUE1A"   }, // DMI Link Upper Entry 1 Address
	{ 0x60, 4, "DMILE2D"    }, // DMI Link Entry 2 Description
	{ 0x68, 4, "DMILE2A"    }, // DMI Link Entry 2 Address
	{ 0x84, 4, "LCAP"       }, // Link Capabilities
	{ 0x88, 2, "LCTL"       }, // Link Control
	{ 0x8A, 2, "LSTS"       }, // DMI Link Status
	{ 0x98, 2, "LCTL2"      }, // Link Control 2
	{ 0x9A, 2, "LSTS2"      }, // DMI Link Status 2
	{ 0x1C4, 4, "DMIUESTS"  }, // DMI Uncorrectable Error Status
	{ 0x1C8, 4, "DMIUEMSK"  }, // DMI Uncorrectable Error Mask
	{ 0x1CC, 4, "DMIUESEV"  }, // DMI Uncorrectable Error Mask
	{ 0x1D0, 4, "DMICESTS"  }, // DMI Correctable Error Status
	{ 0x1D4, 4, "DMICEMSK"  }, // DMI Correctable Error Mask
};

static const io_register_t alderlake_dmi_registers[] = {
	{ 0x00, 4, "DMIVCECH"   }, // DMI Virtual Channel Enhanced Capability
	{ 0x04, 4, "DMIPVCCAP1" }, // DMI Port VC Capability Register 1
	{ 0x08, 4, "DMIPVCCAP2" }, // DMI Port VC Capability Register 2
	{ 0x0C, 2, "DMIPVCCTL"  }, // DMI Port VC Control
	{ 0x10, 4, "DMIVC0RCAP" }, // DMI VC0 Resource Capability
	{ 0x1C, 4, "DMIVC1RCAP" }, // DMI VC1 Resource Capability
	{ 0x26, 2, "DMIVC1RSTS" }, // DMI VC1 Resource Status
	{ 0x34, 4, "DMIVCMRCAP" }, // DMI VCm Resource Capability
	{ 0x38, 4, "DMIVCMRCTL" }, // DMI VCm Resource Control
	{ 0x3E, 2, "DMIVCMRSTS" }, // DMI VCm Resource Status
	{ 0x40, 4, "DMIRCLDECH" }, // DMI Root Complex Link Declaration */
	{ 0x44, 4, "DMIESD"     }, // DMI Element Self Description
	{ 0x50, 4, "DMILE1D"    }, // DMI Link Entry 1 Description
	{ 0x5C, 4, "DMILUE1A"   }, // DMI Link Upper Entry 1 Address
	{ 0x60, 4, "DMILE2D"    }, // DMI Link Entry 2 Description
	{ 0x68, 4, "DMILE2A"    }, // DMI Link Entry 2 Address
	{ 0x88, 2, "LCTL"       }, // Link Control
	{ 0x1C4, 4, "DMIUESTS"  }, // DMI Uncorrectable Error Status
	{ 0x1C8, 4, "DMIUEMSK"  }, // DMI Uncorrectable Error Mask
	{ 0x1CC, 4, "DMIUESEV"  }, // DMI Uncorrectable Error Mask
	{ 0x1D0, 4, "DMICESTS"  }, // DMI Correctable Error Status
	{ 0x1D4, 4, "DMICEMSK"  }, // DMI Correctable Error Mask
};

/* 10th Generation Intel® Core™ Processors, Datasheet Volume 2 of 2, ID 615211 */
static const io_register_t cometlake_dmi_registers[] = {
	{ 0x00, 4, "DMIVCECH"    }, // DMI Virtual Channel Enhanced Capability
	{ 0x04, 4, "DMIPVCCAP1"  }, // DMI Port VC Capability Register 1
	{ 0x08, 4, "DMIPVCCAP2"  }, // DMI Port VC Capability Register 2
	{ 0x0C, 2, "DMIPVCCTL"   }, // DMI Port VC Control
	{ 0x10, 4, "DMIVC0RCAP"  }, // DMI VC0 Resource Capability
	{ 0x14, 4, "DMIVC0RCTL"  }, // DMI VC0 Resource Control
	{ 0x1A, 2, "DMIVC0RSTS"  }, // DMI VC0 Resource Status
	{ 0x1C, 4, "DMIVC1RCAP"  }, // DMI VC1 Resource Capability
	{ 0x20, 4, "DMIVC1RCTL"  }, // DMI VC1 Resource Control
	{ 0x26, 2, "DMIVC1RSTS"  }, // DMI VC1 Resource Status
	{ 0x34, 4, "DMIVCMRCAP"  }, // DMI VCm Resource Capability
	{ 0x38, 4, "DMIVCMRCTL"  }, // DMI VCm Resource Control
	{ 0x3E, 2, "DMIVCMRSTS"  }, // DMI VCm Resource Status
	{ 0x40, 4, "DMIRCLDECH"  }, // DMI Root Complex Link Declaration
	{ 0x44, 4, "DMIESD"      }, // DMI Element Self Description
	{ 0x50, 4, "DMILE1D"     }, // DMI Link Entry 1 Description
	{ 0x58, 4, "DMILE1A"     }, // DMI Link Entry 1 Address
	{ 0x5C, 4, "DMILUE1A"    }, // DMI Link Upper Entry 1 Address
	{ 0x60, 4, "DMILE2D"     }, // DMI Link Entry 2 Description
	{ 0x68, 4, "DMILE2A"     }, // DMI Link Entry 2 Address
	{ 0x84, 4, "LCAP"        }, // Link Capabilities
	{ 0x88, 2, "LCTL"        }, // Link Control
	{ 0x8A, 2, "LSTS"        }, // DMI Link Status
	{ 0x98, 2, "LCTL2"       }, // Link Control 2
	{ 0x9A, 2, "LSTS2"       }, // Link Status 2
};

static const io_register_t raptorlake_dmi_registers[] = {
	{ 0x000, 4, "ID"          }, // Device Identifiers
	{ 0x004, 2, "CMD"         }, // Device Command
	{ 0x006, 2, "PSTS"        }, // Primary Status
	{ 0x008, 4, "RID_CC"      }, // Revision ID
	{ 0x00e, 1, "HTYPE"       }, // Header Type
	{ 0x01e, 2, "SSTS"        }, // Secondary Status
	{ 0x02c, 4, "SVD"         }, // Subsystem Vendor IDs
	{ 0x034, 1, "CAPP"        }, // Capabilities List Pointer
	{ 0x03e, 1, "BCTRL"       }, // Bridge Control
	{ 0x044, 1, "DCAP"        }, // Device Capabilities
	{ 0x048, 2, "DCTL"        }, // Device Control
	{ 0x04a, 2, "DSTS"        }, // Device Status
	{ 0x04c, 4, "LCAP"        }, // Link Capabilities
	{ 0x050, 2, "LCTL"        }, // Link Control
	{ 0x052, 2, "LSTS"        }, // Link Status
	{ 0x05c, 2, "RCTL"        }, // Root Control
	{ 0x060, 4, "RSTS"        }, // Root Status
	{ 0x064, 4, "DCAP2"       }, // Device Capabilities 2
	{ 0x068, 2, "DCTL2"       }, // Device Control 2
	{ 0x06a, 2, "DSTS2"       }, // Device Status 2
	{ 0x06c, 4, "LCAP2"       }, // Link Capabilities 2
	{ 0x070, 2, "LCTL2"       }, // Link Control 2
	{ 0x072, 2, "LSTS2"       }, // Link Status 2
	{ 0x074, 4, "SLCAP2"      }, // Slot Capabilities 2
	{ 0x078, 2, "SLCTL2"      }, // Slot Control 2
	{ 0x07a, 2, "SLSTS2"      }, // Slot Status 2
	{ 0x080, 2, "MID"         }, // Message Signaled Interrupt Identifiers
	{ 0x082, 2, "MC"          }, // Message Signaled Interrupt Message
	{ 0x084, 4, "MA"          }, // Message Signaled Interrupt Message Address
	{ 0x088, 2, "MD"          }, // Message Signaled Interrupt Message Data
	{ 0x090, 2, "SVCAP"       }, // Subsystem Vendor Capability
	{ 0x094, 4, "SVID"        }, // Subsystem Vendor IDs
	{ 0x0a0, 2, "PMCAP"       }, // Power Management Capability
	{ 0x0a2, 2, "PMC"         }, // PCI Power Management Capabilities
	{ 0x0a4, 4, "PMCS"        }, // PCI Power Management Control
	{ 0x100, 4, "AECH"        }, // Advanced Error Extended
	{ 0x104, 4, "UES"         }, // Uncorrectable Error Status
	{ 0x108, 4, "UEM"         }, // Uncorrectable Error Mask
	{ 0x10c, 4, "UEV"         }, // Uncorrectable Error Severity
	{ 0x110, 4, "CES"         }, // Correctable Error Status
	{ 0x114, 4, "CEM"         }, // Correctable Error Mask
	{ 0x118, 4, "AECC"        }, // Advanced Error Capabilities And Control
	{ 0x11c, 4, "HL_DW1"      }, // Header Log
	{ 0x120, 4, "HL_DW2"      }, // Header Log
	{ 0x124, 4, "HL_DW3"      }, // Header Log
	{ 0x128, 4, "HL_DW4"      }, // Header Log
	{ 0x12c, 4, "REC"         }, // Root Error Command
	{ 0x130, 4, "RES"         }, // Root Error Status
	{ 0x134, 4, "ESID"        }, // Error Source Identification
	{ 0x150, 4, "PTMECH"      }, // PTM Extended Capability Header
	{ 0x284, 4, "PVCCR1"      }, // Port VC Capability Register 1
	{ 0x288, 4, "PVCC2"       }, // Port VC Capability 2
	{ 0x28c, 2, "PVCC"        }, // Port VC Control
	{ 0x28e, 2, "PVCS"        }, // Port VC Status
	{ 0x290, 4, "V0VCRC"      }, // Virtual Channel 0 Resource Capability
	{ 0x294, 4, "V0CTL"       }, // Virtual Channel 0 Resource Control
	{ 0x29a, 2, "V0STS"       }, // Virtual Channel 0 Resource Status
	{ 0x29c, 4, "V1VCRC"      }, // Virtual Channel 1 Resource Capability
	{ 0x2a0, 4, "V1CTL"       }, // Virtual Channel 1 Resource Control
	{ 0x2a6, 2, "V1STS"       }, // Virtual Channel 1 Resource Status
	{ 0xa30, 4, "SPEECH"      }, // Secondary PCI Express Extended Capability Header
	{ 0xa34, 4, "LCTL3"       }, // Link Control 3
	{ 0xa38, 4, "LES"         }, // Lane Error Status
	{ 0xa3c, 4, "L01EC"       }, // Lane 0 And Lane 1 Equalization Control
	{ 0xa40, 4, "L23EC"       }, // Lane 2 And Lane 3 Equalization Control
	{ 0xa44, 4, "L45EC"       }, // Lane 4 And Lane 5 Equalization Control
	{ 0xa48, 4, "L67EC"       }, // Lane 6 And Lane 7 Equalization Control
	{ 0xa4c, 4, "L89EC"       }, // Lane 8 And Lane 9 Equalization Control
	{ 0xa50, 4, "L1011EC"     }, // Lane 10 And Lane 11 Equalization Control
	{ 0xa54, 4, "L1213EC"     }, // Lane 12 And Lane 13 Equalization Control
	{ 0xa58, 4, "L1415EC"     }, // Lane 14 And Lane 15 Equalization Control
	{ 0xa90, 4, "DLFECH"      }, // Data Link Feature Extended Capability Header
	{ 0xa94, 4, "DLFCAP"      }, // Data Link Feature Capabilities Register
	{ 0xa98, 4, "DLFSTS"      }, // Data Link Feature Status Register
	{ 0xa9c, 4, "PL16GECH"    }, // Physical Layer 16.0 GT/s Extended Capability Header
	{ 0xaa0, 4, "PL16CAP"     }, // Physical Layer 16.0 GT/s Capability Register
	{ 0xaa4, 4, "PL16CTL"     }, // Physical Layer 16.0 GT/s Control Register
	{ 0xaa8, 4, "PL16S"       }, // Physical Layer 16.0 GT/s Status Register
	{ 0xaac, 4, "PL16LDPMS"   }, // Physical Layer 16.0 GT/s Local Data Parity Mismatch Status Register
	{ 0xab0, 4, "PL16FRDPMS"  }, // Physical Layer 16.0 GT/s First Retimer Data Parity Mismatch Status Register
	{ 0xab4, 4, "PL16SRDPMS"  }, // Physical Layer 16.0 GT/s Second Retimer Data Parity Mismatch Status Register
	{ 0xab8, 4, "PL16ES"      }, // Physical Layer 16.0 GT/s Extra Status Register
	{ 0xabc, 2, "PL16L01EC"   }, // Physical Layer 16.0 GT/s Lane 01 Equalization Control Register
	{ 0xabe, 2, "PL16L23EC"   }, // Physical Layer 16.0 GT/s Lane 23 Equalization Control Register
	{ 0xac0, 2, "PL16L45EC"   }, // Physical Layer 16.0 GT/s Lane 45 Equalization Control Register
	{ 0xac2, 2, "PL16L67EC"   }, // Physical Layer 16.0 GT/s Lane 67 Equalization Control Register
	{ 0xac4, 2, "PL16L89EC"   }, // Physical Layer 16.0 GT/s Lane 89 Equalization Control Register
	{ 0xac6, 2, "PL16L1011EC" }, // Physical Layer 16.0 GT/s Lane 1011 Equalization Control Register
	{ 0xac8, 2, "PL16L1213EC" }, // Physical Layer 16.0 GT/s Lane 1213 Equalization Control Register
	{ 0xaca, 2, "PL16L1415EC" }, // Physical Layer 16.0 GT/s Lane 1415 Equalization Control Register
	{ 0xc70, 4, "VNNREMCTL"   }, // VNN Removal Control
	{ 0xc74, 4, "VNNRSNRC1"   }, // VNN Removal Save And Restore Hardware Contexts 1
	{ 0xd00, 4, "DIDOVR"      }, // Device ID Override
	{ 0xedc, 4, "PL16MECH"    }, // Physical Layer 16.0 GT/s Margining Extended Capability Header
	{ 0xee0, 4, "PL16MPCPS"   }, // Physical Layer 16.0 GT/s Margining Port Capabilities and Port Status
	{ 0xee4, 4, "PL16L0MCS"   }, // Physical Layer 16.0 GT/s Lane0 Margin Control and Status Register
	{ 0xee8, 4, "PL16L1MCS"   }, // Physical Layer 16.0 GT/s Lane1 Margin Control and Status Register
	{ 0xeec, 4, "PL16L2MCS"   }, // Physical Layer 16.0 GT/s Lane2 Margin Control and Status Register
	{ 0xef0, 4, "PL16L3MCS"   }, // Physical Layer 16.0 GT/s Lane3 Margin Control and Status Register
	{ 0xef4, 4, "PL16L4MCS"   }, // Physical Layer 16.0 GT/s Lane4 Margin Control and Status Register
	{ 0xef8, 4, "PL16L5MCS"   }, // Physical Layer 16.0 GT/s Lane5 Margin Control and Status Register
	{ 0xefc, 4, "PL16L6MCS"   }, // Physical Layer 16.0 GT/s Lane6 Margin Control and Status Register
	{ 0xf00, 4, "PL16L7MCS"   }, // Physical Layer 16.0 GT/s Lane7 Margin Control and Status Register
	{ 0xf04, 4, "PL16L8MCS"   }, // Physical Layer 16.0 GT/s Lane8 Margin Control and Status Register
	{ 0xf08, 4, "PL16L9MCS"   }, // Physical Layer 16.0 GT/s Lane9 Margin Control and Status Register
	{ 0xf0c, 4, "PL16L10MCS"  }, // Physical Layer 16.0 GT/s Lane10 Margin Control and Status Register
	{ 0xf10, 4, "PL16L11MCS"  }, // Physical Layer 16.0 GT/s Lane11 Margin Control and Status Register
	{ 0xf14, 4, "PL16L12MCS"  }, // Physical Layer 16.0 GT/s Lane12 Margin Control and Status Register
	{ 0xf18, 4, "PL16L13MCS"  }, // Physical Layer 16.0 GT/s Lane13 Margin Control and Status Register
	{ 0xf1c, 4, "PL16L14MCS"  }, // Physical Layer 16.0 GT/s Lane14 Margin Control and Status Register
	{ 0xf20, 4, "PL16L15MCS"  }, // Physical Layer 16.0 GT/s Lane15 Margin Control and Status Register
};

/*
 * Egress Port Root Complex MMIO configuration space
 */
int print_epbar(struct pci_dev *nb)
{
	int i, size = (4 * 1024);
	volatile uint8_t *epbar;
	uint64_t epbar_phys;

	printf("\n============= EPBAR =============\n\n");

	switch (nb->device_id) {
	case PCI_DEVICE_ID_INTEL_82915:
	case PCI_DEVICE_ID_INTEL_82945GM:
	case PCI_DEVICE_ID_INTEL_82945GSE:
	case PCI_DEVICE_ID_INTEL_82945P:
	case PCI_DEVICE_ID_INTEL_82946:
	case PCI_DEVICE_ID_INTEL_82975X:
		epbar_phys = pci_read_long(nb, 0x40) & 0xfffffffe;
		break;
	case PCI_DEVICE_ID_INTEL_82965PM:
	case PCI_DEVICE_ID_INTEL_82Q965:
	case PCI_DEVICE_ID_INTEL_82Q35:
	case PCI_DEVICE_ID_INTEL_82G33:
	case PCI_DEVICE_ID_INTEL_82Q33:
	case PCI_DEVICE_ID_INTEL_82X38:
	case PCI_DEVICE_ID_INTEL_32X0:
	case PCI_DEVICE_ID_INTEL_82XX4X:
	case PCI_DEVICE_ID_INTEL_82Q45:
	case PCI_DEVICE_ID_INTEL_82G45:
	case PCI_DEVICE_ID_INTEL_82G41:
	case PCI_DEVICE_ID_INTEL_82B43:
	case PCI_DEVICE_ID_INTEL_82B43_2:
	case PCI_DEVICE_ID_INTEL_ATOM_DXXX:
	case PCI_DEVICE_ID_INTEL_ATOM_NXXX:
	case PCI_DEVICE_ID_INTEL_CORE_2ND_GEN_D:
	case PCI_DEVICE_ID_INTEL_CORE_2ND_GEN_M:
	case PCI_DEVICE_ID_INTEL_CORE_2ND_GEN_E3:
	case PCI_DEVICE_ID_INTEL_CORE_3RD_GEN_D:
	case PCI_DEVICE_ID_INTEL_CORE_3RD_GEN_M:
	case PCI_DEVICE_ID_INTEL_CORE_3RD_GEN_E3:
	case PCI_DEVICE_ID_INTEL_CORE_3RD_GEN_015c:
	case PCI_DEVICE_ID_INTEL_CORE_4TH_GEN_D:
	case PCI_DEVICE_ID_INTEL_CORE_4TH_GEN_M:
	case PCI_DEVICE_ID_INTEL_CORE_4TH_GEN_E3:
	case PCI_DEVICE_ID_INTEL_CORE_4TH_GEN_U:
	case PCI_DEVICE_ID_INTEL_CORE_5TH_GEN_U:
	case PCI_DEVICE_ID_INTEL_CORE_5TH_GEN_D:
	case PCI_DEVICE_ID_INTEL_CORE_5TH_GEN_M:
	case PCI_DEVICE_ID_INTEL_CORE_6TH_GEN_D2:
	case PCI_DEVICE_ID_INTEL_CORE_6TH_GEN_U:
	case PCI_DEVICE_ID_INTEL_CORE_6TH_GEN_Y:
	case PCI_DEVICE_ID_INTEL_CORE_6TH_GEN_M:
	case PCI_DEVICE_ID_INTEL_CORE_6TH_GEN_WST:
	case PCI_DEVICE_ID_INTEL_CORE_6TH_GEN_D:
	case PCI_DEVICE_ID_INTEL_CORE_6TH_GEN_E:
	case PCI_DEVICE_ID_INTEL_CORE_7TH_GEN_U:
	case PCI_DEVICE_ID_INTEL_CORE_7TH_GEN_Y:
	case PCI_DEVICE_ID_INTEL_CORE_7TH_GEN_U_Q:
	case PCI_DEVICE_ID_INTEL_CORE_7TH_GEN_E3:
	case PCI_DEVICE_ID_INTEL_CORE_8TH_GEN_U_1:
	case PCI_DEVICE_ID_INTEL_CORE_8TH_GEN_U_2:
	case PCI_DEVICE_ID_INTEL_CORE_ADL_ID_N_0_8:
	case PCI_DEVICE_ID_INTEL_CORE_ADL_ID_N_0_4:
	case PCI_DEVICE_ID_INTEL_CORE_ADL_ID_N_0_4_1:
	case PCI_DEVICE_ID_INTEL_CORE_RPL_ID_S_8_16:
	case PCI_DEVICE_ID_INTEL_CORE_RPL_ID_S_8_8:
	case PCI_DEVICE_ID_INTEL_CORE_RPL_ID_S_6_8:
	case PCI_DEVICE_ID_INTEL_CORE_RPL_ID_S_6_4:
	case PCI_DEVICE_ID_INTEL_CORE_RPL_ID_S_6_8_2:
	case PCI_DEVICE_ID_INTEL_CORE_RPL_ID_S_6_4_2:
	case PCI_DEVICE_ID_INTEL_CORE_RPL_ID_S_4_0:
	case PCI_DEVICE_ID_INTEL_CORE_RPL_ID_S_8_12:
	case PCI_DEVICE_ID_INTEL_CORE_RPL_ID_S_2_0:
	case PCI_DEVICE_ID_INTEL_CORE_RPL_ID_HX_8_16:
	case PCI_DEVICE_ID_INTEL_CORE_RPL_ID_HX_8_12:
	case PCI_DEVICE_ID_INTEL_CORE_RPL_ID_HX_8_8:
	case PCI_DEVICE_ID_INTEL_CORE_RPL_ID_HX_6_8:
	case PCI_DEVICE_ID_INTEL_CORE_RPL_ID_HX_6_4:
	case PCI_DEVICE_ID_INTEL_CORE_RPL_ID_HX_8_8_2:
	case PCI_DEVICE_ID_INTEL_CORE_RPL_ID_HX_6_8_2:
	case PCI_DEVICE_ID_INTEL_CORE_RPL_ID_HX_6_4_2:
	case PCI_DEVICE_ID_INTEL_CORE_RPL_ID_H_6_8:
	case PCI_DEVICE_ID_INTEL_CORE_RPL_ID_H_4_8:
	case PCI_DEVICE_ID_INTEL_CORE_RPL_ID_H_6_4:
	case PCI_DEVICE_ID_INTEL_CORE_RPL_ID_H_4_4:
	case PCI_DEVICE_ID_INTEL_CORE_RPL_ID_PX_6_8:
	case PCI_DEVICE_ID_INTEL_CORE_RPL_ID_PX_4_8:
	case PCI_DEVICE_ID_INTEL_CORE_RPL_ID_U_2_8:
	case PCI_DEVICE_ID_INTEL_CORE_RPL_ID_U_2_4:
	case PCI_DEVICE_ID_INTEL_CORE_RPL_ID_U_1_4:
	case PCI_DEVICE_ID_INTEL_CORE_RPL_ID_E_8_0:
	case PCI_DEVICE_ID_INTEL_CORE_RPL_ID_E_6_0:
	case PCI_DEVICE_ID_INTEL_CORE_RPL_ID_E_4_0:
	case PCI_DEVICE_ID_INTEL_CORE_CML_U1:
	case PCI_DEVICE_ID_INTEL_CORE_CML_U2:
	case PCI_DEVICE_ID_INTEL_CORE_CML_U3:
	case PCI_DEVICE_ID_INTEL_CORE_CML_H_8_2:
	case PCI_DEVICE_ID_INTEL_CORE_CML_H_6_2:
	case PCI_DEVICE_ID_INTEL_CORE_CML_H_4_2:
	case PCI_DEVICE_ID_INTEL_CORE_CML_S_10:
	case PCI_DEVICE_ID_INTEL_CORE_CML_S_8:
	case PCI_DEVICE_ID_INTEL_CORE_CML_S_6:
	case PCI_DEVICE_ID_INTEL_CORE_CML_S_4:
	case PCI_DEVICE_ID_INTEL_CORE_CML_S_2:
		epbar_phys = pci_read_long(nb, 0x40) & 0xfffffffe;
		epbar_phys |= ((uint64_t)pci_read_long(nb, 0x44)) << 32;
		break;
	case PCI_DEVICE_ID_INTEL_82810:
	case PCI_DEVICE_ID_INTEL_82810_DC:
	case PCI_DEVICE_ID_INTEL_82810E_DC:
	case PCI_DEVICE_ID_INTEL_82830M:
	case PCI_DEVICE_ID_INTEL_82865:
		printf("This northbridge does not have EPBAR.\n");
		return 1;
	default:
		printf("Error: Dumping EPBAR on this northbridge is not (yet) supported.\n");
		return 1;
	}

	epbar = map_physical(epbar_phys, size);

	if (epbar == NULL) {
		perror("Error mapping EPBAR");
		exit(1);
	}

	printf("EPBAR = 0x%08" PRIx64 " (MEM)\n\n", epbar_phys);
	for (i = 0; i < size; i += 4) {
		if (read32(epbar + i))
			printf("0x%04x: 0x%08x\n", i, read32(epbar+i));
	}

	unmap_physical((void *)epbar, size);
	return 0;
}

/*
 * MCH-ICH Serial Interconnect Ingress Root Complex MMIO configuration space
 */
int print_dmibar(struct pci_dev *nb)
{
	int i, size = (4 * 1024);
	volatile uint8_t *dmibar;
	uint64_t dmibar_phys;
	const io_register_t *dmi_registers = NULL;

	printf("\n============= DMIBAR ============\n\n");

	switch (nb->device_id) {
	case PCI_DEVICE_ID_INTEL_82915:
	case PCI_DEVICE_ID_INTEL_82945GM:
	case PCI_DEVICE_ID_INTEL_82945GSE:
	case PCI_DEVICE_ID_INTEL_82945P:
	case PCI_DEVICE_ID_INTEL_82975X:
		dmibar_phys = pci_read_long(nb, 0x4c) & 0xfffffffe;
		break;
	case PCI_DEVICE_ID_INTEL_82946:
	case PCI_DEVICE_ID_INTEL_82965PM:
	case PCI_DEVICE_ID_INTEL_82Q965:
	case PCI_DEVICE_ID_INTEL_82Q35:
	case PCI_DEVICE_ID_INTEL_82G33:
	case PCI_DEVICE_ID_INTEL_82Q33:
	case PCI_DEVICE_ID_INTEL_82X38:
	case PCI_DEVICE_ID_INTEL_32X0:
	case PCI_DEVICE_ID_INTEL_82XX4X:
	case PCI_DEVICE_ID_INTEL_82Q45:
	case PCI_DEVICE_ID_INTEL_82G45:
	case PCI_DEVICE_ID_INTEL_82G41:
	case PCI_DEVICE_ID_INTEL_82B43:
	case PCI_DEVICE_ID_INTEL_82B43_2:
	case PCI_DEVICE_ID_INTEL_ATOM_DXXX:
	case PCI_DEVICE_ID_INTEL_ATOM_NXXX:
		dmibar_phys = pci_read_long(nb, 0x68) & 0xfffffffe;
		dmibar_phys |= ((uint64_t)pci_read_long(nb, 0x6c)) << 32;
		break;
	case PCI_DEVICE_ID_INTEL_82810:
	case PCI_DEVICE_ID_INTEL_82810_DC:
	case PCI_DEVICE_ID_INTEL_82810E_DC:
	case PCI_DEVICE_ID_INTEL_82865:
		printf("This northbridge does not have DMIBAR.\n");
		return 1;
	case PCI_DEVICE_ID_INTEL_82X58:
		dmibar_phys = pci_read_long(nb, 0x50) & 0xfffff000;
		break;
	case PCI_DEVICE_ID_INTEL_CORE_0TH_GEN:
		/* DMIBAR is called DMIRCBAR in Nehalem */
		dmibar_phys = pci_read_long(nb, 0x50) & 0xfffff000; /* 31:12 */
		dmi_registers = nehalem_dmi_registers;
		size = ARRAY_SIZE(nehalem_dmi_registers);
		break;
	case PCI_DEVICE_ID_INTEL_CORE_1ST_GEN_D:
	case PCI_DEVICE_ID_INTEL_CORE_1ST_GEN_M:
	case PCI_DEVICE_ID_INTEL_CORE_1ST_GEN_0048:
		dmibar_phys = pci_read_long(nb, 0x68);
		dmibar_phys |= ((uint64_t)pci_read_long(nb, 0x6c)) << 32;
		dmibar_phys &= 0x0000000ffffff000UL; /* 35:12 */
		dmi_registers = westmere_dmi_registers;
		size = ARRAY_SIZE(westmere_dmi_registers);
		break;
	case PCI_DEVICE_ID_INTEL_CORE_2ND_GEN_D:
	case PCI_DEVICE_ID_INTEL_CORE_2ND_GEN_M:
	case PCI_DEVICE_ID_INTEL_CORE_2ND_GEN_E3:
		dmi_registers = sandybridge_dmi_registers;
		size = ARRAY_SIZE(sandybridge_dmi_registers);
		/* fall through */
	case PCI_DEVICE_ID_INTEL_CORE_3RD_GEN_D: /* pretty printing not implemented yet */
	case PCI_DEVICE_ID_INTEL_CORE_3RD_GEN_M:
	case PCI_DEVICE_ID_INTEL_CORE_3RD_GEN_E3:
	case PCI_DEVICE_ID_INTEL_CORE_3RD_GEN_015c:
	case PCI_DEVICE_ID_INTEL_CORE_4TH_GEN_D:
	case PCI_DEVICE_ID_INTEL_CORE_4TH_GEN_M:
	case PCI_DEVICE_ID_INTEL_CORE_4TH_GEN_E3:
	case PCI_DEVICE_ID_INTEL_CORE_5TH_GEN_D:
	case PCI_DEVICE_ID_INTEL_CORE_5TH_GEN_M:
		dmibar_phys = pci_read_long(nb, 0x68);
		dmibar_phys |= ((uint64_t)pci_read_long(nb, 0x6c)) << 32;
		dmibar_phys &= 0x0000007ffffff000UL; /* 38:12 */
		break;
	case PCI_DEVICE_ID_INTEL_CORE_4TH_GEN_U:
	case PCI_DEVICE_ID_INTEL_CORE_5TH_GEN_U:
		dmi_registers = haswell_ult_dmi_registers;
		size = ARRAY_SIZE(haswell_ult_dmi_registers);
		dmibar_phys = pci_read_long(nb, 0x68);
		dmibar_phys |= ((uint64_t)pci_read_long(nb, 0x6c)) << 32;
		dmibar_phys &= 0x0000007ffffff000UL; /* 38:12 */
		break;
	case PCI_DEVICE_ID_INTEL_CORE_6TH_GEN_D2:
	case PCI_DEVICE_ID_INTEL_CORE_6TH_GEN_U:
	case PCI_DEVICE_ID_INTEL_CORE_6TH_GEN_Y:
	case PCI_DEVICE_ID_INTEL_CORE_6TH_GEN_M:
	case PCI_DEVICE_ID_INTEL_CORE_6TH_GEN_WST:
	case PCI_DEVICE_ID_INTEL_CORE_6TH_GEN_D:
	case PCI_DEVICE_ID_INTEL_CORE_6TH_GEN_E:
	case PCI_DEVICE_ID_INTEL_CORE_7TH_GEN_U:
	case PCI_DEVICE_ID_INTEL_CORE_7TH_GEN_Y:
	case PCI_DEVICE_ID_INTEL_CORE_7TH_GEN_U_Q:
	case PCI_DEVICE_ID_INTEL_CORE_7TH_GEN_E3:
	case PCI_DEVICE_ID_INTEL_CORE_8TH_GEN_U_1:
	case PCI_DEVICE_ID_INTEL_CORE_8TH_GEN_U_2:
		dmi_registers = skylake_dmi_registers;
		size = ARRAY_SIZE(skylake_dmi_registers);
		dmibar_phys = pci_read_long(nb, 0x68);
		dmibar_phys |= ((uint64_t)pci_read_long(nb, 0x6c)) << 32;
		dmibar_phys &= 0x0000007ffffff000UL; /* 38:12 */
		break;
	case PCI_DEVICE_ID_INTEL_CORE_CML_U1:
	case PCI_DEVICE_ID_INTEL_CORE_CML_U2:
	case PCI_DEVICE_ID_INTEL_CORE_CML_U3:
	case PCI_DEVICE_ID_INTEL_CORE_CML_H_8_2:
	case PCI_DEVICE_ID_INTEL_CORE_CML_H_6_2:
	case PCI_DEVICE_ID_INTEL_CORE_CML_H_4_2:
	case PCI_DEVICE_ID_INTEL_CORE_CML_S_10:
	case PCI_DEVICE_ID_INTEL_CORE_CML_S_8:
	case PCI_DEVICE_ID_INTEL_CORE_CML_S_6:
	case PCI_DEVICE_ID_INTEL_CORE_CML_S_4:
	case PCI_DEVICE_ID_INTEL_CORE_CML_S_2:
		dmi_registers = cometlake_dmi_registers;
		size = ARRAY_SIZE(cometlake_dmi_registers);
		dmibar_phys = pci_read_long(nb, 0x68);
		dmibar_phys |= ((uint64_t)pci_read_long(nb, 0x6c)) << 32;
		dmibar_phys &= 0x0000007ffffff000UL; /* 38:12 */
		break;
	case PCI_DEVICE_ID_INTEL_CORE_ADL_ID_N_0_8:
	case PCI_DEVICE_ID_INTEL_CORE_ADL_ID_N_0_4:
	case PCI_DEVICE_ID_INTEL_CORE_ADL_ID_N_0_4_1:
		dmibar_phys = pci_read_long(nb, 0x68) & 0xfffffffe;
		dmibar_phys |= ((uint64_t)pci_read_long(nb, 0x6c)) << 32;
		dmi_registers = alderlake_dmi_registers;
		size = ARRAY_SIZE(alderlake_dmi_registers);
		break;
	case PCI_DEVICE_ID_INTEL_CORE_RPL_ID_S_8_16:
	case PCI_DEVICE_ID_INTEL_CORE_RPL_ID_S_8_8:
	case PCI_DEVICE_ID_INTEL_CORE_RPL_ID_S_6_8:
	case PCI_DEVICE_ID_INTEL_CORE_RPL_ID_S_6_4:
	case PCI_DEVICE_ID_INTEL_CORE_RPL_ID_S_6_8_2:
	case PCI_DEVICE_ID_INTEL_CORE_RPL_ID_S_6_4_2:
	case PCI_DEVICE_ID_INTEL_CORE_RPL_ID_S_4_0:
	case PCI_DEVICE_ID_INTEL_CORE_RPL_ID_S_8_12:
	case PCI_DEVICE_ID_INTEL_CORE_RPL_ID_S_2_0:
	case PCI_DEVICE_ID_INTEL_CORE_RPL_ID_HX_8_16:
	case PCI_DEVICE_ID_INTEL_CORE_RPL_ID_HX_8_12:
	case PCI_DEVICE_ID_INTEL_CORE_RPL_ID_HX_8_8:
	case PCI_DEVICE_ID_INTEL_CORE_RPL_ID_HX_6_8:
	case PCI_DEVICE_ID_INTEL_CORE_RPL_ID_HX_6_4:
	case PCI_DEVICE_ID_INTEL_CORE_RPL_ID_HX_8_8_2:
	case PCI_DEVICE_ID_INTEL_CORE_RPL_ID_HX_6_8_2:
	case PCI_DEVICE_ID_INTEL_CORE_RPL_ID_HX_6_4_2:
	case PCI_DEVICE_ID_INTEL_CORE_RPL_ID_H_6_8:
	case PCI_DEVICE_ID_INTEL_CORE_RPL_ID_H_4_8:
	case PCI_DEVICE_ID_INTEL_CORE_RPL_ID_H_6_4:
	case PCI_DEVICE_ID_INTEL_CORE_RPL_ID_H_4_4:
	case PCI_DEVICE_ID_INTEL_CORE_RPL_ID_PX_6_8:
	case PCI_DEVICE_ID_INTEL_CORE_RPL_ID_PX_4_8:
	case PCI_DEVICE_ID_INTEL_CORE_RPL_ID_U_2_8:
	case PCI_DEVICE_ID_INTEL_CORE_RPL_ID_U_2_4:
	case PCI_DEVICE_ID_INTEL_CORE_RPL_ID_U_1_4:
	case PCI_DEVICE_ID_INTEL_CORE_RPL_ID_E_8_0:
	case PCI_DEVICE_ID_INTEL_CORE_RPL_ID_E_6_0:
	case PCI_DEVICE_ID_INTEL_CORE_RPL_ID_E_4_0:
		dmibar_phys = pci_read_long(nb, 0x68) & 0xfffffffe;
		dmibar_phys |= ((uint64_t)pci_read_long(nb, 0x6c)) << 32;
		dmi_registers = raptorlake_dmi_registers;
		size = ARRAY_SIZE(raptorlake_dmi_registers);
		break;
	default:
		printf("Error: Dumping DMIBAR on this northbridge is not (yet) supported.\n");
		return 1;
	}

	dmibar = map_physical(dmibar_phys, size);

	if (dmibar == NULL) {
		perror("Error mapping DMIBAR");
		exit(1);
	}

	printf("DMIBAR = 0x%08" PRIx64 " (MEM)\n\n", dmibar_phys);
	if (dmi_registers != NULL) {
		for (i = 0; i < size; i++) {
			switch (dmi_registers[i].size) {
				case 4:
					printf("dmibase+0x%04x: 0x%08x (%s)\n",
						dmi_registers[i].addr,
						read32(dmibar+dmi_registers[i].addr),
						dmi_registers[i].name);
					break;
				case 2:
					printf("dmibase+0x%04x: 0x%04x     (%s)\n",
						dmi_registers[i].addr,
						read16(dmibar+dmi_registers[i].addr),
						dmi_registers[i].name);
					break;
				case 1:
					printf("dmibase+0x%04x: 0x%02x       (%s)\n",
						dmi_registers[i].addr,
						read8(dmibar+dmi_registers[i].addr),
						dmi_registers[i].name);
					break;
			}
		}
	} else {
		for (i = 0; i < size; i += 4) {
			if (read32(dmibar + i))
				printf("0x%04x: 0x%08x\n", i, read32(dmibar+i));
		}
	}

	unmap_physical((void *)dmibar, size);
	return 0;
}

/*
 * PCIe MMIO configuration space
 */
int print_pciexbar(struct pci_dev *nb)
{
	uint64_t pciexbar_reg;
	uint64_t pciexbar_phys;
	volatile uint8_t *pciexbar;
	int max_busses, devbase, i;
	int bus, dev, fn;

	printf("========= PCIEXBAR ========\n\n");

	switch (nb->device_id) {
	case PCI_DEVICE_ID_INTEL_82915:
	case PCI_DEVICE_ID_INTEL_82945GM:
	case PCI_DEVICE_ID_INTEL_82945GSE:
	case PCI_DEVICE_ID_INTEL_82945P:
	case PCI_DEVICE_ID_INTEL_82975X:
		pciexbar_reg = pci_read_long(nb, 0x48);
		break;
	case PCI_DEVICE_ID_INTEL_82946:
	case PCI_DEVICE_ID_INTEL_82965PM:
	case PCI_DEVICE_ID_INTEL_82Q965:
	case PCI_DEVICE_ID_INTEL_82Q35:
	case PCI_DEVICE_ID_INTEL_82G33:
	case PCI_DEVICE_ID_INTEL_82Q33:
	case PCI_DEVICE_ID_INTEL_82X38:
	case PCI_DEVICE_ID_INTEL_32X0:
	case PCI_DEVICE_ID_INTEL_82XX4X:
	case PCI_DEVICE_ID_INTEL_82Q45:
	case PCI_DEVICE_ID_INTEL_82G45:
	case PCI_DEVICE_ID_INTEL_82G41:
	case PCI_DEVICE_ID_INTEL_82B43:
	case PCI_DEVICE_ID_INTEL_82B43_2:
	case PCI_DEVICE_ID_INTEL_ATOM_DXXX:
	case PCI_DEVICE_ID_INTEL_ATOM_NXXX:
	case PCI_DEVICE_ID_INTEL_CORE_2ND_GEN_D:
	case PCI_DEVICE_ID_INTEL_CORE_2ND_GEN_M:
	case PCI_DEVICE_ID_INTEL_CORE_2ND_GEN_E3:
	case PCI_DEVICE_ID_INTEL_CORE_3RD_GEN_D:
	case PCI_DEVICE_ID_INTEL_CORE_3RD_GEN_M:
	case PCI_DEVICE_ID_INTEL_CORE_3RD_GEN_E3:
	case PCI_DEVICE_ID_INTEL_CORE_3RD_GEN_015c:
	case PCI_DEVICE_ID_INTEL_CORE_4TH_GEN_D:
	case PCI_DEVICE_ID_INTEL_CORE_4TH_GEN_M:
	case PCI_DEVICE_ID_INTEL_CORE_4TH_GEN_E3:
	case PCI_DEVICE_ID_INTEL_CORE_4TH_GEN_U:
	case PCI_DEVICE_ID_INTEL_CORE_5TH_GEN_U:
	case PCI_DEVICE_ID_INTEL_CORE_5TH_GEN_D:
	case PCI_DEVICE_ID_INTEL_CORE_5TH_GEN_M:
	case PCI_DEVICE_ID_INTEL_CORE_6TH_GEN_D2:
	case PCI_DEVICE_ID_INTEL_CORE_6TH_GEN_U:
	case PCI_DEVICE_ID_INTEL_CORE_6TH_GEN_Y:
	case PCI_DEVICE_ID_INTEL_CORE_6TH_GEN_M:
	case PCI_DEVICE_ID_INTEL_CORE_6TH_GEN_WST:
	case PCI_DEVICE_ID_INTEL_CORE_6TH_GEN_D:
	case PCI_DEVICE_ID_INTEL_CORE_6TH_GEN_E:
	case PCI_DEVICE_ID_INTEL_CORE_7TH_GEN_U:
	case PCI_DEVICE_ID_INTEL_CORE_7TH_GEN_Y:
	case PCI_DEVICE_ID_INTEL_CORE_7TH_GEN_U_Q:
	case PCI_DEVICE_ID_INTEL_CORE_7TH_GEN_E3:
	case PCI_DEVICE_ID_INTEL_CORE_8TH_GEN_U_1:
	case PCI_DEVICE_ID_INTEL_CORE_8TH_GEN_U_2:
	case PCI_DEVICE_ID_INTEL_CORE_ADL_ID_N_0_8:
	case PCI_DEVICE_ID_INTEL_CORE_ADL_ID_N_0_4:
	case PCI_DEVICE_ID_INTEL_CORE_ADL_ID_N_0_4_1:
	case PCI_DEVICE_ID_INTEL_CORE_RPL_ID_S_8_16:
	case PCI_DEVICE_ID_INTEL_CORE_RPL_ID_S_8_8:
	case PCI_DEVICE_ID_INTEL_CORE_RPL_ID_S_6_8:
	case PCI_DEVICE_ID_INTEL_CORE_RPL_ID_S_6_4:
	case PCI_DEVICE_ID_INTEL_CORE_RPL_ID_S_6_8_2:
	case PCI_DEVICE_ID_INTEL_CORE_RPL_ID_S_6_4_2:
	case PCI_DEVICE_ID_INTEL_CORE_RPL_ID_S_4_0:
	case PCI_DEVICE_ID_INTEL_CORE_RPL_ID_S_8_12:
	case PCI_DEVICE_ID_INTEL_CORE_RPL_ID_S_2_0:
	case PCI_DEVICE_ID_INTEL_CORE_RPL_ID_HX_8_16:
	case PCI_DEVICE_ID_INTEL_CORE_RPL_ID_HX_8_12:
	case PCI_DEVICE_ID_INTEL_CORE_RPL_ID_HX_8_8:
	case PCI_DEVICE_ID_INTEL_CORE_RPL_ID_HX_6_8:
	case PCI_DEVICE_ID_INTEL_CORE_RPL_ID_HX_6_4:
	case PCI_DEVICE_ID_INTEL_CORE_RPL_ID_HX_8_8_2:
	case PCI_DEVICE_ID_INTEL_CORE_RPL_ID_HX_6_8_2:
	case PCI_DEVICE_ID_INTEL_CORE_RPL_ID_HX_6_4_2:
	case PCI_DEVICE_ID_INTEL_CORE_RPL_ID_H_6_8:
	case PCI_DEVICE_ID_INTEL_CORE_RPL_ID_H_4_8:
	case PCI_DEVICE_ID_INTEL_CORE_RPL_ID_H_6_4:
	case PCI_DEVICE_ID_INTEL_CORE_RPL_ID_H_4_4:
	case PCI_DEVICE_ID_INTEL_CORE_RPL_ID_PX_6_8:
	case PCI_DEVICE_ID_INTEL_CORE_RPL_ID_PX_4_8:
	case PCI_DEVICE_ID_INTEL_CORE_RPL_ID_U_2_8:
	case PCI_DEVICE_ID_INTEL_CORE_RPL_ID_U_2_4:
	case PCI_DEVICE_ID_INTEL_CORE_RPL_ID_U_1_4:
	case PCI_DEVICE_ID_INTEL_CORE_RPL_ID_E_8_0:
	case PCI_DEVICE_ID_INTEL_CORE_RPL_ID_E_6_0:
	case PCI_DEVICE_ID_INTEL_CORE_RPL_ID_E_4_0:
	case PCI_DEVICE_ID_INTEL_CORE_CML_U1:
	case PCI_DEVICE_ID_INTEL_CORE_CML_U2:
	case PCI_DEVICE_ID_INTEL_CORE_CML_U3:
	case PCI_DEVICE_ID_INTEL_CORE_CML_H_8_2:
	case PCI_DEVICE_ID_INTEL_CORE_CML_H_6_2:
	case PCI_DEVICE_ID_INTEL_CORE_CML_H_4_2:
	case PCI_DEVICE_ID_INTEL_CORE_CML_S_10:
	case PCI_DEVICE_ID_INTEL_CORE_CML_S_8:
	case PCI_DEVICE_ID_INTEL_CORE_CML_S_6:
	case PCI_DEVICE_ID_INTEL_CORE_CML_S_4:
	case PCI_DEVICE_ID_INTEL_CORE_CML_S_2:
		pciexbar_reg = pci_read_long(nb, 0x60);
		pciexbar_reg |= ((uint64_t)pci_read_long(nb, 0x64)) << 32;
		break;
	case PCI_DEVICE_ID_INTEL_82810:
	case PCI_DEVICE_ID_INTEL_82810_DC:
	case PCI_DEVICE_ID_INTEL_82810E_DC:
	case PCI_DEVICE_ID_INTEL_82865:
		printf("Error: This northbridge does not have PCIEXBAR.\n");
		return 1;
	default:
		printf("Error: Dumping PCIEXBAR on this northbridge is not (yet) supported.\n");
		return 1;
	}

	if (!(pciexbar_reg & (1 << 0))) {
		printf("PCIEXBAR register is disabled.\n");
		return 0;
	}

	switch ((pciexbar_reg >> 1) & 3) {
	case 0: // 256MB
		pciexbar_phys = pciexbar_reg & (0xffULL << 28);
		max_busses = 256;
		break;
	case 1: // 128M
		pciexbar_phys = pciexbar_reg & (0x1ffULL << 27);
		max_busses = 128;
		break;
	case 2: // 64M
		pciexbar_phys = pciexbar_reg & (0x3ffULL << 26);
		max_busses = 64;
		break;
	default: // RSVD
		printf("Undefined address base. Bailing out.\n");
		return 1;
	}

	printf("PCIEXBAR: 0x%08" PRIx64 "\n", pciexbar_phys);

	pciexbar = map_physical(pciexbar_phys, (max_busses * 1024 * 1024));

	if (pciexbar == NULL) {
		perror("Error mapping PCIEXBAR");
		exit(1);
	}

	for (bus = 0; bus < max_busses; bus++) {
		for (dev = 0; dev < 32; dev++) {
			for (fn = 0; fn < 8; fn++) {
				devbase = (bus * 1024 * 1024) + (dev * 32 * 1024) + (fn * 4 * 1024);

				if (read16(pciexbar + devbase) == 0xffff)
					continue;

				/* This is a heuristics. Anyone got a better check? */
				if( (read32(pciexbar + devbase + 256) == 0xffffffff) &&
					(read32(pciexbar + devbase + 512) == 0xffffffff) ) {
#if DEBUG
					printf("Skipped non-PCIe device %02x:%02x.%01x\n", bus, dev, fn);
#endif
					continue;
				}

				printf("\nPCIe %02x:%02x.%01x extended config space:", bus, dev, fn);
				for (i = 0; i < 4096; i++) {
					if((i % 0x10) == 0)
						printf("\n%04x:", i);
					printf(" %02x", *(pciexbar+devbase+i));
				}
				printf("\n");
			}
		}
	}

	unmap_physical((void *)pciexbar, (max_busses * 1024 * 1024));

	return 0;
}
