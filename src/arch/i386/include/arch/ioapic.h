#ifndef ARCH_IOAPIC_H
#define ARCH_IOAPIC_H


#ifdef IOAPIC
extern void setup_ioapic(void);
#else
#define setup_ioapic() do {} while(0)
#endif

#endif /* ARCH_IOAPIC_H */
