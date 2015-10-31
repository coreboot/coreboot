/*
 * Copyright (C) 2014 Vladimir Serbinenko
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <stdio.h>
#include <sys/mman.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;


typedef struct {
	u16 signature;
	u8 size;
	u8 reserved[21];
	u16 pcir_offset;
	u16 vbt_offset;
} __attribute__ ((packed)) optionrom_header_t;

struct vbt_header {
	u8 signature[20];
	u16 version;
	u16 header_size;
	u16 vbt_size;
	u8 vbt_checksum;
	u8 reserved0;
	u32 bdb_offset;
	u32 aim_offset[4];
} __attribute__ ((packed));

struct bdb_header {
	u8 signature[16];
	u16 version;
	u16 header_size;
	u16 bdb_size;
};

struct vbios_data {
	u8 type;		/* 0 == desktop, 1 == mobile */
	u8 relstage;
	u8 chipset;
	u8 lvds_present:1;
	u8 tv_present:1;
	u8 rsvd2:6;		/* finish byte */
	u8 rsvd3[4];
	u8 signon[155];
	u8 copyright[61];
	u16 code_segment;
	u8 dos_boot_mode;
	u8 bandwidth_percent;
	u8 rsvd4;		/* popup memory size */
	u8 resize_pci_bios;
	u8 rsvd5;		/* is crt already on ddc2 */
} __attribute__ ((packed));

struct bdb_general_features {
	/* bits 1 */
	u8 panel_fitting:2;
	u8 flexaim:1;
	u8 msg_enable:1;
	u8 clear_screen:3;
	u8 color_flip:1;

	/* bits 2 */
	u8 download_ext_vbt:1;
	u8 enable_ssc:1;
	u8 ssc_freq:1;
	u8 enable_lfp_on_override:1;
	u8 disable_ssc_ddt:1;
	u8 rsvd7:1;
	u8 display_clock_mode:1;
	u8 rsvd8:1;		/* finish byte */

	/* bits 3 */
	u8 disable_smooth_vision:1;
	u8 single_dvi:1;
	u8 rsvd9:1;
	u8 fdi_rx_polarity_inverted:1;
	u8 rsvd10:4;		/* finish byte */

	/* bits 4 */
	u8 legacy_monitor_detect;

	/* bits 5 */
	u8 int_crt_support:1;
	u8 int_tv_support:1;
	u8 int_efp_support:1;
	u8 dp_ssc_enb:1;	/* PCH attached eDP supports SSC */
	u8 dp_ssc_freq:1;	/* SSC freq for PCH attached eDP */
	u8 rsvd11:3;		/* finish byte */
} __attribute__ ((packed));

struct common_child_dev_config {
	u16 handle;
	u16 device_type;
	u8 not_common1[12];
	u8 dvo_port;
	u8 i2c_pin;
	u8 slave_addr;
	u8 ddc_pin;
	u16 edid_ptr;
	u8 not_common3[6];
	u8 dvo_wiring;
	u8 not_common4[4];
} __attribute__ ((packed));

struct bdb_general_definitions {
	/* DDC GPIO */
	u8 crt_ddc_gmbus_pin;

	/* DPMS bits */
	u8 dpms_acpi:1;
	u8 skip_boot_crt_detect:1;
	u8 dpms_aim:1;
	u8 rsvd1:5;		/* finish byte */

	/* boot device bits */
	u8 boot_display[2];
	u8 child_dev_size;

	/*
	 * Device info:
	 * If TV is present, it'll be at devices[0].
	 * LVDS will be next, either devices[0] or [1], if present.
	 * On some platforms the number of device is 6. But could be as few as
	 * 4 if both TV and LVDS are missing.
	 * And the device num is related with the size of general definition
	 * block. It is obtained by using the following formula:
	 * number = (block_size - sizeof(bdb_general_definitions))/
	 *           sizeof(child_device_config);
	 */
	struct common_child_dev_config devices[0];
} __attribute__ ((packed));

struct bdb_driver_features {
	u8 boot_dev_algorithm:1;
	u8 block_display_switch:1;
	u8 allow_display_switch:1;
	u8 hotplug_dvo:1;
	u8 dual_view_zoom:1;
	u8 int15h_hook:1;
	u8 sprite_in_clone:1;
	u8 primary_lfp_id:1;

	u16 boot_mode_x;
	u16 boot_mode_y;
	u8 boot_mode_bpp;
	u8 boot_mode_refresh;

	u16 enable_lfp_primary:1;
	u16 selective_mode_pruning:1;
	u16 dual_frequency:1;
	u16 render_clock_freq:1;	/* 0: high freq; 1: low freq */
	u16 nt_clone_support:1;
	u16 power_scheme_ui:1;	/* 0: CUI; 1: 3rd party */
	u16 sprite_display_assign:1;	/* 0: secondary; 1: primary */
	u16 cui_aspect_scaling:1;
	u16 preserve_aspect_ratio:1;
	u16 sdvo_device_power_down:1;
	u16 crt_hotplug:1;
	u16 lvds_config:2;
	u16 tv_hotplug:1;
	u16 hdmi_config:2;

	u8 static_display:1;
	u8 reserved2:7;
	u16 legacy_crt_max_x;
	u16 legacy_crt_max_y;
	u8 legacy_crt_max_refresh;

	u8 hdmi_termination;
	u8 custom_vbt_version;
} __attribute__ ((packed));

struct bdb_lvds_options {
	u8 panel_type;
	u8 rsvd1;
	/* LVDS capabilities, stored in a dword */
	u8 pfit_mode:2;
	u8 pfit_text_mode_enhanced:1;
	u8 pfit_gfx_mode_enhanced:1;
	u8 pfit_ratio_auto:1;
	u8 pixel_dither:1;
	u8 lvds_edid:1;
	u8 rsvd2:1;
	u8 rsvd4;
} __attribute__ ((packed));

struct bdb_sdvo_lvds_options {
	u8 panel_backlight;
	u8 h40_set_panel_type;
	u8 panel_type;
	u8 ssc_clk_freq;
	u16 als_low_trip;
	u16 als_high_trip;
	u8 sclalarcoeff_tab_row_num;
	u8 sclalarcoeff_tab_row_size;
	u8 coefficient[8];
	u8 panel_misc_bits_1;
	u8 panel_misc_bits_2;
	u8 panel_misc_bits_3;
	u8 panel_misc_bits_4;
} __attribute__ ((packed));


#define BDB_GENERAL_FEATURES	  1
#define BDB_GENERAL_DEFINITIONS	  2

#define BDB_DRIVER_FEATURES	 12
#define BDB_SDVO_LVDS_OPTIONS	 22
#define BDB_SDVO_PANEL_DTDS	 23
#define BDB_LVDS_OPTIONS	 40
#define BDB_LVDS_LFP_DATA_PTRS	 41
#define BDB_LVDS_LFP_DATA	 42

#define BDB_SKIP		254

static void parse_vbt(const void *vbt)
{
	const struct vbt_header *head = vbt;
	const struct bdb_header *bdb;
	int i;
	const u8 *ptr;
	int is_first_skip = 1;

	if (memcmp(head->signature, "$VBT", 4) != 0) {
		fprintf(stderr, "invalid VBT signature\n");
		exit(1);
	}
	printf("signature: <%20.20s>\n", head->signature);
	printf("version: %d.%02d\n", head->version / 100,
	       head->version % 100);
	if (head->header_size != sizeof(struct vbt_header))
		printf("header size: 0x%x\n", head->header_size);
	printf("VBT size: 0x%x\n", head->vbt_size);
	printf("VBT checksum: 0x%x\n", head->vbt_checksum);
	if (head->reserved0)
		printf("header reserved0: 0x%x\n", head->reserved0);
	if (head->bdb_offset != sizeof(struct vbt_header))
		printf("BDB offset: 0x%x\n", head->bdb_offset);

	for (i = 0; i < 4; i++)
		if (head->aim_offset[i])
			printf("AIM[%d] offset: 0x%x\n", i,
			       head->aim_offset[i]);
	bdb = (const void *) ((const char *) vbt + head->bdb_offset);

	if (memcmp("BIOS_DATA_BLOCK ", bdb->signature, 16) != 0) {
		fprintf(stderr, "invalid BDB signature:%s\n",
			bdb->signature);
		exit(1);
	}
	printf("BDB version: %d.%02d\n", bdb->version / 100,
	       bdb->version % 100);
	if (bdb->header_size != sizeof(struct bdb_header))
		printf("BDB header size: 0x%x\n", bdb->header_size);
	if (bdb->bdb_size != head->vbt_size - head->bdb_offset)
		printf("BDB size: 0x%x\n", bdb->bdb_size);
	for (ptr = (const u8 *) bdb + bdb->header_size;
	     ptr < (const u8 *) bdb + bdb->bdb_size;) {
		u16 secsz = (ptr[1] | (ptr[2] << 8));
		u8 sectype = ptr[0];
		const u8 *section = ptr + 3;

		printf("section type %d, size 0x%x\n", sectype, secsz);
		ptr += secsz + 3;
		switch (sectype) {
		case BDB_GENERAL_FEATURES:{
				const struct bdb_general_features *sec =
				    (const void *) section;
				printf("General features:\n");

				if (sec->panel_fitting)
					printf("\tpanel_fitting = 0x%x\n",
					       sec->panel_fitting);
				if (sec->flexaim)
					printf("\tflexaim = 0x%x\n",
					       sec->flexaim);
				if (sec->msg_enable)
					printf("\tmsg_enable = 0x%x\n",
					       sec->msg_enable);
				if (sec->clear_screen)
					printf("\tclear_screen = 0x%x\n",
					       sec->clear_screen);
				if (sec->color_flip)
					printf("\tcolor_flip = 0x%x\n",
					       sec->color_flip);
				if (sec->download_ext_vbt)
					printf
					    ("\tdownload_ext_vbt = 0x%x\n",
					     sec->download_ext_vbt);
				printf("\t*enable_ssc = 0x%x\n",
				       sec->enable_ssc);
				printf("\t*ssc_freq = 0x%x\n",
				       sec->ssc_freq);
				if (sec->enable_lfp_on_override)
					printf
					    ("\tenable_lfp_on_override = 0x%x\n",
					     sec->enable_lfp_on_override);
				if (sec->disable_ssc_ddt)
					printf
					    ("\tdisable_ssc_ddt = 0x%x\n",
					     sec->disable_ssc_ddt);
				if (sec->rsvd7)
					printf("\trsvd7 = 0x%x\n",
					       sec->rsvd7);
				printf("\t*display_clock_mode = 0x%x\n",
				       sec->display_clock_mode);
				if (sec->rsvd8)
					printf("\trsvd8 = 0x%x\n",
					       sec->rsvd8);
				printf("\tdisable_smooth_vision = 0x%x\n",
				       sec->disable_smooth_vision);
				if (sec->single_dvi)
					printf("\tsingle_dvi = 0x%x\n",
					       sec->single_dvi);
				if (sec->rsvd9)
					printf("\trsvd9 = 0x%x\n",
					       sec->rsvd9);
				printf
				    ("\t*fdi_rx_polarity_inverted = 0x%x\n",
				     sec->fdi_rx_polarity_inverted);
				if (sec->rsvd10)
					printf("\trsvd10 = 0x%x\n",
					       sec->rsvd10);
				if (sec->legacy_monitor_detect)
					printf
					    ("\tlegacy_monitor_detect = 0x%x\n",
					     sec->legacy_monitor_detect);
				printf("\t*int_crt_support = 0x%x\n",
				       sec->int_crt_support);
				printf("\t*int_tv_support = 0x%x\n",
				       sec->int_tv_support);
				if (sec->int_efp_support)
					printf
					    ("\tint_efp_support = 0x%x\n",
					     sec->int_efp_support);
				if (sec->dp_ssc_enb)
					printf("\tdp_ssc_enb = 0x%x\n",
					       sec->dp_ssc_enb);
				if (sec->dp_ssc_freq)
					printf("\tdp_ssc_freq = 0x%x\n",
					       sec->dp_ssc_freq);
				if (sec->rsvd11)
					printf("\trsvd11 = 0x%x\n",
					       sec->rsvd11);
				break;
			}
		case BDB_DRIVER_FEATURES:{
				const struct bdb_driver_features *sec =
				    (const void *) section;
				printf("\t*LVDS config: %d\n",
				       sec->lvds_config);
				printf("\t*Dual frequency: %d\n",
				       sec->dual_frequency);

				break;
			}
		case BDB_SDVO_LVDS_OPTIONS:{
				const struct bdb_sdvo_lvds_options *sec =
				    (const void *) section;
				printf("\t*Panel type: %d\n",
				       sec->panel_type);

				break;
			}
		case BDB_GENERAL_DEFINITIONS:{
				const struct bdb_general_definitions *sec =
				    (const void *) section;
				int ndev;
				printf("\t*CRT DDC GMBUS pin: %d\n",
				       sec->crt_ddc_gmbus_pin);

				printf("\tDPMS ACPI: %d\n",
				       sec->dpms_acpi);
				printf("\tSkip boot CRT detect: %d\n",
				       sec->skip_boot_crt_detect);
				printf("\tDPMS aim: %d\n", sec->dpms_aim);
				if (sec->rsvd1)
					printf("\trsvd1: 0x%x\n",
					       sec->rsvd1);
				printf("\tboot_display: { %x, %x }\n",
				       sec->boot_display[0],
				       sec->boot_display[1]);
				if (sec->child_dev_size !=
				    sizeof(struct common_child_dev_config))
					printf("\tchild_dev_size: %d\n",
					       sec->child_dev_size);
				ndev = (secsz - sizeof(*sec)) /
					sizeof(struct common_child_dev_config);
				printf("\t%d devices\n", ndev);
				for (i = 0; i < ndev; i++) {
					printf("\t*device type: %x ",
					       sec->devices[i].
					       device_type);
#define	 DEVICE_TYPE_INT_LFP	0x1022
#define	 DEVICE_TYPE_INT_TV	0x1009
#define DEVICE_TYPE_EFP_DVI_HOTPLUG_PWR	0x6052
					switch (sec->devices[i].device_type) {
					case DEVICE_TYPE_INT_LFP:
						printf("(flat panel)\n");
						break;
					case DEVICE_TYPE_INT_TV:
						printf("(TV)\n");
						break;
					case DEVICE_TYPE_EFP_DVI_HOTPLUG_PWR:
						printf
						    ("(DVI)\n");
						break;
					case 0:
						printf("(Empty)\n");
						break;
					default:
						printf("(Unknown)\n");
						break;
					}
					if (!sec->devices[i].device_type)
						continue;
					printf("\t *dvo_port: %x\n",
					       sec->devices[i].dvo_port);
					printf("\t *i2c_pin: %x\n",
					       sec->devices[i].i2c_pin);
					printf("\t *slave_addr: %x\n",
					       sec->devices[i].slave_addr);
					printf("\t *ddc_pin: %x\n",
					       sec->devices[i].ddc_pin);
					printf("\t *dvo_wiring: %x\n",
					       sec->devices[i].dvo_wiring);
					printf("\t edid_ptr: %x\n",
					       sec->devices[i].edid_ptr);
				}

				break;
			}
		case BDB_SKIP:{
				const struct vbios_data *sec =
				    (const void *) section;
				if (!is_first_skip)
					break;
				is_first_skip = 0;
				printf("\ttype: %x\n", sec->type);
				printf("\trelstage: %x\n", sec->relstage);
				printf("\tchipset: %x\n", sec->chipset);
				printf(sec->lvds_present ? "\tLVDS\n"
				       : "\tNo LVDS\n");
				printf(sec->tv_present ? "\tTV\n"
				       : "\tNo TV\n");
				if (sec->rsvd2)
					printf("\trsvd2: 0x%x\n",
					       sec->rsvd2);
				for (i = 0; i < 4; i++)
					if (sec->rsvd3[i])
						printf
						    ("\trsvd3[%d]: 0x%x\n",
						     i, sec->rsvd3[i]);
				printf("\tSignon: %.155s\n", sec->signon);
				printf("\tCopyright: %.155s\n",
				       sec->copyright);
				printf("\tCode segment: %x\n",
				       sec->code_segment);
				printf("\tDOS Boot mode: %x\n",
				       sec->dos_boot_mode);
				printf("\tBandwidth percent: %x\n",
				       sec->bandwidth_percent);
				if (sec->rsvd4)
					printf("\trsvd4: 0x%x\n",
					       sec->rsvd4);
				printf("\tBandwidth percent: %x\n",
				       sec->resize_pci_bios);
				if (sec->rsvd5)
					printf("\trsvd5: 0x%x\n",
					       sec->rsvd5);
				break;
			}
		}
	}

}

static void parse_vbios(const void *ptr)
{
	const optionrom_header_t *oh;
	oh = ptr;
	if (oh->signature != 0xaa55) {
		fprintf(stderr, "bad oprom signature: %x\n",
			oh->signature);
		return;
	}
	if (!oh->vbt_offset) {
		fprintf(stderr, "no VBT found\n");
		return;
	}
	parse_vbt((const char *) ptr + oh->vbt_offset);
}

int main(int argc, char **argv)
{
	const void *ptr;
	int fd;
	if (argc == 2) {
		fd = open(argv[1], O_RDONLY);
		ptr = mmap(0, 65536, PROT_READ, MAP_SHARED, fd, 0);
	} else {
		fd = open("/dev/mem", O_RDONLY);
		ptr = mmap(0, 65536, PROT_READ, MAP_SHARED, fd, 0xc0000);
	}
	if (ptr == MAP_FAILED) {
		fprintf(stderr, "mmap failed: %s\n", strerror(errno));
		return 1;
	}
	parse_vbios(ptr);
	close(fd);
	return 0;
}
