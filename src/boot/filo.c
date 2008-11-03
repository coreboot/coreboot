/*
 * Copyright (C) 2003 by SONE Takeshi <ts1@tsn.or.jp> and others.
 * This program is licensed under the terms of GNU General Public License.
 *
 * Modified for coreboot by Greg Watson <gwatson@lanl.gov>
 */

#include <console/console.h>
#include <delay.h>
#include <string.h>
#include <boot/tables.h>
#include <boot/elf.h>

#define ENTER '\r'
#define ESCAPE '\x1b'

#ifndef AUTOBOOT_CMDLINE
#define autoboot(mem)
#endif

#if !AUTOBOOT_DELAY
#define autoboot_delay() 0 /* success */
#endif

#define havechar() console_tst_byte()
#define putchar(c) console_tx_byte(c)
#define getchar(c) console_rx_byte(c)

extern char *boot_file;

int getline(char *buf, int max)
{
    int cur, ch, nonspace_seen;

    cur = 0;
    while (buf[cur]) {
	putchar(buf[cur]);
	cur++;
    }
    for (;;) {
	ch = getchar();
	switch (ch) {
	/* end of line */
	case '\r':
	case '\n':
	    putchar('\n');
	    goto out;
	/* backspace */
	case '\b':
	case '\x7f':
	    if (cur > 0) {
		cur--;
		putchar('\b');
		putchar(' ');
		putchar('\b');
	    }
	    break;
	/* word erase */
	case 'W' & 0x1f: /* ^W */
	    nonspace_seen = 0;
	    while (cur) {
		if (buf[cur-1] != ' ')
		    nonspace_seen = 1;
		putchar('\b');
		putchar(' ');
		putchar('\b');
		cur--;
		if (nonspace_seen && cur < max-1 && cur > 0 && buf[cur-1]==' ')
		    break;
	    }
	    break;
	/* line erase */
	case 'U' & 0x1f: /* ^U */
	    while (cur) {
		putchar('\b');
		putchar(' ');
		putchar('\b');
		cur--;
	    }
	    cur = 0;
	    break;
	default:
	    if (ch < 0x20)
		break; /* ignore control char */
	    if (ch >= 0x7f)
		break;
	    if (cur + 1 < max) {
		putchar(ch); /* echo back */
		buf[cur] = ch;
		cur++;
	    }
	}
    }
out:
    if (cur >= max)
	cur = max - 1;
    buf[cur] = '\0';
    return cur;
}

static void boot(struct lb_memory *mem, const char *line)
{
    char *param;

    /* Split filename and parameter */
    boot_file = strdup(line);
    param = strchr(boot_file, ' ');
    if (param) {
	*param = '\0';
	param++;
    }

    if (!elfboot(mem))
	printk_info("Unsupported image format\n");
    free(boot_file);
}

#ifdef AUTOBOOT_CMDLINE
#if AUTOBOOT_DELAY
static inline int autoboot_delay(void)
{
    unsigned int timeout;
    int sec, tmp;
    char key;
    
    key = 0;

    printk_info("Press <Enter> for default boot, or <Esc> for boot prompt... ");
    for (sec = AUTOBOOT_DELAY; sec>0 && key==0; sec--) {
	printk_info("%d", sec);
	timeout = 10;
	while (timeout-- > 0) {
	    if (havechar()) {
		key = getchar();
		if (key==ENTER || key==ESCAPE)
		    break;
	    }
	    mdelay(100);
	}
	for (tmp = sec; tmp; tmp /= 10)
	    printk_info("\b \b");
    }
    if (key == 0) {
	printk_info("timed out\n");
	return 0; /* success */
    } else {
	putchar('\n');
	if (key == ESCAPE)
	    return -1; /* canceled */
	else
	    return 0; /* default accepted */
    }
}
#endif /* AUTOBOOT_DELAY */

static void autoboot(struct lb_memory *mem)
{
    /* If Escape key is pressed already, skip autoboot */
    if (havechar() && getchar()==ESCAPE)
	return;

    if (autoboot_delay()==0) {
	printk_info("boot: %s\n", AUTOBOOT_CMDLINE);
	boot(mem, AUTOBOOT_CMDLINE);
    }
}
#endif /* AUTOBOOT_CMDLINE */

/* The main routine */
int filo(struct lb_memory *mem)
{
    char line[256];

    printk_info("FILO version 0.4.1\n");

    /* Try default image */
    autoboot(mem);

    /* The above didn't work, ask user */
    while (havechar())
	getchar();
#ifdef AUTOBOOT_CMDLINE
    strncpy(line, AUTOBOOT_CMDLINE, sizeof(line)-1);
    line[sizeof(line)-1] = '\0';
#else
    line[0] = '\0';
#endif
    for (;;) {
	printk_info("boot: ");
	getline(line, sizeof line);
	if (line[0])
	    boot(mem, line);
    }
}
