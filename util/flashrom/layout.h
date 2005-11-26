#ifndef __LAYOUT_H__
#define __LAYOUT_H__ 1

int show_id(uint8_t *bios, int size);
int read_romlayout(char *name);
int find_romentry(char *name);
int handle_romentries(uint8_t *buffer, uint8_t *content);


#endif				/* !__LAYOUT_H__ */
