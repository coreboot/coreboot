#ifndef AMD8111_H
#define AMD8111_H

#include "chip.h"

#ifndef __SIMPLE_DEVICE__
void amd8111_enable(struct device *dev);
#endif

#ifdef __PRE_RAM__
void enable_fid_change_on_sb(unsigned sbbusn, unsigned sbdn);
#endif

#endif /* AMD8111_H */
