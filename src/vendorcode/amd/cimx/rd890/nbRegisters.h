/**
 * @file
 *
 *  Registers definition.
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      CIMx-NB
 * @e sub-project:
 * @e \$Revision:$   @e \$Date:$
 *
 */
/*****************************************************************************
 *
 * Copyright (C) 2012 Advanced Micro Devices, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of Advanced Micro Devices, Inc. nor the names of
 *       its contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL ADVANCED MICRO DEVICES, INC. BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *
 ***************************************************************************/

#ifndef _NBREGISTERS_H_
#define _NBREGISTERS_H_

#define NB_PCI_REG04   0x04
#define NB_PCI_REG1C   0x1C
#define NB_PCI_REG44   0x44
#define NB_PCI_REG4C   0x4C
#define NB_PCI_REG4E   0x4E
#define NB_PCI_REG50   0x50
#define NB_PCI_REG54   0x54
#define NB_PCI_REG58   0x58
#define NB_PCI_REG5C   0x5c
#define NB_PCI_REG60   0x60
#define NB_PCI_REG78   0x78
#define NB_PCI_REG7C   0x7C
#define NB_PCI_REG7F   0x7f
#define NB_PCI_REG95   0x95
#define NB_PCI_REG97   0x97
#define NB_PCI_REG80   0x80
#define NB_PCI_REG84   0x84
#define NB_PCI_REG8C   0x8C
#define NB_PCI_REG8D   0x8D
#define NB_PCI_REG90   0x90
#define NB_PCI_REG94   0x94
#define NB_PCI_REG98   0x98
#define NB_PCI_REG9C   0x9C
#define NB_PCI_REGA0   0xA0
#define NB_PCI_REGA4   0xA4
#define NB_PCI_REGA6   0xA6
#define NB_PCI_REGA7   0xA7
#define NB_PCI_REGA8   0xA8
#define NB_PCI_REGAC   0xAC
#define NB_PCI_REGC8   0xC8
#define NB_PCI_REGCB   0xCB
#define NB_PCI_REGD1   0xD1
#define NB_PCI_REGD2   0xD2
#define NB_PCI_REGE0   0xE0
#define NB_PCI_REGE8   0xE8
#define NB_PCI_REGF8   0xF8
#define NB_PCI_REGFC   0xFC

#define NB_APC_REG04   0x04
#define NB_APC_REG18   0x18

/******************************************************************************************
; PCIE Port PCI config registers
;******************************************************************************************/

#define NB_PCIP_REG04   0x04
#define NB_PCIP_REG18   0x18
#define NB_PCIP_REG19   0x19
#define NB_PCIP_REG20   0x20
#define NB_PCIP_REG24   0x24
#define NB_PCIP_REG3D   0x3D
#define NB_PCIP_REG5A   0x5A
#define NB_PCIP_REG64   0x64
#define NB_PCIP_REG68   0x68
#define NB_PCIP_REG6A   0x6A
#define NB_PCIP_REG6B   0x6b
#define NB_PCIP_REG6C   0x6c
#define NB_PCIP_REG70   0x70
#define NB_PCIP_REG72   0x72
#define NB_PCIP_REG80   0x80
#define NB_PCIP_REG88   0x88
#define NB_PCIP_REGE0   0xE0
#define NB_PCIP_REG108  0x108
#define NB_PCIP_REG124  0x124
#define NB_PCIP_REG12A  0x12A
#define NB_PCIP_REG130  0x130
#define NB_PCIP_REG134  0x134


/******************************************************************************************
; HTIUNBIND register definition
;******************************************************************************************/

#define NB_HTIU_INDEX   NB_PCI_REG94
#define HTIU_WRITE      0x100

#define NB_HTIU_REG05   (0x05 | HTIU_WRITE)
#define NB_HTIU_REG06   (0x06 | HTIU_WRITE)
#define NB_HTIU_REG07   (0x07 | HTIU_WRITE)
#define NB_HTIU_REG0C   (0x0C | HTIU_WRITE)
#define NB_HTIU_REG12   (0x12 | HTIU_WRITE)
#define NB_HTIU_REG15   (0x15 | HTIU_WRITE)
#define NB_HTIU_REG16   (0x16 | HTIU_WRITE)
#define NB_HTIU_REG17   (0x17 | HTIU_WRITE)
#define NB_HTIU_REG19   (0x19 | HTIU_WRITE)
#define NB_HTIU_REG1A   (0x1A | HTIU_WRITE)
#define NB_HTIU_REG1C   (0x1C | HTIU_WRITE)
#define NB_HTIU_REG1D   (0x1D | HTIU_WRITE)
#define NB_HTIU_REG1E   (0x1E | HTIU_WRITE)
#define NB_HTIU_REG2A   (0x2A | HTIU_WRITE)
#define NB_HTIU_REG2D   (0x2D | HTIU_WRITE)
#define NB_HTIU_REG30   (0x30 | HTIU_WRITE)
#define NB_HTIU_REG31   (0x31 | HTIU_WRITE)
#define NB_HTIU_REG32   (0x32 | HTIU_WRITE)
#define NB_HTIU_REG34   (0x34 | HTIU_WRITE)
#define NB_HTIU_REG37   (0x37 | HTIU_WRITE)
#define NB_HTIU_REG3A   (0x3A | HTIU_WRITE)
#define NB_HTIU_REG3B   (0x3B | HTIU_WRITE)
#define NB_HTIU_REG3C   (0x3C | HTIU_WRITE)
#define NB_HTIU_REG46   (0x46 | HTIU_WRITE)
#define NB_HTIU_REG4B   (0x4B | HTIU_WRITE)
#define NB_HTIU_REG50   (0x50 | HTIU_WRITE)
#define NB_HTIU_REG55   (0x55 | HTIU_WRITE)
#define NB_HTIU_REG56   (0x56 | HTIU_WRITE)
#define NB_HTIU_REG57   (0x57 | HTIU_WRITE)
#define NB_HTIU_REG58   (0x58 | HTIU_WRITE)
#define NB_HTIU_REG59   (0x59 | HTIU_WRITE)
#define NB_HTIU_REG5A   (0x5A | HTIU_WRITE)
#define NB_HTIU_REG5C   (0x5C | HTIU_WRITE)
#define NB_HTIU_REG5D   (0x5D | HTIU_WRITE)
#define NB_HTIU_REG5E   (0x5E | HTIU_WRITE)
#define NB_HTIU_REG5F   (0x5F | HTIU_WRITE)
#define NB_HTIU_REG60   (0x60 | HTIU_WRITE)
#define NB_HTIU_REG61   (0x61 | HTIU_WRITE)
#define NB_HTIU_REG62   (0x62 | HTIU_WRITE)
#define NB_HTIU_REG63   (0x63 | HTIU_WRITE)
#define NB_HTIU_REG64   (0x64 | HTIU_WRITE)
#define NB_HTIU_REG65   (0x65 | HTIU_WRITE)
#define NB_HTIU_REG66   (0x66 | HTIU_WRITE)
#define NB_HTIU_REG68   (0x68 | HTIU_WRITE)
#define NB_HTIU_REG6B   (0x6B | HTIU_WRITE)
#define NB_HTIU_REG6D   (0x6D | HTIU_WRITE)
#define NB_HTIU_REG70   (0x70 | HTIU_WRITE)
#define NB_HTIU_REG71   (0x71 | HTIU_WRITE)
#define NB_HTIU_REG72   (0x72 | HTIU_WRITE)
#define NB_HTIU_REG73   (0x73 | HTIU_WRITE)
#define NB_HTIU_REG74   (0x74 | HTIU_WRITE)
#define NB_HTIU_REG75   (0x75 | HTIU_WRITE)
#define NB_HTIU_REG87   (0x87 | HTIU_WRITE)
#define NB_HTIU_REG88   (0x88 | HTIU_WRITE)
#define NB_HTIU_REGA8   (0xA8 | HTIU_WRITE)
#define NB_HTIU_REGA9   (0xA9 | HTIU_WRITE)

/******************************************************************************************
; Clock Configuration register
;******************************************************************************************/
#define NB_CLK_REG48    0x48
#define NB_CLK_REG4C    0x4C
#define NB_CLK_REG5C    0x5C
#define NB_CLK_REG60    0x60
#define NB_CLK_REG78    0x78
#define NB_CLK_REG84    0x84
#define NB_CLK_REG8C    0x8C
#define NB_CLK_REG90    0x90
#define NB_CLK_REG94    0x94
#define NB_CLK_REGB0    0xB0
#define NB_CLK_REGB4    0xB4
#define NB_CLK_REGCC    0xCC
#define NB_CLK_REGD4    0xD4
#define NB_CLK_REGD5    0xD5
#define NB_CLK_REGD6    0xD6
#define NB_CLK_REGD8    0xD8
#define NB_CLK_REGE0    0xE0
#define NB_CLK_REGE4    0xE4
#define NB_CLK_REGE8    0xE8
#define NB_CLK_REGF0    0xF0
#define NB_CLK_REGF4    0xF4
#define NB_CLK_REGF8    0xF8
#define NB_CLK_REGF9    0xF9
#define NB_CLK_REGFA    0xFA
#define NB_CLK_REGFB    0xFB

/******************************************************************************************
; MISCIND/NBCFG register definition
;******************************************************************************************/
#define NB_MISC_INDEX   NB_PCI_REG60
#define MISC_WRITE      0x80

#define NB_MISC_REG00   (0x00 | MISC_WRITE)
#define NB_MISC_REG01   (0x01 | MISC_WRITE)
#define NB_MISC_REG07   (0x07 | MISC_WRITE)
#define NB_MISC_REG08   (0x08 | MISC_WRITE)
#define NB_MISC_REG0B   (0x0B | MISC_WRITE)
#define NB_MISC_REG0C   (0x0C | MISC_WRITE)
#define NB_MISC_REG12   (0x12 | MISC_WRITE)
#define NB_MISC_REG1E   (0x1E | MISC_WRITE)
#define NB_MISC_REG1F   (0x1F | MISC_WRITE)
#define NB_MISC_REG20   (0x20 | MISC_WRITE)
#define NB_MISC_REG21   (0x21 | MISC_WRITE)
#define NB_MISC_REG22   (0x22 | MISC_WRITE)
#define NB_MISC_REG23   (0x23 | MISC_WRITE)
#define NB_MISC_REG24   (0x24 | MISC_WRITE)
#define NB_MISC_REG26   (0x26 | MISC_WRITE)
#define NB_MISC_REG27   (0x27 | MISC_WRITE)
#define NB_MISC_REG28   (0x28 | MISC_WRITE)
#define NB_MISC_REG29   (0x29 | MISC_WRITE)
#define NB_MISC_REG2A   (0x2A | MISC_WRITE)
#define NB_MISC_REG2B   (0x2B | MISC_WRITE)
#define NB_MISC_REG2C   (0x2C | MISC_WRITE)
#define NB_MISC_REG2D   (0x2D | MISC_WRITE)
#define NB_MISC_REG2E   (0x2E | MISC_WRITE)
#define NB_MISC_REG2F   (0x2F | MISC_WRITE)
#define NB_MISC_REG32   (0x32 | MISC_WRITE)
#define NB_MISC_REG33   (0x33 | MISC_WRITE)
#define NB_MISC_REG34   (0x34 | MISC_WRITE)
#define NB_MISC_REG35   (0x35 | MISC_WRITE)
#define NB_MISC_REG36   (0x36 | MISC_WRITE)
#define NB_MISC_REG37   (0x37 | MISC_WRITE)
#define NB_MISC_REG38   (0x38 | MISC_WRITE)
#define NB_MISC_REG39   (0x39 | MISC_WRITE)
#define NB_MISC_REG3A   (0x3A | MISC_WRITE)
#define NB_MISC_REG3B   (0x3B | MISC_WRITE)
#define NB_MISC_REG3C   (0x3C | MISC_WRITE)
#define NB_MISC_REG40   (0x40 | MISC_WRITE)
#define NB_MISC_REG48   (0x48 | MISC_WRITE)
#define NB_MISC_REG49   (0x49 | MISC_WRITE)
#define NB_MISC_REG4A   (0x4A | MISC_WRITE)
#define NB_MISC_REG4B   (0x4B | MISC_WRITE)
#define NB_MISC_REG4E   (0x4E | MISC_WRITE)
#define NB_MISC_REG4F   (0x4F | MISC_WRITE)
#define NB_MISC_REG51   (0x51 | MISC_WRITE)
#define NB_MISC_REG53   (0x53 | MISC_WRITE)
#define NB_MISC_REG55   (0x55 | MISC_WRITE)
#define NB_MISC_REG57   (0x57 | MISC_WRITE)
#define NB_MISC_REG59   (0x59 | MISC_WRITE)
#define NB_MISC_REG5B   (0x5B | MISC_WRITE)
#define NB_MISC_REG5D   (0x5D | MISC_WRITE)
#define NB_MISC_REG5F   (0x5F | MISC_WRITE)
#define NB_MISC_REG61   (0x61 | MISC_WRITE)
#define NB_MISC_REG63   (0x63 | MISC_WRITE)
#define NB_MISC_REG66   (0x66 | MISC_WRITE)
#define NB_MISC_REG67   (0x67 | MISC_WRITE)
#define NB_MISC_REG68   (0x68 | MISC_WRITE)
#define NB_MISC_REG69   (0x69 | MISC_WRITE)
#define NB_MISC_REG6A   (0x6A | MISC_WRITE)
#define NB_MISC_REG6B   (0x6B | MISC_WRITE)
#define NB_MISC_REG6C   (0x6C | MISC_WRITE)
#define NB_MISC_REG6F   (0x6f | MISC_WRITE)
#define NB_MISC_REG74   (0x74 | MISC_WRITE)
#define NB_MISC_REG75   (0x75 | MISC_WRITE)
#define NB_MISC_REG76   (0x76 | MISC_WRITE)
#define NB_MISC_REG7D   (0x7D | MISC_WRITE)

/******************************************************************************************
; MISCIND/NBCFG register definition
;******************************************************************************************/
#define NB_MC_INDEX    NB_PCI_REGE8
#define MC_WRITE       0x200

#define NB_MC_REG01    (0x01 | MC_WRITE)
#define NB_MC_REG02    (0x02 | MC_WRITE)
#define NB_MC_REG04    (0x04 | MC_WRITE)
#define NB_MC_REG05    (0x05 | MC_WRITE)
#define NB_MC_REG06    (0x06 | MC_WRITE)
#define NB_MC_REG07    (0x07 | MC_WRITE)
#define NB_MC_REG08    (0x08 | MC_WRITE)
#define NB_MC_REG09    (0x09 | MC_WRITE)
#define NB_MC_REG0B    (0x0B | MC_WRITE)
#define NB_MC_REG0C    (0x0C | MC_WRITE)
#define NB_MC_REG0D    (0x0D | MC_WRITE)
#define NB_MC_REG0E    (0x0E | MC_WRITE)
#define NB_MC_REG0F    (0x0F | MC_WRITE)
#define NB_MC_REG10    (0x10 | MC_WRITE)
#define NB_MC_REG11    (0x11 | MC_WRITE)
#define NB_MC_REG12    (0x12 | MC_WRITE)
#define NB_MC_REG13    (0x13 | MC_WRITE)
#define NB_MC_REG14    (0x14 | MC_WRITE)
#define NB_MC_REG16    (0x16 | MC_WRITE)
#define NB_MC_REG23    (0x23 | MC_WRITE)
#define NB_MC_REG25    (0x25 | MC_WRITE)
#define NB_MC_REG29    (0x29 | MC_WRITE)
#define NB_MC_REG2A    (0x2A | MC_WRITE)
#define NB_MC_REG2C    (0x2C | MC_WRITE)
#define NB_MC_REG30    (0x30 | MC_WRITE)
#define NB_MC_REG3C    (0x3C | MC_WRITE)
#define NB_MC_REG3D    (0x3D | MC_WRITE)
#define NB_MC_REG49    (0x49 | MC_WRITE)
#define NB_MC_REG4A    (0x4A | MC_WRITE)
#define NB_MC_REG4B    (0x4B | MC_WRITE)
#define NB_MC_REG4C    (0x4C | MC_WRITE)
#define NB_MC_REG4D    (0x4D | MC_WRITE)
#define NB_MC_REGA0    (0xA0 | MC_WRITE)
#define NB_MC_REGA1    (0xA1 | MC_WRITE)
#define NB_MC_REGA2    (0xA2 | MC_WRITE)
#define NB_MC_REGA3    (0xA3 | MC_WRITE)
#define NB_MC_REGA4    (0xA4 | MC_WRITE)
#define NB_MC_REGA5    (0xA5 | MC_WRITE)
#define NB_MC_REGA6    (0xA6 | MC_WRITE)
#define NB_MC_REGA7    (0xA7 | MC_WRITE)
#define NB_MC_REGA8    (0xA8 | MC_WRITE)
#define NB_MC_REGAA    (0xAA | MC_WRITE)
#define NB_MC_REGAF    (0xAF | MC_WRITE)
#define NB_MC_REGB0    (0xB0 | MC_WRITE)
#define NB_MC_REGB2    (0xB2 | MC_WRITE)
#define NB_MC_REGB1    (0xB1 | MC_WRITE)
#define NB_MC_REGB4    (0xB4 | MC_WRITE)
#define NB_MC_REGB5    (0xB5 | MC_WRITE)
#define NB_MC_REGB6    (0xB6 | MC_WRITE)
#define NB_MC_REGB7    (0xB7 | MC_WRITE)
#define NB_MC_REGB8    (0xB8 | MC_WRITE)
#define NB_MC_REGB9    (0xB9 | MC_WRITE)
#define NB_MC_REGBA    (0xBA | MC_WRITE)
#define NB_MC_REGC1    (0xC1 | MC_WRITE)
#define NB_MC_REGC2    (0xC2 | MC_WRITE)
#define NB_MC_REGC3    (0xC3 | MC_WRITE)
#define NB_MC_REGC4    (0xC4 | MC_WRITE)
#define NB_MC_REGC5    (0xC5 | MC_WRITE)
#define NB_MC_REGC8    (0xC8 | MC_WRITE)
#define NB_MC_REGC9    (0xC9 | MC_WRITE)
#define NB_MC_REGCA    (0xCA | MC_WRITE)
#define NB_MC_REGCB    (0xCB | MC_WRITE)
#define NB_MC_REGCC    (0xCC | MC_WRITE)
#define NB_MC_REGCE    (0xCE | MC_WRITE)
#define NB_MC_REGD0    (0xD0 | MC_WRITE)
#define NB_MC_REGD2    (0xD2 | MC_WRITE)
#define NB_MC_REGD3    (0xD3 | MC_WRITE)
#define NB_MC_REGD6    (0xD6 | MC_WRITE)
#define NB_MC_REGD7    (0xD7 | MC_WRITE)
#define NB_MC_REGD8    (0xD8 | MC_WRITE)
#define NB_MC_REGD9    (0xD9 | MC_WRITE)
#define NB_MC_REGE0    (0xE0 | MC_WRITE)
#define NB_MC_REGE1    (0xE1 | MC_WRITE)
#define NB_MC_REGE8    (0xE8 | MC_WRITE)
#define NB_MC_REGE9    (0xE9 | MC_WRITE)
#define NB_MC_REGF0    (0xF0 | MC_WRITE)

/******************************************************************************************
; PCIEIND_P(BIFNBP) register definition
;******************************************************************************************/
#define NB_BIF_INDEX   NB_PCI_REGE0

#define NB_BIFNBP_REG01   0x01
#define NB_BIFNBP_REG02   0x02
#define NB_BIFNBP_REG10   0x10
#define NB_BIFNBP_REG20   0x20
#define NB_BIFNBP_REG40   0x40
#define NB_BIFNBP_REG50   0x50
#define NB_BIFNBP_REG70   0x70
#define NB_BIFNBP_REGA0   0xA0
#define NB_BIFNBP_REGA1   0xA1
#define NB_BIFNBP_REGA2   0xA2
#define NB_BIFNBP_REGA3   0xA3
#define NB_BIFNBP_REGA4   0xA4
#define NB_BIFNBP_REGA5   0xA5
#define NB_BIFNBP_REGB1   0xB1
#define NB_BIFNBP_REGC0   0xC0
#define NB_BIFNBP_REGC1   0xC1

/******************************************************************************************
; PCIEIND(BIFNB) register definition
;******************************************************************************************/
#define NB_BIFNB_REG01   0x01
#define NB_BIFNB_REG02   0x02
#define NB_BIFNB_REG10   0x10
#define NB_BIFNB_REG11   0x11
#define NB_BIFNB_REG1C   0x1C
#define NB_BIFNB_REG20   0x20
#define NB_BIFNB_REG40   0x40
#define NB_BIFNB_REG65   0x65
#define NB_BIFNB_REGC1   0xC1
#define NB_BIFNB_REGC2   0xC2
#define NB_BIFNB_REGF9   0xF9


#define MC_CLK_INDEX     0x60

/******************************************************************************************
; IOAPICCFG register definition
;******************************************************************************************/
#define NB_IOAPICCFG_INDEX    NB_PCI_REGF8

#define NB_IOAPICCFG_REG00   0x00
#define NB_IOAPICCFG_REG01   0x01
#define NB_IOAPICCFG_REG02   0x02
#define NB_IOAPICCFG_REG03   0x03
#define NB_IOAPICCFG_REG04   0x04
#define NB_IOAPICCFG_REG05   0x05
#define NB_IOAPICCFG_REG06   0x06
#define NB_IOAPICCFG_REG07   0x07
#define NB_IOAPICCFG_REG08   0x08
#define NB_IOAPICCFG_REG09   0x09
#define NB_IOAPICCFG_REG0A   0x0A

#endif
