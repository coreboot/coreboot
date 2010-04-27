#ifndef X86_LINUX_H
#define X86_LINUX_H

#define E820MAP	0x2d0		/* our map */
#define E820MAX	32		/* number of entries in E820MAP */
#define E820NR	0x1e8		/* # entries in E820MAP */

#ifndef ASSEMBLY

struct e820entry {
	uint64_t addr;	/* start of memory segment */
	uint64_t size;	/* size of memory segment */
	uint32_t type;		/* type of memory segment */
#define E820_RAM	1
#define E820_RESERVED	2
#define E820_ACPI	3 /* usable as RAM once ACPI tables have been read */
#define E820_NVS	4
} __attribute__((packed));

/* FIXME expand on drive_info_struct... */
struct drive_info_struct {
	uint8_t dummy[32];
};
struct sys_desc_table {
	uint16_t length;
	uint8_t  table[318];
};

struct apm_bios_info {
	uint16_t version;       /* 0x40 */
	uint16_t cseg;		/* 0x42 */
	uint32_t offset;	/* 0x44 */
	uint16_t cseg_16;	/* 0x48 */
	uint16_t dseg;		/* 0x4a */
	uint16_t flags;		/* 0x4c */
	uint16_t cseg_len;	/* 0x4e */
	uint16_t cseg_16_len;	/* 0x50 */
	uint16_t dseg_len;	/* 0x52 */
	uint8_t  reserved[44];	/* 0x54 */
};


struct x86_linux_param_header {
	uint8_t  orig_x;			/* 0x00 */
	uint8_t  orig_y;			/* 0x01 */
	uint16_t ext_mem_k;			/* 0x02 -- EXT_MEM_K sits here */
	uint16_t orig_video_page;		/* 0x04 */
	uint8_t  orig_video_mode;		/* 0x06 */
	uint8_t  orig_video_cols;		/* 0x07 */
	uint16_t unused2;			/* 0x08 */
	uint16_t orig_video_ega_bx;		/* 0x0a */
	uint16_t unused3;			/* 0x0c */
	uint8_t	 orig_video_lines;		/* 0x0e */
	uint8_t	 orig_video_isVGA;		/* 0x0f */
	uint16_t orig_video_points;		/* 0x10 */

	/* VESA graphic mode -- linear frame buffer */
	uint16_t lfb_width;			/* 0x12 */
	uint16_t lfb_height;			/* 0x14 */
	uint16_t lfb_depth;			/* 0x16 */
	uint32_t lfb_base;			/* 0x18 */
	uint32_t lfb_size;			/* 0x1c */
	uint16_t cl_magic;			/* 0x20 */
#define CL_MAGIC_VALUE 0xA33F
	uint16_t cl_offset;			/* 0x22 */
	uint16_t lfb_linelength;		/* 0x24 */
	uint8_t	 red_size;			/* 0x26 */
	uint8_t	 red_pos;			/* 0x27 */
	uint8_t	 green_size;			/* 0x28 */
	uint8_t	 green_pos;			/* 0x29 */
	uint8_t	 blue_size;			/* 0x2a */
	uint8_t	 blue_pos;			/* 0x2b */
	uint8_t	 rsvd_size;			/* 0x2c */
	uint8_t	 rsvd_pos;			/* 0x2d */
	uint16_t vesapm_seg;			/* 0x2e */
	uint16_t vesapm_off;			/* 0x30 */
	uint16_t pages;				/* 0x32 */
	uint8_t  reserved4[12];			/* 0x34 -- 0x3f reserved for future expansion */

	struct apm_bios_info apm_bios_info;	/* 0x40 */
	struct drive_info_struct drive_info;	/* 0x80 */
	struct sys_desc_table sys_desc_table;	/* 0xa0 */
	uint32_t alt_mem_k;			/* 0x1e0 */
	uint8_t  reserved5[4];			/* 0x1e4 */
	uint8_t  e820_map_nr;			/* 0x1e8 */
	uint8_t  reserved6[8];			/* 0x1e9 */
	uint8_t  setup_sects;			/* 0x1f1 */
	uint16_t mount_root_rdonly;		/* 0x1f2 */
	uint8_t  reserved7[4];			/* 0x1f4 */
	uint16_t ramdisk_flags;			/* 0x1f8 */
#define RAMDISK_IMAGE_START_MASK  	0x07FF
#define RAMDISK_PROMPT_FLAG		0x8000
#define RAMDISK_LOAD_FLAG		0x4000
	uint16_t vid_mode;			/* 0x1fa */
	uint16_t root_dev;			/* 0x1fc */
	uint8_t  reserved9[1];			/* 0x1fe */
	uint8_t  aux_device_info;		/* 0x1ff */
	/* 2.00+ */
	uint8_t  reserved10[2];			/* 0x200 */
	uint8_t  header_magic[4];		/* 0x202 */
	uint16_t protocol_version;		/* 0x206 */
	uint8_t  reserved11[8];			/* 0x208 */
	uint8_t  loader_type;			/* 0x210 */
#define LOADER_TYPE_LOADLIN         1
#define LOADER_TYPE_BOOTSECT_LOADER 2
#define LOADER_TYPE_SYSLINUX        3
#define LOADER_TYPE_ETHERBOOT       4
#define LOADER_TYPE_UNKNOWN         0xFF
	uint8_t  loader_flags;			/* 0x211 */
	uint8_t  reserved12[2];			/* 0x212 */
	uint32_t kernel_start;			/* 0x214 */
	uint32_t initrd_start;			/* 0x218 */
	uint32_t initrd_size;			/* 0x21c */
	uint8_t  reserved13[4];			/* 0x220 */
	/* 2.01+ */
	uint16_t heap_end_ptr;			/* 0x224 */
	uint8_t  reserved14[2];			/* 0x226 */
	/* 2.02+ */
	uint32_t cmd_line_ptr;			/* 0x228 */
	/* 2.03+ */
	uint32_t initrd_addr_max;		/* 0x22c */
	uint8_t  reserved15[160];		/* 0x230 */
	struct e820entry e820_map[E820MAX];	/* 0x2d0 */
						/* 0x550 */
#define COMMAND_LINE_SIZE 256
};

struct x86_linux_header {
	uint8_t  reserved1[0x1f1];		/* 0x000 */
	uint8_t  setup_sects;			/* 0x1f1 */
	uint16_t root_flags;			/* 0x1f2 */
	uint8_t  reserved2[6];			/* 0x1f4 */
	uint16_t vid_mode;			/* 0x1fa */
	uint16_t root_dev;			/* 0x1fc */
	uint16_t boot_sector_magic;		/* 0x1fe */
	/* 2.00+ */
	uint8_t  reserved3[2];			/* 0x200 */
	uint8_t  header_magic[4];		/* 0x202 */
	uint16_t protocol_version;		/* 0x206 */
	uint32_t realmode_swtch;		/* 0x208 */
	uint16_t start_sys;			/* 0x20c */
	uint16_t kver_addr;			/* 0x20e */
	uint8_t  type_of_loader;		/* 0x210 */
	uint8_t  loadflags;			/* 0x211 */
	uint16_t setup_move_size;		/* 0x212 */
	uint32_t code32_start;			/* 0x214 */
	uint32_t ramdisk_image;			/* 0x218 */
	uint32_t ramdisk_size;			/* 0x21c */
	uint8_t  reserved4[4];			/* 0x220 */
	/* 2.01+ */
	uint16_t heap_end_ptr;			/* 0x224 */
	uint8_t  reserved5[2];			/* 0x226 */
	/* 2.02+ */
	uint32_t cmd_line_ptr;			/* 0x228 */
	/* 2.03+ */
	uint32_t initrd_addr_max;		/* 0x22c */
	uint8_t  tail[32*1024];			/* 0x230 */
} __attribute__ ((packed));

#endif /* ASSEMBLY */

#endif /* X86_LINUX_H */
