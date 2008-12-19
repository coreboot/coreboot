#ifndef PC80_KEYBOARD_H
#define PC80_KEYBOARD_H

struct pc_keyboard {
	/* No initialization parameters for now */
};

void init_pc_keyboard(unsigned port0, unsigned port1, struct pc_keyboard *kbd);
void set_kbc_ps2_mode(void);

#endif /* PC80_KEYBOARD_H */
