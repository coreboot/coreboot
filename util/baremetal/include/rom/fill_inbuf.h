#ifndef ROM_FILL_INBUF_H
#define ROM_FILL_INBUF_H

extern unsigned char *inbuf;	/* input buffer */
extern unsigned int insize;	/* valid bytes in inbuf */
extern unsigned int inptr;	/* index of next byte to be processed in inbuf */

extern int fill_inbuf(void);

#ifndef DEBUG_GET_BYTE
#define get_byte()  (inptr < insize ? inbuf[inptr++] : fill_inbuf())
#else
#include <printk.h>
static unsigned char get_byte() {
  static int byteno = 0;
  unsigned char c = (inptr < insize ? inbuf[inptr++] : fill_inbuf());
  printk_debug("get_byte 0x%x 0x%x\n", byteno++, c);
  return c;
}
#endif


#endif /* ROM_FILL_INBUF_H */
