

	/* bus 03 */

        Device(PBIO) {
            Name (_HID, "ACPI000A")
            Name (_ADR, 0x001c0000)
	}

        Device(P64B) {
            Name (_ADR, 0x001d0000)

            Name (_PRT, Package() {
               Package() { 0x0002ffff, 0, 0, 24 }, /* eth0 */
               Package() { 0x0002ffff, 1, 0, 25 }, /* eth1 */
               Package() { 0x0002ffff, 2, 0, 26 },
               Package() { 0x0002ffff, 3, 0, 27 },
               Package() { 0x0003ffff, 0, 0, 28 }, /* eth2 */
               Package() { 0x0003ffff, 1, 0, 29 }, /* eth3 */ 
               Package() { 0x0003ffff, 2, 0, 30 },
               Package() { 0x0003ffff, 3, 0, 31 },
               Package() { 0x0004ffff, 0, 0, 32 }, /* eth4 */ 
            })

            Name (_PRW, Package () { 0x0B, 0x05 })    /* PME# _STS */
            OperationRegion (PBPC, PCI_Config, 0x00, 0xFF)
            Field (PBPC, ByteAcc, NoLock, Preserve)
                    {   Offset (0x3E), BCRL,   8,  BCRH,   8  }
                    
                      
            Device (ETH0) {
               Name (_ADR, 0x00040000)
               Name (_PRW, Package () { 0x0B, 0x05 })    /* PME# _STS */
            }
        }


	/* bus 04 */

        Device(PAIO) {
            Name (_HID, "ACPI000A")
            Name (_ADR, 0x001e0000)
        }

        Device(P64A) {
            Name (_ADR, 0x001f0000)
            Name (_PRT, Package() {
               Package() { 0x0002ffff, 0, 0, 48 },  /* eth5 */ 
               Package() { 0x0002ffff, 1, 0, 49 },  /* eth6 */
               Package() { 0x0002ffff, 2, 0, 50 },
               Package() { 0x0002ffff, 3, 0, 51 },

               Package() { 0x0003ffff, 0, 0, 52 },
               Package() { 0x0003ffff, 1, 0, 53 },
               Package() { 0x0003ffff, 2, 0, 54 }, /* ? */
               Package() { 0x0003ffff, 3, 0, 55 }, /* ? */

               Package() { 0x0004ffff, 0, 0, 54 }, /* not 56 ?? */
               Package() { 0x0004ffff, 1, 0, 55 }, /* not 57 ?? */
            })


            Name (_PRW, Package () { 0x0B, 0x05 })    /* PME# _STS */
            OperationRegion (PBPC, PCI_Config, 0x00, 0xFF)
            Field (PBPC, ByteAcc, NoLock, Preserve)
                    {   Offset (0x3E), BCRL,   8,  BCRH,   8  }

            #include "acpi/scsi.asl"

       }


