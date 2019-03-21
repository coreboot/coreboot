Roda RK9 Flash Header
=====================

There is a 5x2 pin, 1.27mm pitch header *J1* south of the BIOS flash. It
follows the pinout of the Dediprog adaptor board:

    +------+
    | 1  2 |    1: HOLD 2   2: CS 2
    | 3  4 |    3: CS 1     4: VCC
    | 5  6 |    5: MISO     6: HOLD 1
    | 7  8 |    7:          8: CLK
    | 9 10 |    9: GND     10: MOSI
    +------+

Pins 3 to 10 directly map to the regular SPI flash pinout.

There is also a *JP17* around. Ideally, it should be closed during
programming (isolates the SPI bus from the southbridge):

    +---+
    | 1 |   1: SF100-I/O3
    | 2 |   2: GND
    +---+
