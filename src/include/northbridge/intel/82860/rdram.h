void display_rdram_regs(int);
void display_spd_dev_row_col_bank(u8*, u8*, u8*);
void display_rdram_regs_tparm(int);
void display_smbus_spd(void);
void display_mch_regs(void);
void init_memory(void);

struct rdram_reg_values {
        u16 channel_a;
        u16 channel_b;
};

void rdram_read_reg(u8, u16, u16, struct rdram_reg_values*);


