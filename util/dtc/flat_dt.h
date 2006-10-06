#ifndef _FLAT_DT_H_
#define _FLAT_DT_H_


#define OF_DT_HEADER            0xd00dfeed      /* 4: version, 4: total size */

#define OF_DT_BEGIN_NODE	0x1             /* Start node: full name */
#define OF_DT_END_NODE		0x2             /* End node */
#define OF_DT_PROP		0x3             /* Property: name off,
						   size, content */
#define OF_DT_NOP		0x4		/* nop */
#define OF_DT_END               0x9

struct boot_param_header {
	uint32_t magic;                  /* magic word OF_DT_HEADER */
	uint32_t totalsize;              /* total size of DT block */
	uint32_t off_dt_struct;          /* offset to structure */
	uint32_t off_dt_strings;         /* offset to strings */
	uint32_t off_mem_rsvmap;         /* offset to memory reserve map */
	uint32_t version;                /* format version */
	uint32_t last_comp_version;      /* last compatible version */

        /* version 2 fields below */
	uint32_t boot_cpuid_phys;        /* Which physical CPU id we're
					    booting on */
	/* version 3 fields below */
        uint32_t size_dt_strings;        /* size of the strings block */
};

#define BPH_V1_SIZE	(7*sizeof(uint32_t))
#define BPH_V2_SIZE	(BPH_V1_SIZE + sizeof(uint32_t))
#define BPH_V3_SIZE	(BPH_V2_SIZE + sizeof(uint32_t))

struct reserve_entry {
	uint64_t address;
	uint64_t size;
};

struct flat_dt_property {
	uint32_t nameoff;
	uint32_t len;
	char data[0];
};

#endif /* _FLAT_DT_H_ */
