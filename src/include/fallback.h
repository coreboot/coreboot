#ifndef FALLBACK_H
#define FALLBACK_H

#if !defined(__ASSEMBLER__)

void boot_successful(void);
void set_boot_successful(void);

#endif /* __ASSEMBLER__ */

#define RTC_BOOT_BYTE	48

#endif /* FALLBACK_H */
