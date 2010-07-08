#ifndef AMD8111_H
#define AMD8111_H

#include "chip.h"

void amd8111_enable(device_t dev);

#ifdef __PRE_RAM__
void enable_fid_change_on_sb(unsigned sbbusn, unsigned sbdn);
#endif

#endif /* AMD8111_H */
