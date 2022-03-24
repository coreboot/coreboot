/* SPDX-License-Identifier: Apache-2.0 */
/* SPDX-License-Identifier: GPL-2.0-or-later */
/* See the file LICENSE for further information */

#ifndef _SIFIVE_UX00DDR_H
#define _SIFIVE_UX00DDR_H

#ifndef __ASSEMBLER__

#include <stdint.h>
#include <stddef.h>

#define _REG32(p, i) (*(volatile uint32_t *)((p) + (i)))

#define DRAM_CLASS_OFFSET                   8
#define DRAM_CLASS_DDR4                     0xA
#define OPTIMAL_RMODW_EN_OFFSET             0
#define DISABLE_RD_INTERLEAVE_OFFSET        16
#define OUT_OF_RANGE_OFFSET                 1
#define MULTIPLE_OUT_OF_RANGE_OFFSET        2
#define PORT_COMMAND_CHANNEL_ERROR_OFFSET   7
#define MC_INIT_COMPLETE_OFFSET             8
#define LEVELING_OPERATION_COMPLETED_OFFSET 22
#define DFI_PHY_WRLELV_MODE_OFFSET          24
#define DFI_PHY_RDLVL_MODE_OFFSET           24
#define DFI_PHY_RDLVL_GATE_MODE_OFFSET      0
#define VREF_EN_OFFSET                      24
#define PORT_ADDR_PROTECTION_EN_OFFSET      0
#define AXI0_ADDRESS_RANGE_ENABLE           8
#define AXI0_RANGE_PROT_BITS_0_OFFSET       24
#define RDLVL_EN_OFFSET                     16
#define RDLVL_GATE_EN_OFFSET                24
#define WRLVL_EN_OFFSET                     0

#define PHY_RX_CAL_DQ0_0_OFFSET             0
#define PHY_RX_CAL_DQ1_0_OFFSET             16

static inline void phy_reset(volatile uint32_t *ddrphyreg, const uint32_t *physettings) {
  unsigned int i;
  for (i=1152;i<=1214;i++) {
    uint32_t physet = physettings[i];
    /*if (physet!=0)*/ ddrphyreg[i] = physet;
  }
  for (i=0;i<=1151;i++) {
    uint32_t physet = physettings[i];
    /*if (physet!=0)*/ ddrphyreg[i] = physet;
  }
}

static inline void ux00ddr_writeregmap(size_t ahbregaddr, const uint32_t *ctlsettings, const uint32_t *physettings) {
  volatile uint32_t *ddrctlreg = (volatile uint32_t *) ahbregaddr;
  volatile uint32_t *ddrphyreg = ((volatile uint32_t *) ahbregaddr) + (0x2000 / sizeof(uint32_t));

  unsigned int i;
  for (i=0;i<=264;i++) {
    uint32_t ctlset = ctlsettings[i];
    /*if (ctlset!=0)*/ ddrctlreg[i] = ctlset;
  }

  phy_reset(ddrphyreg, physettings);
}

static inline void ux00ddr_start(size_t ahbregaddr, size_t filteraddr, size_t ddrend) {
  // START register at ddrctl register base offset 0
  uint32_t regdata = _REG32(0<<2, ahbregaddr);
  regdata |= 0x1;
  _REG32(0<<2, ahbregaddr) = regdata;
  // WAIT for initialization complete : bit 8 of INT_STATUS (DENALI_CTL_132) 0x210
  while ((_REG32(132<<2, ahbregaddr) & (1<<MC_INIT_COMPLETE_OFFSET)) == 0) {}

  // Disable the BusBlocker in front of the controller AXI slave ports
  volatile uint64_t *filterreg = (volatile uint64_t *)filteraddr;
  filterreg[0] = 0x0f00000000000000UL | (ddrend >> 2);
  //                ^^ RWX + TOR
}

static inline void ux00ddr_mask_mc_init_complete_interrupt(size_t ahbregaddr) {
  // Mask off Bit 8 of Interrupt Status
  // Bit [8] The MC initialization has been completed
  _REG32(136<<2, ahbregaddr) |= (1<<MC_INIT_COMPLETE_OFFSET);
}

static inline void ux00ddr_mask_outofrange_interrupts(size_t ahbregaddr) {
  // Mask off Bit 8, Bit 2 and Bit 1 of Interrupt Status
  // Bit [2] Multiple accesses outside the defined PHYSICAL memory space have occurred
  // Bit [1] A memory access outside the defined PHYSICAL memory space has occurred
  _REG32(136<<2, ahbregaddr) |= ((1<<OUT_OF_RANGE_OFFSET) | (1<<MULTIPLE_OUT_OF_RANGE_OFFSET));
}

static inline void ux00ddr_mask_port_command_error_interrupt(size_t ahbregaddr) {
  // Mask off Bit 7 of Interrupt Status
  // Bit [7] An error occurred on the port command channel
  _REG32(136<<2, ahbregaddr) |= (1<<PORT_COMMAND_CHANNEL_ERROR_OFFSET);
}

static inline void ux00ddr_mask_leveling_completed_interrupt(size_t ahbregaddr) {
  // Mask off Bit 22 of Interrupt Status
  // Bit [22] The leveling operation has completed
  _REG32(136<<2, ahbregaddr) |= (1<<LEVELING_OPERATION_COMPLETED_OFFSET);
}

static inline void ux00ddr_setuprangeprotection(size_t ahbregaddr, size_t end_addr) {
  _REG32(209<<2, ahbregaddr) = 0x0;
  size_t end_addr_16Kblocks = ((end_addr >> 14) & 0x7FFFFF)-1;
  _REG32(210<<2, ahbregaddr) = ((uint32_t) end_addr_16Kblocks);
  _REG32(212<<2, ahbregaddr) = 0x0;
  _REG32(214<<2, ahbregaddr) = 0x0;
  _REG32(216<<2, ahbregaddr) = 0x0;
  _REG32(224<<2, ahbregaddr) |= (0x3 << AXI0_RANGE_PROT_BITS_0_OFFSET);
  _REG32(225<<2, ahbregaddr) = 0xFFFFFFFF;
  _REG32(208<<2, ahbregaddr) |= (1 << AXI0_ADDRESS_RANGE_ENABLE);
  _REG32(208<<2, ahbregaddr) |= (1 << PORT_ADDR_PROTECTION_EN_OFFSET);

}

static inline void ux00ddr_disableaxireadinterleave(size_t ahbregaddr) {
  _REG32(120<<2, ahbregaddr) |= (1<<DISABLE_RD_INTERLEAVE_OFFSET);
}

static inline void ux00ddr_disableoptimalrmodw(size_t ahbregaddr) {
  _REG32(21<<2, ahbregaddr) &= (~(1<<OPTIMAL_RMODW_EN_OFFSET));
}

static inline void ux00ddr_enablewriteleveling(size_t ahbregaddr) {
  _REG32(170<<2, ahbregaddr) |= ((1<<WRLVL_EN_OFFSET) | (1<<DFI_PHY_WRLELV_MODE_OFFSET));
}

static inline void ux00ddr_enablereadleveling(size_t ahbregaddr) {
  _REG32(181<<2, ahbregaddr) |= (1<<DFI_PHY_RDLVL_MODE_OFFSET);
  _REG32(260<<2, ahbregaddr) |= (1<<RDLVL_EN_OFFSET);
}

static inline void ux00ddr_enablereadlevelinggate(size_t ahbregaddr) {
  _REG32(260<<2, ahbregaddr) |= (1<<RDLVL_GATE_EN_OFFSET);
  _REG32(182<<2, ahbregaddr) |= (1<<DFI_PHY_RDLVL_GATE_MODE_OFFSET);
}

static inline void ux00ddr_enablevreftraining(size_t ahbregaddr) {
  _REG32(184<<2, ahbregaddr) |= (1<<VREF_EN_OFFSET);
}

static inline uint32_t ux00ddr_getdramclass(size_t ahbregaddr) {
  return ((_REG32(0, ahbregaddr) >> DRAM_CLASS_OFFSET) & 0xF);
}

static inline uint64_t ux00ddr_phy_fixup(size_t ahbregaddr) {
  // return bitmask of failed lanes

  size_t ddrphyreg = ahbregaddr + 0x2000;

  uint64_t fails=0;
  uint32_t slicebase = 0;
  uint32_t dq = 0;

  // check errata condition
  for (uint32_t slice = 0; slice < 8; slice++) {
    uint32_t regbase = slicebase + 34;
    for (uint32_t reg = 0 ; reg < 4; reg++) {
      uint32_t updownreg = _REG32((regbase+reg)<<2, ddrphyreg);
      for (uint32_t bit = 0; bit < 2; bit++) {
        uint32_t phy_rx_cal_dqn_0_offset;

        if (bit==0) {
          phy_rx_cal_dqn_0_offset = PHY_RX_CAL_DQ0_0_OFFSET;
        }else{
          phy_rx_cal_dqn_0_offset = PHY_RX_CAL_DQ1_0_OFFSET;
        }

        uint32_t down = (updownreg >> phy_rx_cal_dqn_0_offset) & 0x3F;
        uint32_t up = (updownreg >> (phy_rx_cal_dqn_0_offset+6)) & 0x3F;

        uint8_t failc0 = ((down == 0) && (up == 0x3F));
        uint8_t failc1 = ((up == 0) && (down == 0x3F));

        // print error message on failure
        if (failc0 || failc1) {
          //if (fails==0) uart_puts((void*) UART0_CTRL_ADDR, "DDR error in fixing up \n");
          fails |= (1<<dq);
          /* char slicelsc = '0'; */
          /* char slicemsc = '0'; */
          /* slicelsc += (dq % 10); */
          /* slicemsc += (dq / 10); */
          //uart_puts((void*) UART0_CTRL_ADDR, "S ");
          //uart_puts((void*) UART0_CTRL_ADDR, &slicemsc);
          //uart_puts((void*) UART0_CTRL_ADDR, &slicelsc);
          //if (failc0) uart_puts((void*) UART0_CTRL_ADDR, "U");
          //else uart_puts((void*) UART0_CTRL_ADDR, "D");
          //uart_puts((void*) UART0_CTRL_ADDR, "\n");
        }
        dq++;
      }
    }
    slicebase+=128;
  }
  return (0);
}

#endif

#endif
