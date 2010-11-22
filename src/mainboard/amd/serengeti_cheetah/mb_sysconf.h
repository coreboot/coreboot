#ifndef MB_SYSCONF_H

#define MB_SYSCONF_H

struct mb_sysconf_t {
	unsigned char bus_8132_0;
	unsigned char bus_8132_1;
	unsigned char bus_8132_2;
	unsigned char bus_8111_0;
	unsigned char bus_8111_1;

        unsigned char bus_8132a[7][3];

        unsigned char bus_8151[7][2];

        unsigned apicid_8111;
        unsigned apicid_8132_1;
        unsigned apicid_8132_2;
        unsigned apicid_8132a[7][2];

        unsigned sbdn3;
        unsigned sbdn3a[7];
        unsigned sbdn5[7];

};

#endif

