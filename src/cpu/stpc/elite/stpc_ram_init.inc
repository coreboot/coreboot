/*
 * STPC Consumer II ram setup
 *
 * by Steve M. Gehlbach
 *
 * borrowed from johryd and st web site code
 *
 */
        /****************************************
         *                                      *
         *   Set Memory Size to 4MB Default     *
         *                                      *
         *   Set default memory bank size.      *
         * Bank 0 must be installed or it just  *
         * won't work.  So we set bank 0 to     *
         * 4M and set the rest empty until      *
         * the actual memory size can be        *
         * determined.                          *
         *                                      *
         ****************************************/

		movb	$0x12, %al
		outb	%al, $0x80  

        movb    $0x03, %al                      // 4 MB (4-1) Bank 0
        movb    $0x30, %bh                      /* reg 0x30: STPC_MEM_BANK0 */
        stpc_set_memreg_macro                   // setting in %al, first reg in %bh, trashes %ah
