/* SPDX-License-Identifier: GPL-2.0-only */

#include "coreinfo.h"

#define KEY_ESC 27

extern struct coreinfo_module cpuinfo_module;
extern struct coreinfo_module pci_module;
extern struct coreinfo_module coreboot_module;
extern struct coreinfo_module multiboot_module;
extern struct coreinfo_module nvram_module;
extern struct coreinfo_module bootlog_module;
extern struct coreinfo_module ramdump_module;
extern struct coreinfo_module cbfs_module;
extern struct coreinfo_module timestamps_module;

struct coreinfo_module *system_modules[] = {
#if CONFIG(MODULE_CPUINFO)
	&cpuinfo_module,
#endif
#if CONFIG(MODULE_PCI)
	&pci_module,
#endif
#if CONFIG(MODULE_NVRAM)
	&nvram_module,
#endif
#if CONFIG(MODULE_RAMDUMP)
	&ramdump_module,
#endif
};

struct coreinfo_module *firmware_modules[] = {
#if CONFIG(MODULE_COREBOOT)
	&coreboot_module,
#endif
#if CONFIG(MODULE_MULTIBOOT)
	&multiboot_module,
#endif
#if CONFIG(MODULE_BOOTLOG)
	&bootlog_module,
#endif
#if CONFIG(MODULE_CBFS)
	&cbfs_module,
#endif
#if CONFIG(MODULE_TIMESTAMPS)
	&timestamps_module,
#endif
};

struct coreinfo_cat {
	char name[15];
	int cur;
	int count;
	struct coreinfo_module **modules;
} categories[] = {
	{
		.name = "System",
		.modules = system_modules,
		.count = ARRAY_SIZE(system_modules),
	},
	{
		.name = "Firmware",
		.modules = firmware_modules,
		.count = ARRAY_SIZE(firmware_modules),
	}
};

static WINDOW *modwin, *menuwin;
static int curwin;

void print_module_title(WINDOW *win, const char *title)
{
	int i;

	wattrset(win, COLOR_PAIR(2));
	mvwprintw(win, 0, 1, title);

	wmove(win, 1, 1);
	for (i = 0; i < 78; i++)
		waddch(win, ACS_HLINE);
}

static void print_submenu(struct coreinfo_cat *cat)
{
	int i, j;
	char menu[80];
	char *ptr = menu;

	wmove(menuwin, 0, 0);

	for (j = 0; j < SCREEN_X; j++)
		waddch(menuwin, ' ');

	if (!cat->count)
		return;

	for (i = 0; i < cat->count; i++)
		ptr += sprintf(ptr, "[%c: %s] ", 'A' + i,
			       cat->modules[i]->name);

	mvwprintw(menuwin, 0, 0, menu);
}

#if CONFIG(SHOW_DATE_TIME)
static void print_time_and_date(void)
{
	struct tm tm;

	while (nvram_updating())
		mdelay(10);

	rtc_read_clock(&tm);

	mvwprintw(menuwin, 1, 57, "%02d/%02d/%04d - %02d:%02d:%02d",
		  tm.tm_mon + 1, tm.tm_mday, 1900 + tm.tm_year, tm.tm_hour,
		  tm.tm_min, tm.tm_sec);
}
#endif

static void print_menu(void)
{
	int j;
	char menu[80];
	char *ptr = menu;

	wmove(menuwin, 1, 0);
	for (j = 0; j < SCREEN_X; j++)
		waddch(menuwin, ' ');

	for (size_t i = 0; i < ARRAY_SIZE(categories); i++) {
		if (categories[i].count == 0)
			continue;

		ptr += sprintf(ptr, "F%zu: %s ", i + 1, categories[i].name);
	}

	mvwprintw(menuwin, 1, 0, menu);

#if CONFIG(SHOW_DATE_TIME)
	print_time_and_date();
#endif
}

static void center(int row, const char *str)
{
	int j, len = strlen(str);

	wmove(stdscr, row, 0);
	for (j = 0; j < SCREEN_X; j++)
		waddch(stdscr, ' ');

	mvprintw(row, (SCREEN_X - len) / 2, str);
}

/* FIXME: Currently unused. */
#if 0
static void header(int row, const char *str)
{
	char buf[SCREEN_X];
	char *ptr = buf;
	int i;
	int len = strlen(str) + 4;

	for (i = 0; i < (SCREEN_X - len) / 2; i++)
		ptr += sprintf(ptr, "=");

	ptr += sprintf(ptr, "[ %s ]", str);

	for (i = ((SCREEN_X - len) / 2) + len; i < SCREEN_X; i++)
		ptr += sprintf(ptr, "=");

	mvprintw(row, 0, buf);
}
#endif

static void redraw_module(struct coreinfo_cat *cat)
{
	if (cat->count == 0)
		return;

	wclear(modwin);
	cat->modules[cat->cur]->redraw(modwin);
	wrefresh(modwin);
}

static void handle_category_key(struct coreinfo_cat *cat, int key)
{
	if ((key >= 'a' && key <= 'z') || (key >= 'A' && key <= 'Z')) {
		int index;
		if (key >= 'A' && key <= 'Z') {
			index = key - 'A';
		} else {
			index = key - 'a';
		}
		if (index < cat->count) {
			cat->cur = index;
			redraw_module(cat);
			return;
		}
	}

	if (cat->count && cat->modules[cat->cur]->handle) {
		if (cat->modules[cat->cur]->handle(key))
			redraw_module(cat);
	}
}

static void print_no_modules_selected(void)
{
	int height = getmaxy(stdscr);

	for (size_t i = 0; i < ARRAY_SIZE(categories); i++)
		if (categories[i].count > 0)
			return;

	color_set(2, NULL); // White on black
	center(height / 2, "No modules selected");
}

static int first_nonempty_category(void)
{
	for (size_t i = 0; i < ARRAY_SIZE(categories); i++)
		if (categories[i].count > 0)
			return i;
	return 0;
}

static void loop(void)
{
	int key;

	center(0, CONFIG_PAYLOAD_INFO_NAME " " CONFIG_PAYLOAD_INFO_VERSION);
	print_no_modules_selected();
	refresh();

	curwin = first_nonempty_category();
	print_menu();
	print_submenu(&categories[curwin]);
	redraw_module(&categories[curwin]);

	halfdelay(10);

	while (1) {
		int ch = -1;

#if CONFIG(SHOW_DATE_TIME)
		print_time_and_date();
		wrefresh(menuwin);
#endif

		key = getch();

		if (key == ERR)
			continue;

		if (key >= KEY_F(1) && key <= KEY_F(9))
			ch = key - KEY_F(1);
		if (key >= '1' && key <= '9')
			ch = key - '1';

		if (ch >= 0 && (unsigned int)ch <= ARRAY_SIZE(categories)) {
			if (ch == ARRAY_SIZE(categories))
				continue;
			if (categories[ch].count == 0)
				continue;

			curwin = ch;
			print_submenu(&categories[curwin]);
			redraw_module(&categories[curwin]);
			continue;
		}

		if (key == KEY_ESC)
			return;

		handle_category_key(&categories[curwin], key);
	}
}

int main(int argc, char **argv)
{
	(void)argc;
	(void)argv;

	int j;

	if (CONFIG(LP_USB))
		usb_initialize();

	initscr();

	start_color();
	init_pair(1, COLOR_WHITE, COLOR_GREEN);
	init_pair(2, COLOR_WHITE, COLOR_BLACK);
	init_pair(3, COLOR_BLACK, COLOR_WHITE);

	modwin = newwin(SCREEN_Y - 3, SCREEN_X, 1, 0);
	menuwin = newwin(2, SCREEN_X, SCREEN_Y - 2, 0);

	wattrset(stdscr, COLOR_PAIR(1) | A_BOLD);
	wattrset(modwin, COLOR_PAIR(2));
	wattrset(menuwin, COLOR_PAIR(1) | A_BOLD);

	werase(modwin);

	for (size_t i = 0; i < ARRAY_SIZE(categories); i++) {
		for (j = 0; j < categories[i].count; j++)
			categories[i].modules[j]->init();
	}

	noecho(); /* don't let curses echo keyboard chars */
	keypad(stdscr, TRUE); /* allow KEY_F(n) keys to be seen */
	curs_set(0); /* Hide blinking cursor */

	loop();

	/* reboot */
	outb(0x6, 0xcf9);
	halt();
	return 0;
}

PAYLOAD_INFO(name, CONFIG_PAYLOAD_INFO_NAME);
PAYLOAD_INFO(listname, CONFIG_PAYLOAD_INFO_LISTNAME);
PAYLOAD_INFO(desc, CONFIG_PAYLOAD_INFO_DESC);
