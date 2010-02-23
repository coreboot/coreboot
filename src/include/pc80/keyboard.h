#ifndef PC80_KEYBOARD_H
#define PC80_KEYBOARD_H

struct pc_keyboard {
	/* No initialization parameters for now */
};

void pc_keyboard_init(struct pc_keyboard *keyboard);
void set_kbc_ps2_mode(void);

#endif /* PC80_KEYBOARD_H */
