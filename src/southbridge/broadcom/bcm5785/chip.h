#ifndef BCM5785_CHIP_H
#define BCM5785_CHIP_H

struct southbridge_broadcom_bcm5785_config
{
        unsigned int ide0_enable : 1;
        unsigned int ide1_enable : 1;
        unsigned int sata0_enable : 1;
        unsigned int sata1_enable : 1;
};
struct chip_operations;
extern struct chip_operations southbridge_broadcom_bcm5785_ops;

#endif /* BCM5785_CHIP_H */
