#ifndef ROM_FILL_INBUF_H
#define ROM_FILL_INBUF_H

extern unsigned char *inbuf;	/* input buffer */
extern unsigned int insize;	/* valid bytes in inbuf */
extern unsigned int inptr;	/* index of next byte to be processed in inbuf */

extern int fill_inbuf(void);

#define get_byte()  (inptr < insize ? inbuf[inptr++] : fill_inbuf())


#endif /* ROM_FILL_INBUF_H */
