#ifndef SOUTHBRIDGE_INTEL_82801_H
#define SOUTHBRIDGE_INTEL_82801_H

void ich2_enable_serial_irqs(void);
void ich2_lpc_route_dma(unsigned char mask);
void ich2_enable_ioapic(void);
void ich2_enable_ide(int enable_a, int enable_b);
void ich2_hard_reset(void);
void ich2_set_cpu_multiplier(unsigned);
void ich2_rtc_init(void);
void ich2_power_after_power_fail(int on);

#endif /* SOUTHBRIDGE_INTEL_82801_H */
