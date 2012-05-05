#include <arch/cpu.h>

int init_processor_name(void);

#if !CONFIG_K8_REV_F_SUPPORT
static inline int is_cpu_rev_a0(void)
{
	return (cpuid_eax(1) & 0xfffef) == 0x0f00;
}
static inline int is_cpu_pre_c0(void)
{
        return (cpuid_eax(1) & 0xfffef) < 0x0f48;
}

static inline int is_cpu_c0(void)
{
        return (cpuid_eax(1) & 0xfffef) == 0x0f48;
}

static inline int is_cpu_pre_b3(void)
{
        return (cpuid_eax(1) & 0xfffef) < 0x0f41;
}

static inline int is_cpu_b3(void)
{
        return (cpuid_eax(1) & 0xfffef) == 0x0f41;
}
//AMD_D0_SUPPORT
static inline int is_cpu_pre_d0(void)
{
        return (cpuid_eax(1) & 0xfff0f) < 0x10f00;
}

static inline int is_cpu_d0(void)
{
        return (cpuid_eax(1) & 0xfff0f) == 0x10f00;
}

//AMD_E0_SUPPORT
static inline int is_cpu_pre_e0(void)
{
        return (cpuid_eax(1) & 0xfff0f) < 0x20f00;
}

static inline int is_cpu_e0(void)
{
        return (cpuid_eax(1) & 0xfff00) == 0x20f00;
}


#ifdef __PRE_RAM__
static int is_e0_later_in_bsp(int nodeid)
{
        uint32_t val;
        uint32_t val_old;
        int e0_later;
        if(nodeid==0) { // we don't need to do that for node 0 in core0/node0
                return !is_cpu_pre_e0();
        }
        // d0 will be treated as e0 with this methods, but the d0 nb_cfg_54 always 0
        device_t dev;
        dev = PCI_DEV(0, 0x18+nodeid,2);
        val_old = pci_read_config32(dev, 0x80);
        val = val_old;
        val |= (1<<3);
        pci_write_config32(dev, 0x80, val);
        val = pci_read_config32(dev, 0x80);
        e0_later = !!(val & (1<<3));
        if(e0_later) { // pre_e0 bit 3 always be 0 and can not be changed
                pci_write_config32(dev, 0x80, val_old); // restore it
        }

        return e0_later;
}
#else
int is_e0_later_in_bsp(int nodeid); //defined model_fxx_init.c
#endif

#endif

#if CONFIG_K8_REV_F_SUPPORT
//AMD_F0_SUPPORT
static inline int is_cpu_pre_f0(void)
{
        return (cpuid_eax(1) & 0xfff0f) < 0x40f00;
}

static inline int is_cpu_f0(void)
{
        return (cpuid_eax(1) & 0xfff00) ==  0x40f00;
}

static inline int is_cpu_pre_f2(void)
{
        return (cpuid_eax(1) & 0xfff0f) <  0x40f02;
}

#ifdef __PRE_RAM__
//AMD_F0_SUPPORT
static inline int is_cpu_f0_in_bsp(int nodeid)
{
	uint32_t dword;
	device_t dev;
	dev = PCI_DEV(0, 0x18+nodeid, 3);
	dword = pci_read_config32(dev, 0xfc);
        return (dword & 0xfff00) == 0x40f00;
}
static inline int is_cpu_pre_f2_in_bsp(int nodeid)
{
        uint32_t dword;
	device_t dev;
        dev = PCI_DEV(0, 0x18+nodeid, 3);
        dword = pci_read_config32(dev, 0xfc);
        return (dword & 0xfff0f) < 0x40f02;
}
#else
int is_cpu_f0_in_bsp(int nodeid); // defined in model_fxx_init.c
#endif

#endif

