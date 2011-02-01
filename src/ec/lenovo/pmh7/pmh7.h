#ifndef EC_LENOVO_PMH7_H
#define EC_LENOVO_PMH7_H

#define EC_LENOVO_PMH7_INDEX 0x77

#define EC_LENOVO_PMH7_BASE 0x15e0
#define EC_LENOVO_PMH7_ADDR (EC_LENOVO_PMH7_BASE + 0x0c)
#define EC_LENOVO_PMH7_DATA (EC_LENOVO_PMH7_BASE + 0x0e)

void pmh7_register_set_bit(int reg, int bit);
void pmh7_register_clear_bit(int reg, int bit);
char pmh7_register_read(int reg);
void pmh7_register_write(int reg, int val);

#endif
