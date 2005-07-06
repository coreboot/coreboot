/* by yhlu 6.2005 
	moved from nrv2v.c and some lines from crt0.S
*/
#ifndef ENDIAN
#define ENDIAN   0
#endif
#ifndef BITSIZE
#define BITSIZE 32
#endif

#define GETBIT_8(bb, src, ilen) \
    (((bb = bb & 0x7f ? bb*2 : ((unsigned)src[ilen++]*2+1)) >> 8) & 1)

#define GETBIT_LE16(bb, src, ilen) \
    (bb*=2,bb&0xffff ? (bb>>16)&1 : (ilen+=2,((bb=(src[ilen-2]+src[ilen-1]*256u)*2+1)>>16)&1))

#define GETBIT_LE32(bb, src, ilen) \
    (bc > 0 ? ((bb>>--bc)&1) : (bc=31,\
    bb=*(const uint32_t *)((src)+ilen),ilen+=4,(bb>>31)&1))

#if ENDIAN == 0 && BITSIZE == 8
#define GETBIT(bb, src, ilen) GETBIT_8(bb, src, ilen)
#endif
#if ENDIAN == 0 && BITSIZE == 16
#define GETBIT(bb, src, ilen) GETBIT_LE16(bb, src, ilen)
#endif
#if ENDIAN == 0 && BITSIZE == 32
#define GETBIT(bb, src, ilen) GETBIT_LE32(bb, src, ilen)
#endif

static void copy_and_run(unsigned cpu_reset)
{
	uint8_t *src, *dst; 
	unsigned long dst_len;
        unsigned long ilen = 0, olen = 0, last_m_off =  1;
        uint32_t bb = 0;
        unsigned bc = 0;

	print_debug("Copying LinuxBIOS to ram.\r\n");

#if !CONFIG_COMPRESS 
	__asm__ volatile (
		"leal _liseg, %0\n\t"
		"leal _iseg, %1\n\t"
		"leal _eiseg, %2\n\t"
		"subl %1, %2\n\t"
		: "=a" (src), "=b" (dst), "=c" (dst_len)
	);
	memcpy(src, dst, dst_len);
#else 

        __asm__ volatile (
	        "leal  4+_liseg, %0\n\t"
	        "leal    _iseg,  %1\n\t"
                : "=a" (src) , "=b" (dst)
        );

#if CONFIG_USE_INIT		
	printk_debug("src=%08x\r\n",src); 
	printk_debug("dst=%08x\r\n",dst);
#else
        print_debug("src="); print_debug_hex32(src); print_debug("\r\n");
        print_debug("dst="); print_debug_hex32(dst); print_debug("\r\n");
#endif

        for(;;) {
                unsigned int m_off, m_len;
                while(GETBIT(bb, src, ilen)) {
                        dst[olen++] = src[ilen++];
                }
                m_off = 1;
                do {
                        m_off = m_off*2 + GETBIT(bb, src, ilen);
                } while (!GETBIT(bb, src, ilen));
                if (m_off == 2)
                {
                        m_off = last_m_off;
                }
                else
                {
                        m_off = (m_off - 3)*256 + src[ilen++];
                        if(m_off == 0xffffffffU) 
                                break;
                        last_m_off = ++m_off;
                }
                m_len = GETBIT(bb, src, ilen);
                m_len = m_len*2 + GETBIT(bb, src, ilen);
                if (m_len == 0)
                {
                        m_len++;
                        do {
                                m_len = m_len*2 + GETBIT(bb, src, ilen);
                        } while(!GETBIT(bb, src, ilen));
                        m_len += 2;
                }
                m_len += (m_off > 0xd00);
                {
                        const uint8_t *m_pos;
                        m_pos = dst + olen - m_off;
                        dst[olen++] = *m_pos++;
                        do {
                                dst[olen++] = *m_pos++;
                        } while(--m_len > 0);
                }
        }
#endif
//	dump_mem(dst, dst+0x100);
#if CONFIG_USE_INIT
	printk_debug("linxbios_ram.bin length = %08x\r\n", olen);
#else
	print_debug("linxbios_ram.bin length = "); print_debug_hex32(olen); print_debug("\r\n");
#endif
	print_debug("Jumping to LinuxBIOS.\r\n");

	if(cpu_reset == 1 ) {
		__asm__ volatile (
			"movl $0xffffffff, %ebp\n\t"
		);
	}
	else {
                __asm__ volatile (
                        "xorl %ebp, %ebp\n\t"
                );
	}
	
	__asm__ volatile (
		"cli\n\t"
		"leal    _iseg, %edi\n\t"
		"jmp     %edi\n\t"
	);

}
