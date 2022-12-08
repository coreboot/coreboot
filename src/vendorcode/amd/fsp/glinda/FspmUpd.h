/** @file
 *
 * This file is _NOT_ automatically generated in coreboot!
 *
 */

/* TODO: Update for Glinda */

#ifndef __FSPMUPD_H__
#define __FSPMUPD_H__

#include <FspUpd.h>
#include <FspUsb.h>

#define FSPM_UPD_DXIO_DESCRIPTOR_COUNT	14
#define FSPM_UPD_DDI_DESCRIPTOR_COUNT	5

/** Fsp M Configuration
**/
typedef struct __packed {
	/** Offset 0x0040**/	uint32_t                    bert_size;
	/** Offset 0x0044**/	uint32_t                    tseg_size;
	/** Offset 0x0048**/	uint32_t                    pci_express_base_addr;
	/** Offset 0x004C**/	uint8_t                     misc_reserved[32];
	/** Offset 0x006C**/	uint32_t                    serial_port_base;
	/** Offset 0x0070**/	uint32_t                    serial_port_use_mmio;
	/** Offset 0x0074**/	uint32_t                    serial_port_baudrate;
	/** Offset 0x0078**/	uint32_t                    serial_port_refclk;
	/** Offset 0x007C**/	uint32_t                    serial_reserved;
	/** Offset 0x0080**/	uint8_t                     dxio_descriptor[FSPM_UPD_DXIO_DESCRIPTOR_COUNT][52];
	/** Offset 0x0358**/	uint8_t                     fsp_owns_pcie_resets;
	/** Offset 0x0359**/	uint8_t                     pcie_reserved[51];
	/** Offset 0x038C**/	uint32_t                    ddi_descriptor[FSPM_UPD_DDI_DESCRIPTOR_COUNT];
	/** Offset 0x03A0**/	uint8_t                     ddi_reserved[6];
	/** Offset 0x03A6**/	uint8_t                     ccx_down_core_mode;
	/** Offset 0x03A7**/	uint8_t                     ccx_disable_smt;
	/** Offset 0x03A8**/	uint8_t                     ccx_reserved[32];
	/** Offset 0x03C8**/	uint8_t                     stt_control;
	/** Offset 0x03C9**/	uint8_t                     stt_pcb_sensor_count;
	/** Offset 0x03CA**/	uint16_t                    stt_min_limit;
	/** Offset 0x03CC**/	uint16_t                    stt_m1;
	/** Offset 0x03CE**/	uint16_t                    stt_m2;
	/** Offset 0x03D0**/	uint16_t                    stt_m3;
	/** Offset 0x03D2**/	uint16_t                    stt_m4;
	/** Offset 0x03D4**/	uint16_t                    stt_m5;
	/** Offset 0x03D6**/	uint16_t                    stt_m6;
	/** Offset 0x03D8**/	uint16_t                    stt_c_apu;
	/** Offset 0x03DA**/	uint16_t                    stt_c_gpu;
	/** Offset 0x03DC**/	uint16_t                    stt_c_hs2;
	/** Offset 0x03DE**/	uint16_t                    stt_alpha_apu;
	/** Offset 0x03E0**/	uint16_t                    stt_alpha_gpu;
	/** Offset 0x03E2**/	uint16_t                    stt_alpha_hs2;
	/** Offset 0x03E4**/	uint16_t                    stt_skin_temp_apu;
	/** Offset 0x03E6**/	uint16_t                    stt_skin_temp_gpu;
	/** Offset 0x03E8**/	uint16_t                    stt_skin_temp_hs2;
	/** Offset 0x03EA**/	uint16_t                    stt_error_coeff;
	/** Offset 0x03EC**/	uint16_t                    stt_error_rate_coefficient;
	/** Offset 0x03EE**/	uint8_t                     smartshift_enable;
	/** Offset 0x03EF**/	uint32_t                    apu_only_sppt_limit;
	/** Offset 0x03F3**/	uint32_t                    sustained_power_limit;
	/** Offset 0x03F7**/	uint32_t                    fast_ppt_limit;
	/** Offset 0x03FB**/	uint32_t                    slow_ppt_limit;
	/** Offset 0x03FF**/	uint8_t                     system_configuration;
	/** Offset 0x0400**/	uint8_t                     cppc_ctrl;
	/** Offset 0x0401**/	uint8_t                     cppc_perf_limit_max_range;
	/** Offset 0x0402**/	uint8_t                     cppc_perf_limit_min_range;
	/** Offset 0x0403**/	uint8_t                     cppc_epp_max_range;
	/** Offset 0x0404**/	uint8_t                     cppc_epp_min_range;
	/** Offset 0x0405**/	uint8_t                     cppc_preferred_cores;
	/** Offset 0x0406**/	uint8_t                     stapm_boost;
	/** Offset 0x0407**/	uint32_t                    stapm_time_constant;
	/** Offset 0x040B**/	uint32_t                    slow_ppt_time_constant;
	/** Offset 0x040F**/	uint32_t                    thermctl_limit;
	/** Offset 0x0413**/	uint8_t                     smu_soc_tuning_reserved[9];
	/** Offset 0x041C**/	uint8_t                     iommu_support;
	/** Offset 0x041D**/	uint8_t                     pspp_policy;
	/** Offset 0x041E**/	uint8_t                     enable_nb_azalia;
	/** Offset 0x041F**/	uint8_t                     audio_io_ctl;
	/** Offset 0x0420**/	uint8_t                     pdm_mic_selection;
	/** Offset 0x0421**/	uint8_t                     hda_enable;
	/** Offset 0x0422**/	uint8_t                     nbio_reserved[31];
	/** Offset 0x0441**/	uint32_t                    emmc0_mode;
	/** Offset 0x0445**/	uint16_t                    emmc0_init_khz_preset;
	/** Offset 0x0447**/	uint8_t                     emmc0_sdr104_hs400_driver_strength;
	/** Offset 0x0448**/	uint8_t                     emmc0_ddr50_driver_strength;
	/** Offset 0x0449**/	uint8_t                     emmc0_sdr50_driver_strength;
	/** Offset 0x044A**/	uint8_t                     UnusedUpdSpace0[85];
	/** Offset 0x049F**/	uint32_t                    gnb_ioapic_base;
	/** Offset 0x04A3**/	uint8_t                     gnb_ioapic_id;
	/** Offset 0x04A4**/	uint8_t                     fch_ioapic_id;
	/** Offset 0x04A5**/	uint8_t                     sata_enable;
	/** Offset 0x04A6**/	uint8_t                     fch_reserved[32];
	/** Offset 0x04C6**/	uint8_t                     s0i3_enable;
	/** Offset 0x04C7**/	uint32_t                    telemetry_vddcrvddfull_scale_current;
	/** Offset 0x04CB**/	uint32_t                    telemetry_vddcrvddoffset;
	/** Offset 0x04CF**/	uint32_t                    telemetry_vddcrsocfull_scale_current;
	/** Offset 0x04D3**/	uint32_t                    telemetry_vddcrsocOffset;
	/** Offset 0x04D7**/	uint8_t                     UnusedUpdSpace1;
	/* usb_phy_ptr is actually struct usb_phy_config *, but that won't work for 64bit coreboot */
	/** Offset 0x04D8**/	uint32_t                    usb_phy_ptr;
	/** Offset 0x04DC**/	uint8_t                     UnusedUpdSpace2[292];
	/** Offset 0x0600**/	uint16_t                    UpdTerminator;
} FSP_M_CONFIG;

/** Fsp M UPD Configuration
**/
typedef struct __packed {
	/** Offset 0x0000**/	FSP_UPD_HEADER              FspUpdHeader;
	/** Offset 0x0020**/	FSPM_ARCH_UPD               FspmArchUpd;
	/** Offset 0x0040**/	FSP_M_CONFIG                FspmConfig;
} FSPM_UPD;

#define IMAGE_REVISION_MAJOR_VERSION 0x01
#define IMAGE_REVISION_MINOR_VERSION 0x00
#define IMAGE_REVISION_REVISION 0x05
#define IMAGE_REVISION_BUILD_NUMBER 0x00

#endif
