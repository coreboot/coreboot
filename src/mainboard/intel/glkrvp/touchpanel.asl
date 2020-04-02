
/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

Scope(\_SB.PCI0.I2C7) {
// Touch Panels on I2C7
// GPIO_212:TCH_PNL_INTR_LS_N     North Community, IRQ number 0x75.
//------------------------
  Device (TPL1) {
    Name (HID2, 1)
    Name (_HID, "WCOM508E")  // _HID: Hardware ID
    Name (_CID, "PNP0C50")  // _CID: Compatible ID
    Name (_S0W, 0x04)  // _S0W: S0 Device Wake State
    Name (SBFB, ResourceTemplate () {
      I2cSerialBus (
        0x000A,
        ControllerInitiated,
        1000000,
        AddressingMode7Bit,
        "\\_SB.PCI0.I2C7",
        0x00,
        ResourceConsumer,
        ,
        )
    })
    //
    // GLK: Touchpanel Interrupt: GPIO_212: Northwest
    // Pin 77
    // Direct IRQ 0x75
    //
    Name (SBFG, ResourceTemplate () {
      GpioInt (Level, ActiveLow, Exclusive, PullUp, 0x0000,
        "\\_SB.GPO0", 0x00, ResourceConsumer, ,
        )
        {
            77
        }
    })
    Name (SBFI, ResourceTemplate () {
      Interrupt (ResourceConsumer, Level, ActiveLow, Exclusive, ,, )
      {
        0x75,
      }
    })

    Method (_INI, 0, NotSerialized)  // _INI: Initialize
    {
    }

    Method (_STA, 0, NotSerialized)  // _STA: Status
    {
        Return (0x0F)
    }

    Method (_CRS, 0, NotSerialized) {
      Return (ConcatenateResTemplate(SBFB, SBFG))
    }
  } // Device (TPL0)
}
