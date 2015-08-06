unsigned char microcode[] __attribute__ ((aligned(16))) = {
#if !CONFIG_K8_REV_F_SUPPORT
#include "../../../../3rdparty/blobs/cpu/amd/model_fxx/microcode.h"
#endif
};
