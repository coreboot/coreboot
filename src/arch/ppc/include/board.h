#ifndef _BOARD_H
#define _BOARD_H

/*
 * Provided for all PPC boards to do board-level initialization. This
 * happens prior to entry into hardwaremain().
 */
extern void board_init(void);
extern void board_init2(void);
#endif /* _BOARD_H */
