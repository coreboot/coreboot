/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2009 coresystems GmbH
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

OperationRegion (PRR0, PCI_Config, 0x00, 0x100)
Field (PRR0, AnyAcc, NoLock, Preserve) {
  Offset(0x60),
  PIRA, 8,
  PIRB, 8,
  PIRC, 8,
  PIRD, 8,
  Offset(0x68),
  PIRE, 8,
  PIRF, 8,
  PIRG, 8,
  PIRH, 8
}

Device (LNKA) {      // PCI IRQ link A
  Name (_HID,EISAID("PNP0C0F"))
  //Name(_UID, 1)
  Method (_STA,0,NotSerialized) {
    If(And(PIRA, 0x80)) {
      Return (0x9)
    } Else {
      Return (0xB)
    } // Don't display
  }

  Method (_DIS,0,NotSerialized) {
    Or (PIRA, 0x80, PIRA)
  }

  Method (_CRS,0,Serialized) {
    Name (BUF0, ResourceTemplate() {IRQ(Level,ActiveLow,Shared){0}})
    //
    // Define references to buffer elements
    //
    CreateWordField (BUF0, 0x01, IRQW)  // IRQ low
    //
    // Write current settings into IRQ descriptor
    //
    If (And(PIRA, 0x80)) {
      Store (Zero, Local0)
    } Else {
      Store (One,Local0)
    }
    //
    // Shift 1 by value in register 70, Save in buffer
    //
    ShiftLeft (Local0,And (PIRA,0x0F),IRQW)       // Save in buffer
    Return (BUF0)                                 // Return Buf0
  }                                               // End of _CRS method

  Name (_PRS, ResourceTemplate()
        {IRQ(Level,ActiveLow,Shared){3,4,5,6,7,9,10,11,12,14,15}})

    Method (_SRS,1,NotSerialized) {
      CreateWordField (ARG0, 0x01, IRQW)  // IRQ low

      FindSetRightBit(IRQW,Local0)          // Set IRQ
      If (LNotEqual (IRQW,Zero)){
        And (Local0, 0x7F,Local0)
        Decrement (Local0)
      } Else {
        Or (Local0, 0x80,Local0)
      }
      Store (Local0, PIRA)
    }   // End of _SRS Method
}

Device(LNKB) {  // PCI IRQ link B
  Name (_HID,EISAID("PNP0C0F"))
  //Name(_UID, 2)
  Method (_STA,0,NotSerialized) {
    If (And (PIRB, 0x80)) {
      Return (0x9)
    } Else {
      Return (0xB)
    } // Don't display
  }

  Method (_DIS,0,NotSerialized) {
    Or (PIRB, 0x80,PIRB)
  }

  Method (_CRS,0,Serialized) {
    Name(BUF0, ResourceTemplate()
        {IRQ(Level,ActiveLow,Shared){0}})
    //
    // Define references to buffer elements
    //
    CreateWordField (BUF0, 0x01, IRQW)  // IRQ low
    //
    // Write current settings into IRQ descriptor
    //
    If (And (PIRB, 0x80)) {
      Store (Zero, Local0)
    } Else {
      Store (One,Local0)
    }
    //
    // Shift 1 by value in register 70, Save in buffer
    //
    ShiftLeft (Local0,And (PIRB,0x0F),IRQW)   // Save in buffer
    Return (BUF0)               // Return Buf0
  }                             // End of _CRS method

  Name (_PRS,
      ResourceTemplate()
      {IRQ(Level,ActiveLow,Shared){3,4,5,6,7,9,10,11,12,14,15}})

  Method (_SRS,1,NotSerialized) {
    CreateWordField (ARG0, 0x01, IRQW)      // IRQ low

    FindSetRightBit(IRQW,Local0) // Set IRQ
    If (LNotEqual(IRQW,Zero)) {
      And (Local0, 0x7F, Local0)
      Decrement (Local0)
    } Else  {
      Or (Local0, 0x80, Local0)
    }
    Store (Local0, PIRB)
  }   // End of _SRS Method
}

Device(LNKC) {  // PCI IRQ link C
  Name(_HID, EISAID("PNP0C0F"))
  //Name(_UID, 3)

  Method (_STA,0,NotSerialized) {
    If (And (PIRC, 0x80)) {
      Return (0x9)
    } Else {
      Return (0xB)
    } // Don't display
  }

  Method (_DIS, 0, NotSerialized) {
    Or (PIRC, 0x80, PIRC)
  }

  Method (_CRS, 0, Serialized) {
    Name (BUF0, ResourceTemplate()
        {IRQ(Level,ActiveLow,Shared){0}})
    //
    // Define references to buffer elements
    //
    CreateWordField (BUF0, 0x01, IRQW)  // IRQ low
    //
    // Write current settings into IRQ descriptor
    //
    If (And (PIRC, 0x80)) {
      Store (Zero, Local0)
    } Else {
      Store (One,Local0)
    }
    //
    // Shift 1 by value in register 70, Save in buffer
    //
    ShiftLeft (Local0,And (PIRC,0x0F),IRQW)
    Return (BUF0)
  } // End of _CRS method

  Name (_PRS, ResourceTemplate()
      {IRQ(Level,ActiveLow,Shared){3,4,5,6,7,9,10,11,12,14,15}})

  Method (_SRS,1,NotSerialized) {
    CreateWordField (ARG0, 0x01, IRQW)  // IRQ low
    FindSetRightBit(IRQW,Local0)        // Set IRQ
    If (LNotEqual (IRQW,Zero)) {
      And (Local0, 0x7F, Local0)
      Decrement (Local0)
    } Else {
      Or (Local0, 0x80,Local0)
    }
    Store (Local0, PIRC)
  }   // End of _SRS Method
}

Device (LNKD) {  // PCI IRQ link D
  Name (_HID,EISAID ("PNP0C0F"))

  //Name(_UID, 4)

  Method (_STA, 0, NotSerialized) {
    If (And (PIRD, 0x80)) {
      Return (0x9)
    } Else  {
      Return (0xB)
    }    // Don't display
  }

  Method (_DIS, 0, NotSerialized) {
    Or(PIRD, 0x80,PIRD)
  }

  Method (_CRS,0,Serialized) {
    Name (BUF0, ResourceTemplate()
        {IRQ(Level,ActiveLow,Shared){0}})
    //
    // Define references to buffer elements
    //
    CreateWordField (BUF0, 0x01, IRQW)  // IRQ low
    //
    // Write current settings into IRQ descriptor
    //
    If (And (PIRD, 0x80)) {
      Store (Zero, Local0)
    } Else {
      Store (One,Local0)
    }
    //
    // Shift 1 by value in register 70, Save in buffer
    //
    ShiftLeft (Local0, And (PIRD,0x0F), IRQW)
    Return (BUF0)  // Return Buf0
  } // End of _CRS method

  Name (_PRS, ResourceTemplate()
      {IRQ(Level,ActiveLow,Shared){3,4,5,6,7,9,10,11,12,14,15}})

  Method (_SRS,1,NotSerialized) {
    CreateWordField (ARG0, 0x01, IRQW)  // IRQ low
    FindSetRightBit (IRQW, Local0)// Set IRQ
    If (LNotEqual (IRQW, Zero)) {
        And (Local0, 0x7F, Local0)
        Decrement (Local0)
    } Else {
        Or (Local0, 0x80, Local0)
    }
    Store(Local0, PIRD)
  }  // End of _SRS Method
}

Device(LNKE) {  // PCI IRQ link E
  Name(_HID,EISAID("PNP0C0F"))

  //Name(_UID, 5)

  Method (_STA,0,NotSerialized) {
    If (And (PIRE, 0x80)) {
      Return(0x9)
    } Else  {
      Return(0xB)
    }    // Don't display
  }

  Method (_DIS,0,NotSerialized) {
    Or (PIRE, 0x80, PIRE)
  }

  Method (_CRS, 0, Serialized) {
    Name (BUF0, ResourceTemplate()
        {IRQ(Level,ActiveLow,Shared){0}})
    //
    // Define references to buffer elements
    //
    CreateWordField (BUF0, 0x01, IRQW)  // IRQ low
    //
    // Write current settings into IRQ descriptor
    //
    If (And (PIRE, 0x80)) {
      Store (Zero, Local0)
    } Else {
      Store (One, Local0)
    }
    //
    // Shift 1 by value in register 70, Save in buffer
    //
    ShiftLeft (Local0, And (PIRE,0x0F), IRQW)
    Return (BUF0) // Return Buf0
  }  // End of _CRS method

  Name(_PRS, ResourceTemplate()
      {IRQ(Level,ActiveLow,Shared){3,4,5,6,7,9,10,11,12,14,15}})

  Method (_SRS,1,NotSerialized) {
    CreateWordField (ARG0, 0x01, IRQW)  // IRQ low
    FindSetRightBit (IRQW, Local0)      // Set IRQ
    If (LNotEqual (IRQW, Zero)) {
        And (Local0, 0x7F, Local0)
        Decrement (Local0)
    } Else  {
        Or (Local0, 0x80, Local0)
    }
    Store (Local0, PIRE)
  }   // End of _SRS Method
}

Device(LNKF) { // PCI IRQ link F
  Name (_HID,EISAID("PNP0C0F"))

  //Name(_UID, 6)

  Method (_STA,0,NotSerialized) {
    If (And (PIRF, 0x80)) {
      Return (0x9)
    } Else {
      Return (0xB)
    }    // Don't display
  }

  Method (_DIS,0,NotSerialized) {
    Or (PIRB, 0x80, PIRF)
  }

  Method (_CRS,0,Serialized) {
    Name(BUF0, ResourceTemplate()
        {IRQ(Level,ActiveLow,Shared){0}})
    //
    // Define references to buffer elements
    //
    CreateWordField (BUF0, 0x01, IRQW)  // IRQ low
    //
    // Write current settings into IRQ descriptor
    //
    If (And (PIRF, 0x80)) {
      Store (Zero, Local0)
    } Else {
      Store (One, Local0)
    }
    //
    // Shift 1 by value in register 70, Save in buffer
    //
    ShiftLeft (Local0, And (PIRF, 0x0F),IRQW)
    Return (BUF0)
  }  // End of _CRS method

  Name(_PRS, ResourceTemplate()
      {IRQ(Level,ActiveLow,Shared){3,4,5,6,7,9,10,11,12,14,15}})

  Method (_SRS,1,NotSerialized) {
    CreateWordField (ARG0, 0x01, IRQW)      // IRQ low
    FindSetRightBit (IRQW,Local0)           // Set IRQ
    If (LNotEqual (IRQW,Zero)) {
      And (Local0, 0x7F,Local0)
      Decrement (Local0)
    } Else {
      Or (Local0, 0x80, Local0)
    }
    Store (Local0, PIRF)
  }  // End of _SRS Method
}

Device(LNKG) { // PCI IRQ link G
    Name(_HID,EISAID("PNP0C0F"))
    //Name(_UID, 7)
    Method(_STA,0,NotSerialized) {
      If (And (PIRG, 0x80)) {
        Return (0x9)
      } Else  {
        Return (0xB)
      }    // Don't display
    }

    Method (_DIS, 0, NotSerialized) {
      Or(PIRG, 0x80,PIRG)
    }

    Method (_CRS,0,Serialized){
      Name(BUF0,ResourceTemplate()
          {IRQ(Level,ActiveLow,Shared){0}})
      //
      // Define references to buffer elements
      //
      CreateWordField (BUF0, 0x01, IRQW)  // IRQ low
      //
      // Write current settings into IRQ descriptor
      //
      If (And(PIRG, 0x80)) {
          Store(Zero, Local0)
      } Else {
          Store(One,Local0)
      }
      //
      // Shift 1 by value in register 70, Save in buffer
      //
      ShiftLeft (Local0,And(PIRG,0x0F),IRQW)
      Return (BUF0)
    }  // End of _CRS method

    Name (_PRS, ResourceTemplate()
        {IRQ(Level,ActiveLow,Shared){3,4,5,6,7,9,10,11,12,14,15}})

    Method (_SRS,1,NotSerialized) {
      CreateWordField (ARG0, 0x01, IRQW)  // IRQ low
      FindSetRightBit(IRQW,Local0)        // Set IRQ
      If (LNotEqual (IRQW,Zero)) {
        And (Local0, 0x7F,Local0)
        Decrement (Local0)
      } Else {
        Or (Local0, 0x80,Local0)
      }
      Store (Local0, PIRG)
    }  // End of _SRS Method
}

Device(LNKH) { // PCI IRQ link H
    Name (_HID,EISAID("PNP0C0F"))

    //Name(_UID, 8)

    Method (_STA,0,NotSerialized) {
      If (And(PIRH, 0x80)) {
        Return(0x9)
      } Else    {
        Return(0xB)
      }    // Don't display
    }

    Method (_DIS,0,NotSerialized) {
      Or(PIRH, 0x80,PIRH)
    }

    Method (_CRS,0,Serialized) {
      Name(BUF0, ResourceTemplate()
          {IRQ(Level,ActiveLow,Shared){0}})
      //
      // Define references to buffer elements
      //
      CreateWordField (BUF0, 0x01, IRQW)  // IRQ low
      //
      // Write current settings into IRQ descriptor
      //
      If (And (PIRH, 0x80)) {
        Store (Zero, Local0)
      } Else {
        Store (One,Local0)
      }
      //
      // Shift 1 by value in register 70, Save in buffer
      //
      ShiftLeft (Local0,And(PIRH,0x0F),IRQW)
      Return (BUF0)
    } // End of _CRS method

    Name(_PRS, ResourceTemplate()
        {IRQ(Level,ActiveLow,Shared){3,4,5,6,7,9,10,11,12,14,15}})

    Method (_SRS,1,NotSerialized) {
      CreateWordField (ARG0, 0x01, IRQW)  // IRQ low
      FindSetRightBit (IRQW,Local0)// Set IRQ
      If (LNotEqual (IRQW,Zero)) {
        And (Local0, 0x7F,Local0)
        Decrement (Local0)
      } Else {
        Or (Local0, 0x80,Local0)
      }
      Store (Local0, PIRH)
    }
}
