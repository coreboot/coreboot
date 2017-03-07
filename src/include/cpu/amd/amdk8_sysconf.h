#ifndef AMDK8_SYSCONF_H
#define AMDK8_SYSCONF_H

#define HC_POSSIBLE_NUM 8

struct amdk8_sysconf_t {
	//ht
	unsigned int nodes;
	unsigned int hc_possible_num;
	unsigned int pci1234[HC_POSSIBLE_NUM];
	unsigned int hcdn[HC_POSSIBLE_NUM];
	unsigned int hcid[HC_POSSIBLE_NUM]; //record ht chain type
	unsigned int sbdn;
	unsigned int sblk;

	unsigned int hcdn_reg[4]; // it will be used by get_sblk_pci1234

	int enabled_apic_ext_id;
	unsigned int lift_bsp_apicid;
	int apicid_offset;

	void *mb; // pointer for mb related struct

};

extern struct amdk8_sysconf_t sysconf;

void get_sblk_pci1234(void);
void get_bus_conf(void);
#endif
