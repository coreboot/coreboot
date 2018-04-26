/* Public Domain Curses */

#include "lppdc.h"
#include <usb/usb.h>

unsigned long pdc_key_modifiers = 0L;

#if IS_ENABLED(CONFIG_LP_SERIAL_CONSOLE)
/* We treat serial like a vt100 terminal.  For now we
   do the cooking in here, but we should probably eventually
   pass it to dedicated vt100 code */

static int getkeyseq(char *buffer, int len, int max)
{
	int i;

	while (1) {
		for(i = 0; i < 75; i++) {
			if (serial_havechar())
				break;
			mdelay(1);
		}

		if (i == 75)
			return len;

		buffer[len++] = serial_getchar();
		if (len == max)
			return len;
	}
}

static struct {
	const char *seq;
	int key;
} escape_codes[] = {
	{ "[A", KEY_UP },
	{ "[B", KEY_DOWN },
	{ "[C", KEY_RIGHT },
	{ "[D", KEY_LEFT },
	{ "[F", KEY_END },
	{ "[H", KEY_HOME },
	{ "[2~", KEY_IC },
	{ "[3~", KEY_DC },
	{ "[5~", KEY_PPAGE },
	{ "[6~", KEY_NPAGE },
	{ "OP", KEY_F(1) },
	{ "OQ", KEY_F(2) },
	{ "OR", KEY_F(3) },
	{ "OS", KEY_F(4) },
	{ "[15~", KEY_F(5) },
	{ "[17~", KEY_F(6) },
	{ "[18~", KEY_F(7) },
	{ "[19~", KEY_F(8) },
	{ "[20~", KEY_F(9) },
	{ "[21~", KEY_F(10) },
	{ "[23~", KEY_F(11) },
	{ "[24~", KEY_F(12) },
	{ NULL },
};

static int handle_escape(void)
{
	char buffer[5];
	int len = getkeyseq(buffer, 0, sizeof(buffer));
	int i, t;

	if (len == 0)
		return 27;

	for(i = 0; escape_codes[i].seq != NULL; i++) {
		const char *p = escape_codes[i].seq;

		for(t = 0; t < len; t++) {
			if (!*p || *p != buffer[t])
				break;
			p++;
		}

		if (t == len)
			return escape_codes[i].key;
	}

	return 0;
}

static int cook_serial(unsigned char ch)
{
	switch(ch) {
	case 8:
		return KEY_BACKSPACE;

	case 27:
		return handle_escape();

	default:
		return ch;
	}
}
#endif

void PDC_set_keyboard_binary(bool on)
{
    PDC_LOG(("PDC_set_keyboard_binary() - called\n"));
}

/* check if a key event is waiting */

bool PDC_check_key(void)
{
#if IS_ENABLED(CONFIG_LP_USB_HID)
    usb_poll();
    if ((curses_flags & F_ENABLE_CONSOLE) &&
        usbhid_havechar()) {
        return TRUE;
    }
#endif

#if IS_ENABLED(CONFIG_LP_PC_KEYBOARD)
    if ((curses_flags & F_ENABLE_CONSOLE) &&
        keyboard_havechar()) {
        return TRUE;
    }
#endif

#if IS_ENABLED(CONFIG_LP_SERIAL_CONSOLE)
    if ((curses_flags & F_ENABLE_SERIAL) &&
        serial_havechar()) {
        return TRUE;
    }
#endif

    return FALSE;
}

/* return the next available key event */

int PDC_get_key(void)
{
    int c = 0;

#if IS_ENABLED(CONFIG_LP_USB_HID)
    usb_poll();
    if ((curses_flags & F_ENABLE_CONSOLE) &&
        usbhid_havechar()) {
        c = usbhid_getchar();
    }
#endif

#if IS_ENABLED(CONFIG_LP_PC_KEYBOARD)
    if ((curses_flags & F_ENABLE_CONSOLE) &&
        keyboard_havechar() && (c == 0)) {
        c = keyboard_getchar();
    }
#endif

#if IS_ENABLED(CONFIG_LP_SERIAL_CONSOLE)
    if ((curses_flags & F_ENABLE_SERIAL) &&
        serial_havechar() && (c == 0)) {
        c = cook_serial(serial_getchar());
    }
#endif

    SP->key_code = FALSE;

    if (c == 0) {
        c = ERR;
    }
    if (c >= KEY_MIN) {
        SP->key_code = TRUE;
    }

    return c;
}

/* discard any pending keyboard input -- this is the core
   routine for flushinp() */

void PDC_flushinp(void)
{
    PDC_LOG(("PDC_flushinp() - called\n"));

    while (PDC_check_key()) PDC_get_key();
}

int PDC_mouse_set(void)
{
    return ERR;
}

int PDC_modifiers_set(void)
{
    return OK;
}
