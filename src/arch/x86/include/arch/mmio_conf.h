#ifndef ARCH_MMIO_H
#define ARCH_MMIO_H 1


// Extended read, constrain to use registers as mandated by AMD MMCONFIG mechanism.

static inline __attribute__((always_inline)) uint8_t read8x(uint32_t addr)
{
	uint8_t value;
        __asm__ volatile (
                "movb (%1), %%al\n\t"
                :"=a"(value): "b" (addr)
        );
        return value;
}

static inline __attribute__((always_inline)) uint16_t read16x(uint32_t addr)
{
        uint16_t value;
        __asm__ volatile (
                "movw (%1), %%ax\n\t"
                :"=a"(value): "b" (addr)
        );

        return value;

}

static inline __attribute__((always_inline)) uint32_t read32x(uint32_t addr)
{
        uint32_t value;
        __asm__ volatile (
                "movl (%1), %%eax\n\t"
                :"=a"(value): "b" (addr)
        );

        return value;

}

static inline __attribute__((always_inline)) void write8x(uint32_t addr, uint8_t value)
{
        __asm__ volatile (
                "movb %%al, (%0)\n\t"
                :: "b" (addr), "a" (value)
        );

}

static inline __attribute__((always_inline)) void write16x(uint32_t addr, uint16_t value)
{
        __asm__ volatile (
                "movw %%ax, (%0)\n\t"
                :: "b" (addr), "a" (value)
        );

}

static inline __attribute__((always_inline)) void write32x(uint32_t addr, uint32_t value)
{
        __asm__ volatile (
                "movl %%eax, (%0)\n\t"
                :: "b" (addr), "a" (value)
        );
}

#endif /* ARCH_MMIO_H */
