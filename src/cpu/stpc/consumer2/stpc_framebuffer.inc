/*
 * originally by johryd
 * modified by
 * Steve M. Gehlbach
 *
 */
	/************************************************
	*                                              *
	*  Set Graphics memory (framebuffer) size      *
	*  register (0x36).                            *
	*  This register defines the size of DRAM      *
	*  used by graphics for frame buffer.          *
	*                                              *
	*  Bits 0-5 (Top of Graphics Memory) indicates * 
	*  frame buffer size in 128KB units. The range *
	*  is 0 to 32 for 0 to 4MB framebuffer, so 6   *
	*  bits are necessary.                         *
	*                                              *
	*  Note: cant change framebuffer size if       *
	*        executing from ram since FB memory    *
	*        is taken from bottom not top so mem   *
	*        addresses change.                     *
	*                                              *
	************************************************/

#if (STPC_FRAME_BUF_SZ / 128) > 33
#error  "frame buffer size >= 4MB)"
#endif

	movb    $0x36, %ah
	movb    $(STPC_FRAME_BUF_SZ >> 7), %al
	stpc_conf_write_macro

