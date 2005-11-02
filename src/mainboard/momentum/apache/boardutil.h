
#ifndef _boardutil_h_
#define _boardutil_h_


/*----------------------------------------------------------------------------+
| Board specific defines.
+----------------------------------------------------------------------------*/
#define FLASH_INTEL_SECTORSIZE 		0x00020000
#define FLASH_AMD_SECTORSIZE 		0x00010000
#define PIBS2_MAX_SIZE			0x000E0000
#define PIBS_DATABASE_SIZE     		0x00020000
#define PIBS_DATABASE_ADDR		0x00000000FFFE0000UL

#define PIBS_DATA_FIELDSIZE  		256
#define ETHERNET_HW_ADDR_LEN		6

/*----------------------------------------------------------------------------+
| Current board settings.
+----------------------------------------------------------------------------*/
typedef struct board_cfg_data {
   char           			usr_config_ver[4];
   unsigned char 			reserved[28];
   unsigned long  			tmr_freq;
   unsigned long  			mem_size;
   unsigned long  			ei_ratio;
   unsigned long  			sys_freq;
   unsigned long  			cpu_freq;
   unsigned long            		freq_ratio;
   unsigned long            		ser_freq;
   unsigned long  			procver;
   unsigned long			hid0;
   unsigned long			hid1;
   unsigned long			hid4;
   unsigned long			hid5;
   unsigned long			hior;
   unsigned long			sdr1;
   char           			procstr[16];
   unsigned char			hwaddr0[ETHERNET_HW_ADDR_LEN];
   unsigned char			pad_size[2];
} board_cfg_data_t;

/*----------------------------------------------------------------------------+
| PIBS data CPU2.
+----------------------------------------------------------------------------*/
typedef struct cpu_data {
   unsigned long			img_srr0;
   unsigned long			img_srr1;
   unsigned long			r3_value;
   unsigned long			img_valid;
} cpu_data_t;

/*----------------------------------------------------------------------------+
| PIBS data.
+----------------------------------------------------------------------------*/
typedef struct bios_data_struct {
   /*-------------------------------------------------------------------------+
   | Is this data section valid? [TRUE|FALSE] string.
   +-------------------------------------------------------------------------*/
   char 				bios_data_valid[PIBS_DATA_FIELDSIZE];
   /*-------------------------------------------------------------------------+
   | Information about the main PIBS board image [TRUE|FALSE] string.
   +-------------------------------------------------------------------------*/
   char                                 pibs2_valid[PIBS_DATA_FIELDSIZE];
   /*-------------------------------------------------------------------------+
   | Autoboot configuration.
   +-------------------------------------------------------------------------*/
   char 				autoboot_parm[PIBS_DATA_FIELDSIZE];
   /*-------------------------------------------------------------------------+
   | Configuration.
   +-------------------------------------------------------------------------*/
   char 				bios_eth_hwd0[PIBS_DATA_FIELDSIZE];
   char 				ifconfig_parm0[PIBS_DATA_FIELDSIZE];
   char 				route_parm[PIBS_DATA_FIELDSIZE];
   /*-------------------------------------------------------------------------+
   | TFTP information.
   +-------------------------------------------------------------------------*/
   char 				bios_tftp_fname[PIBS_DATA_FIELDSIZE];
   char 				bios_tftp_destip[PIBS_DATA_FIELDSIZE];
   /*-------------------------------------------------------------------------+
   | Chip and board clocking information.
   +-------------------------------------------------------------------------*/
   char 				clocking_valid[PIBS_DATA_FIELDSIZE];
   char 				clocking_parm[PIBS_DATA_FIELDSIZE];
   /*-------------------------------------------------------------------------+
   | User data, alias list, autoboot delay, dhcp flag.
   +-------------------------------------------------------------------------*/
   char 				user_data[PIBS_DATA_FIELDSIZE];
   char					aliaslist[PIBS_DATA_FIELDSIZE];
   char					autoboot_delay[PIBS_DATA_FIELDSIZE];
   char					dhcp0[PIBS_DATA_FIELDSIZE];
   /*-------------------------------------------------------------------------+
   | HT link optimization variable.
   +-------------------------------------------------------------------------*/
   char					opthtlink[PIBS_DATA_FIELDSIZE];
   /*-------------------------------------------------------------------------+
   | Indicates IDE cable type.
   +-------------------------------------------------------------------------*/
   char					ide80wire[PIBS_DATA_FIELDSIZE];
   /*-------------------------------------------------------------------------+
   | Automatically initialize IDE.
   +-------------------------------------------------------------------------*/
   char					initide[PIBS_DATA_FIELDSIZE];
   /*-------------------------------------------------------------------------+
   | OpenFirmware interface private variable
   +-------------------------------------------------------------------------*/
   char					openfirmware[PIBS_DATA_FIELDSIZE];
} bios_data_struct_t;

/*----------------------------------------------------------------------------+
| Function prototypes.
+----------------------------------------------------------------------------*/
void get_system_info(
     board_cfg_data_t			*board_cfg );

int get_hwd_addr(
    char 				*dest,
    int 				ethernet_num );

int get_sys_clk(
    unsigned long 			*value );

int get_pll_mult(
    unsigned long 			*value );

int get_ei_ratio(
    unsigned long 			*value );

int read_sp_data(
    unsigned int			offset,
    unsigned int			count,
    unsigned long			*data );

int write_sp_data(
    unsigned int			offset,
    unsigned int			data );

unsigned long read_psr(
 					void );

void write_pcr_pcrh(
     unsigned long			data );

int is_writable(
    unsigned long			addr,
    unsigned long			len );

void super_io_setup(
					void );

unsigned long sdram_size(
					void );

#endif /* _boardutil_h_ */
