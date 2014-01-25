#ifndef SPKMODEM_H
#define SPKMODEM_H 1

#include <console/streams.h>

void spkmodem_init(void);
void spkmodem_tx_byte(unsigned char c);

#if CONFIG_SPKMODEM && (ENV_ROMSTAGE || ENV_RAMSTAGE)
#define __spkmodem_init()	spkmodem_init()
#define __spkmodem_tx_byte(x)	spkmodem_tx_byte(x)
#else
#define __spkmodem_init()
#define __spkmodem_tx_byte(x)
#endif

#endif
