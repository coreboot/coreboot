#ifndef PART_KEYBOARD_H
#define KEYBOARD_H

#ifndef NO_KEYBOARD
void keyboard_on(void);
#else
#  define keyboard_on() do {} while(0)
#endif

#endif /* PART_KEYBOARD_H */
