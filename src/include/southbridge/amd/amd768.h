#ifndef AMD768_H
#define AMD768_H

void amd768_disable_watchdog(void);
void amd768_enable_ioapic(void);
void amd768_enable_port92_reset(void);
void amd768_mouse_sends_irq12(void);
void amd768_enable_serial_irqs(int continuous, unsigned frames, unsigned startclocks);
void amd768_cpu_reset_sends_init(void);
#define DECODE_STPGNT_ADDR 0
#define DECODE_STPGNT_DATA 1 
void amd768_decode_stop_grant(unsigned how);
void amd768_set_pm_classcode(void);
void amd768_usb_setup(void);
void amd768_power_after_power_fail(int on);
void amd768_posted_memory_write_enable(void);
void amd768_hard_reset(void);
void amd768_enable_ide(int enable_a, int enable_b);
#endif /* AMD768_H */
