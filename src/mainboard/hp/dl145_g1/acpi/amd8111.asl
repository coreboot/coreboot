/*
 * Copyright 2005 AMD
 * Copyright (C) 2011, 2014 Oskar Enoksson <enok@lysator.liu.se>
 */
//AMD8111
// APIC version of the interrupt routing table
Name (APIC, Package (0x04) {
	Package (0x04) { 0x0004FFFF, 0x00, 0x00, 0x10},// 0x0004ffff : assusme 8131 is present
	Package (0x04) { 0x0004FFFF, 0x01, 0x00, 0x11},
	Package (0x04) { 0x0004FFFF, 0x02, 0x00, 0x12},
	Package (0x04) { 0x0004FFFF, 0x03, 0x00, 0x13}
})
// PIC version of the interrupt routing table
Name (PICM, Package (0x04) {
	Package (0x04) { 0x0004FFFF, 0x00, \_SB.PCI0.LNKA, 0x00},
	Package (0x04) { 0x0004FFFF, 0x01, \_SB.PCI0.LNKB, 0x00},
	Package (0x04) { 0x0004FFFF, 0x02, \_SB.PCI0.LNKC, 0x00},
	Package (0x04) { 0x0004FFFF, 0x03, \_SB.PCI0.LNKD, 0x00}
})
Name (DNCG, Ones)
Method (_PRT, 0, NotSerialized) {
	If (LEqual (^DNCG, Ones)) {
		Store (DADD(\_SB.PCI0.SBDN, 0x0001ffff), Local0)
		// Update the Device Number according to SBDN
		Store(Local0, Index (DeRefOf (Index (PICM, 0)), 0))
		Store(Local0, Index (DeRefOf (Index (PICM, 1)), 0))
		Store(Local0, Index (DeRefOf (Index (PICM, 2)), 0))
		Store(Local0, Index (DeRefOf (Index (PICM, 3)), 0))

		Store(Local0, Index (DeRefOf (Index (APIC, 0)), 0))
		Store(Local0, Index (DeRefOf (Index (APIC, 1)), 0))
		Store(Local0, Index (DeRefOf (Index (APIC, 2)), 0))
		Store(Local0, Index (DeRefOf (Index (APIC, 3)), 0))

		Store (0x00, ^DNCG)
	}

	If (LNot (PICF)) {
		Return (PICM)
	} Else {
		Return (APIC)
	}
}

// AMD8111 System Management I/O Mapped Registers (PMxx)
OperationRegion (PMIO, SystemIO, PMBS, 0xDF)
Field (PMIO, ByteAcc, NoLock, Preserve) {
					Offset (0x1E),
	SWSM,   8,  // Software SMI Trigger (sets GSTS)
					Offset (0x28),
	GSTS,   16, // Global STatuS
	GNBL,   16, // Global SMI enable
					Offset (0x30),
	STMC,   5,  // Miscellaneous SMI Status
					Offset (0x32),
	ENMC,   5,  // Miscellaneous SMI Enable
					Offset (0x44),
	STC0,   9,  // TCO Status 1
					Offset (0x46),
	STC1,   4,  // TCO Status 2
					Offset (0xA8),
	STHW,   20  // Device monitor SMI Interrupt Enable
}
Device (HPET) {
	Name (HPT, 0x00)
	Name (_HID, EisaId ("PNP0103"))
	Name (_UID, 0x00)
	Method (_STA, 0, NotSerialized) {
		Return (0x0F)
	}
	Method (_CRS, 0, Serialized) {
		Name (BUF0, ResourceTemplate () {
			Memory32Fixed (ReadWrite, 0xFED00000, 0x00000400)
		})
		Return (BUF0)
	}
}
#include "amd8111_pic.asl"
#include "amd8111_isa.asl"

Device (TP2P) {
	// 8111 P2P and it should 0x00030000 when 8131 present
	Method (_ADR, 0, NotSerialized) {
		Return (DADD(\_SB.PCI0.SBDN, 0x00000000))
	}
	Method (_PRW, 0, NotSerialized) { // Power Resource for Wake
	// result :
	// [0] Bit index into GPEx_EN in the GPE block described by FADT.
	// [1] The lowest power state from which the system can be awakened.
		//If (CondRefOf (\_S3, Local0)) {
		//	Return (Package (0x02) { 0x08, 0x03 })
		//} Else {
			Return (Package (0x02) { 0x08, 0x01 })
		//}
	}
	Device (ETHR) {
		Name (_ADR, 0x00010000)
		Method (_PRW, 0, NotSerialized) { // Power Resource for Wake
			//If (CondRefOf (\_S3, Local0)) {
			//	Return (Package (0x02) { 0x08, 0x03 })
			//} Else {
				Return (Package (0x02) { 0x08, 0x01 })
			//}
		}
	}
	Device (USB0) {
		Name (_ADR, 0x00000000)
		Method (_PSW, 1, NotSerialized) { // Power State Wake
			And (GNBL, 0x7FFF, GNBL)
		}
		Method (_PRW, 0, NotSerialized) { // Power Resource for Wake
			//If (CondRefOf (\_S3, Local0)) {
			//	Return (Package (0x02) { 0x0F, 0x03 })
			//} Else {
				Return (Package (0x02) { 0x0F, 0x01 })
			//}
		}
	}
	Device (USB1) {
		Name (_ADR, 0x00000001)
		Method (_PSW, 1, NotSerialized) { // Power State Wake
			And (GNBL, 0x7FFF, GNBL)
		}
		Method (_PRW, 0, NotSerialized) { // Power Resource for Wake
			//If (CondRefOf (\_S3, Local0)) {
			//	Return (Package (0x02) { 0x0F, 0x03 })
			//} Else {
				Return (Package (0x02) { 0x0F, 0x01 })
			//}
		}
	}
	Name (APIC, Package (0x0C) {
		Package (0x04) { 0x0000FFFF, 0x00, 0x00, 0x10 }, //USB
		Package (0x04) { 0x0000FFFF, 0x01, 0x00, 0x11 },
		Package (0x04) { 0x0000FFFF, 0x02, 0x00, 0x12 },
		Package (0x04) { 0x0000FFFF, 0x03, 0x00, 0x13 },
		Package (0x04) { 0x0004FFFF, 0x00, 0x00, 0x10 }, //Slot 6
		Package (0x04) { 0x0004FFFF, 0x01, 0x00, 0x11 },
		Package (0x04) { 0x0004FFFF, 0x02, 0x00, 0x12 },
		Package (0x04) { 0x0004FFFF, 0x03, 0x00, 0x13 },
		Package (0x04) { 0x0005FFFF, 0x00, 0x00, 0x11 }, //Slot 5
		Package (0x04) { 0x0005FFFF, 0x01, 0x00, 0x12 },
		Package (0x04) { 0x0005FFFF, 0x02, 0x00, 0x13 },
		Package (0x04) { 0x0005FFFF, 0x03, 0x00, 0x10 }
	})
	Name (PICM, Package (0x0C) {
		Package (0x04) { 0x0000FFFF, 0x00, \_SB.PCI0.LNKA, 0x00 }, //USB
		Package (0x04) { 0x0000FFFF, 0x01, \_SB.PCI0.LNKB, 0x00 },
		Package (0x04) { 0x0000FFFF, 0x02, \_SB.PCI0.LNKC, 0x00 },
		Package (0x04) { 0x0000FFFF, 0x03, \_SB.PCI0.LNKD, 0x00 },
		Package (0x04) { 0x0004FFFF, 0x00, \_SB.PCI0.LNKA, 0x00 }, //Slot 6
		Package (0x04) { 0x0004FFFF, 0x01, \_SB.PCI0.LNKB, 0x00 },
		Package (0x04) { 0x0004FFFF, 0x02, \_SB.PCI0.LNKC, 0x00 },
		Package (0x04) { 0x0004FFFF, 0x03, \_SB.PCI0.LNKD, 0x00 },
		Package (0x04) { 0x0005FFFF, 0x00, \_SB.PCI0.LNKB, 0x00 }, //Slot 5
		Package (0x04) { 0x0005FFFF, 0x01, \_SB.PCI0.LNKC, 0x00 },
		Package (0x04) { 0x0005FFFF, 0x02, \_SB.PCI0.LNKD, 0x00 },
		Package (0x04) { 0x0005FFFF, 0x03, \_SB.PCI0.LNKA, 0x00 }
	})
	Method (_PRT, 0, NotSerialized) {
		If (LNot (PICF)) { Return (PICM) }
		Else             { Return (APIC) }
	}
}
Device (IDE0) {
	Method (_ADR, 0, NotSerialized) {
		Return (DADD(\_SB.PCI0.SBDN, 0x00010001))
	}
	Name (REGF, 0x01)
	Method (_REG, 2, NotSerialized) {
		If (LEqual (Arg0, 0x02)) {
			Store (Arg1, REGF)
		}
	}
	OperationRegion (BAR0, PCI_Config, 0x00, 0x60)
	Field (BAR0, ByteAcc, NoLock, Preserve) {
						Offset (0x40), // EIDE Controller Configuration Register
		SCEN,   1,  // Secondary Channel Enable
		PCEN,   1,  // Primary Channel Enable
				,   10,
		SPWB,   1,  // Secondary Port posted-write buffer for PIO modes enable
		SRPB,   1,  // RW (controls nothing)
		PPWB,   1,  // Primary Port posted-write buffer for PIO modes enable
		PRPB,   1,  // RW (controls nothing)
		PM80,   1,  // High-speed 80-pin cable enable Primary Master
		PS80,   1,  // High-speed 80-pin cable enable Primary Slave
		SM80,   1,  // High-speed 80-pin cable enable Secondary Master
		SS80,   1,  // High-speed 80-pin cable enable Secondary Slave
				,   4,  // RW (controls nothing)
						Offset (0x48),
		SSRT,   4,  //
		SSPW,   4,  //
		SMRT,   4,  //
		SMPW,   4,
		PSRT,   4,
		PSPW,   4,
		PMRT,   4,
		PMPW,   4,
		SSAD,   2,
		SMAD,   2,
		PSAD,   2,
		PMAD,   2,
						Offset (0x4E),
		SXRT,   4,
		SXPW,   4,
		PXRT,   4,
		PXPW,   4,
		SSUD,   8,
		SMUD,   8,
		PSUD,   8,
		PMUD,   8,
		PPDN,   1,
		PPDS,   1,
				,   2,
		SPDN,   1,
		SPDS,   1
	}
	Name (TIM0, Package (0x06) {
		Package (0x05) {
			0x78,
			0xB4,
			0xF0,
			0x0186,
			0x0258
		},
		Package (0x07) {
			0x78,
			0x5A,
			0x3C,
			0x2D,
			0x1E,
			0x14,
			0x0F
		},
		Package (0x08) {
			0x04,
			0x03,
			0x02,
			0x01,
			0x00,
			0x00,
			0x00,
			0x00
		},
		Package (0x03) {
			0x02,
			0x01,
			0x00
		},
		Package (0x05) {
			0x20,
			0x22,
			0x42,
			0x65,
			0xA8
		},
		Package (0x07) {
			0xC2,
			0xC1,
			0xC0,
			0xC4,
			0xC5,
			0xC6,
			0xC7
		}
	})
	Name (TMD0, Buffer (0x14) {})
	CreateDWordField (TMD0, 0x00, PIO0)
	CreateDWordField (TMD0, 0x04, DMA0)
	CreateDWordField (TMD0, 0x08, PIO1)
	CreateDWordField (TMD0, 0x0C, DMA1)
	CreateDWordField (TMD0, 0x10, CHNF)
	Device (CHN0) {
		Name (_ADR, 0x00)
		Method (_STA, 0, NotSerialized) {
			If (PCEN) { Return (0x0F) }
			Else      { Return (0x09) }
		}
		Method (_GTM, 0, NotSerialized) {
			Return (GTM (PMPW, PMRT, PSPW, PSRT, PMUD, PSUD))
		}
		Method (_STM, 3, NotSerialized) {
			Store (Arg0, TMD0)
			Store (STM (), Local0)
			And (Local0, 0xFF, PSUD)
			ShiftRight (Local0, 0x08, Local0)
			And (Local0, 0xFF, PMUD)
			ShiftRight (Local0, 0x08, Local0)
			And (Local0, 0x0F, PSRT)
			ShiftRight (Local0, 0x04, Local0)
			And (Local0, 0x0F, PSPW)
			ShiftRight (Local0, 0x04, Local0)
			And (Local0, 0x0F, PMRT)
			ShiftRight (Local0, 0x04, Local0)
			And (Local0, 0x0F, PMPW)
			Store (GTF (0x00, Arg1), ATA0)
			Store (GTF (0x01, Arg2), ATA1)
		}
		Device (DRV0) {
			Name (_ADR, 0x00)
			Method (_GTF, 0, NotSerialized) {
				Return (RATA (ATA0))
			}
		}
		Device (DRV1) {
			Name (_ADR, 0x01)
			Method (_GTF, 0, NotSerialized) {
				Return (RATA (ATA1))
			}
		}
	}
	Device (CHN1) {
		Name (_ADR, 0x01)
		Method (_STA, 0, NotSerialized) {
			If (SCEN) { Return (0x0F) }
			Else      { Return (0x09) }
		}
		Method (_GTM, 0, NotSerialized) {
			Return (GTM (SMPW, SMRT, SSPW, SSRT, SMUD, SSUD))
		}
		Method (_STM, 3, NotSerialized) {
			Store (Arg0, TMD0)
			Store (STM (), Local0)
			And (Local0, 0xFF, SSUD)
			ShiftRight (Local0, 0x08, Local0)
			And (Local0, 0xFF, SMUD)
			ShiftRight (Local0, 0x08, Local0)
			And (Local0, 0x0F, SSRT)
			ShiftRight (Local0, 0x04, Local0)
			And (Local0, 0x0F, SSPW)
			ShiftRight (Local0, 0x04, Local0)
			And (Local0, 0x0F, SMRT)
			ShiftRight (Local0, 0x04, Local0)
			And (Local0, 0x0F, SMPW)
			Store (GTF (0x00, Arg1), ATA2)
			Store (GTF (0x01, Arg2), ATA3)
		}
		Device (DRV0) {
			Name (_ADR, 0x00)
			Method (_GTF, 0, NotSerialized) {
				Return (RATA (ATA2))
			}
		}
		Device (DRV1) {
			Name (_ADR, 0x01)
			Method (_GTF, 0, NotSerialized) {
				Return (RATA (ATA3))
			}
		}
	}
	Method (GTM, 6, Serialized) {
		Store (Ones, PIO0)
		Store (Ones, PIO1)
		Store (Ones, DMA0)
		Store (Ones, DMA1)
		Store (0x1A, CHNF)
		If (REGF) {}
		Else      { Return (TMD0) }
		Add (Arg0, Arg1, Local0)
		Add (Local0, 0x02, Local0)
		Multiply (Local0, 0x1E, PIO0)
		Add (Arg2, Arg3, Local0)
		Add (Local0, 0x02, Local0)
		Multiply (Local0, 0x1E, PIO1)
		If (And (Arg4, 0x40)) {
			Or (CHNF, 0x01, CHNF)
			And (Arg4, 0x07, Local0)
			If (LLess (Local0, 0x04)) {
				Add (Local0, 0x02, Local0)
				Multiply (Local0, 0x1E, DMA0)
			} Else {
				If (LEqual (Local0, 0x04)) {
					Store (0x2D, DMA0)
				} Else {
					If (LEqual (Local0, 0x05)) {
						Store (0x1E, DMA0)
					} Else {
						If (LEqual (Local0, 0x06)) {
							Store (0x14, DMA0)
						} Else {
							If (LEqual (Local0, 0x07)) {
								Store (0x0F, DMA0)
							} Else {
								Store (PIO0, DMA0)
							}
						}
					}
				}
			}
		} Else {
			Store (PIO0, DMA0)
		}
		If (And (Arg5, 0x40)) {
			Or (CHNF, 0x04, CHNF)
			And (Arg5, 0x07, Local0)
			If (LLess (Local0, 0x04)) {
				Add (Local0, 0x02, Local0)
				Multiply (Local0, 0x1E, DMA1)
			} Else {
				If (LEqual (Local0, 0x04)) {
					Store (0x2D, DMA1)
				} Else {
					If (LEqual (Local0, 0x05)) {
						Store (0x1E, DMA1)
					} Else {
						If (LEqual (Local0, 0x06)) {
							Store (0x14, DMA1)
						} Else {
							If (LEqual (Local0, 0x07)) {
								Store (0x0F, DMA0)
							} Else {
								Store (PIO1, DMA1)
							}
						}
					}
				}
			}
		} Else {
			Store (PIO1, DMA1)
		}
		Return (TMD0)
	}
	Method (STM, 0, Serialized) {
		If (REGF) {}
		Else      { Return (0xFFFFFFFF) }
		If (LEqual (PIO0, 0xFFFFFFFF)) {
			Store (0xA8, Local1)
		} Else {
			And (Match (DerefOf (Index (TIM0, 0x00)),
									MGE, PIO0, MTR,
									0x00, 0x00),
					0x07, Local0)
			Store (DerefOf (Index (DerefOf (Index (TIM0, 0x04)), Local0)),
						Local1)
		}
		ShiftLeft (Local1, 0x08, Local1)
		If (LEqual (PIO1, 0xFFFFFFFF)) {
			Or (Local1, 0xA8, Local1)
		} Else {
			And (Match (DerefOf (Index (TIM0, 0x00)), MGE, PIO1, MTR,
									0x00, 0x00), 0x07, Local0)
			Or (DerefOf (Index (DerefOf (Index (TIM0, 0x04)), Local0)),
					Local1, Local1)
		}
		ShiftLeft (Local1, 0x08, Local1)
		If (LEqual (DMA0, 0xFFFFFFFF)) {
			Or (Local1, 0x03, Local1)
		} Else {
			If (And (CHNF, 0x01)) {
				And (Match (DerefOf (Index (TIM0, 0x01)), MLE, DMA0, MTR,
										0x00, 0x00), 0x07, Local0)
				Or (DerefOf (Index (DerefOf (Index (TIM0, 0x05)), Local0)),
						Local1, Local1)
			} Else {
				Or (Local1, 0x03, Local1)
			}
		}
		ShiftLeft (Local1, 0x08, Local1)
		If (LEqual (DMA1, 0xFFFFFFFF)) {
			Or (Local1, 0x03, Local1)
		} Else {
			If (And (CHNF, 0x04)) {
				And (Match (DerefOf (Index (TIM0, 0x01)), MLE, DMA1, MTR,
										0x00, 0x00), 0x07, Local0)
				Or (DerefOf (Index (DerefOf (Index (TIM0, 0x05)), Local0)),
						Local1, Local1)
			} Else {
				Or (Local1, 0x03, Local1)
			}
		}
		Return (Local1)
	}
	Name (AT01, Buffer (0x07) {
		0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0xEF
	})
	Name (AT02, Buffer (0x07) {
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x90
	})
	Name (AT03, Buffer (0x07) {
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC6
	})
	Name (AT04, Buffer (0x07) {
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x91
	})
	Name (ATA0, Buffer (0x1D) {})
	Name (ATA1, Buffer (0x1D) {})
	Name (ATA2, Buffer (0x1D) {})
	Name (ATA3, Buffer (0x1D) {})
	Name (ATAB, Buffer (0x1D) {})
	CreateByteField (ATAB, 0x00, CMDC)
	Method (GTFB, 3, Serialized) {
		Multiply (CMDC, 0x38, Local0)
		Add (Local0, 0x08, Local1)
		CreateField (ATAB, Local1, 0x38, CMDX)
		Multiply (CMDC, 0x07, Local0)
		CreateByteField (ATAB, Add (Local0, 0x02), A001)
		CreateByteField (ATAB, Add (Local0, 0x06), A005)
		Store (Arg0, CMDX)
		Store (Arg1, A001)
		Store (Arg2, A005)
		Increment (CMDC)
	}
	Method (GTF, 2, Serialized) {
		Store (Arg1, Debug)
		Store (0x00, CMDC)
		Name (ID49, 0x0C00)
		Name (ID59, 0x00)
		Name (ID53, 0x04)
		Name (ID63, 0x0F00)
		Name (ID88, 0x0F00)
		Name (IRDY, 0x01)
		Name (PIOT, 0x00)
		Name (DMAT, 0x00)
		If (LEqual (SizeOf (Arg1), 0x0200)) {
			CreateWordField (Arg1, 0x62, IW49)
			Store (IW49, ID49)
			CreateWordField (Arg1, 0x6A, IW53)
			Store (IW53, ID53)
			CreateWordField (Arg1, 0x7E, IW63)
			Store (IW63, ID63)
			CreateWordField (Arg1, 0x76, IW59)
			Store (IW59, ID59)
			CreateWordField (Arg1, 0xB0, IW88)
			Store (IW88, ID88)
		}
		Store (0xA0, Local7)
		If (Arg0) {
			Store (0xB0, Local7)
			And (CHNF, 0x08, IRDY)
			If (And (CHNF, 0x10)) {
				Store (PIO1, PIOT)
			} Else {
				Store (PIO0, PIOT)
			}
			If (And (CHNF, 0x04)) {
				If (And (CHNF, 0x10)) {
					Store (DMA1, DMAT)
				} Else {
					Store (DMA0, DMAT)
				}
			} Else {
				Store (PIO1, DMAT)
			}
		} Else {
			And (CHNF, 0x02, IRDY)
			Store (PIO0, PIOT)
			If (And (CHNF, 0x01)) {
				Store (DMA0, DMAT)
			}
		}
		If (LAnd (LAnd (And (ID53, 0x04), And (ID88, 0xFF00)), DMAT)) {
			Store (Match (DerefOf (Index (TIM0, 0x01)), MLE, DMAT, MTR,
										0x00, 0x00), Local1)
			If (LGreater (Local1, 0x06)) {
				Store (0x06, Local1)
			}
			GTFB (AT01, Or (0x40, Local1), Local7)
		} Else {
			If (LAnd (And (ID63, 0xFF00), PIOT)) {
				And (Match (DerefOf (Index (TIM0, 0x00)), MGE, PIOT, MTR,
										0x00, 0x00), 0x07, Local0)
				If (Local0) {
					If (And (Local0, 0x04)) {
						Store (0x02, Local0)
					} Else {
						Store (0x01, Local0)
					}
				}
				Or (0x20, DerefOf (Index (DerefOf (Index (TIM0, 0x03)), Local0
												)), Local1)
				GTFB (AT01, Local1, Local7)
			}
		}
		If (IRDY) {
			And (Match (DerefOf (Index (TIM0, 0x00)), MGE, PIOT, MTR,
									0x00, 0x00), 0x07, Local0)
			Or (0x08, DerefOf (Index (DerefOf (Index (TIM0, 0x02)), Local0
											)), Local1)
			GTFB (AT01, Local1, Local7)
		} Else {
			If (And (ID49, 0x0400)) {
				GTFB (AT01, 0x01, Local7)
			}
		}
		If (LAnd (And (ID59, 0x0100), And (ID59, 0xFF))) {
			GTFB (AT03, And (ID59, 0xFF), Local7)
		}
		Store (ATAB, Debug)
		Return (ATAB)
	}
	Method (RATA, 1, NotSerialized) {
		CreateByteField (Arg0, 0x00, CMDN)
		Multiply (CMDN, 0x38, Local0)
		CreateField (Arg0, 0x08, Local0, RETB)
		Store (RETB, Debug)
		Return (RETB)
	}
}
Device (PMF) {
	// acpi smbus   it should be 0x00040003 if 8131 present
	Method (_ADR, 0, NotSerialized)
	{
		Return (DADD(\_SB.PCI0.SBDN, 0x00010003))
	}
	OperationRegion (BAR0, PCI_Config, 0x00, 0xff)
	Field (BAR0, ByteAcc, NoLock, Preserve) {
						Offset (0x56),
		PIRA,   4,
		PIRB,   4,
		PIRC,   4,
		PIRD,   4
	}
	//OperationRegion (TS3_, PCI_Config, 0xC4, 0x02)
	//Field (TS3_, DWordAcc, NoLock, Preserve) {
	//	PTS3,   16
	//}
}
