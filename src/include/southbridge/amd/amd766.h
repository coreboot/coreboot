#ifndef AMD766_H
#define AMD766_H

void amd766_disable_watchdog(void);
void amd766_enable_ioapic(void);
void amd766_enable_port92_reset(void);
void amd766_mouse_sends_irq12(void);
void amd766_enable_serial_irqs(int continuous, unsigned frames, unsigned startclocks);
void amd766_cpu_reset_sends_init(void);
#define DECODE_STPGNT_ADDR 0
#define DECODE_STPGNT_DATA 1 
void amd766_decode_stop_grant(unsigned how);
void amd766_set_pm_classcode(void);
void amd766_usb_setup(void);
void amd766_power_after_power_fail(int on);
void amd766_posted_memory_write_enable(void);
void amd766_hard_reset(void);
void amd766_enable_ide(int enable_a, int enable_b);
#endif /* AMD766_H */
