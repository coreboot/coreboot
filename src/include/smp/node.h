#ifndef _SMP_NODE_H_
#define _SMP_NODE_H_

#if IS_ENABLED(CONFIG_SMP)
int boot_cpu(void);
#else
#define boot_cpu(x) 1
#endif

static inline int is_smp_boot(void)
{
	return IS_ENABLED(CONFIG_SMP) && CONFIG_MAX_CPUS > 1;
}

#endif /* _SMP_NODE_H_ */
