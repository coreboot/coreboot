#define     VIA_BIOS_REG_TABLE_MAX_NUM      32
#define t_outb(val,reg) outb(val,reg)
#define t_inb(reg) inb(reg)

typedef struct _VIABIOSREGTABLE {
    unsigned char   port[VIA_BIOS_REG_TABLE_MAX_NUM];
    unsigned char   offset[VIA_BIOS_REG_TABLE_MAX_NUM];
    unsigned char   mask[VIA_BIOS_REG_TABLE_MAX_NUM];
    unsigned char   data[VIA_BIOS_REG_TABLE_MAX_NUM];
    int     numEntry;
} VIABIOSRegTableRec;

typedef struct _VIABIOSSTDVGATABLE {
    unsigned char           columns;
    unsigned char           rows;
    unsigned char           fontHeight;
    unsigned int            pageSize;
    unsigned char           SR[5];
    unsigned char           misc;
    unsigned char           CR[25];
    unsigned char           AR[20];
    unsigned char           GR[9];
} VIABIOSStdVGATableRec;

typedef struct _VIAVMODEENTRY {
    unsigned short          Width;
    unsigned short          Height;
    unsigned short          Bpp;
    unsigned short          Mode;
    unsigned short          MemNeed;
    unsigned short          MClk;
    unsigned short          VClk;
    VIABIOSStdVGATableRec   stdVgaTable;
    VIABIOSRegTableRec      extModeExtTable;
} VIAModeEntry;

VIAModeEntry Mode= { 640, 400, 8, 0X30, 4, 0X432E, 0X86B1, { 80, 24, 16, 0X8000, { 0, 0X1, 0XF, 0, 0XE }, 0X4F, { 0X61, 0X4F, 0X4F, 0X85, 0X53, 0X9B, 0XA3, 0X1F, 0, 0X40, 0X1E, 0, 0, 0, 0, 0, 0X90, 0X63, 0X8F, 0X50, 0X40, 0X8F, 0XA4, 0X23, 0XFF }, { 0, 0X1, 0X2, 0X3, 0X4, 0X5, 0X6, 0X7, 0X8, 0X9, 0XA, 0XB, 0XC, 0XD, 0XE, 0XF, 0X41, 0, 0XF, 0 }, { 0, 0, 0, 0, 0, 0X40, 0X5, 0XF, 0XFF } }, { { 0XC4, 0XD4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0X15, 0X33, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0XFE, 0X27, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0X22, 0X26, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, 2 } };

static inline unsigned char read3X4(int reg)
{
	outb(reg, 0x3D4);
	return inb(0x3D5);
}

static inline void write3X4(int reg, unsigned char val)
{
	outb(reg, 0x3D4);
	outb(val, 0x3D5);
}

static inline unsigned char read3C4(int reg)
{
	t_outb(reg, 0x3C4);
	return t_inb(0x3C5);
}

static inline void write3C4(int reg, unsigned char val)
{
	t_outb(reg, 0x3C4);
	t_outb(val, 0x3C5);
}

static inline unsigned char read3CE(int reg)
{
	t_outb(reg, 0x3CE);
	return t_inb(0x3CF);
}

static inline void writeAttr(int reg, unsigned char val)
{
	t_inb(0x3DA);
	t_outb(reg, 0x3C0);
	t_outb(val, 0x3C0);
}

static inline unsigned char readAttr(int reg)
{
	t_inb(0x3DA);
	t_outb(reg, 0x3C0);
	return t_inb(0x3C1);
}

static inline void write3CE(int reg, unsigned char val)
{
	t_outb(reg, 0x3CE);
	t_outb(val, 0x3CF);
}

void setmode()
{
    int j, k = 0;
    int port, offset, mask, data;

    /* Turn off Screen */
    write3X4(0x17, read3X4(0x17) & 0x7f);

    /* Set Sequences regs */
    for (j = 1; j < 5; j++) {
        write3C4(j, Mode.stdVgaTable.SR[j]);
    }
    /* Set Misc reg */
    t_outb(Mode.stdVgaTable.misc,0x3c2);
    /* Set CRTC regs */
    for (j = 0; j < 25; j++) {
        write3X4(j,Mode.stdVgaTable.CR[j]);
    }
    /* Set attribute regs */
    for (j = 0; j < 20; j++) {
        writeAttr(j,Mode.stdVgaTable.AR[j]);
    }
    for (j = 0; j < 9; j++) {
        write3CE(j,Mode.stdVgaTable.GR[j]);
    }

    /* Unlock Extended regs */
    write3C4(0x10, 0x01);

	/* Set Extended Mode-Spec. Extend Regs */
	for (j = 0; j < Mode.extModeExtTable.numEntry; j++) {
		port = Mode.extModeExtTable.port[j];
		offset = Mode.extModeExtTable.offset[j];
		mask = Mode.extModeExtTable.mask[j];
		data = Mode.extModeExtTable.data[j] & mask;
		t_outb(offset, 0x300+port);
		t_outb(data, 0x301+port);
	}

    k= 0;
    write3X4(0x17, read3X4(0x17) & 0x7F);

	write3C4(0x46, (Mode.VClk >> 8));
	write3C4(0x47, (Mode.VClk & 0xFF));

	write3X4(0x17, read3X4(0x17) | 0x80);

	write3C4(0x40, read3C4(0x40) | 0x02);
	write3C4(0x40, read3C4(0x40) & 0xFD);

	/* Use external clock */
	t_outb(t_inb(0x3cc) | 0x0C, 0x3c2);

    /* Open Screen */
    t_inb(0x3da);
    t_outb(0x20, 0x3c0);
}

