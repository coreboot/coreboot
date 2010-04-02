#ifndef AMDK8_H

#define AMDK8_H

#if CONFIG_K8_REV_F_SUPPORT == 1
        #include "amdk8_f.h"
#else
        #include "amdk8_pre_f.h"
#endif

#ifndef __ROMCC__
void showallroutes(int level, device_t dev);
#endif

#endif /* AMDK8_H */
