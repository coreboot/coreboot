#if HAVE_FRAMEBUFFER

/* Frame buffer size in MBytes */
#ifndef SMA_SIZE
#define SMA_SIZE 8
#endif

#if SMA_SIZE==2
#define FBREG 0x90
#elif SMA_SIZE==4
#define FBREG 0xa0
#elif SMA_SIZE==8
#define FBREG 0xb0
#else
#error SMA_SIZE should be 2, 4, or 8 (MB)
#endif

CS_WRITE($0xf8, $0x22) // DRAM arbitation timer - AGP, Host
CS_WRITE($0xf9, $0x42) // DRAM arbitation timer - VGA priority, normal
CS_WRITE($0xfb, $FBREG) // VGA enable

#endif

#if VIDEO_CONSOLE

/*
 * by Steve M. Gehlbach, Ph.D.
 * steve @ kesa . com
 *
 * 9/7/03 smg
 * minor chipset settings for vga
 * more work needed for init, since reboot
 * from linux has problems (hdwr reset okay)
 *
 */
	// set shadow ram to award settings

	CS_WRITE($0x61, $0x2a)
	CS_WRITE($0x62, $0x00)
	CS_WRITE($0x63, $0x20)

	// turn off GA
	
	CS_WRITE($0x88,$0x00)

	// enable vga, fb

	CS_WRITE($0xF9,$0x42)
	CS_WRITE($0xFB,$0xb0)

#endif

