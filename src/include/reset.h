#ifndef RESET_H
#define RESET_H

#if CONFIG_HAVE_HARD_RESET
void hard_reset(void);
#else
#define hard_reset() do {} while (0)
#endif
void soft_reset(void);
void cpu_reset(void);
/* Some Intel SoCs use a special reset that is specific to SoC */
void global_reset(void);
/* Some Intel SoCs may need to prepare/wait before reset */
void reset_prepare(void);
#endif
