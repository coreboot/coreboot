#ifndef FALLBACK_H
#define FALLBACK_H

#ifndef ASSEMBLY

#if CONFIG_HAVE_FALLBACK_BOOT == 1
void set_boot_successful(void);
#else
#define set_boot_successful()
#endif

void boot_successful(void);

#endif /* ASSEMBLY */

#define RTC_BOOT_BYTE	48

#endif /* FALLBACK_H */
