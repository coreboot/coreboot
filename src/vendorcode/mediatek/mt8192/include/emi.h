/* SPDX-License-Identifier: BSD-3-Clause */

#ifndef __EMI_H__
#define __EMI_H__

#include <soc/addressmap.h>
#include <dramc_top.h>

#define EMI_APB_BASE	EMI_BASE

#if __ETT__
#define emi_log	printf
#elif __FLASH_TOOL_DA__
#define emi_log	LOGD
#else
#define emi_log(_x_...) printk(BIOS_INFO, _x_)
#endif

struct isu_info_t {
	unsigned int buf_size;
	unsigned long long buf_addr;
	unsigned long long ver_addr;
	unsigned long long con_addr;
};

#define EMI_ISU_BUF_SIZE	0x800000
#define LAST_EMI_MAGIC_PATTERN 0x19870611
typedef struct {
	unsigned int isu_magic;
	unsigned int isu_ctrl;
	unsigned int isu_dram_type;
	unsigned int isu_diff_us;
	unsigned int isu_buf_l;
	unsigned int isu_buf_h;
	unsigned int isu_version;
	unsigned int snst_last;
	unsigned int snst_past;
	unsigned int os_flag_sspm;
	unsigned int os_flag_ap;
} LAST_EMI_INFO_T;
#define DEF_LAST_EMI LAST_EMI_INFO_T

void emi_init(void);
void emi_init2(void);
void clr_emi_mpu_prot(void);
void dis_emi_apb_prot(void);
int get_row_width_by_emi(unsigned int rank);
int get_channel_nr_by_emi(void);
int get_rank_nr_by_emi(void);
void get_rank_size_by_emi(unsigned long long dram_rank_size[DRAMC_MAX_RK]);
void set_cen_emi_cona(unsigned int cona_val);
void set_cen_emi_conf(unsigned int conf_val);
void set_cen_emi_conh(unsigned int conh_val);
void set_chn_emi_cona(unsigned int cona_val);
void set_chn_emi_conc(unsigned int conc_val);
unsigned int get_cen_emi_cona(void);
unsigned int get_chn_emi_cona(void);
void phy_addr_to_dram_addr(dram_addr_t *dram_addr, unsigned long long phy_addr);
unsigned int set_emi_before_rank1_mem_test(void);
void restore_emi_after_rank1_mem_test(void);
void get_emi_isu_info(struct isu_info_t *isu_info_ptr);
void reserve_emi_isu_buf(void);
void reserve_emi_mbist_buf(void);
void record_emi_snst(void);
unsigned long long platform_memory_size(void);

#endif /* __EMI_H__ */

