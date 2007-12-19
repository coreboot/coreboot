/*
 * This file is part of the LinuxBIOS project.
 *
 * Copyright (C) 2007 Advanced Micro Devices, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include <console/console.h>
#include <cpu/x86/msr.h>
#include <cpu/amd/mtrr.h>
#include <device/device.h>
#include <device/pci.h>
#include <string.h>
#include <cpu/x86/msr.h>
#include <cpu/x86/pae.h>
#include <pc80/mc146818rtc.h>
#include <cpu/x86/lapic.h>

#include "../../../northbridge/amd/amdfam10/amdfam10.h"

#include <cpu/amd/model_10xxx_rev.h>
#include <cpu/cpu.h>
#include <cpu/x86/cache.h>
#include <cpu/x86/mtrr.h>
#include <cpu/x86/mem.h>

#include <cpu/amd/quadcore.h>

#include <cpu/amd/model_10xxx_msr.h>
#include <cpu/amd/amdfam10_sysconf.h>

extern device_t get_node_pci(u32 nodeid, u32 fn);

#include "fidvid_common.c"

#define PSTATES_DEBUG 0



static void inline dump_msr_pstates(u32 nodes)
{
#if PSTATES_DEBUG==1
	int i, j;
	for(j=0; j<5; j++) {
		printk_debug("P%d:", j);
		for(i=0;i<nodes;i++)  {
			printk_debug(" [%08x %08x] ", sysconf.msr_pstate[i*5+j].hi, sysconf.msr_pstate[i*5+j].lo);
		}
		printk_debug("\n");
	}
#endif
}


static void inline dump_p(const char *p_c, u32 nodes, u32 *p)
{
#if PSTATES_DEBUG==1
	int i, j;
		printk_debug(p_c);
		printk_debug("p:");
		for(i=0;i<nodes;i++)  {
			printk_debug(" %d ", p[i]);
		}
		printk_debug("\n");

#endif
}


//according the pstate and make it work conformed to mixed conf system
static u32 get_pwrvalue(u32 val)
{
	u32 times;
	switch((val>>8)&3) {
	case 0: times = 1000; break;
	case 1: times = 100;  break;
	case 2: times = 10;   break;
	default:
		//error
		times = 1;
	}

	return (val & 0xff) * times;

}


static u32 get_powerstep(u32 val)
{
	u32 time;
	if(val<4) {time = (4 - val)*100;}
	else if(val<8) { time = (9+4-val)*10;}
	else { time = (10+8-val) * 5; }

	return time;

}


static u32 get_plllocktime(u32 val)
{
	u32 time;
	switch(val) {
	case 0:
	case 1:
	case 2:
	case 3:
		time = val+1; break;
	case 4: time = 8; break;
	case 5: time = 16; break;
	default:
		//erro2
		time = 1;
	}
	return time;
}


static void disable_pstate(u32 nodes, u32 *p)
{
	int i;

	for(i=0;i<nodes; i++) {
		sysconf.msr_pstate[i*5+p[i]].hi &= ~(1<<(63-32));
	}
}


static void match_pstate(u32 nodes, u32 *p)
{
	int i;
	u32 corecof_min, pwrvalue_max, pwrval_max;
	u32 enable;
	enable = (sysconf.msr_pstate[0*5+p[0]].hi >> 31);
	if(!enable)  {
		disable_pstate(nodes, p);
		return;
	}
	corecof_min = ((sysconf.msr_pstate[0*5+p[0]].lo & 0x3f) + 0x10)>>((sysconf.msr_pstate[0*5+p[0]].lo>>6) & 7);
	pwrval_max = sysconf.msr_pstate[0*5+p[0]].hi & 0x3ff;
	pwrvalue_max = get_pwrvalue(pwrval_max);

	for(i=1; i<nodes; i++) {
		enable = (sysconf.msr_pstate[0*5+p[i]].hi >> 31);
		if(!enable) {
			disable_pstate(nodes, p);
			return;
		}

		u32 coredid = ((sysconf.msr_pstate[i*5+p[i]].lo>>6) & 7);
		u32 corecof = ((sysconf.msr_pstate[i*5+p[i]].lo & 0x3f) + 0x10)>>coredid;
		if(corecof<corecof_min) corecof_min = corecof;
		u32 pwrval, pwrvalue;
		pwrval = sysconf.msr_pstate[i*5+p[i]].hi & 0x3ff;
		pwrvalue = get_pwrvalue(pwrval);
		if(pwrvalue>pwrvalue_max) {
			pwrvalue_max = pwrvalue;
			pwrval_max = pwrval;
		}
	}

	for(i=0; i<nodes; i++) {
		u32 coredid = ((sysconf.msr_pstate[i*5+p[i]].lo>>6) & 7);
		u32 corefid = (corecof_min<<coredid);
		while(corefid<0x10) {
			coredid++;
			corefid = (corecof_min<<coredid);
		}
		sysconf.msr_pstate[i*5+p[i]].lo &= ~(0x1ff);
		sysconf.msr_pstate[i*5+p[i]].lo |= (corefid - 0x10) | (coredid << 6);
		sysconf.msr_pstate[i*5+p[i]].hi &= ~(0x3ff);
		sysconf.msr_pstate[i*5+p[i]].hi |= pwrval_max;
	}
}


static void match_pstates(u32 nodes, u32 *p, u32 *px)
{
	int i;
	int j;
	u32 p_int[NODE_NUMS];

	int jj=1;
	u32 end = 0;
	for(i=0;i<nodes; i++) {
		p_int[i] = px[i];
	}
	while(1){
		for(i=0;i<nodes; i++) {
			if(px[i]<=(p[i]+jj)) {
				end = 1;
				break;
			}
		}
		if(!end) {
			for(i=0; i<nodes; i++) {
				p_int[i] = px[i] - jj;
			}
			match_pstate(nodes, p_int);
			dump_p("P int\n", nodes, p_int);
			jj++;
		}
		else {
			for(i=0;i<nodes; i++) {
				for(j=p[i]+1; j<p_int[i]; j++) {
					sysconf.msr_pstate[i*5+j].hi &= ~(1<<(63-32));
				}
			}
			break;
		}
	}
}


void prep_pstates_all(void)
{
	device_t f3_dev[NODE_NUMS], f4_dev[NODE_NUMS];
	u32 p[NODE_NUMS];
	u32 p_htc[NODE_NUMS];
	u32 p_lowest[NODE_NUMS];
	u32 htc_cap = 1;
	u32 lowest_htc_equal = 0;

	u32 nodes = sysconf.nodes;
	int i;
	int j;
	u32 val;
	u32 nbdid;
	u32 nbvid0;
	u32 nbvid1;

	for(i=0;i<nodes; i++) { // get the value from F4x1F0:E0 or we can get that msr in CAR stage...
		f3_dev[i] = get_node_pci(i, 3);
		f4_dev[i] = get_node_pci(i, 4);
	}

	for(i=0;i<nodes; i++) { // get the value from F4x1F0:E0 or we can get that msr in CAR stage...
		val = pci_read_config32(f4_dev[i], 0x1f4);
		nbvid0 = val & 0x3f;
		nbvid1 = (val>>7) & 0x3f;
		for(j=0; j<5; j++) {
			val = pci_read_config32(f4_dev[i], 0x1e0 + (j<<2));
			nbdid = ((val>>16) & 1);
			sysconf.msr_pstate[i*5+j].lo = (val & 0xffff)  | (nbdid<<22) | ((nbdid?nbvid1:nbvid0)<<25);
			sysconf.msr_pstate[i*5+j].hi = (((val>>17) & 0x3ff) << (32-32)) | (((val>>27) & 1)<<(63-32));
		}
	}

	dump_msr_pstates(nodes);

	sysconf.needs_update_pstate_msrs = 0; // normal case for all sockets are installed same conf CPU

	for(i=1; (i<nodes) && (!sysconf.needs_update_pstate_msrs); i++) {
		for(j=0; j<5; j++) {
			if((sysconf.msr_pstate[i*5+j].lo != sysconf.msr_pstate[0*5+j].lo) || (sysconf.msr_pstate[i*5+j].hi != sysconf.msr_pstate[0*5+j].hi)) {
				sysconf.needs_update_pstate_msrs = 1;
				break;
			}
		}
	}

	if(sysconf.needs_update_pstate_msrs) {

		// update msr_pstate for mixed conf

		//P0
		/* Match P0 cpu cof for all cpu cores to the lowest P0 cpu cof value in the coherent fabric, and match P0 power for all cpu cores to the highest P0 power value */
		for(i=0;i<nodes; i++) p[i] = 0;
		match_pstate(nodes, p);
		dump_p("P0\n", nodes, p);
		 dump_msr_pstates(nodes);


		//p_htc
		for(i=0;i<nodes; i++) {
			val = pci_read_config32(f3_dev[i], 0xe8); //htc cap
			if(!(val & (1<<10))) {
				htc_cap = 0;
				break;
			}

			//HtcPstateLimit
			val = pci_read_config32(f3_dev[i], 0x64);
			p_htc[i] = (((val>>28) & 7));
			if(p_htc[i] == 0) {
				val |= 1<<28;
				pci_write_config32(f3_dev[i], 0x64, val);
				val = pci_read_config32(f3_dev[i], 0x68); //stc
				val &= ~(7<<28);
				val |= (1<<28);
				pci_write_config32(f3_dev[i], 0x68, val);

				p_htc[i] = 1;
			}
		}
		if(htc_cap) {
			match_pstate(nodes, p_htc);

			dump_p("P_htc\n", nodes, p_htc);
			dump_msr_pstates(nodes);
		}

		//p_lowest
		for(i=0;i<nodes; i++) {
			p_lowest[i] = 0;
			for(j=1; j<5; j++) {
				if(sysconf.msr_pstate[i*5+j].hi & (1<<(63-32)))	{
					p_lowest[i] = j;
				}
			}
			// PstateMaxVal
			val = pci_read_config32(f3_dev[i], 0xdc);
			if(p_lowest[i]>((val>>8) & 7)) {
				val &= ~(7<<8);
				val |= (p_lowest[i])<<8;
				pci_write_config32(f3_dev[i], 0xdc, val);
			}
			else {
				p_lowest[i] = (val>>8) & 7;
			}
		}
		if(htc_cap) {
			for(i=0;i<nodes; i++) {
				if(p_lowest[i]==p_htc[i]){
					lowest_htc_equal = 1;
					break;
				}
			}
		}
		if(lowest_htc_equal) {
			for(i=0;i<nodes; i++) {
				// PstateMaxVal
				val = pci_read_config32(f3_dev[i], 0xdc);
				val &= ~(7<<8);
				val |= p_htc[i];
				pci_write_config32(f3_dev[i], 0xdc, val);
				for(j=p_htc[i]+1; j<5; j++) {
					sysconf.msr_pstate[i*5+j].hi &= ~(1<<(63-32));
				}
			}
		} else {
			match_pstate(nodes, p_lowest);
			for(i=0; i<nodes; i++) {
				for(j=p_lowest[i]+1; j<5; j++) {
				      sysconf.msr_pstate[i*5+j].hi &= ~(1<<(63-32));
				}
			}

		}

		dump_p("Px\n", nodes, p_lowest);
		dump_msr_pstates(nodes);


		if(htc_cap) {
			//p_up_int
			match_pstates(nodes, p, p_htc);

			dump_msr_pstates(nodes);

			//p_lower_int
			match_pstates(nodes, p_htc, p_lowest);
		} else {
			match_pstates(nodes, p, p_lowest);
		}

		dump_msr_pstates(nodes);

	}

	// fill data into p_state
	for(i=0; i<nodes; i++) {
		sysconf.p_state_num = 0;
		u32 corefid_equal = 1;
		u32 corefid;
		corefid = (sysconf.msr_pstate[i*5+0].lo & 0x3f);
		for(j=1; j<5; j++) {
			msr_t *msr_pstate;
			msr_pstate = &(sysconf.msr_pstate[i*5+j]);
			if(!(msr_pstate->hi & (1<<(63-32)) )) continue;
			if((msr_pstate->lo & 0x3f) != corefid) {
				corefid_equal = 0;
				break;
			}
		}
		for(j=0; j<5; j++) {
			struct p_state_t *p_state;
			msr_t *msr_pstate;
			msr_pstate = &sysconf.msr_pstate[i*5+j];
			if(!(msr_pstate->hi & (1<<(63-32)) )) continue;
			p_state = &sysconf.p_state[i*5+sysconf.p_state_num];
			u32 coredid = ((msr_pstate->lo>>6) & 7);
			u32 corecof = ((msr_pstate->lo & 0x3f) + 0x10)>>coredid;
			p_state->corefreq = corecof;

			u32 pwrval, pwrvalue;
			pwrval = msr_pstate->hi & 0x3ff;
			pwrvalue = get_pwrvalue(pwrval);
			p_state->power = pwrvalue;

			u32 lat;
			val = pci_read_config32(f3_dev[i], 0xd4);
			lat = 15 * (get_powerstep((val>>24)& 0xf)+get_powerstep((val>>20)& 0xf)) /1000;
			if(!corefid_equal) {
				val = pci_read_config32(f3_dev[i], 0xa0);
				lat += get_plllocktime((val >> 11 ) & 7);
			}
			p_state->transition_lat = lat;
			p_state->busmaster_lat = lat;

			p_state->control = j;
			p_state->status = j;

			sysconf.p_state_num++;
		}
		// don't need look at other nodes
		if(!sysconf.p_state_num) break;
	}
}


//it will update pstates info from ram into MSR
void init_pstates(device_t dev, u32 nodeid, u32 coreid)
{
	int j;
	msr_t msr;

	if(sysconf.needs_update_pstate_msrs) {
		for(j=0; j < 5; j++) {
			wrmsr(0xC0010064 + j, sysconf.msr_pstate[nodeid * 5 + j]);
		}
	}

	/* Set TSC Freq Select: TSC increments at the rate of the core P-state 0 */
	msr = rdmsr(0xC0010015);
	msr.lo |= 1 << 24;
	wrmsr(0xC0010015, msr);

	// Enter the state P0
	//FIXME I don't think that this works correctly. May depend on early fid/vid setup.
	if(sysconf.p_state_num)
		set_core_nb_max_pstate_after_other_warm_reset(nodeid, coreid);

}
