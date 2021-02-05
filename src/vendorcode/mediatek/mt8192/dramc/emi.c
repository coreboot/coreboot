/* SPDX-License-Identifier: BSD-3-Clause */

#include <assert.h>
#include <emi_hw.h>
#include <emi.h>
#include "dramc_reg_base_addr.h"
#include <dramc_top.h>
#include <soc/emi.h>

#ifdef LAST_EMI
static LAST_EMI_INFO_T* last_emi_info_ptr;
#endif

static inline unsigned int mt_emi_sync_read(unsigned long long addr)
{
	dsb();
	return *((volatile unsigned int *)addr);
}

#define mt_emi_sync_write(addr, value)				\
	do {							\
		*((volatile unsigned int *)(addr)) = value;	\
		dsb();						\
	} while (0)

#define mt_emi_sync_write_or(addr, or_value)			\
	do {							\
		mt_emi_sync_write(addr,				\
			mt_emi_sync_read(addr) | or_value);	\
	} while (0)

static void emi_cen_config(void)
{
#ifndef ONE_CH
  #ifdef RANK_512MB  // => 2channel , dual rank , total=2G
	mt_emi_sync_write(EMI_APB_BASE+0x00000000,0xa053a154);
  #else  //RANK_1G	=> 2channel , dual rank , total=4G
	mt_emi_sync_write(EMI_APB_BASE+0x00000000,0xf053f154);
  #endif
#else
  #ifdef RANK_512MB
	mt_emi_sync_write(EMI_APB_BASE+0x00000000,0xa053a054);
  #else
	mt_emi_sync_write(EMI_APB_BASE+0x00000000,0xf053f054);
  #endif
#endif

  // overhead: 20190821 item1 - synced
  mt_emi_sync_write(EMI_APB_BASE+0x00000004,0x182e2d33); //3733 (1:8) r4  - 	r1	overhead  // TBD - change to 4266
  mt_emi_sync_write(EMI_APB_BASE+0x00000008,0x0f251025); //3733 (1:8) r8  - 	r5	overhead  // TBD - change to 4266
  mt_emi_sync_write(EMI_APB_BASE+0x0000000c,0x122a1027); //3733 (1:8) r12 - 	r9	overhead  // TBD - change to 4266
  mt_emi_sync_write(EMI_APB_BASE+0x00000010,0x1a31162d); //3733 (1:8) r16 - 	r13 overhead  // TBD - change to 4266
  mt_emi_sync_write(EMI_APB_BASE+0x000008b0,0x182e2d33); //3200 (1:8) r4  - 	r1	overhead
  mt_emi_sync_write(EMI_APB_BASE+0x000008b4,0x0f251025); //3200 (1:8) r8  - 	r5	overhead
  mt_emi_sync_write(EMI_APB_BASE+0x0000001c,0x122a1027); //3200 (1:8) r12 - 	r9	overhead
  mt_emi_sync_write(EMI_APB_BASE+0x00000024,0x1a31162d); //3200 (1:8) r16 - 	r13 overhead
  mt_emi_sync_write(EMI_APB_BASE+0x00000034,0x1024202c); //2400 (1:8) r4  - 	r1	overhead
  mt_emi_sync_write(EMI_APB_BASE+0x0000006c,0x0b210c21); //2400 (1:8) r8  - 	r5	overhead
  mt_emi_sync_write(EMI_APB_BASE+0x0000013c,0x0f250d23); //2400 (1:8) r12 - 	r9	overhead
  mt_emi_sync_write(EMI_APB_BASE+0x00000640,0x152b1228); //2400 (1:8) r16 - 	r13 overhead
  mt_emi_sync_write(EMI_APB_BASE+0x00000044,0x0c201a28); //1866 (1:8) r4  - 	r1	overhead
  mt_emi_sync_write(EMI_APB_BASE+0x00000074,0x0d230a20); //1866 (1:8) r8  - 	r5	overhead
  mt_emi_sync_write(EMI_APB_BASE+0x000001e0,0x0e260d24); //1866 (1:8) r12 - 	r9	overhead
  mt_emi_sync_write(EMI_APB_BASE+0x00000644,0x132d1229); //1866 (1:8) r16 - 	r13 overhead
  mt_emi_sync_write(EMI_APB_BASE+0x0000004c,0x0c201a28); //1600 (1:8) r4  - 	r1	overhead
  mt_emi_sync_write(EMI_APB_BASE+0x00000084,0x0d230a20); //1600 (1:8) r8  - 	r5	overhead
  mt_emi_sync_write(EMI_APB_BASE+0x000001e4,0x0e260d24); //1600 (1:8) r12 - 	r9	overhead
  mt_emi_sync_write(EMI_APB_BASE+0x00000648,0x132d1229); //1600 (1:8) r16 - 	r13 overhead
  mt_emi_sync_write(EMI_APB_BASE+0x00000054,0x0c201a28); //1200 (1:8) r4  - 	r1	overhead
  mt_emi_sync_write(EMI_APB_BASE+0x0000008c,0x0d230a20); //1200 (1:8) r8  - 	r5	overhead
  mt_emi_sync_write(EMI_APB_BASE+0x000001e8,0x0e260d24); //1200 (1:8) r12 - 	r9	overhead
  mt_emi_sync_write(EMI_APB_BASE+0x0000064c,0x132d1229); //1200 (1:8) r16 - r13 overhead
  mt_emi_sync_write(EMI_APB_BASE+0x0000005c,0x0e290e28); //800	(1:4) r12 - r9	overhead
  mt_emi_sync_write(EMI_APB_BASE+0x00000094,0x091e1322); //800	(1:4) r4  - r1	overhead
  mt_emi_sync_write(EMI_APB_BASE+0x000001c8,0x0f29112a); //800	(1:4) r16 - r13 overhead
  mt_emi_sync_write(EMI_APB_BASE+0x00000660,0x0c240a1f); //800	(1:4) r8  - r5	overhead
  mt_emi_sync_write(EMI_APB_BASE+0x00000064,0x0e290e28); //800	(1:4) r12 - r9	overhead
  mt_emi_sync_write(EMI_APB_BASE+0x0000009c,0x091e1322); //800	(1:4) r4  - r1	overhead
  mt_emi_sync_write(EMI_APB_BASE+0x000001f4,0x0f29112a); //800	(1:4) r16 - r13 overhead
  mt_emi_sync_write(EMI_APB_BASE+0x00000664,0x0c240a1f); //800	(1:4) r8  - r5	overhead

  mt_emi_sync_write(EMI_APB_BASE+0x00000030,0x37373a57); //3733 (1:8) r8  - r2	non-align	  overhead	// TBD - change to 4266
  mt_emi_sync_write(EMI_APB_BASE+0x00000014,0x3f3f3c39); //3733 (1:8) r16 - r10 non-align	  overhead	// TBD - change to 4266
  mt_emi_sync_write(EMI_APB_BASE+0x000008b8,0x3836374e); //3200 (1:8) r8  - r2	non-align	  overhead
  mt_emi_sync_write(EMI_APB_BASE+0x0000002c,0x41413d3a); //3200 (1:8) r16 - r10 non-align	  overhead
  mt_emi_sync_write(EMI_APB_BASE+0x000000c4,0x33313241); //2400 (1:8) r8  - r2	non-align	  overhead
  mt_emi_sync_write(EMI_APB_BASE+0x00000668,0x3a3a3835); //2400 (1:8) r16 - r10 non-align	  overhead
  mt_emi_sync_write(EMI_APB_BASE+0x000000c8,0x34343542); //1866 (1:8) r8  - r2	non-align	  overhead
  mt_emi_sync_write(EMI_APB_BASE+0x0000066c,0x3b3b3835); //1866 (1:8) r16 - r10 non-align	  overhead
  mt_emi_sync_write(EMI_APB_BASE+0x000000cc,0x34343542); //1600 (1:8) r8  - r2	non-align	  overhead
  mt_emi_sync_write(EMI_APB_BASE+0x00000694,0x3b3b3835); //1600 (1:8) r16 - r10 non-align	  overhead
  mt_emi_sync_write(EMI_APB_BASE+0x000000e4,0x34343542); //1200 (1:8) r8  - r2	non-align	  overhead
  mt_emi_sync_write(EMI_APB_BASE+0x00000708,0x3b3b3835); //1200 (1:8) r16 - r10 non-align	  overhead
  mt_emi_sync_write(EMI_APB_BASE+0x000000f4,0x37333034); //800	(1:4) r8  - r2	non-align	  overhead
  mt_emi_sync_write(EMI_APB_BASE+0x0000070c,0x39393a39); //800	(1:4) r16 - r10 non-align	  overhead
  mt_emi_sync_write(EMI_APB_BASE+0x0000012c,0x37333034); //800	(1:4) r8  - r2	non-align	  overhead
  mt_emi_sync_write(EMI_APB_BASE+0x00000748,0x39393a39); //800	(1:4) r16 - r10 non-align	  overhead

  //
  mt_emi_sync_write(EMI_APB_BASE+0x00000018,0x3657587a);
  mt_emi_sync_write(EMI_APB_BASE+0x00000020,0x0000c042);
  mt_emi_sync_write(EMI_APB_BASE+0x00000028,0x08421000);
  mt_emi_sync_write(EMI_APB_BASE+0x00000038,0x00000083);
  mt_emi_sync_write(EMI_APB_BASE+0x0000003c,0x00073210);
  mt_emi_sync_write(EMI_APB_BASE+0x00000040,0x00008802);
  mt_emi_sync_write(EMI_APB_BASE+0x00000048,0x00000000);
  mt_emi_sync_write(EMI_APB_BASE+0x00000060,0x007812ff); // reserved buffer to normal rea	 d/write :8/7
  mt_emi_sync_write(EMI_APB_BASE+0x00000068,0x00000000);
  mt_emi_sync_write(EMI_APB_BASE+0x00000078,0x11120c1f);  //22:20=ultra_w=1
  mt_emi_sync_write(EMI_APB_BASE+0x00000710,0x11120c1f);  //22:20=ultra_w=1
  mt_emi_sync_write(EMI_APB_BASE+0x0000007c,0x00001123);
  mt_emi_sync_write(EMI_APB_BASE+0x00000718,0x00001123);
  mt_emi_sync_write(EMI_APB_BASE+0x000000d0,0xa8a8a8a8);
  mt_emi_sync_write(EMI_APB_BASE+0x000000d4,0x25252525);
  mt_emi_sync_write(EMI_APB_BASE+0x000000d8,0xa8a8a8a8);
  mt_emi_sync_write(EMI_APB_BASE+0x000000dc,0x25252525);
  mt_emi_sync_write(EMI_APB_BASE+0x000000e8,0x00060037); // initial starvation counter di	 v2, [4]=1
  mt_emi_sync_write(EMI_APB_BASE+0x000000f0,0x384a0014);
  mt_emi_sync_write(EMI_APB_BASE+0x000000f8,0xa0000000);
  mt_emi_sync_write(EMI_APB_BASE+0x00000100,0x20107244);
  mt_emi_sync_write(EMI_APB_BASE+0x00000108,0x10107044);
  mt_emi_sync_write(EMI_APB_BASE+0x00000110,0x343450df);
  mt_emi_sync_write(EMI_APB_BASE+0x00000118,0x0000f0d0);
  mt_emi_sync_write(EMI_APB_BASE+0x00000120,0x10106048);
  mt_emi_sync_write(EMI_APB_BASE+0x00000128,0x343450df);
  mt_emi_sync_write(EMI_APB_BASE+0x00000130,0x83837044);
  mt_emi_sync_write(EMI_APB_BASE+0x00000138,0x83837044);
  mt_emi_sync_write(EMI_APB_BASE+0x00000140,0x00007108);
  mt_emi_sync_write(EMI_APB_BASE+0x00000144,0x00007108);
  mt_emi_sync_write(EMI_APB_BASE+0x00000150,0x090a4000);
  mt_emi_sync_write(EMI_APB_BASE+0x00000158,0xff0bff00);
  mt_emi_sync_write(EMI_APB_BASE+0x00000400,0x00ff0001); //[27:20] enable monitor
  mt_emi_sync_write(EMI_APB_BASE+0x0000071c,0x10000008);
  mt_emi_sync_write(EMI_APB_BASE+0x00000800,0xffffffff);
  mt_emi_sync_write(EMI_APB_BASE+0x00000820,0x24240101);
  mt_emi_sync_write(EMI_APB_BASE+0x00000824,0x01012424);
  mt_emi_sync_write(EMI_APB_BASE+0x00000828,0x50500101);
  mt_emi_sync_write(EMI_APB_BASE+0x0000082c,0x01015050);
  mt_emi_sync_write(EMI_APB_BASE+0x00000830,0x0fc39a30); // [6] MD_HRT_URGENT_MASK, if 1	 -> mask MD_HRT_URGENT,
  mt_emi_sync_write(EMI_APB_BASE+0x00000834,0x05050003);
  mt_emi_sync_write(EMI_APB_BASE+0x00000838,0x254dffff);
  mt_emi_sync_write(EMI_APB_BASE+0x0000083c,0x465a788c); //update
  mt_emi_sync_write(EMI_APB_BASE+0x00000840,0x000003e8);
  mt_emi_sync_write(EMI_APB_BASE+0x00000844,0x0000036b);
  mt_emi_sync_write(EMI_APB_BASE+0x00000848,0x00000290);
  mt_emi_sync_write(EMI_APB_BASE+0x0000084c,0x00000200);
  mt_emi_sync_write(EMI_APB_BASE+0x00000850,0x00000000);
  mt_emi_sync_write(EMI_APB_BASE+0x00000854,0x00000000);
  mt_emi_sync_write(EMI_APB_BASE+0x00000858,0x02531cff); //ignore rff threshold
  mt_emi_sync_write(EMI_APB_BASE+0x0000085c,0x00002785);
  mt_emi_sync_write(EMI_APB_BASE+0x00000874,0x000001b5);
  mt_emi_sync_write(EMI_APB_BASE+0x00000878,0x003c0000); //update
  mt_emi_sync_write(EMI_APB_BASE+0x0000087c,0x0255250d);
  mt_emi_sync_write(EMI_APB_BASE+0x00000890,0xffff3c59);
  mt_emi_sync_write(EMI_APB_BASE+0x00000894,0xffff00ff);
  mt_emi_sync_write(EMI_APB_BASE+0x000008a0,0xffffffff);
  mt_emi_sync_write(EMI_APB_BASE+0x000008a4,0x0000ffff);
  mt_emi_sync_write(EMI_APB_BASE+0x000008c0,0x0000014b);
  mt_emi_sync_write(EMI_APB_BASE+0x000008c4,0x002d0000); //update
  mt_emi_sync_write(EMI_APB_BASE+0x000008c8,0x00000185);
  mt_emi_sync_write(EMI_APB_BASE+0x000008cc,0x003c0000); //update
  mt_emi_sync_write(EMI_APB_BASE+0x000008d0,0x00000185);
  mt_emi_sync_write(EMI_APB_BASE+0x000008d4,0x003c0000); //update
  mt_emi_sync_write(EMI_APB_BASE+0x000008e0,0xffffffff);
  mt_emi_sync_write(EMI_APB_BASE+0x000008e4,0xffffffff);
  mt_emi_sync_write(EMI_APB_BASE+0x000008e8,0xffffffff);
  mt_emi_sync_write(EMI_APB_BASE+0x00000920,0xffffffff);
  mt_emi_sync_write(EMI_APB_BASE+0x00000924,0x0000ffff);
  mt_emi_sync_write(EMI_APB_BASE+0x00000930,0xffffffff);
  mt_emi_sync_write(EMI_APB_BASE+0x00000934,0xffffffff);
  mt_emi_sync_write(EMI_APB_BASE+0x00000938,0xffffffff);
  mt_emi_sync_write(EMI_APB_BASE+0x000009f0,0x41547082);
  mt_emi_sync_write(EMI_APB_BASE+0x000009f4,0x38382a38);
  mt_emi_sync_write(EMI_APB_BASE+0x000009f8,0x000001d4);
  mt_emi_sync_write(EMI_APB_BASE+0x000009fc,0x00000190);
  mt_emi_sync_write(EMI_APB_BASE+0x00000b00,0x0000012c);
  mt_emi_sync_write(EMI_APB_BASE+0x00000b04,0x000000ed);
  mt_emi_sync_write(EMI_APB_BASE+0x00000b08,0x000000c8);
  mt_emi_sync_write(EMI_APB_BASE+0x00000b0c,0x00000096);
  mt_emi_sync_write(EMI_APB_BASE+0x00000b10,0x000000c8);
  mt_emi_sync_write(EMI_APB_BASE+0x00000b14,0x000000c8);
  mt_emi_sync_write(EMI_APB_BASE+0x00000b28,0x26304048);
  mt_emi_sync_write(EMI_APB_BASE+0x00000b2c,0x20201820);
  mt_emi_sync_write(EMI_APB_BASE+0x00000b60,0x181e282f);
  mt_emi_sync_write(EMI_APB_BASE+0x00000b64,0x14140f18);
  mt_emi_sync_write(EMI_APB_BASE+0x00000b98,0x7496c8ea);
  mt_emi_sync_write(EMI_APB_BASE+0x00000b9c,0x64644b64);
  mt_emi_sync_write(EMI_APB_BASE+0x00000bd0,0x01010101);
  mt_emi_sync_write(EMI_APB_BASE+0x00000bd4,0x01010101);
  mt_emi_sync_write(EMI_APB_BASE+0x00000c08,0x7496c8ea); // 20190821 item3 - synced // TB	 D- 4266 may need changes
  mt_emi_sync_write(EMI_APB_BASE+0x00000c0c,0x64644b64); // 20190821 item3 - synced // TB	 D- 4266 may need changes
  mt_emi_sync_write(EMI_APB_BASE+0x00000c40,0x01010101);
  mt_emi_sync_write(EMI_APB_BASE+0x00000c44,0x01010101);
  mt_emi_sync_write(EMI_APB_BASE+0x00000c4c,0x300ff025);	//ignore wff threshold
  mt_emi_sync_write(EMI_APB_BASE+0x00000c80,0x000003e8);
  mt_emi_sync_write(EMI_APB_BASE+0x00000c84,0x0000036b);
  mt_emi_sync_write(EMI_APB_BASE+0x00000c88,0x00000290);
  mt_emi_sync_write(EMI_APB_BASE+0x00000c8c,0x00000200);
  mt_emi_sync_write(EMI_APB_BASE+0x00000c90,0x000001b5);
  mt_emi_sync_write(EMI_APB_BASE+0x00000c94,0x0000014b);
  mt_emi_sync_write(EMI_APB_BASE+0x00000c98,0x00000185);
  mt_emi_sync_write(EMI_APB_BASE+0x00000c9c,0x00000185);
  mt_emi_sync_write(EMI_APB_BASE+0x00000cb0,0x52698ca0);
  mt_emi_sync_write(EMI_APB_BASE+0x00000cb4,0x46463546);
  mt_emi_sync_write(EMI_APB_BASE+0x00000cf8,0x01010101);
  mt_emi_sync_write(EMI_APB_BASE+0x00000cfc,0x01010101);

  mt_emi_sync_write(EMI_APB_BASE+0x00000d04,0x00000009); //MDR shf0 event selet
  mt_emi_sync_write(EMI_APB_BASE+0x00000d0c,0x00000000); //MDR shf1 event selet
  mt_emi_sync_write(EMI_APB_BASE+0x00000d14,0x00730000); //MDR shf0
  mt_emi_sync_write(EMI_APB_BASE+0x00000d18,0x00000808); //MDR shf1
  mt_emi_sync_write(EMI_APB_BASE+0x00000d1c,0x00000028); //MDW shf0 event selet
  mt_emi_sync_write(EMI_APB_BASE+0x00000d24,0x00000000); //MDW shf1 event selet
  mt_emi_sync_write(EMI_APB_BASE+0x00000d2c,0x00730000); //MDW shf0
  mt_emi_sync_write(EMI_APB_BASE+0x00000d30,0x00000808); //MDW shf1
  mt_emi_sync_write(EMI_APB_BASE+0x00000d34,0x00000080); //APR shf0 event selet
  mt_emi_sync_write(EMI_APB_BASE+0x00000d3c,0x00000000); //APR shf1 event selet
  mt_emi_sync_write(EMI_APB_BASE+0x00000d44,0x30201008); //APR shf0/shf1
  mt_emi_sync_write(EMI_APB_BASE+0x00000d48,0x00000800); //APW shf0 event selet
  mt_emi_sync_write(EMI_APB_BASE+0x00000d50,0x00000000); //APW shf1 event selet
  mt_emi_sync_write(EMI_APB_BASE+0x00000d58,0x00008000); //MMR shf0 event selet
  mt_emi_sync_write(EMI_APB_BASE+0x00000d60,0x00020000); //MMR shf1 event selet
  mt_emi_sync_write(EMI_APB_BASE+0x00000d64,0x00001000); //MMR shf1 event selet
  mt_emi_sync_write(EMI_APB_BASE+0x00000d68,0x00010000); //MMR shf2 event selet
  mt_emi_sync_write(EMI_APB_BASE+0x00000d6c,0x00000800); //MMR shf2 event selet
  mt_emi_sync_write(EMI_APB_BASE+0x00000d70,0x08080000); //MMR shf0
  mt_emi_sync_write(EMI_APB_BASE+0x00000d74,0x00073030); //MMR shf1
  mt_emi_sync_write(EMI_APB_BASE+0x00000d78,0x00040000); //MMW shf0 event selet
  mt_emi_sync_write(EMI_APB_BASE+0x00000d80,0x00100000); //MMW shf1 event selet
  mt_emi_sync_write(EMI_APB_BASE+0x00000d84,0x00004000); //MMW shf1 event selet
  mt_emi_sync_write(EMI_APB_BASE+0x00000d88,0x00080000); //MMW shf2 event selet
  mt_emi_sync_write(EMI_APB_BASE+0x00000d8c,0x00002000); //MMW shf2 event selet
  mt_emi_sync_write(EMI_APB_BASE+0x00000d90,0x08080000); //MMW shf0
  mt_emi_sync_write(EMI_APB_BASE+0x00000d94,0x00074040); //MMW shf1
  mt_emi_sync_write(EMI_APB_BASE+0x00000d98,0x00400000); //MDHWR sh0 event select
  mt_emi_sync_write(EMI_APB_BASE+0x00000da0,0x00200000); //MDHWR sh1 event select
  mt_emi_sync_write(EMI_APB_BASE+0x00000da8,0x10100404); //MDHWWR sh
  mt_emi_sync_write(EMI_APB_BASE+0x00000dac,0x01000000); //MDHWW sh0 event select
  mt_emi_sync_write(EMI_APB_BASE+0x00000db4,0x00800000); //MDHWW sh1 event select
  mt_emi_sync_write(EMI_APB_BASE+0x00000dbc,0x04000000); //GPUR sh0 event select
  mt_emi_sync_write(EMI_APB_BASE+0x00000dc4,0x02000000); //GPUR sh1 event select
  mt_emi_sync_write(EMI_APB_BASE+0x00000dcc,0x60602010); //GPUR
  mt_emi_sync_write(EMI_APB_BASE+0x00000dd0,0x10000000); //GPUW sh0 event select
  mt_emi_sync_write(EMI_APB_BASE+0x00000dd8,0x08000000); //GPUW sh1 event select
  mt_emi_sync_write(EMI_APB_BASE+0x00000de0,0x00000009); //ARBR sh0 event select
  mt_emi_sync_write(EMI_APB_BASE+0x00000de8,0x04400080); //ARBR sh1 event select
  mt_emi_sync_write(EMI_APB_BASE+0x00000df0,0x0f170f11); //ARB
  mt_emi_sync_write(EMI_APB_BASE+0x00000df4,0x0303f7f7); //QOS control
  mt_emi_sync_write(EMI_APB_BASE+0x00000e04,0x00000166);
  mt_emi_sync_write(EMI_APB_BASE+0x00000e08,0xffffffff);
  mt_emi_sync_write(EMI_APB_BASE+0x00000e0c,0xffffffff);
  mt_emi_sync_write(EMI_APB_BASE+0x00000e14,0x00400166);
  mt_emi_sync_write(EMI_APB_BASE+0x00000e18,0xffffffff);
  mt_emi_sync_write(EMI_APB_BASE+0x00000e1c,0xffffffff);
  mt_emi_sync_write(EMI_APB_BASE+0x00000e24,0x00000266);
  mt_emi_sync_write(EMI_APB_BASE+0x00000e28,0xffffffff);
  mt_emi_sync_write(EMI_APB_BASE+0x00000e2c,0xffffffff);
  mt_emi_sync_write(EMI_APB_BASE+0x00000e34,0x00400266);
  mt_emi_sync_write(EMI_APB_BASE+0x00000e38,0xffffffff);
  mt_emi_sync_write(EMI_APB_BASE+0x00000e3c,0xffffffff);

  // Added by Wei-Lun - START
  // prtcl chker - golden setting
  mt_emi_sync_write(EMI_APB_BASE+0x00000304,0xffffffff); // cyc
  mt_emi_sync_write(EMI_APB_BASE+0x0000030c,0x001ffc85); // ctl
  mt_emi_sync_write(EMI_APB_BASE+0x00000314,0xffffffff); // msk

  mt_emi_sync_write(EMI_APB_BASE+0x0000034c,0xffffffff);
  mt_emi_sync_write(EMI_APB_BASE+0x00000354,0x001ffc85);
  mt_emi_sync_write(EMI_APB_BASE+0x0000035c,0xffffffff); // msk

  mt_emi_sync_write(EMI_APB_BASE+0x00000394,0xffffffff);
  mt_emi_sync_write(EMI_APB_BASE+0x0000039c,0x001ffc85);
  mt_emi_sync_write(EMI_APB_BASE+0x000003a4,0xffffffff); // msk

  mt_emi_sync_write(EMI_APB_BASE+0x000003d8,0xffffffff);
  mt_emi_sync_write(EMI_APB_BASE+0x000003dc,0x001ffc85);
  mt_emi_sync_write(EMI_APB_BASE+0x000003e0,0xffffffff); // msk

  mt_emi_sync_write(EMI_APB_BASE+0x000003fc,0xffffffff);
  mt_emi_sync_write(EMI_APB_BASE+0x0000040c,0x001ffc85);
  mt_emi_sync_write(EMI_APB_BASE+0x00000414,0xffffffff); // msk

  mt_emi_sync_write(EMI_APB_BASE+0x0000044c,0xffffffff);
  mt_emi_sync_write(EMI_APB_BASE+0x00000454,0x001ffc85);
  mt_emi_sync_write(EMI_APB_BASE+0x0000045c,0xffffffff); // msk

  mt_emi_sync_write(EMI_APB_BASE+0x0000049c,0xffffffff);
  mt_emi_sync_write(EMI_APB_BASE+0x000004a4,0x001ffc85);
  mt_emi_sync_write(EMI_APB_BASE+0x000004ac,0xffffffff); // msk

  mt_emi_sync_write(EMI_APB_BASE+0x0000050c,0xffffffff);
  mt_emi_sync_write(EMI_APB_BASE+0x00000514,0x001ffc85);
  mt_emi_sync_write(EMI_APB_BASE+0x0000051c,0xffffffff); // msk

  // maxbw monitor - golden setting

  // enable dbw0 cnter for max bw correlation (m3/4 no filters are enabled)
  // Added by Wei-Lun - END

  //weilun for new feature
  mt_emi_sync_write(EMI_APB_BASE+0x00000714,0x00000000); // dvfs level setting for chn_em	 i rw switching shf

  // cen_emi timeout value
  mt_emi_sync_write(EMI_APB_BASE+0x00000628,0x60606060);
  mt_emi_sync_write(EMI_APB_BASE+0x0000062c,0x60606060);

  // fine-grained qos
  mt_emi_sync_write(EMI_APB_BASE+0x00000050,0x00000000);

  // ostd->bw
  mt_emi_sync_write(EMI_APB_BASE+0x0000061c,0x08ffbbff);
  mt_emi_sync_write(EMI_APB_BASE+0x00000624,0xffff5b3c);
  mt_emi_sync_write(EMI_APB_BASE+0x00000774,0xffff00ff);
  mt_emi_sync_write(EMI_APB_BASE+0x0000077c,0x00ffffff);
  mt_emi_sync_write(EMI_APB_BASE+0x00000784,0xffff00ff);
  mt_emi_sync_write(EMI_APB_BASE+0x0000078c,0x00ffffff);
  mt_emi_sync_write(EMI_APB_BASE+0x00000958,0x00000000);

  // hash rule
  mt_emi_sync_write(EMI_APB_BASE+0x000007a4,0xC0000000);
}

static void emi_chn_config(void)
{
#ifdef RANK_512MB  // => 2channel , dual rank , total=2G
  mt_emi_sync_write(CHN0_EMI_BASE+0x00000000,0x0400a051);
#else  //RANK_1G  => 2channel , dual rank , total=4G
  mt_emi_sync_write(CHN0_EMI_BASE+0x00000000,0x0400f051);
#endif
  mt_emi_sync_write(CHN0_EMI_BASE+0x00000008,0x00ff6048);
  mt_emi_sync_write(CHN0_EMI_BASE+0x00000010,0x00000004);
  mt_emi_sync_write(CHN0_EMI_BASE+0x00000018,0x99f08c03);
  mt_emi_sync_write(CHN0_EMI_BASE+0x00000710,0x9a508c17); // [24:20] = 0x2 : bank	  throttling (default=0x01f00000)
  mt_emi_sync_write(CHN0_EMI_BASE+0x00000048,0x00038137); //RD_INORDER_THR[20:16]=	  2
  mt_emi_sync_write(CHN0_EMI_BASE+0x00000050,0x38460002); // [1] : MD_RD_AFT_WR_EN
  mt_emi_sync_write(CHN0_EMI_BASE+0x00000058,0x00000000);
  mt_emi_sync_write(CHN0_EMI_BASE+0x00000090,0x000002ff);
  mt_emi_sync_write(CHN0_EMI_BASE+0x00000098,0x00003111); //mw2
  mt_emi_sync_write(CHN0_EMI_BASE+0x00000140,0x22607188);
  mt_emi_sync_write(CHN0_EMI_BASE+0x00000144,0x22607188);
  mt_emi_sync_write(CHN0_EMI_BASE+0x00000148,0x3719595e); // chuan
  mt_emi_sync_write(CHN0_EMI_BASE+0x0000014c,0x2719595e); // chuan
  mt_emi_sync_write(CHN0_EMI_BASE+0x00000150,0x64f3ff79);
  mt_emi_sync_write(CHN0_EMI_BASE+0x00000154,0x64f3ff79); // update timeout settin	  g: bit 12~15
  mt_emi_sync_write(CHN0_EMI_BASE+0x00000158,0x011b0868);

// #ifdef SCN_ICFP
//	 mt_emi_sync_write(CHN0_EMI_BASE+0x0000015c,0x88410222); // Stop urgent read f	  irst when write command buffer remain < 8
// #else //SCN_UI
//	 mt_emi_sync_write(CHN0_EMI_BASE+0x0000015c,0x82410222); // Stop urgent read f	  irst when write command buffer remain < 2
// #endif

  mt_emi_sync_write(CHN0_EMI_BASE+0x0000015c,0xa7414222); // Stop urgent read firs	  t when write command buffer remain < 7, [31] ultra_read_first, [30:28] wr_rsv_thr_l, [27:    24] wr_rsv_thr_h,
  mt_emi_sync_write(CHN0_EMI_BASE+0x0000016c,0x0000f801);
  mt_emi_sync_write(CHN0_EMI_BASE+0x00000170,0x40000000);
  mt_emi_sync_write(CHN0_EMI_BASE+0x000001b0,0x000c802f); // Rank-Aware arbitration
  mt_emi_sync_write(CHN0_EMI_BASE+0x000001b4,0xbd3f3f7e); // Rank-Aware arbitration
  mt_emi_sync_write(CHN0_EMI_BASE+0x000001b8,0x7e003d7e); // Rank-Aware arbitration
  mt_emi_sync_write(CHN0_EMI_BASE+0x000003fc,0x00000000); // Write M17_toggle_mask = 0
  mt_emi_sync_write(CHN0_EMI_BASE+0x00000080,0xaa0148ff);
  mt_emi_sync_write(CHN0_EMI_BASE+0x00000088,0xaa6168ff);
  mt_emi_sync_write(CHN0_EMI_BASE+0x00000404,0xaa516cff);
  mt_emi_sync_write(CHN0_EMI_BASE+0x00000408,0xaa0140ff);
  mt_emi_sync_write(CHN0_EMI_BASE+0x0000040c,0x9f658633);
}

static void emi_sw_setting(void)
{
	int emi_dcm;
	/* Enable MPU violation interrupt to MD for D1 and D7 */
	*((volatile unsigned int *)EMI_MPU_CTRL_D(1)) |= 0x10;
	*((volatile unsigned int *)EMI_MPU_CTRL_D(7)) |= 0x10;

	/* for DVFS BW monitor */
	*((volatile unsigned int *) EMI_BWCT0) = 0x05008305;
	*((volatile unsigned int *) EMI_BWCT0_6TH) = 0x08FF8705;
	*((volatile unsigned int *) EMI_BWCT0_3RD) = 0x0DFF8A05;
	*((volatile unsigned int *) EMI_THRO_CTRL1) |= 0x300;

#ifdef LAST_EMI
	last_emi_info_ptr = (LAST_EMI_INFO_T *) get_dbg_info_base(KEY_LAST_EMI);
	if (last_emi_info_ptr->isu_magic != LAST_EMI_MAGIC_PATTERN) {
	last_emi_info_ptr->isu_magic = LAST_EMI_MAGIC_PATTERN;
	last_emi_info_ptr->isu_version = 0xFFFFFFFF;
	last_emi_info_ptr->isu_dram_type = 0;
	last_emi_info_ptr->isu_diff_us = 0;
		last_emi_info_ptr->os_flag_sspm = 0;
		last_emi_info_ptr->os_flag_ap = 0;
	}
	emi_isu = atoi(dconfig_getenv("emi_isu"));
	emi_log("[EMI DOE] emi_isu %d\n", emi_isu);
	if (emi_isu == 1)
		last_emi_info_ptr->isu_ctrl = 0xDECDDECD;
	else if (emi_isu == 2)
		last_emi_info_ptr->isu_ctrl = 0xDEC0DEC0;
	else {
#if CFG_LAST_EMI_BW_DUMP
	last_emi_info_ptr->isu_ctrl = 0xDECDDECD;
#else
	last_emi_info_ptr->isu_ctrl = 0xDEC0DEC0;
#endif
	}
#endif
	*((volatile unsigned int *)0x10219858) |= 0x1 << 11;
	emi_dcm = 0;//atoi(dconfig_getenv("emi_dcm"));
	emi_log("[EMI DOE] emi_dcm %d\n", emi_dcm);
	if (emi_dcm == 1) {
		*((volatile unsigned int *)EMI_CONM) &= ~0xFF000000;
		*((volatile unsigned int *)EMI_CONN) &= ~0xFF000000;
	} else if (emi_dcm == 2) {
		*((volatile unsigned int *)EMI_CONM) |= 0xFF000000;
		*((volatile unsigned int *)EMI_CONN) |= 0xFF000000;
	}
	dsb();
}

void emi_init(void)
{
  /* emi_config_lpddr4_2ch_golden_20191202_1000.c */
  //Broadcast on
  mt_emi_sync_write(INFRA_DRAMC_REG_CONFIG,0x0000007f);

  emi_cen_config();
  emi_chn_config();

  //Broadcast off
  mt_emi_sync_write(INFRA_DRAMC_REG_CONFIG,0x00000000);
}

void emi_init2(void)
{
  unsigned int emi_temp_data;

  //Broadcast on
  mt_emi_sync_write(INFRA_DRAMC_REG_CONFIG, 0x0000007f);

  mt_emi_sync_write_or(CHN0_EMI_BASE+0x00000010, 0x00000001);	// [0] EMI enable
  mt_emi_sync_write_or(EMI_BASE+0x00000060, 0x00000400);		//[10] EMI enable

  #ifdef MARGAUX_REAL_CHIP_EMI_GOLDEN_SETTING

  mt_emi_sync_write_or(EMI_MPU_BASE+0x00000000,0x00000010); // [4] Disable emi_mpu_reg in	 terrupt

  // Clear rank_arb_en
  emi_temp_data = mt_emi_sync_read(CHN0_EMI_BASE+0x000001b0); // read ch0
  emi_temp_data = emi_temp_data & ~(0x1);
  mt_emi_sync_write(CHN0_EMI_BASE+0x000001b0, emi_temp_data); // broadcast to all	  channel
  // auto-config rank_arb_en according to dual_rank_en setting
  // assume all channel with same configuration
  emi_temp_data = mt_emi_sync_read(CHN0_EMI_BASE+0x00000000); // read ch0
  emi_temp_data = emi_temp_data & 0x1;
  mt_emi_sync_write_or(CHN0_EMI_BASE+0x000001b0, emi_temp_data); // broadcast to a	  ll channel

  // ----- from dcm_setting.c -----
  mt_emi_sync_write(INFRACFG_AO_MEM_BASE+0x100, 0xFFFFFFFF);
  mt_emi_sync_write(INFRACFG_AO_MEM_BASE+0x104, 0xFFFFFFFF);
  mt_emi_sync_write(INFRACFG_AO_MEM_BASE+0x108, 0xFFFFFFFF);
  mt_emi_sync_write(INFRACFG_AO_MEM_BASE+0x10C, 0xFFFFFFFF);
  mt_emi_sync_write(INFRACFG_AO_MEM_BASE+0x110, 0x01F00000);
  mt_emi_sync_write(INFRACFG_AO_MEM_BASE+0x114, 0xC0040180);
  mt_emi_sync_write(INFRACFG_AO_MEM_BASE+0x118, 0x00000000);
  mt_emi_sync_write(INFRACFG_AO_MEM_BASE+0x11C, 0x00000003);
  mt_emi_sync_write(INFRACFG_AO_MEM_BASE+0x120, 0x0C000000);
  mt_emi_sync_write(INFRACFG_AO_MEM_BASE+0x124, 0x00C00000);
  mt_emi_sync_write(INFRACFG_AO_MEM_BASE+0x128, 0x01F08000);
  mt_emi_sync_write(INFRACFG_AO_MEM_BASE+0x12C, 0x00000000);
  mt_emi_sync_write(INFRACFG_AO_MEM_BASE+0x130, 0x20003040);
  mt_emi_sync_write(INFRACFG_AO_MEM_BASE+0x134, 0x00000000);
  mt_emi_sync_write(INFRACFG_AO_MEM_BASE+0x138, 0x00001000);
  mt_emi_sync_write(INFRACFG_AO_MEM_BASE+0x13C, 0x00000000);
  mt_emi_sync_write(INFRACFG_AO_MEM_BASE+0x140, 0x10020F20);
  mt_emi_sync_write(INFRACFG_AO_MEM_BASE+0x144, 0x00019000);
  mt_emi_sync_write(INFRACFG_AO_MEM_BASE+0x148, 0x040A0818);
  mt_emi_sync_write(INFRACFG_AO_MEM_BASE+0x14C, 0x00000370);
  mt_emi_sync_write(INFRACFG_AO_MEM_BASE+0x150, 0xC001C080);
  mt_emi_sync_write(INFRACFG_AO_MEM_BASE+0x154, 0x33000E01);
  mt_emi_sync_write(INFRACFG_AO_MEM_BASE+0x158, 0x180067E1);
  mt_emi_sync_write(INFRACFG_AO_MEM_BASE+0x15C, 0x000C008C);
  mt_emi_sync_write(INFRACFG_AO_MEM_BASE+0x160, 0x020C0008);
  mt_emi_sync_write(INFRACFG_AO_MEM_BASE+0x164, 0x0C00007E);
  mt_emi_sync_write(INFRACFG_AO_MEM_BASE+0x168, 0x80050006);
  mt_emi_sync_write(INFRACFG_AO_MEM_BASE+0x16C, 0x00030000);


  mt_emi_sync_write(INFRACFG_AO_MEM_BASE+0x028, 0x0000000F);
  mt_emi_sync_write(INFRACFG_AO_MEM_BASE+0x02C, 0x00000000);
  mt_emi_sync_write(INFRACFG_AO_MEM_BASE+0x030, 0x001F0044);
  mt_emi_sync_write(INFRACFG_AO_MEM_BASE+0x038, 0x200000FF);

  mt_emi_sync_write_or(INFRACFG_AO_BASE+0x00000078, 0x08000000); // enable infra_local_cg

  #ifdef MARGAUX_EMI_MP_SETTING
  // Enable rdata_prty_gen & wdata_prty_chk

  // emi bus parity workaround
  emi_temp_data = mt_emi_sync_read(0x40000000);
  mt_emi_sync_write(0x40000000, emi_temp_data);
  emi_temp_data = mt_emi_sync_read(0x40000100);
  mt_emi_sync_write(0x40000100, emi_temp_data);
  emi_temp_data = mt_emi_sync_read(0x40000200);
  mt_emi_sync_write(0x40000200, emi_temp_data);
  emi_temp_data = mt_emi_sync_read(0x40000300);
  mt_emi_sync_write(0x40000300, emi_temp_data);

  mt_emi_sync_write_or(EMI_APB_BASE+0x00000068,0x00400000);  // enable cen_emi parity (w)
  mt_emi_sync_write_or(CHN0_EMI_BASE+0x00000050,0x00000004); // enable chn_emi par	  ity

  /*TINFO="Enable APMCU Early CKE"*/
  //mt_emi_sync_write_or(EMI_APB_BASE+0x000007f4, 0x00006000);
  //emi_temp_data = mt_emi_sync_read(EMI_APB_BASE+0x000007f4);
  mt_emi_sync_write(INFRACFG_AO_MEM_BASE+0x00000050, 0x00000007); // set disph_chn_en = 0x7
  mt_emi_sync_write(INFRACFG_AO_MEM_BASE+0x00000050, 0x80000007); // set disph_chg_en = 0x1

  /*TINFO="read emi_reg_pd then write apmcu config reg"*/
  emi_temp_data = mt_emi_sync_read(INFRACFG_AO_MEM_BASE+0x050);
  emi_temp_data = emi_temp_data & 0xf;
  mt_emi_sync_write_or(EMI_BASE+0x07A4, emi_temp_data);

  /*TINFO="Enable EMI wdata bus encode function"*/
  mt_emi_sync_write_or(EMI_APB_BASE+0x00000068,0x00200000); // enable cen_emi wdata bus e	 ncode			// *EMI_CONN |= (0x1 << 21);
  mt_emi_sync_write_or(CHN0_EMI_BASE+0x00000050,0x00000010); // enable chn_emi wda	  ta bus encode  // *CHN0_EMI_CHN_EMI_DFTC |= (0x1 <<4);
  #else
  // MP_dsim_v02 test (from v01) - all fr
  mt_emi_sync_write(INFRACFG_AO_MEM_BASE+0x028, 0x003F0000);
  mt_emi_sync_write(INFRACFG_AO_MEM_BASE+0x02C, 0xA0000000);
  #endif

  #endif

  mt_emi_sync_write(CHN0_EMI_BASE+0x0020, 0x00000040); // disable EBG

  //Broadcast off
  mt_emi_sync_write(INFRA_DRAMC_REG_CONFIG, 0x00000000);

  emi_sw_setting();
}

int get_row_width_by_emi(unsigned int rank)
{
	unsigned int emi_cona;
	unsigned int shift_row, shift_ext;
	int row_width;

	if (rank == 0) {
		shift_row = 12;
		shift_ext = 22;
	} else if (rank == 1) {
		shift_row = 14;
		shift_ext = 23;
	} else
		return -1;

	emi_cona = mt_emi_sync_read(EMI_CONA);
	row_width =
		((emi_cona >> shift_row) & 0x3) |
		((emi_cona >> shift_ext) & 0x4);

	return (row_width + 13);
}

int get_channel_nr_by_emi(void)
{
	int channel_nr;

	channel_nr = 0x1 << ((mt_emi_sync_read(EMI_CONA) >> 8) & 0x3);

	return channel_nr;
}

int get_rank_nr_by_emi(void)
{
	unsigned int cen_emi_cona = mt_emi_sync_read(EMI_CONA);

	if (cen_emi_cona & (0x3 << 16))
		return 2;
	else
		return 1;
}

_Static_assert(DRAMC_MAX_RK > 1, "rank number is violated");
void get_rank_size_by_emi(unsigned long long dram_rank_size[DRAMC_MAX_RK])
{
	unsigned int quad_ch_ratio;
	unsigned long long ch0_rank0_size, ch0_rank1_size;
	unsigned long long ch1_rank0_size, ch1_rank1_size;
	unsigned int cen_emi_conh = mt_emi_sync_read(EMI_CONH);
	unsigned long long dq_width;

	dq_width = 2;

	dram_rank_size[0] = 0;
	dram_rank_size[1] = 0;

	ch0_rank0_size = (cen_emi_conh >> 16) & 0xF;
	ch0_rank1_size = (cen_emi_conh >> 20) & 0xF;
	ch1_rank0_size = (cen_emi_conh >> 24) & 0xF;
	ch1_rank1_size = (cen_emi_conh >> 28) & 0xF;

	quad_ch_ratio = (get_rank_nr_by_emi() == 4)? 2 : 1;
	ch0_rank0_size = (ch0_rank0_size * quad_ch_ratio) << 28;
	ch0_rank1_size = (ch0_rank1_size * quad_ch_ratio) << 28;
	ch1_rank0_size = (ch1_rank0_size * quad_ch_ratio) << 28;
	ch1_rank1_size = (ch1_rank1_size * quad_ch_ratio) << 28;

	if(ch0_rank0_size == 0) {
		die("[EMI] undefined CONH for CH0 RANK0\n");
		}
	dram_rank_size[0] += ch0_rank0_size;

		if (get_rank_nr_by_emi() > 1) {
		if(ch0_rank1_size == 0) {
			die("[EMI] undefined CONH for CH0 RANK1\n");
		}
		dram_rank_size[1] += ch0_rank1_size;
		}

	if(get_channel_nr_by_emi() > 1) {
		if(ch1_rank0_size == 0) {
			die("[EMI] undefined CONH for CH1 RANK0\n");
		}
		dram_rank_size[0] += ch1_rank0_size;

		if (get_rank_nr_by_emi() > 1) {
			if(ch1_rank1_size == 0) {
				die("[EMI] undefined CONH for CH1 RANK1\n");
					}
			dram_rank_size[1] += ch1_rank1_size;
		}
		}

	emi_log("DRAM rank0 size:0x%llx,\nDRAM rank1 size=0x%llx\n",
		dram_rank_size[0], dram_rank_size[1]);
}

void set_cen_emi_cona(unsigned int cona_val)
{
	mt_emi_sync_write(EMI_CONA, cona_val);
}

void set_cen_emi_conf(unsigned int conf_val)
{
	mt_emi_sync_write(EMI_CONF, conf_val);
}

void set_cen_emi_conh(unsigned int conh_val)
{
	mt_emi_sync_write(EMI_CONH, conh_val);
}

void set_chn_emi_cona(unsigned int cona_val)
{
	mt_emi_sync_write(CHN_EMI_CONA(CHN0_EMI_BASE), cona_val);
	mt_emi_sync_write(CHN_EMI_CONA(CHN1_EMI_BASE), cona_val);
}

void set_chn_emi_conc(unsigned int conc_val)
{
	mt_emi_sync_write(CHN_EMI_CONC(CHN0_EMI_BASE), conc_val);
	mt_emi_sync_write(CHN_EMI_CONC(CHN1_EMI_BASE), conc_val);
}

unsigned int get_cen_emi_cona(void)
{
	return mt_emi_sync_read(EMI_CONA);
}

/* assume all chn emi setting are the same */
unsigned int get_chn_emi_cona(void)
{
	unsigned int ch0_emi_cona;

	ch0_emi_cona = mt_emi_sync_read(CHN0_EMI_BASE);

	return ch0_emi_cona;
}

void phy_addr_to_dram_addr(dram_addr_t *dram_addr, unsigned long long phy_addr)
{
	unsigned int cen_emi_cona, cen_emi_conf;
	unsigned long long rank_size[DRAMC_MAX_RK];
	unsigned int channel_num, rank_num;
	unsigned int bit_scramble, bit_xor, bit_shift, channel_pos, channel_width;
	unsigned int temp;
	unsigned int index;

	cen_emi_cona = mt_emi_sync_read(EMI_CONA);
	cen_emi_conf = mt_emi_sync_read(EMI_CONF) >> 8;
	get_rank_size_by_emi(rank_size);
	rank_num = (unsigned int) get_rank_nr_by_emi();
	channel_num = (unsigned int) get_channel_nr_by_emi();

	phy_addr -= 0x40000000;
	for (index = 0; index < rank_num; index++) {
		if (phy_addr >= rank_size[index])
			phy_addr -= rank_size[index];
		else
			break;
	}

	for (bit_scramble = 11; bit_scramble < 17; bit_scramble++) {
		bit_xor = (cen_emi_conf >> (4 * (bit_scramble - 11))) & 0xf;
		bit_xor &= phy_addr >> 16;
		for (bit_shift = 0; bit_shift < 4; bit_shift++)
			phy_addr ^= ((bit_xor>>bit_shift)&0x1) << bit_scramble;
	}

	if (channel_num > 1) {
		channel_pos = ((cen_emi_cona >> 2) & 0x3) + 7;

		for (channel_width = bit_shift = 0; bit_shift < 4; bit_shift++) {
			if ((unsigned int)(1 << bit_shift) >= channel_num)
				break;
			channel_width++;
		}

		switch (channel_width) {
		case 2:
			dram_addr->addr = ((phy_addr & ~(((0x1 << 2) << channel_pos) - 1)) >> 2);
			break;
		default:
			dram_addr->addr = ((phy_addr & ~(((0x1 << 1) << channel_pos) - 1)) >> 1);
			break;
		}
		dram_addr->addr |= (phy_addr & ((0x1 << channel_pos) - 1));
	}

	temp = dram_addr->addr >> 1;
	switch ((cen_emi_cona >> 4) & 0x3) {
	case 0:
		dram_addr->col = temp & 0x1FF;
		temp = temp >> 9;
		break;
	case 1:
		dram_addr->col = temp & 0x3FF;
		temp = temp >> 10;
		break;
	case 2:
	default:
		dram_addr->col = temp & 0x7FF;
		temp = temp >> 11;
		break;
	}
	dram_addr->bk = temp & 0x7;
	temp = temp >> 3;

	dram_addr->row = temp;

	emi_log("[EMI] ch%d, rk%d, dram addr: %x\n", dram_addr->ch, dram_addr->rk, dram_addr->addr);
	emi_log("[EMI] bk%x, row%x, col%x\n", dram_addr->bk, dram_addr->row, dram_addr->col);
}

static unsigned int cen_emi_conh_backup = 0;
static unsigned int chn_emi_cona_backup = 0;

/* return the start address of rank1 */
unsigned int set_emi_before_rank1_mem_test(void)
{
	cen_emi_conh_backup = mt_emi_sync_read(EMI_CONH);
	chn_emi_cona_backup = get_chn_emi_cona();

	if (get_rank_nr_by_emi() == 2) {
		/* set the rank size to 1GB for 2 channels */
		mt_emi_sync_write(EMI_CONH,
			(cen_emi_conh_backup & 0x0000ffff) | 0x22220000);
		set_chn_emi_cona(
			(chn_emi_cona_backup & 0xff00ffff) | 0x00220000);
	} else {
		/* set the rank size to 1GB for 1 channel */
		mt_emi_sync_write(EMI_CONH,
			(cen_emi_conh_backup & 0x0000ffff) | 0x44440000);
		set_chn_emi_cona(
			(chn_emi_cona_backup & 0xff00ffff) | 0x00440000);
	}

	return 0x40000000;
}

void restore_emi_after_rank1_mem_test(void)
{
	mt_emi_sync_write(EMI_CONH, cen_emi_conh_backup);
	set_chn_emi_cona(chn_emi_cona_backup);
}

unsigned long long platform_memory_size(void)
{
	static unsigned long long mem_size = 0;
	int nr_rank;
	int i;
	unsigned long long rank_size[DRAMC_MAX_RK];

	if (!mem_size) {
		nr_rank = get_dram_rank_nr();

		get_dram_rank_size(rank_size);

		for (i = 0; i < nr_rank; i++)
			mem_size += rank_size[i];
	}

	return mem_size;
}

size_t sdram_size(void)
{
	int rank_num;
	size_t dram_size = 0;
	u64 rank_size[RANK_MAX];

	get_rank_size_by_emi(rank_size);
	rank_num = get_rank_nr_by_emi();

	for (int i = 0; i < rank_num; i++)
		dram_size += rank_size[i];

	return dram_size;
}
