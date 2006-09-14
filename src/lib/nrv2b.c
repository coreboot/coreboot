// This GETBIT is supposed to work on little endian 
// 32bit systems. The algorithm will definitely need
// some fixing on other systems, but it might not be
// a problem since the nrv2b binary behaves the same..

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

static unsigned long unrv2b(uint8_t * src, uint8_t * dst)
{
	unsigned long ilen = 0, olen = 0, last_m_off = 1;
	uint32_t bb = 0;
	unsigned bc = 0;
	const uint8_t *m_pos;
//	unsigned long file_len = *(unsigned long *) src;

	// we only have printk_debug in copy_and_run.c if CONFIG_USE_INIT is
	// not set, so comment it out.
	// printk_debug("compressed file len is supposed to be %d bytes\n", file_len);
	// skip length
	src += 4;
	/* FIXME: check olen with the length stored in first 4 bytes */	

	for (;;) {
		unsigned int m_off, m_len;
		while (GETBIT(bb, src, ilen)) {
			dst[olen++] = src[ilen++];
		}

		m_off = 1;
		do {
			m_off = m_off * 2 + GETBIT(bb, src, ilen);
		} while (!GETBIT(bb, src, ilen));
		if (m_off == 2) {
			m_off = last_m_off;
		} else {
			m_off = (m_off - 3) * 256 + src[ilen++];
			if (m_off == 0xffffffffU)
				break;
			last_m_off = ++m_off;
		}

		m_len = GETBIT(bb, src, ilen);
		m_len = m_len * 2 + GETBIT(bb, src, ilen);
		if (m_len == 0) {
			m_len++;
			do {
				m_len = m_len * 2 + GETBIT(bb, src, ilen);
			} while (!GETBIT(bb, src, ilen));
			m_len += 2;
		}
		m_len += (m_off > 0xd00);

		m_pos = dst + olen - m_off;
		dst[olen++] = *m_pos++;
		do {
			dst[olen++] = *m_pos++;
		} while (--m_len > 0);
	}

	// we only have printk_debug in copy_and_run.c if CONFIG_USE_INIT is
	// not set, so comment it out.
	//printk_debug("computed len is %d, file len is %d\n", olen, file_len);
	return olen;

}

