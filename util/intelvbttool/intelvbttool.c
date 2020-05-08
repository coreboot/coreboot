/* SPDX-License-Identifier: GPL-2.0-only */

#include <stdio.h>
#include <sys/mman.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <getopt.h>
#include <errno.h>
#include <stdarg.h>
#include <commonlib/helpers.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;

#define DEF_ALLOC 1024

typedef struct {
	u16 signature;
	u8 size;
	u8 entrypoint[4];
	u8 checksum;
	u8 reserved[16];
	u16 pcir_offset;
	u16 vbt_offset;
} __attribute__ ((packed)) optionrom_header_t;

typedef struct {
	u32 signature;
	u16 vendor;
	u16 device;
	u16 reserved1;
	u16 length;
	u8  revision;
	u8  classcode[3];
	u16 imagelength;
	u16 coderevision;
	u8  codetype;
	u8  indicator;
	u16 reserved2;
} __attribute__((packed)) optionrom_pcir_t;

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


static const size_t ignore_checksum = 1;

#define BDB_GENERAL_FEATURES	  1
#define BDB_GENERAL_DEFINITIONS	  2

#define BDB_DRIVER_FEATURES	 12
#define BDB_SDVO_LVDS_OPTIONS	 22
#define BDB_SDVO_PANEL_DTDS	 23
#define BDB_LVDS_OPTIONS	 40
#define BDB_LVDS_LFP_DATA_PTRS	 41
#define BDB_LVDS_LFP_DATA	 42

#define BDB_SKIP		254

/* print helpers */
static void print(const char *format, ...)
{
	va_list args;
	fprintf(stdout, "VBTTOOL: ");
	va_start(args, format);
	vfprintf(stdout, format, args);
	va_end(args);
}

static void printt(const char *format, ...)
{
	va_list args;
	fprintf(stdout, "\t");
	va_start(args, format);
	vfprintf(stdout, format, args);
	va_end(args);
}

static void printwarn(const char *format, ...)
{
	va_list args;
	fprintf(stderr, "VBTTOOL: WARN: ");
	va_start(args, format);
	vfprintf(stderr, format, args);
	va_end(args);
}

static void printerr(const char *format, ...)
{
	va_list args;
	fprintf(stderr, "VBTTOOL: ERR: ");
	va_start(args, format);
	vfprintf(stderr, format, args);
	va_end(args);
}

struct fileobject {
	unsigned char *data;
	size_t size;
};

/* file object helpers */

/*
 * Alloc a file object of given size.
 * Returns NULL on error.
 */
static struct fileobject *malloc_fo(const size_t size)
{
	struct fileobject *fo;
	if (!size)
		return NULL;

	fo = malloc(sizeof(*fo));
	if (!fo)
		return NULL;
	fo->data = malloc(size);
	if (!fo->data) {
		free(fo);
		return NULL;
	}
	fo->size = size;

	return fo;
}

/* Free a fileobject structure */
static void free_fo(struct fileobject *fo)
{
	if (fo) {
		free(fo->data);
		free(fo);
	}
}

/* Resize file object and keep memory content */
static struct fileobject *remalloc_fo(struct fileobject *old,
				      const size_t size)
{
	struct fileobject *fo = old;

	if (!old || !size)
		return NULL;

	fo->data = realloc(fo->data, size);
	if (!fo->data)
		return NULL;

	if (fo->size < size)
		memset(&fo->data[fo->size], 0, size - fo->size);

	fo->size = size;

	return fo;
}

/*
 * Creates a new subregion copy of fileobject.
 * Returns NULL if offset is greater than fileobject size.
 * Returns NULL on error.
 */
static struct fileobject *malloc_fo_sub(const struct fileobject *old,
					const size_t off)
{
	struct fileobject *fo;

	if (!old || off > old->size)
		return NULL;

	fo = malloc_fo(old->size - off);
	if (!fo)
		return NULL;

	memcpy(fo->data, old->data + off, fo->size);

	return fo;
}

/* file helpers */

/* Create fileobject from file */
static struct fileobject *read_file(const char *filename)
{
	FILE *fd = fopen(filename, "rb");
	off_t read_size = DEF_ALLOC;

	if (!fd) {
		printerr("%s open failed: %s\n", filename, strerror(errno));
		return NULL;
	}

	struct fileobject *fo = malloc_fo(read_size);
	if (!fo) {
		printerr("malloc failed\n");
		fclose(fd);
		return NULL;
	}

	off_t total_bytes_read = 0, bytes_read;
	while ((bytes_read = fread(fo->data + total_bytes_read, 1, read_size, fd)) > 0) {
		total_bytes_read += bytes_read;
		struct fileobject *newfo = remalloc_fo(fo, fo->size + read_size);
		if (!newfo) {
			fclose(fd);
			free_fo(fo);
			return NULL;
		}
		fo = newfo;
	}

	if (!total_bytes_read) {
		fclose(fd);
		free_fo(fo);
		return NULL;
	}

	if (fclose(fd)) {
		printerr("%s close failed: %s\n", filename, strerror(errno));
		free_fo(fo);
		return NULL;
	}

	fo->size = total_bytes_read;

	return fo;
}

/* Create fileobject from physical memory at given address of size 64 KiB */
static struct fileobject *read_physmem(size_t addr)
{
	const int fd = open("/dev/mem", O_RDONLY);
	const size_t size = 64 * 2 * KiB;
	if (fd < 0) {
		printerr("/dev/mem open failed: %s\n", strerror(errno));
		return NULL;
	}

	const void *data = mmap(0, size, PROT_READ, MAP_SHARED, fd, addr);
	if (data == MAP_FAILED) {
		close(fd);
		printerr("mmap failed: %s\n", strerror(errno));
		return NULL;
	}

	struct fileobject *fo = malloc_fo(size);
	if (!fo) {
		printerr("malloc failed\n");
		munmap((void *)data, size);
		close(fd);
		return NULL;
	}

	memcpy(fo->data, data, size);
	munmap((void *)data, size);

	if (close(fd)) {
		printerr("/dev/mem close failed: %s\n", strerror(errno));
		free_fo(fo);
		return NULL;
	}

	return fo;
}

/* Write fileobject contents to file */
static int write_file(const char *filename, const struct fileobject *fo)
{
	FILE *fd_out = fopen(filename, "wb");

	if (!fd_out) {
		printerr("%s open failed: %s\n", filename, strerror(errno));
		return 1;
	}
	if (fwrite(fo->data, 1, fo->size, fd_out) != fo->size) {
		fclose(fd_out);
		return 1;
	}
	return fclose(fd_out);
}

/* dump VBT contents in human readable form */
static void dump_vbt(const struct fileobject *fo)
{
	if (fo->size < sizeof(struct vbt_header))
		return;

	const struct vbt_header *head = (const struct vbt_header *)fo->data;
	const struct bdb_header *bdb;
	const u8 *ptr;
	int i;
	int is_first_skip = 1;

	printt("signature: <%20.20s>\n", head->signature);
	printt("version: %d.%02d\n", head->version / 100,
	       head->version % 100);
	if (head->header_size != sizeof(struct vbt_header))
		printt("header size: 0x%x\n", head->header_size);
	printt("VBT size: 0x%x\n", head->vbt_size);
	printt("VBT checksum: 0x%x\n", head->vbt_checksum);
	if (head->reserved0)
		printt("header reserved0: 0x%x\n", head->reserved0);
	if (head->bdb_offset != sizeof(struct vbt_header))
		printt("BDB offset: 0x%x\n", head->bdb_offset);

	for (i = 0; i < 4; i++)
		if (head->aim_offset[i])
			printt("AIM[%d] offset: 0x%x\n", i,
			       head->aim_offset[i]);
	if (head->bdb_offset + sizeof(*bdb) > fo->size)
		return;
	bdb = (const void *) (fo->data + head->bdb_offset);

	if (memcmp("BIOS_DATA_BLOCK ", bdb->signature, 16) != 0) {
		printerr("invalid BDB signature:%s\n",
			bdb->signature);
		exit(1);
	}
	printt("BDB version: %d.%02d\n", bdb->version / 100,
	       bdb->version % 100);
	if (bdb->header_size != sizeof(struct bdb_header))
		printt("BDB header size: 0x%x\n", bdb->header_size);
	if (bdb->bdb_size != head->vbt_size - head->bdb_offset)
		printt("BDB size: 0x%x\n", bdb->bdb_size);
	for (ptr = (const u8 *) bdb + bdb->header_size;
	     ptr < (const u8 *) bdb + bdb->bdb_size;) {
		u16 secsz = (ptr[1] | (ptr[2] << 8));
		u8 sectype = ptr[0];
		const u8 *section = ptr + 3;

		printt("section type %d, size 0x%x\n", sectype, secsz);
		ptr += secsz + 3;
		switch (sectype) {
		case BDB_GENERAL_FEATURES:{
				const struct bdb_general_features *sec =
				    (const void *) section;
				printt("General features:\n");

				if (sec->panel_fitting)
					printt("\tpanel_fitting = 0x%x\n",
					       sec->panel_fitting);
				if (sec->flexaim)
					printt("\tflexaim = 0x%x\n",
					       sec->flexaim);
				if (sec->msg_enable)
					printt("\tmsg_enable = 0x%x\n",
					       sec->msg_enable);
				if (sec->clear_screen)
					printt("\tclear_screen = 0x%x\n",
					       sec->clear_screen);
				if (sec->color_flip)
					printt("\tcolor_flip = 0x%x\n",
					       sec->color_flip);
				if (sec->download_ext_vbt)
					printt
					    ("\tdownload_ext_vbt = 0x%x\n",
					     sec->download_ext_vbt);
				printt("\t*enable_ssc = 0x%x\n",
				       sec->enable_ssc);
				printt("\t*ssc_freq = 0x%x\n",
				       sec->ssc_freq);
				if (sec->enable_lfp_on_override)
					printt
					    ("\tenable_lfp_on_override = 0x%x\n",
					     sec->enable_lfp_on_override);
				if (sec->disable_ssc_ddt)
					printt
					    ("\tdisable_ssc_ddt = 0x%x\n",
					     sec->disable_ssc_ddt);
				if (sec->rsvd7)
					printt("\trsvd7 = 0x%x\n",
					       sec->rsvd7);
				printt("\t*display_clock_mode = 0x%x\n",
				       sec->display_clock_mode);
				if (sec->rsvd8)
					printt("\trsvd8 = 0x%x\n",
					       sec->rsvd8);
				printt("\tdisable_smooth_vision = 0x%x\n",
				       sec->disable_smooth_vision);
				if (sec->single_dvi)
					printt("\tsingle_dvi = 0x%x\n",
					       sec->single_dvi);
				if (sec->rsvd9)
					printt("\trsvd9 = 0x%x\n",
					       sec->rsvd9);
				printt
				    ("\t*fdi_rx_polarity_inverted = 0x%x\n",
				     sec->fdi_rx_polarity_inverted);
				if (sec->rsvd10)
					printt("\trsvd10 = 0x%x\n",
					       sec->rsvd10);
				if (sec->legacy_monitor_detect)
					printt
					    ("\tlegacy_monitor_detect = 0x%x\n",
					     sec->legacy_monitor_detect);
				printt("\t*int_crt_support = 0x%x\n",
				       sec->int_crt_support);
				printt("\t*int_tv_support = 0x%x\n",
				       sec->int_tv_support);
				if (sec->int_efp_support)
					printt
					    ("\tint_efp_support = 0x%x\n",
					     sec->int_efp_support);
				if (sec->dp_ssc_enb)
					printt("\tdp_ssc_enb = 0x%x\n",
					       sec->dp_ssc_enb);
				if (sec->dp_ssc_freq)
					printt("\tdp_ssc_freq = 0x%x\n",
					       sec->dp_ssc_freq);
				if (sec->rsvd11)
					printt("\trsvd11 = 0x%x\n",
					       sec->rsvd11);
				break;
			}
		case BDB_DRIVER_FEATURES:{
				const struct bdb_driver_features *sec =
				    (const void *) section;
				printt("\t*LVDS config: %d\n",
				       sec->lvds_config);
				printt("\t*Dual frequency: %d\n",
				       sec->dual_frequency);

				break;
			}
		case BDB_SDVO_LVDS_OPTIONS:{
				const struct bdb_sdvo_lvds_options *sec =
				    (const void *) section;
				printt("\t*Panel type: %d\n",
				       sec->panel_type);

				break;
			}
		case BDB_GENERAL_DEFINITIONS:{
				const struct bdb_general_definitions *sec =
				    (const void *) section;
				int ndev;
				printt("\t*CRT DDC GMBUS pin: %d\n",
				       sec->crt_ddc_gmbus_pin);

				printt("\tDPMS ACPI: %d\n",
				       sec->dpms_acpi);
				printt("\tSkip boot CRT detect: %d\n",
				       sec->skip_boot_crt_detect);
				printt("\tDPMS aim: %d\n", sec->dpms_aim);
				if (sec->rsvd1)
					printt("\trsvd1: 0x%x\n",
					       sec->rsvd1);
				printt("\tboot_display: { %x, %x }\n",
				       sec->boot_display[0],
				       sec->boot_display[1]);
				if (sec->child_dev_size !=
				    sizeof(struct common_child_dev_config))
					printt("\tchild_dev_size: %d\n",
					       sec->child_dev_size);
				ndev = (secsz - sizeof(*sec)) /
					sizeof(struct common_child_dev_config);
				printt("\t%d devices\n", ndev);
				for (i = 0; i < ndev; i++) {
					printt("\t*device type: %x ",
					       sec->devices[i].
					       device_type);
#define	 DEVICE_TYPE_INT_LFP	0x1022
#define	 DEVICE_TYPE_INT_TV	0x1009
#define DEVICE_TYPE_EFP_DVI_HOTPLUG_PWR	0x6052
					switch (sec->devices[i].device_type) {
					case DEVICE_TYPE_INT_LFP:
						printt("(flat panel)\n");
						break;
					case DEVICE_TYPE_INT_TV:
						printt("(TV)\n");
						break;
					case DEVICE_TYPE_EFP_DVI_HOTPLUG_PWR:
						printt
						    ("(DVI)\n");
						break;
					case 0:
						printt("(Empty)\n");
						break;
					default:
						printt("(Unknown)\n");
						break;
					}
					if (!sec->devices[i].device_type)
						continue;
					printt("\t *dvo_port: %x\n",
					       sec->devices[i].dvo_port);
					printt("\t *i2c_pin: %x\n",
					       sec->devices[i].i2c_pin);
					printt("\t *slave_addr: %x\n",
					       sec->devices[i].slave_addr);
					printt("\t *ddc_pin: %x\n",
					       sec->devices[i].ddc_pin);
					printt("\t *dvo_wiring: %x\n",
					       sec->devices[i].dvo_wiring);
					printt("\t edid_ptr: %x\n",
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
				printt("\ttype: %x\n", sec->type);
				printt("\trelstage: %x\n", sec->relstage);
				printt("\tchipset: %x\n", sec->chipset);
				printt(sec->lvds_present ? "\tLVDS\n"
				       : "\tNo LVDS\n");
				printt(sec->tv_present ? "\tTV\n"
				       : "\tNo TV\n");
				if (sec->rsvd2)
					printt("\trsvd2: 0x%x\n",
					       sec->rsvd2);
				for (i = 0; i < 4; i++)
					if (sec->rsvd3[i])
						printt
						    ("\trsvd3[%d]: 0x%x\n",
						     i, sec->rsvd3[i]);
				printt("\tSignon: %.155s\n", sec->signon);
				printt("\tCopyright: %.155s\n",
				       sec->copyright);
				printt("\tCode segment: %x\n",
				       sec->code_segment);
				printt("\tDOS Boot mode: %x\n",
				       sec->dos_boot_mode);
				printt("\tBandwidth percent: %x\n",
				       sec->bandwidth_percent);
				if (sec->rsvd4)
					printt("\trsvd4: 0x%x\n",
					       sec->rsvd4);
				printt("\tBandwidth percent: %x\n",
				       sec->resize_pci_bios);
				if (sec->rsvd5)
					printt("\trsvd5: 0x%x\n",
					       sec->rsvd5);
				break;
			}
		}
	}
}

/* Returns a new fileobject containing a valid VBT */
static void parse_vbt(const struct fileobject *fo,
		      struct fileobject **vbt)
{
	*vbt = NULL;

	if (fo->size < sizeof(struct vbt_header)) {
		printerr("image is to small\n");
		return;
	}

	const struct vbt_header *head =
	    (const struct vbt_header *)fo->data;

	if (memcmp(head->signature, "$VBT", 4) != 0) {
		printerr("invalid VBT signature\n");
		return;
	}

	if (!head->vbt_size || head->vbt_size > fo->size) {
		printerr("invalid VBT size\n");
		return;
	}

	if (!head->bdb_offset ||
	    head->bdb_offset > fo->size - sizeof(struct bdb_header)) {
		printerr("invalid BDB offset\n");
		return;
	}

	if (!head->header_size || head->header_size > fo->size) {
		printerr("invalid header size\n");
		return;
	}

	const struct bdb_header *const bdb_head =
	    (const struct bdb_header *)((const u8 *)head + head->bdb_offset);
	if (memcmp(bdb_head->signature, "BIOS_DATA_BLOCK ", 16) != 0) {
		printerr("invalid BDB signature\n");
		return;
	}

	if (!bdb_head->header_size || bdb_head->header_size > fo->size) {
		printerr("invalid BDB header size\n");
		return;
	}

	/* Duplicate fo as caller is owner and remalloc frees the object */
	struct fileobject *dupfo = malloc_fo_sub(fo, 0);
	if (!dupfo) {
		printerr("malloc failed\n");
		return;
	}

	struct fileobject *newfo = remalloc_fo(dupfo, head->vbt_size);
	if (!newfo) {
		printerr("remalloc failed\n");
		free_fo(dupfo);
		return;
	}

	*vbt = newfo;
}

/* Option ROM checksum */
static u8 checksum_vbios(const optionrom_header_t *oh)
{
	const u8 *ptr = (const u8 *)oh;
	size_t i;

	u8 cksum = 0;
	for (i = 0; i < ((MIN(oh->size, 128)) * 512); i++)
		cksum += ptr[i];

	return cksum;
}

/* Verify Option ROM contents */
static int is_valid_vbios(const struct fileobject *fo)
{
	if (fo->size > 64 * 2 * KiB) {
		printerr("VBIOS is to big\n");
		return 0;
	}

	if (fo->size < sizeof(optionrom_header_t)) {
		printerr("VBIOS is to small\n");
		return 0;
	}

	const optionrom_header_t *oh = (const optionrom_header_t *)fo->data;

	if (oh->signature != 0xaa55) {
		printerr("bad oprom signature: 0x%x\n", oh->signature);
		return 0;
	}

	if (oh->size == 0 || oh->size > 0x80 || oh->size * 512 > fo->size) {
		printerr("bad oprom size: 0x%x\n", oh->size);
		return 0;
	}

	const u8 cksum = checksum_vbios(oh);
	if (cksum) {
		if (!ignore_checksum) {
			printerr("bad oprom checksum: 0x%x\n", cksum);
			return 0;
		}
		printwarn("bad oprom checksum: 0x%x\n", cksum);
	}

	if (oh->pcir_offset + sizeof(optionrom_pcir_t) > fo->size) {
		printerr("bad pcir offset: 0x%x\n", oh->pcir_offset);
		return 0;
	}

	if (oh->pcir_offset) {
		const optionrom_pcir_t *pcir;
		pcir = (const optionrom_pcir_t *)
		    ((const u8 *)oh + oh->pcir_offset);

		if (pcir->signature != 0x52494350) {
			printerr("Invalid PCIR signature\n");
			return 0;
		}

		if (pcir->vendor != 0x8086) {
			printerr("Not an Intel VBIOS\n");
			return 0;
		}

		if (pcir->classcode[0] != 0 || pcir->classcode[1] != 0 ||
		    pcir->classcode[2] != 3) {
			printerr("Not a VGA Option Rom\n");
			return 0;
		}
	} else {
		printwarn("no PCIR header found\n");
	}

	return 1;
}

/*
 * Parse Option ROM and return a valid VBT fileobject.
 * Caller has to make sure that it is a valid VBIOS.
 * Return a NULL fileobject on error.
 */
static void parse_vbios(const struct fileobject *fo,
			struct fileobject **vbt)
{
	const optionrom_header_t *oh = (const optionrom_header_t *)fo->data;
	size_t i;

	*vbt = NULL;

	if (!oh->vbt_offset) {
		printerr("no VBT found\n");
		return;
	}

	if (oh->vbt_offset > (fo->size - sizeof(struct vbt_header))) {
		printerr("invalid VBT offset\n");
		return;
	}

	struct fileobject *fo_vbt = malloc_fo_sub(fo, oh->vbt_offset);
	if (fo_vbt) {
		parse_vbt(fo_vbt, vbt);
		free_fo(fo_vbt);
		if (*vbt)
			return;
	}
	printwarn("VBT wasn't found at specified offset of %04x\n",
		  oh->vbt_offset);

	for (i = sizeof(optionrom_header_t);
	     i <= fo->size - sizeof(struct vbt_header); i++) {
		struct fileobject *fo_vbt = malloc_fo_sub(fo, i);
		if (!fo_vbt)
			break;

		parse_vbt(fo_vbt, vbt);

		free_fo(fo_vbt);

		if (*vbt)
			return;
	}
}

/* Short VBT summary in human readable form */
static void print_vbt(const struct fileobject *fo)
{
	const struct bdb_header *bdb;

	if (fo->size < sizeof(struct vbt_header))
		return;

	const struct vbt_header *head = (const struct vbt_header *)fo->data;

	print("Found VBT:\n");
	printt("signature: <%20.20s>\n", head->signature);
	printt("version: %d.%02d\n", head->version / 100, head->version % 100);
	if (head->header_size != sizeof(struct vbt_header))
		printt("header size: 0x%x\n", head->header_size);
	printt("VBT size: 0x%x\n", head->vbt_size);
	printt("VBT checksum: 0x%x\n", head->vbt_checksum);

	if (head->bdb_offset > (fo->size - sizeof(struct bdb_header))) {
		printerr("invalid BDB offset\n");
		return;
	}
	bdb = (const struct bdb_header *)
	    ((const char *)head + head->bdb_offset);

	if (memcmp("BIOS_DATA_BLOCK ", bdb->signature, 16) != 0) {
		printerr("invalid BDB signature:%s\n", bdb->signature);
		return;
	}
	printt("BDB version: %u.%02u\n", bdb->version / 100,
	       bdb->version % 100);
}

static void print_usage(const char *argv0, struct option *long_options)
{
	size_t i = 0;
	printf("\nUsage:\n");
	printf("%s --<SOURCECMD> [filename] --<DESTCMD> [filename]\n\n", argv0);
	printf("SOURCECMD set the VBT source. Supported:\n");
	printf(" %-10s: Legacy BIOS area at phys. memory 0xc0000\n",
	       "inlegacy");
	printf(" %-10s: Read raw Intel VBT file\n", "invbt");
	printf(" %-10s: Read VBT from Intel Option ROM file\n\n", "inoprom");
	printf("DESTCMD set the VBT destination. Supported:\n");
	printf(" %-10s: Print VBT in human readable form\n", "outdump");
	printf(" %-10s: Write raw Intel VBT file\n", "outvbt");
	printf(" %-10s: Patch existing Intel Option ROM\n\n", "patchoprom");

	printf("Supported arguments:\n");
	while (long_options[i].name) {
		printf("\t-%c --%s %s\n", long_options[i].val,
		       long_options[i].name, long_options[i].has_arg ?
		       "<path>"  : "");
		i++;
	};
}

/* Fix VBIOS header and PCIR */
static int fix_vbios_header(struct fileobject *fo)
{
	if (!fo || fo->size < sizeof(optionrom_header_t))
		return 1;

	optionrom_header_t *oh = (optionrom_header_t *)fo->data;

	/* Fix size alignment */
	if (fo->size % 512) {
		print("Aligning size to 512\n");
		fo = remalloc_fo(fo, (fo->size + 511) / 512 * 512);
		if (!fo)
			return 1;
		oh = (optionrom_header_t *)fo->data;
	}

	/* Fix size field */
	oh->size = fo->size / 512;

	/* Fix checksum field */
	oh->checksum = -(checksum_vbios(oh) - oh->checksum);

	return 0;
}

/* Return the VBT structure size in bytes */
static size_t vbt_size(const struct fileobject *fo)
{
	if (!fo || fo->size < sizeof(struct vbt_header))
		return 0;
	const struct vbt_header *head = (const struct vbt_header *)fo->data;

	return head->vbt_size;
}

/*
 * Patch an Intel Option ROM with new VBT.
 * Caller has to make sure that VBIOS and VBT are valid.
 * Return 1 on error.
 */
static int patch_vbios(struct fileobject *fo,
		       const struct fileobject *fo_vbt)
{
	optionrom_header_t *oh = (optionrom_header_t *)fo->data;
	struct vbt_header *head;

	struct fileobject *old_vbt = NULL;
	parse_vbios(fo, &old_vbt);

	if (old_vbt) {
		if (oh->vbt_offset + vbt_size(old_vbt) == fo->size) {
			/* Located at the end of file - reduce file size */
			if (fo->size < vbt_size(old_vbt)) {
				free_fo(old_vbt);
				return 1;
			}
			fo = remalloc_fo(fo, fo->size - vbt_size(old_vbt));
			if (!fo) {
				printerr("Failed to allocate memory\n");
				free_fo(old_vbt);
				return 1;
			}
			oh = (optionrom_header_t *)fo->data;
			oh->vbt_offset = 0;
		} else if (vbt_size(old_vbt) < vbt_size(fo_vbt)) {
			/* In the middle of the file - Remove old VBT */
			memset(fo->data + oh->vbt_offset, 0xff,
			       vbt_size(old_vbt));
			oh->vbt_offset = 0;
		} else {
			/* New VBT overwrites existing one - Clear memory */
			memset(fo->data + oh->vbt_offset, 0xff,
			       vbt_size(old_vbt));
		}

		free_fo(old_vbt);
	}

	if (!oh->vbt_offset) {
		print("increasing VBIOS to append VBT\n");
		if ((fo->size + vbt_size(fo_vbt)) >= 2 * 64 * KiB) {
			printerr("VBT won't fit\n");
			return 1;
		}

		oh->vbt_offset = fo->size;
		fo = remalloc_fo(fo, fo->size + vbt_size(fo_vbt));
		if (!fo) {
			printerr("Failed to allocate memory\n");
			return 1;
		}
		oh = (optionrom_header_t *)fo->data;
	}

	head = (struct vbt_header *)((u8 *)oh + oh->vbt_offset);
	memcpy(head, fo_vbt->data, vbt_size(fo_vbt));

	return 0;
}

int main(int argc, char **argv)
{
	int opt, ret, option_index = 0;

	size_t has_input = 0, has_output = 0;
	size_t dump = 0, in_legacy = 0;
	char *in_vbt = NULL, *in_oprom = NULL;
	char *out_vbt = NULL, *patch_oprom = NULL;
	static struct option long_options[] = {
		{"help", 0, 0, 'h'},
		{"outdump", 0, 0, 'd'},
		{"inlegacy", 0, 0, 'l'},
		{"invbt", required_argument, 0, 'f'},
		{"inoprom", required_argument, 0, 'o'},
		{"outvbt", required_argument, 0, 'v'},
		{"patchoprom", required_argument, 0, 'p'},
		{0, 0, 0, 0}
	};

	while ((opt = getopt_long(argc, argv, "hdlf:o:v:p:i",
		   long_options, &option_index)) != EOF) {
		switch (opt) {
		case 'd':
			dump = 1;
			has_output = 1;
			break;
		case 'l':
			in_legacy = 1;
			has_input = 1;
			break;
		case 'f':
			in_vbt = strdup(optarg);
			has_input = 1;
			break;
		case 'o':
			in_oprom = strdup(optarg);
			has_input = 1;
			break;
		case 'v':
			out_vbt = strdup(optarg);
			has_output = 1;
			break;
		case 'p':
			patch_oprom = strdup(optarg);
			has_output = 1;
			break;
		case '?':
		case 'h':
			print_usage(argv[0], long_options);
			exit(0);
			break;
		}
	}

	if (!has_input)
		printerr("No input specified !\n");
	if (!has_output)
		printerr("No output specified !\n");
	if (argc < 2 || argc > 6 || !has_input || !has_output) {
		print_usage(argv[0], long_options);
		return 1;
	}

	struct fileobject *fo;

	if (in_legacy)
		fo = read_physmem(0xc0000);
	else if (in_vbt)
		fo = read_file(in_vbt);
	else
		fo = read_file(in_oprom);

	if (!fo) {
		printerr("Failed to read input file\n");
		return 1;
	}

	struct fileobject *vbt = NULL;
	if (in_legacy || in_oprom) {
		if (!is_valid_vbios(fo)) {
			printerr("Invalid input file\n");

			free_fo(fo);
			return 1;
		}
		parse_vbios(fo, &vbt);
	} else
		parse_vbt(fo, &vbt);

	free_fo(fo);

	if (!vbt) {
		printerr("Failed to find VBT.\n");
		return 1;
	}

	if (!dump)
		print_vbt(vbt);

	ret = 0;
	if (dump) {
		dump_vbt(vbt);
	} else if (out_vbt) {
		if (write_file(out_vbt, vbt)) {
			printerr("Failed to write VBT\n");
			ret = 1;
		} else {
			print("VBT written to %s\n", out_vbt);
		}
	} else if (patch_oprom) {
		fo = read_file(patch_oprom);
		if (!fo) {
			printerr("Failed to read input file\n");
			ret = 1;
		}
		if (ret != 1 && !is_valid_vbios(fo)) {
			printerr("Invalid input file\n");
			ret = 1;
		}
		if (ret != 1 && patch_vbios(fo, vbt)) {
			printerr("Failed to patch VBIOS\n");
			ret = 1;
		}
		if (ret != 1 && fix_vbios_header(fo)) {
			printerr("Failed to fix VBIOS header\n");
			ret = 1;
		}
		if (ret != 1 && write_file(patch_oprom, fo)) {
			printerr("Failed to write VBIOS\n");
			ret = 1;
		}
		free_fo(fo);
		if (ret != 1)
			print("VBIOS %s successfully patched\n", patch_oprom);
	}

	/* cleanup */
	if (patch_oprom)
		free(patch_oprom);
	if (out_vbt)
		free(out_vbt);
	if (in_vbt)
		free(in_vbt);
	if (in_oprom)
		free(in_oprom);

	free_fo(vbt);

	return ret;
}
