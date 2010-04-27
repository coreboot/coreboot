
/*
 ** NMI A20M IGNNE INTR
 * X8  H H H H
 * X9  H H H L projected
 * X10 H H L H
 * X11 H H L L
 * X12 H L H H
 * X13 H L H L
 * X14 H L L H
 * X15 H L L L
 * X16 L H H H
 * X17 L H H L
 * X18 L H L H
 * X19 L H L L
 * X20 L L H H
 * X21 L L H L  projected
 * X22 L L L H  projected
 * X23 L L L L  projected
 *
 ** NMI INTR IGNNE A20M
 * X8  H H H H
 * X9  H L H H projected
 * X10 H H L H
 * X11 H L L H
 * X12 H H H L
 * X13 H L H L
 * X14 H H L L
 * X15 H L L L
 * X16 L H H H
 * X17 L L H H
 * X18 L H L H
 * X19 L L L H
 * X20 L H H L
 * X21 L L H L projected
 * X22 L H L L projected
 * X23 L L L L projected
 */

#define XEON_X8	 0xf
#define XEON_X9  0xb  /* projected */
#define XEON_X10 0xd
#define XEON_X11 0x9
#define XEON_X12 0xe
#define XEON_X13 0xa
#define XEON_X14 0xc
#define XEON_X15 0x8
#define XEON_X16 0x7
#define XEON_X17 0x3
#define XEON_X18 0x5
#define XEON_X19 0x1
#define XEON_X20 0x6
#define XEON_X21 0x2  /* projected */
#define XEON_X22 0x4  /* projected */
#define XEON_X23 0x0  /* projected */
