#ifndef MB_SYSCONF_H

#define MB_SYSCONF_H

// Global variables for MB layouts and these will be shared by irqtable mptable and acpi_tables
//busnum is default
/*
unsigned char bus_8131_0 = 1;
unsigned char bus_8131_1 = 2;
unsigned char bus_8131_2 = 3;
unsigned char bus_8111_0 = 1;
unsigned char bus_8111_1 = 4;
unsigned apicid_8111 ;
unsigned apicid_8131_1;
unsigned apicid_8131_2;
unsigned sbdn3;
*/
struct mb_sysconf_t {
	unsigned char bus_8131_0;
	unsigned char bus_8131_1;
	unsigned char bus_8131_2;
	unsigned char bus_8111_0;
	unsigned char bus_8111_1;

        unsigned apicid_8111;
        unsigned apicid_8131_1;
        unsigned apicid_8131_2;

        unsigned sbdn3;
};

#endif

