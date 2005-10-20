/* We are interested in the following hid0 bits:
   6  - ECLK  - Enable external test clock (603 only)
   11 - DPM   - Turn on dynamic power management (603 only)
   15 - NHR   - Not hard reset
   16 - ICE   - Instruction cache enable
   17 - DCE   - Data cache enable
   18 - ILOCK - Instruction cache lock
   19 - DLOCK - Data cache lock
   20 - ICFI  - Instruction cache invalidate
   21 - DCFI  - Data cache invalidate
   24 - NOSER - Serial execution disable (604 only - turbo mode)
   24 - SGE   - Store gathering enable (7410 only)
   29 - BHT   - Branch history table (604 only)
   
   I made up the tags for the 604 specific bits, as they aren't
   named in the 604 book.  The 603 book calls the invalidate bits
   ICFI and DCI, and I have no idea why it isn't DCFI. Maybe IBM named
   one, and Motorola named the other. */

#define HID0_ECLK   0x02000000
#define HID0_DPM    0x00100000
#define HID0_NHR    0x00010000
#define HID0_ICE    0x00008000
#define HID0_DCE    0x00004000
#define HID0_ILOCK  0x00002000
#define HID0_DLOCK  0x00001000
#define HID0_ICFI   0x00000800
#define HID0_DCFI   0x00000400
#define HID0_NOSER  0x00000080
#define HID0_SGE    0x00000080
#define HID0_BTIC   0x00000020
#define HID0_BHT    0x00000004

/*----------------------------------------------------------------------------+
| Machine State Register.  MSR_EE, MSR_PR, MSR_FP, MSR_ME, MSR_FE0, MSR_FE1,
+----------------------------------------------------------------------------*/
#define MSR_APE         0x00080000      /* wait state enable                 */
#define MSR_WE          0x00040000      /* wait state enable                 */
#define MSR_CE          0x00020000      /* critical interrupt enable         */
#define MSR_DWE         0x00000400      /* debug wait enable                 */
#define MSR_DE          0x00000200      /* debug interrupt enable            */
#define MSR_IR          0x00000020      /* instruction relocale              */
#define MSR_DR          0x00000010      /* data relocale                     */

