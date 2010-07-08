#ifndef BCM5785_H
#define BCM5785_H

#include "chip.h"

#ifndef __PRE_RAM__
void bcm5785_enable(device_t dev);
#else
void enable_fid_change_on_sb(unsigned sbbusn, unsigned sbdn);
#endif

void ldtstop_sb(void);
unsigned get_sbdn(unsigned bus);

#endif /* BCM5785_H */
