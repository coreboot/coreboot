#ifndef _ISO9660_H
#define _ISO9660_H

/*
 * The isofs filesystem constants/structures
 */

/* This part borrowed from the bsd386 isofs */
#define ISODCL(from, to) (to - from + 1)

struct iso_volume_descriptor {
	char type[ISODCL(1,1)]; /* 711 */
	char id[ISODCL(2,6)];
	char version[ISODCL(7,7)];
	char data[ISODCL(8,2048)];
};

#define ISO_SECTOR_BITS              (11)
#define ISO_SECTOR_SIZE              (1<<ISO_SECTOR_BITS)

#define ISO_REGULAR     1       /* regular file */
#define ISO_DIRECTORY   2       /* directory    */
#define ISO_OTHER       0       /* other file (with Rock Ridge) */

#define RR_FLAG_PX      0x01    /* have POSIX file attributes */
#define RR_FLAG_NM      0x08    /* have alternate file name   */

/* POSIX file attributes for Rock Ridge extensions */
#define POSIX_S_IFMT    0xF000
#define POSIX_S_IFREG   0x8000
#define POSIX_S_IFDIR   0x4000

/* volume descriptor types */
#define ISO_VD_PRIMARY 1
#define ISO_VD_SUPPLEMENTARY 2
#define ISO_VD_END 255

#define ISO_STANDARD_ID "CD001"

struct iso_primary_descriptor {
	char type			[ISODCL (  1,   1)]; /* 711 */
	char id				[ISODCL (  2,   6)];
	char version			[ISODCL (  7,   7)]; /* 711 */
	char unused1			[ISODCL (  8,   8)];
	char system_id			[ISODCL (  9,  40)]; /* achars */
	char volume_id			[ISODCL ( 41,  72)]; /* dchars */
	char unused2			[ISODCL ( 73,  80)];
	char volume_space_size		[ISODCL ( 81,  88)]; /* 733 */
	char unused3			[ISODCL ( 89, 120)];
	char volume_set_size		[ISODCL (121, 124)]; /* 723 */
	char volume_sequence_number	[ISODCL (125, 128)]; /* 723 */
	char logical_block_size		[ISODCL (129, 132)]; /* 723 */
	char path_table_size		[ISODCL (133, 140)]; /* 733 */
	char type_l_path_table		[ISODCL (141, 144)]; /* 731 */
	char opt_type_l_path_table	[ISODCL (145, 148)]; /* 731 */
	char type_m_path_table		[ISODCL (149, 152)]; /* 732 */
	char opt_type_m_path_table	[ISODCL (153, 156)]; /* 732 */
	char root_directory_record	[ISODCL (157, 190)]; /* 9.1 */
	char volume_set_id		[ISODCL (191, 318)]; /* dchars */
	char publisher_id		[ISODCL (319, 446)]; /* achars */
	char preparer_id		[ISODCL (447, 574)]; /* achars */
	char application_id		[ISODCL (575, 702)]; /* achars */
	char copyright_file_id		[ISODCL (703, 739)]; /* 7.5 dchars */
	char abstract_file_id		[ISODCL (740, 776)]; /* 7.5 dchars */
	char bibliographic_file_id	[ISODCL (777, 813)]; /* 7.5 dchars */
	char creation_date		[ISODCL (814, 830)]; /* 8.4.26.1 */
	char modification_date		[ISODCL (831, 847)]; /* 8.4.26.1 */
	char expiration_date		[ISODCL (848, 864)]; /* 8.4.26.1 */
	char effective_date		[ISODCL (865, 881)]; /* 8.4.26.1 */
	char file_structure_version	[ISODCL (882, 882)]; /* 711 */
	char unused4			[ISODCL (883, 883)];
	char application_data		[ISODCL (884, 1395)];
	char unused5			[ISODCL (1396, 2048)];
};

/* Almost the same as the primary descriptor but two fields are specified */
struct iso_supplementary_descriptor {
	char type			[ISODCL (  1,   1)]; /* 711 */
	char id				[ISODCL (  2,   6)];
	char version			[ISODCL (  7,   7)]; /* 711 */
	char flags			[ISODCL (  8,   8)]; /* 853 */
	char system_id			[ISODCL (  9,  40)]; /* achars */
	char volume_id			[ISODCL ( 41,  72)]; /* dchars */
	char unused2			[ISODCL ( 73,  80)];
	char volume_space_size		[ISODCL ( 81,  88)]; /* 733 */
	char escape			[ISODCL ( 89, 120)]; /* 856 */
	char volume_set_size		[ISODCL (121, 124)]; /* 723 */
	char volume_sequence_number	[ISODCL (125, 128)]; /* 723 */
	char logical_block_size		[ISODCL (129, 132)]; /* 723 */
	char path_table_size		[ISODCL (133, 140)]; /* 733 */
	char type_l_path_table		[ISODCL (141, 144)]; /* 731 */
	char opt_type_l_path_table	[ISODCL (145, 148)]; /* 731 */
	char type_m_path_table		[ISODCL (149, 152)]; /* 732 */
	char opt_type_m_path_table	[ISODCL (153, 156)]; /* 732 */
	char root_directory_record	[ISODCL (157, 190)]; /* 9.1 */
	char volume_set_id		[ISODCL (191, 318)]; /* dchars */
	char publisher_id		[ISODCL (319, 446)]; /* achars */
	char preparer_id		[ISODCL (447, 574)]; /* achars */
	char application_id		[ISODCL (575, 702)]; /* achars */
	char copyright_file_id		[ISODCL (703, 739)]; /* 7.5 dchars */
	char abstract_file_id		[ISODCL (740, 776)]; /* 7.5 dchars */
	char bibliographic_file_id	[ISODCL (777, 813)]; /* 7.5 dchars */
	char creation_date		[ISODCL (814, 830)]; /* 8.4.26.1 */
	char modification_date		[ISODCL (831, 847)]; /* 8.4.26.1 */
	char expiration_date		[ISODCL (848, 864)]; /* 8.4.26.1 */
	char effective_date		[ISODCL (865, 881)]; /* 8.4.26.1 */
	char file_structure_version	[ISODCL (882, 882)]; /* 711 */
	char unused4			[ISODCL (883, 883)];
	char application_data		[ISODCL (884, 1395)];
	char unused5			[ISODCL (1396, 2048)];
};


#define HS_STANDARD_ID "CDROM"

struct  hs_volume_descriptor {
	char foo			[ISODCL (  1,   8)]; /* 733 */
	char type			[ISODCL (  9,   9)]; /* 711 */
	char id				[ISODCL ( 10,  14)];
	char version			[ISODCL ( 15,  15)]; /* 711 */
	char data[ISODCL(16,2048)];
};


struct hs_primary_descriptor {
	char foo			[ISODCL (  1,   8)]; /* 733 */
	char type			[ISODCL (  9,   9)]; /* 711 */
	char id				[ISODCL ( 10,  14)];
	char version			[ISODCL ( 15,  15)]; /* 711 */
	char unused1			[ISODCL ( 16,  16)]; /* 711 */
	char system_id			[ISODCL ( 17,  48)]; /* achars */
	char volume_id			[ISODCL ( 49,  80)]; /* dchars */
	char unused2			[ISODCL ( 81,  88)]; /* 733 */
	char volume_space_size		[ISODCL ( 89,  96)]; /* 733 */
	char unused3			[ISODCL ( 97, 128)]; /* 733 */
	char volume_set_size		[ISODCL (129, 132)]; /* 723 */
	char volume_sequence_number	[ISODCL (133, 136)]; /* 723 */
	char logical_block_size		[ISODCL (137, 140)]; /* 723 */
	char path_table_size		[ISODCL (141, 148)]; /* 733 */
	char type_l_path_table		[ISODCL (149, 152)]; /* 731 */
	char unused4			[ISODCL (153, 180)]; /* 733 */
	char root_directory_record	[ISODCL (181, 214)]; /* 9.1 */
};

/* We use this to help us look up the parent inode numbers. */

struct iso_path_table{
	unsigned char  name_len[2];	/* 721 */
	char extent[4];		/* 731 */
	char  parent[2];	/* 721 */
	char name[0];
} __attribute__((packed));

/* high sierra is identical to iso, except that the date is only 6 bytes, and
   there is an extra reserved byte after the flags */

struct iso_directory_record {
	char length			[ISODCL (1, 1)]; /* 711 */
	char ext_attr_length		[ISODCL (2, 2)]; /* 711 */
	char extent			[ISODCL (3, 10)]; /* 733 */
	char size			[ISODCL (11, 18)]; /* 733 */
	char date			[ISODCL (19, 25)]; /* 7 by 711 */
	char flags			[ISODCL (26, 26)];
	char file_unit_size		[ISODCL (27, 27)]; /* 711 */
	char interleave			[ISODCL (28, 28)]; /* 711 */
	char volume_sequence_number	[ISODCL (29, 32)]; /* 723 */
	unsigned char name_len		[ISODCL (33, 33)]; /* 711 */
	char name			[0];
} __attribute__((packed));

struct SU_SP{
  unsigned char magic[2];
  unsigned char skip;
} __attribute__((packed));

struct SU_CE{
  char extent[8];
  char offset[8];
  char size[8];
};

struct SU_ER{
  unsigned char len_id;
  unsigned char len_des;
  unsigned char len_src;
  unsigned char ext_ver;
  char data[0];
} __attribute__((packed));

struct RR_RR{
  char flags[1];
} __attribute__((packed));

struct RR_PX{
  char mode[8];
  char n_links[8];
  char uid[8];
  char gid[8];
};

struct RR_PN{
  char dev_high[8];
  char dev_low[8];
};


struct SL_component{
  unsigned char flags;
  unsigned char len;
  char text[0];
} __attribute__((packed));

struct RR_SL{
  unsigned char flags;
  struct SL_component link;
} __attribute__((packed));

struct RR_NM{
  unsigned char flags;
  char name[0];
} __attribute__((packed));

struct RR_CL{
  char location[8];
};

struct RR_PL{
  char location[8];
};

struct stamp{
  char time[7];
} __attribute__((packed));

struct RR_TF{
  char flags;
  struct stamp times[0];  /* Variable number of these beasts */
} __attribute__((packed));

/* Linux-specific extension for transparent decompression */
struct RR_ZF{
  char algorithm[2];
  char parms[2];
  char real_size[8];
};

struct rock_ridge{
  char signature[2];
  unsigned char len;
  unsigned char version;
  union{
    struct SU_SP SP;
    struct SU_CE CE;
    struct SU_ER ER;
    struct RR_RR RR;
    struct RR_PX PX;
    struct RR_PN PN;
    struct RR_SL SL;
    struct RR_NM NM;
    struct RR_CL CL;
    struct RR_PL PL;
    struct RR_TF TF;
    struct RR_ZF ZF;
  } u;
};

typedef union RR_ptr {
        struct rock_ridge *rr;
        char              *ptr;
        int                i;
} RR_ptr_t;

#include <arch/byteorder.h>

static inline int isonum_711(char *p)
{
	unsigned char x = *(unsigned char *)p;
	return x;
}
static inline int isonum_712(char *p)
{
	return *(char *)p;
}
static inline int isonum_721(char *p)
{
	return le16_to_cpu(*(unsigned short *)p);
}
static inline int isonum_722(char *p)
{
	return be16_to_cpu(*(unsigned short *)p);
}
static inline int isonum_723(char *p)
{
	/* Ignore bigendian datum due to broken mastering programs */
	return le16_to_cpu(*(unsigned short *)p);
}
static inline int isonum_731(char *p)
{
	return le32_to_cpu(*(unsigned int *)p);
}
static inline int isonum_732(char *p)
{
	return be32_to_cpu(*(unsigned int *)p);
}
static inline int isonum_733(char *p)
{
	/* Ignore bigendian datum due to broken mastering programs */
	return le32_to_cpu(*(unsigned int *)p);
}
#endif /* _ISO9660_H */
