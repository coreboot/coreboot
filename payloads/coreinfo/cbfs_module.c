/* SPDX-License-Identifier: GPL-2.0-only */

#include "coreinfo.h"
#include "endian.h"

#if CONFIG(MODULE_CBFS)

#define FILES_VISIBLE		19

#define HEADER_MAGIC		0x4F524243
#define HEADER_ADDR		0xfffffffc
#define LARCHIVE_MAGIC		0x455649484352414cLL	/* "LARCHIVE" */

#define COMPONENT_DELETED	0x00
#define COMPONENT_BOOTBLOCK	0x01
#define COMPONENT_CBFSHEADER	0x02
#define COMPONENT_STAGE		0x10
#define COMPONENT_SELF		0x20
#define COMPONENT_FIT		0x21
#define COMPONENT_OPTIONROM	0x30
#define COMPONENT_RAW		0x50
#define COMPONENT_MICROCODE	0x53
#define COMPONENT_CMOS_LAYOUT	0x1aa
#define COMPONENT_NULL		0xffffffff

struct cbheader {
	u32 magic;
	u32 version;
	u32 romsize;
	u32 bootblocksize;
	u32 align;
	u32 offset;
	u32 architecture;
	u32 pad[1];
} __packed;

struct cbfile {
	u64 magic;
	u32 len;
	u32 type;
	u32 checksum;
	u32 offset;
	char filename[0];
} __packed;

static int filecount = 0, selected = 0, start_row = 0;
static char **filenames;
static struct cbheader *header = NULL;

static struct cbfile *getfile(struct cbfile *f)
{
	while (1) {
		if (f < (struct cbfile *)(0xffffffff - ntohl(header->romsize)))
			return NULL;
		if (f->magic == 0)
			return NULL;
		if (f->magic == LARCHIVE_MAGIC)
			return f;
		f = (struct cbfile *)((u8 *)f + ntohl(header->align));
	}
}

static struct cbfile *firstfile(void)
{
	return getfile((void *)(0 - ntohl(header->romsize) +
				ntohl(header->offset)));
}

static struct cbfile *nextfile(struct cbfile *f)
{
	f = (struct cbfile *)((u8 *)f + ALIGN_UP(ntohl(f->len) + ntohl(f->offset),
			      ntohl(header->align)));
	return getfile(f);
}

static struct cbfile *findfile(const char *filename)
{
	struct cbfile *f;
	for (f = firstfile(); f; f = nextfile(f)) {
		if (strcmp(filename, f->filename) == 0)
			return f;
	}
	return NULL;
}

static int cbfs_module_init(void)
{
	struct cbfile *f;
	int index = 0;

	header = *(void **)HEADER_ADDR;
	if (header->magic != ntohl(HEADER_MAGIC)) {
		header = NULL;
		return 0;
	}

	for (f = firstfile(); f; f = nextfile(f))
		filecount++;

	filenames = malloc(filecount * sizeof(char *));
	if (filenames == NULL)
		return 0;

	for (f = firstfile(); f; f = nextfile(f))
		filenames[index++] = strdup((const char *)f->filename);

	return 0;
}

static int cbfs_module_redraw(WINDOW * win)
{
	struct cbfile *f;
	int i, row, frow;

	print_module_title(win, "CBFS Listing");

	if (!header) {
		mvwprintw(win, 11, 61 / 2, "Bad or missing CBFS header");
		return 0;
	}

	/* Draw a line down the middle. */
	for (i = 2; i < 21; i++)
		mvwaddch(win, i, 30, ACS_VLINE);

	/* Draw the names down the left side. */
	for (frow = 0; frow < FILES_VISIBLE; frow++) {
		row = 2 + frow;
		i = start_row + frow;
		if (i >= filecount)
			break;
		if (i == selected)
			wattrset(win, COLOR_PAIR(3) | A_BOLD);
		else
			wattrset(win, COLOR_PAIR(2));

		if (strlen(filenames[i]) == 0) {
			if (findfile(filenames[i])->type == COMPONENT_NULL)
				mvwprintw(win, row, 1, "<free space>");
			else
				mvwprintw(win, row, 1, "<unnamed>");
		} else {
			mvwprintw(win, row, 1, "%.25s", filenames[i]);
		}
		/* show scroll arrows */
		if (frow == 0 && start_row > 0) {
			wattrset(win, COLOR_PAIR(2));
			mvwaddch(win, row, 28, ACS_UARROW);
		}
		if (frow == FILES_VISIBLE - 1 && i != filecount - 1) {
			wattrset(win, COLOR_PAIR(2));
			mvwaddch(win, row, 28, ACS_DARROW);
		}
	}

	f = findfile(filenames[selected]);
	if (!f) {
		mvwprintw(win, 11, 32, "ERROR: CBFS component not found");
		return 0;
	}

	wattrset(win, COLOR_PAIR(2));

	/* Draw the file information */
	row = 2;
	/* mvwprintw(win, row++, 32, "Offset: 0x%x", f->offset); *//* FIXME */
	mvwprintw(win, row, 32, "Type: ");
	switch (ntohl(f->type)) {
	case COMPONENT_BOOTBLOCK:
		mvwprintw(win, row++, 38, "bootblock");
		break;
	case COMPONENT_CBFSHEADER:
		mvwprintw(win, row++, 38, "CBFS header");
		break;
	case COMPONENT_STAGE:
		mvwprintw(win, row++, 38, "stage");
		break;
	case COMPONENT_SELF:
		mvwprintw(win, row++, 38, "simple ELF");
		break;
	case COMPONENT_FIT:
		mvwprintw(win, row++, 38, "FIT");
		break;
	case COMPONENT_OPTIONROM:
		mvwprintw(win, row++, 38, "optionrom");
		break;
	case COMPONENT_RAW:
		mvwprintw(win, row++, 38, "raw");
		break;
	case COMPONENT_MICROCODE:
		mvwprintw(win, row++, 38, "microcode");
		break;
	case COMPONENT_CMOS_LAYOUT:
		mvwprintw(win, row++, 38, "cmos layout");
		break;
	case COMPONENT_NULL:
		mvwprintw(win, row++, 38, "free");
		break;
	case COMPONENT_DELETED:
		mvwprintw(win, row++, 38, "deleted");
		break;
	default:
		mvwprintw(win, row++, 38, "Unknown (0x%x)", ntohl(f->type));
		break;
	}
	mvwprintw(win, row++, 32, "Size: %d", ntohl(f->len));
	mvwprintw(win, row++, 32, "Checksum: 0x%x", ntohl(f->checksum));

	return 0;
}

static int cbfs_module_handle(int key)
{
	int ret = 0;

	if (filecount == 0)
		return 0;

	switch (key) {
	case KEY_DOWN:
		if (selected + 1 < filecount) {
			selected++;
			if (selected >= start_row + FILES_VISIBLE - 1)
				start_row = selected - (FILES_VISIBLE - 1);
			ret = 1;
		}
		break;
	case KEY_UP:
		if (selected > 0) {
			selected--;
			if (selected < start_row)
				start_row = selected;
			ret = 1;
		}
		break;
	}

	return ret;
}

struct coreinfo_module cbfs_module = {
	.name = "CBFS",
	.init = cbfs_module_init,
	.redraw = cbfs_module_redraw,
	.handle = cbfs_module_handle
};

#else

struct coreinfo_module cbfs_module = {
};

#endif
