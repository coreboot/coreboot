#ifndef AMD8111_H
#define AMD8111_H

struct southbridge_amd_amd8111_config 
{
};
struct chip_control;
extern struct chip_control southbridge_amd_amd8111_control;

void amd8111_enable(device_t dev);

#endif /* AMD8111_H */
