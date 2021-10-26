/* SPDX-License-Identifier: BSD-3-Clause */

#include <emi_mpu_mt.h>
#include <mt8195.h>

#include <emi_hw.h>
#include <emi.h>

#define EMI_APB_BASE EMI_BASE
#define EMI_CHANNEL_APB_BASE CHN0_EMI_BASE


#if DRAM_AUXADC_CONFIG
#include <mtk_auxadc_sw.h>
#endif

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

static void emi_cen_config(void)  {

#ifndef ONE_CH
  #ifdef RANK_512MB
    mt_emi_sync_write(EMI_APB_BASE+0x00000000,0xa053a154);
  #else
    #ifdef RANK_1GB
      mt_emi_sync_write(EMI_APB_BASE+0x00000000,0xf053f154);
    #else
      #ifdef RANK_2GB
        mt_emi_sync_write(EMI_APB_BASE+0x00000000,0x00530154);
      #endif
    #endif 
  #endif
#else
  #ifdef RANK_512MB
    mt_emi_sync_write(EMI_APB_BASE+0x00000000,0xa053a054);
  #else
    mt_emi_sync_write(EMI_APB_BASE+0x00000000,0xf053f054);
  #endif
#endif


  mt_emi_sync_write(EMI_APB_BASE+0x00000004,0x182e2d33);
  mt_emi_sync_write(EMI_APB_BASE+0x00000008,0x0f251025);
  mt_emi_sync_write(EMI_APB_BASE+0x0000000c,0x122a1027);
  mt_emi_sync_write(EMI_APB_BASE+0x00000010,0x1a31162d);
  mt_emi_sync_write(EMI_APB_BASE+0x000008b0,0x182e2d33);
  mt_emi_sync_write(EMI_APB_BASE+0x000008b4,0x0f251025);
  mt_emi_sync_write(EMI_APB_BASE+0x0000001c,0x122a1027);
  mt_emi_sync_write(EMI_APB_BASE+0x00000024,0x1a31162d);
  mt_emi_sync_write(EMI_APB_BASE+0x00000034,0x1024202c);
  mt_emi_sync_write(EMI_APB_BASE+0x0000006c,0x0b210c21);
  mt_emi_sync_write(EMI_APB_BASE+0x0000013c,0x0f250d23);
  mt_emi_sync_write(EMI_APB_BASE+0x00000640,0x152b1228);
  mt_emi_sync_write(EMI_APB_BASE+0x00000044,0x0c201a28);
  mt_emi_sync_write(EMI_APB_BASE+0x00000074,0x0d230a20);
  mt_emi_sync_write(EMI_APB_BASE+0x000001e0,0x0e260d24);
  mt_emi_sync_write(EMI_APB_BASE+0x00000644,0x132d1229);
  mt_emi_sync_write(EMI_APB_BASE+0x0000004c,0x0c201a28);
  mt_emi_sync_write(EMI_APB_BASE+0x00000084,0x0d230a20);
  mt_emi_sync_write(EMI_APB_BASE+0x000001e4,0x0e260d24);
  mt_emi_sync_write(EMI_APB_BASE+0x00000648,0x132d1229);
  mt_emi_sync_write(EMI_APB_BASE+0x00000054,0x0c201a28);
  mt_emi_sync_write(EMI_APB_BASE+0x0000008c,0x0d230a20);
  mt_emi_sync_write(EMI_APB_BASE+0x000001e8,0x0e260d24);
  mt_emi_sync_write(EMI_APB_BASE+0x0000064c,0x132d1229);
  mt_emi_sync_write(EMI_APB_BASE+0x0000005c,0x0e290e28);
  mt_emi_sync_write(EMI_APB_BASE+0x00000094,0x091e1322);
  mt_emi_sync_write(EMI_APB_BASE+0x000001c8,0x0f29112a);
  mt_emi_sync_write(EMI_APB_BASE+0x00000660,0x0c240a1f);
  mt_emi_sync_write(EMI_APB_BASE+0x00000064,0x0e290e28);
  mt_emi_sync_write(EMI_APB_BASE+0x0000009c,0x091e1322);
  mt_emi_sync_write(EMI_APB_BASE+0x000001f4,0x0f29112a);
  mt_emi_sync_write(EMI_APB_BASE+0x00000664,0x0c240a1f);

  mt_emi_sync_write(EMI_APB_BASE+0x00000030,0x37373a57);
  mt_emi_sync_write(EMI_APB_BASE+0x00000014,0x3f3f3c39);
  mt_emi_sync_write(EMI_APB_BASE+0x000008b8,0x3836374e);
  mt_emi_sync_write(EMI_APB_BASE+0x0000002c,0x41413d3a);
  mt_emi_sync_write(EMI_APB_BASE+0x000000c4,0x33313241);
  mt_emi_sync_write(EMI_APB_BASE+0x00000668,0x3a3a3835);
  mt_emi_sync_write(EMI_APB_BASE+0x000000c8,0x34343542);
  mt_emi_sync_write(EMI_APB_BASE+0x0000066c,0x3b3b3835);
  mt_emi_sync_write(EMI_APB_BASE+0x000000cc,0x34343542);
  mt_emi_sync_write(EMI_APB_BASE+0x00000694,0x3b3b3835);
  mt_emi_sync_write(EMI_APB_BASE+0x000000e4,0x34343542);
  mt_emi_sync_write(EMI_APB_BASE+0x00000708,0x3b3b3835);
  mt_emi_sync_write(EMI_APB_BASE+0x000000f4,0x37333034);
  mt_emi_sync_write(EMI_APB_BASE+0x0000070c,0x39393a39);
  mt_emi_sync_write(EMI_APB_BASE+0x0000012c,0x37333034);
  mt_emi_sync_write(EMI_APB_BASE+0x00000748,0x39393a39);


  mt_emi_sync_write(EMI_APB_BASE+0x00000018,0x3657587a);
  mt_emi_sync_write(EMI_APB_BASE+0x00000020,0x0000c042);
  mt_emi_sync_write(EMI_APB_BASE+0x00000028,0x08421000);
  #ifdef RANK_2GB
    mt_emi_sync_write(EMI_APB_BASE+0x00000038,0x000000b3);
  #else
    mt_emi_sync_write(EMI_APB_BASE+0x00000038,0x00000083);
  #endif
  mt_emi_sync_write(EMI_APB_BASE+0x0000003c,0x00073210);
  mt_emi_sync_write(EMI_APB_BASE+0x00000040,0x00008802);
  mt_emi_sync_write(EMI_APB_BASE+0x00000048,0x00000000);
  mt_emi_sync_write(EMI_APB_BASE+0x00000060,0x007812ff);
  mt_emi_sync_write(EMI_APB_BASE+0x00000068,0x00000000);
  mt_emi_sync_write(EMI_APB_BASE+0x00000078,0x11120c1f);
  mt_emi_sync_write(EMI_APB_BASE+0x00000710,0x11120c1f);
  mt_emi_sync_write(EMI_APB_BASE+0x0000007c,0x00001123);
  mt_emi_sync_write(EMI_APB_BASE+0x00000718,0x00001123);
  mt_emi_sync_write(EMI_APB_BASE+0x000000d0,0xa8a8a8a8);
  mt_emi_sync_write(EMI_APB_BASE+0x000000d4,0x25252525);
  mt_emi_sync_write(EMI_APB_BASE+0x000000d8,0xa8a8a8a8);
  mt_emi_sync_write(EMI_APB_BASE+0x000000dc,0x25252525);
  mt_emi_sync_write(EMI_APB_BASE+0x000000e8,0x00060037);
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
  mt_emi_sync_write(EMI_APB_BASE+0x00000150,0x090a0000);
  mt_emi_sync_write(EMI_APB_BASE+0x00000158,0xff0bff00);
  mt_emi_sync_write(EMI_APB_BASE+0x00000400,0x00ff0001);
  mt_emi_sync_write(EMI_APB_BASE+0x0000071c,0x10000008);
  mt_emi_sync_write(EMI_APB_BASE+0x00000800,0xffffffff);
  mt_emi_sync_write(EMI_APB_BASE+0x00000820,0x24240101);
  mt_emi_sync_write(EMI_APB_BASE+0x00000824,0x01012424);
  mt_emi_sync_write(EMI_APB_BASE+0x00000828,0x50500101);
  mt_emi_sync_write(EMI_APB_BASE+0x0000082c,0x01015050);
  mt_emi_sync_write(EMI_APB_BASE+0x00000830,0x0fc39a30);
  mt_emi_sync_write(EMI_APB_BASE+0x00000834,0x05050003);
  mt_emi_sync_write(EMI_APB_BASE+0x00000838,0x254dffff);
  mt_emi_sync_write(EMI_APB_BASE+0x0000083c,0x465a788c);
  mt_emi_sync_write(EMI_APB_BASE+0x00000840,0x000003e8);
  mt_emi_sync_write(EMI_APB_BASE+0x00000844,0x0000036b);
  mt_emi_sync_write(EMI_APB_BASE+0x00000848,0x00000290);
  mt_emi_sync_write(EMI_APB_BASE+0x0000084c,0x00000200);
  mt_emi_sync_write(EMI_APB_BASE+0x00000850,0x00000000);
  mt_emi_sync_write(EMI_APB_BASE+0x00000854,0x00000000);
  mt_emi_sync_write(EMI_APB_BASE+0x00000858,0x02531cff);
  mt_emi_sync_write(EMI_APB_BASE+0x0000085c,0x00002785);
  mt_emi_sync_write(EMI_APB_BASE+0x00000874,0x000001b5);
  mt_emi_sync_write(EMI_APB_BASE+0x00000878,0x003c0000);
  mt_emi_sync_write(EMI_APB_BASE+0x0000087c,0x0255250d);
  mt_emi_sync_write(EMI_APB_BASE+0x00000890,0xffff3c59);
  mt_emi_sync_write(EMI_APB_BASE+0x00000894,0xffff00ff);
  mt_emi_sync_write(EMI_APB_BASE+0x000008a0,0xffffffff);
  mt_emi_sync_write(EMI_APB_BASE+0x000008a4,0x0000ffff);
  mt_emi_sync_write(EMI_APB_BASE+0x000008c0,0x0000014b);
  mt_emi_sync_write(EMI_APB_BASE+0x000008c4,0x002d0000);
  mt_emi_sync_write(EMI_APB_BASE+0x000008c8,0x00000185);
  mt_emi_sync_write(EMI_APB_BASE+0x000008cc,0x003c0000);
  mt_emi_sync_write(EMI_APB_BASE+0x000008d0,0x00000185);
  mt_emi_sync_write(EMI_APB_BASE+0x000008d4,0x003c0000);
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
  mt_emi_sync_write(EMI_APB_BASE+0x00000c08,0x7496c8ea);
  mt_emi_sync_write(EMI_APB_BASE+0x00000c0c,0x64644b64);
  mt_emi_sync_write(EMI_APB_BASE+0x00000c40,0x01010101);
  mt_emi_sync_write(EMI_APB_BASE+0x00000c44,0x01010101);
  mt_emi_sync_write(EMI_APB_BASE+0x00000c4c,0x300ff025);
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

  mt_emi_sync_write(EMI_APB_BASE+0x00000d04,0x00000009);
  mt_emi_sync_write(EMI_APB_BASE+0x00000d0c,0x00000000);
  mt_emi_sync_write(EMI_APB_BASE+0x00000d14,0x00730000);
  mt_emi_sync_write(EMI_APB_BASE+0x00000d18,0x00000808);
  mt_emi_sync_write(EMI_APB_BASE+0x00000d1c,0x00000028);
  mt_emi_sync_write(EMI_APB_BASE+0x00000d24,0x00000000);
  mt_emi_sync_write(EMI_APB_BASE+0x00000d2c,0x00730000);
  mt_emi_sync_write(EMI_APB_BASE+0x00000d30,0x00000808);
  mt_emi_sync_write(EMI_APB_BASE+0x00000d34,0x00000080);
  mt_emi_sync_write(EMI_APB_BASE+0x00000d3c,0x00000000);
  mt_emi_sync_write(EMI_APB_BASE+0x00000d44,0x30201008);
  mt_emi_sync_write(EMI_APB_BASE+0x00000d48,0x00000800);
  mt_emi_sync_write(EMI_APB_BASE+0x00000d50,0x00000000);
  mt_emi_sync_write(EMI_APB_BASE+0x00000d58,0x00008000);
  mt_emi_sync_write(EMI_APB_BASE+0x00000d60,0x00020000);
  mt_emi_sync_write(EMI_APB_BASE+0x00000d64,0x00001000);
  mt_emi_sync_write(EMI_APB_BASE+0x00000d68,0x00010000);
  mt_emi_sync_write(EMI_APB_BASE+0x00000d6c,0x00000800);
  mt_emi_sync_write(EMI_APB_BASE+0x00000d70,0x08080000);
  mt_emi_sync_write(EMI_APB_BASE+0x00000d74,0x00073030);
  mt_emi_sync_write(EMI_APB_BASE+0x00000d78,0x00040000);
  mt_emi_sync_write(EMI_APB_BASE+0x00000d80,0x00100000);
  mt_emi_sync_write(EMI_APB_BASE+0x00000d84,0x00004000);
  mt_emi_sync_write(EMI_APB_BASE+0x00000d88,0x00080000);
  mt_emi_sync_write(EMI_APB_BASE+0x00000d8c,0x00002000);
  mt_emi_sync_write(EMI_APB_BASE+0x00000d90,0x08080000);
  mt_emi_sync_write(EMI_APB_BASE+0x00000d94,0x00074040);
  mt_emi_sync_write(EMI_APB_BASE+0x00000d98,0x00400000);
  mt_emi_sync_write(EMI_APB_BASE+0x00000da0,0x00200000);
  mt_emi_sync_write(EMI_APB_BASE+0x00000da8,0x10100404);
  mt_emi_sync_write(EMI_APB_BASE+0x00000dac,0x01000000);
  mt_emi_sync_write(EMI_APB_BASE+0x00000db4,0x00800000);
  mt_emi_sync_write(EMI_APB_BASE+0x00000dbc,0x04000000);
  mt_emi_sync_write(EMI_APB_BASE+0x00000dc4,0x02000000);
  mt_emi_sync_write(EMI_APB_BASE+0x00000dcc,0x60602010);
  mt_emi_sync_write(EMI_APB_BASE+0x00000dd0,0x10000000);
  mt_emi_sync_write(EMI_APB_BASE+0x00000dd8,0x08000000);
  mt_emi_sync_write(EMI_APB_BASE+0x00000de0,0x00000009);
  mt_emi_sync_write(EMI_APB_BASE+0x00000de8,0x04400080);
  mt_emi_sync_write(EMI_APB_BASE+0x00000df0,0x0f170f11);
  mt_emi_sync_write(EMI_APB_BASE+0x00000df4,0x0303f7f7);
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


  mt_emi_sync_write(EMI_APB_BASE+0x00000304,0xffffffff);
  mt_emi_sync_write(EMI_APB_BASE+0x0000030c,0x001ffc85);
  mt_emi_sync_write(EMI_APB_BASE+0x00000314,0xffffffff);

  mt_emi_sync_write(EMI_APB_BASE+0x0000034c,0xffffffff);
  mt_emi_sync_write(EMI_APB_BASE+0x00000354,0x001ffc85);
  mt_emi_sync_write(EMI_APB_BASE+0x0000035c,0xffffffff);

  mt_emi_sync_write(EMI_APB_BASE+0x00000394,0xffffffff);
  mt_emi_sync_write(EMI_APB_BASE+0x0000039c,0x001ffc85);
  mt_emi_sync_write(EMI_APB_BASE+0x000003a4,0xffffffff);

  mt_emi_sync_write(EMI_APB_BASE+0x000003d8,0xffffffff);
  mt_emi_sync_write(EMI_APB_BASE+0x000003dc,0x001ffc85);
  mt_emi_sync_write(EMI_APB_BASE+0x000003e0,0xffffffff);

  mt_emi_sync_write(EMI_APB_BASE+0x000003fc,0xffffffff);
  mt_emi_sync_write(EMI_APB_BASE+0x0000040c,0x001ffc85);
  mt_emi_sync_write(EMI_APB_BASE+0x00000414,0xffffffff);

  mt_emi_sync_write(EMI_APB_BASE+0x0000044c,0xffffffff);
  mt_emi_sync_write(EMI_APB_BASE+0x00000454,0x001ffc85);
  mt_emi_sync_write(EMI_APB_BASE+0x0000045c,0xffffffff);

  mt_emi_sync_write(EMI_APB_BASE+0x0000049c,0xffffffff);
  mt_emi_sync_write(EMI_APB_BASE+0x000004a4,0x001ffc85);
  mt_emi_sync_write(EMI_APB_BASE+0x000004ac,0xffffffff);

  mt_emi_sync_write(EMI_APB_BASE+0x0000050c,0xffffffff);
  mt_emi_sync_write(EMI_APB_BASE+0x00000514,0x001ffc85);
  mt_emi_sync_write(EMI_APB_BASE+0x0000051c,0xffffffff);


  mt_emi_sync_write(EMI_APB_BASE+0x00000714,0x00000000);


  mt_emi_sync_write(EMI_APB_BASE+0x00000628,0x60606060);
  mt_emi_sync_write(EMI_APB_BASE+0x0000062c,0x60606060);


  mt_emi_sync_write(EMI_APB_BASE+0x00000050,0x00000000);


  mt_emi_sync_write(EMI_APB_BASE+0x0000061c,0x08ffbbff);
  mt_emi_sync_write(EMI_APB_BASE+0x00000624,0xffff5b3c);
  mt_emi_sync_write(EMI_APB_BASE+0x00000774,0xffff00ff);
  mt_emi_sync_write(EMI_APB_BASE+0x0000077c,0x00ffffff);
  mt_emi_sync_write(EMI_APB_BASE+0x00000784,0xffff00ff);
  mt_emi_sync_write(EMI_APB_BASE+0x0000078c,0x00ffffff);
  mt_emi_sync_write(EMI_APB_BASE+0x00000958,0x00000000);


  //mt_emi_sync_write(EMI_APB_BASE+0x000007a4,0xC0000000);
}

static void emi_chn_config(void)  {

#ifdef RANK_512MB
  mt_emi_sync_write(EMI_CHANNEL_APB_BASE+0x00000000,0x0400a051);
#else
  #ifdef RANK_1GB
    mt_emi_sync_write(EMI_CHANNEL_APB_BASE+0x00000000,0x0400f051);
  #else
    #ifdef RANK_2GB
      mt_emi_sync_write(EMI_CHANNEL_APB_BASE+0x00000000,0x0400005D);
    #endif
  #endif
#endif
  mt_emi_sync_write(EMI_CHANNEL_APB_BASE+0x00000008,0x00ff6048);
  mt_emi_sync_write(EMI_CHANNEL_APB_BASE+0x00000010,0x00000004);
  mt_emi_sync_write(EMI_CHANNEL_APB_BASE+0x00000018,0x99f08c03);
  mt_emi_sync_write(EMI_CHANNEL_APB_BASE+0x00000710,0x9a508c17);
  mt_emi_sync_write(EMI_CHANNEL_APB_BASE+0x00000048,0x00038137);
  mt_emi_sync_write(EMI_CHANNEL_APB_BASE+0x00000050,0x38460002);
  mt_emi_sync_write(EMI_CHANNEL_APB_BASE+0x00000058,0x00000000);
  mt_emi_sync_write(EMI_CHANNEL_APB_BASE+0x00000090,0x000002ff);
  mt_emi_sync_write(EMI_CHANNEL_APB_BASE+0x00000098,0x00003111);
  mt_emi_sync_write(EMI_CHANNEL_APB_BASE+0x00000140,0x22607188);
  mt_emi_sync_write(EMI_CHANNEL_APB_BASE+0x00000144,0x22607188);
  mt_emi_sync_write(EMI_CHANNEL_APB_BASE+0x00000148,0x3719595e);
  mt_emi_sync_write(EMI_CHANNEL_APB_BASE+0x0000014c,0x2719595e);
  mt_emi_sync_write(EMI_CHANNEL_APB_BASE+0x00000150,0x64f3ff79);
  mt_emi_sync_write(EMI_CHANNEL_APB_BASE+0x00000154,0x64f3ff79);
  mt_emi_sync_write(EMI_CHANNEL_APB_BASE+0x00000158,0x011b0868);
  mt_emi_sync_write(EMI_CHANNEL_APB_BASE+0x0000015c,0xa7414222);
  mt_emi_sync_write(EMI_CHANNEL_APB_BASE+0x0000016c,0x0000f801);
  mt_emi_sync_write(EMI_CHANNEL_APB_BASE+0x00000170,0x40000000);
  mt_emi_sync_write(EMI_CHANNEL_APB_BASE+0x000001b0,0x000c802f);
  mt_emi_sync_write(EMI_CHANNEL_APB_BASE+0x000001b4,0xbd3f3f7e);
  mt_emi_sync_write(EMI_CHANNEL_APB_BASE+0x000001b8,0x7e003d7e);
  mt_emi_sync_write(EMI_CHANNEL_APB_BASE+0x000003fc,0x00000000);
  mt_emi_sync_write(EMI_CHANNEL_APB_BASE+0x00000080,0xaa0148ff);
  mt_emi_sync_write(EMI_CHANNEL_APB_BASE+0x00000088,0xaa6168ff);
  mt_emi_sync_write(EMI_CHANNEL_APB_BASE+0x00000404,0xaa516cff);
  mt_emi_sync_write(EMI_CHANNEL_APB_BASE+0x00000408,0xaa0140ff);
  mt_emi_sync_write(EMI_CHANNEL_APB_BASE+0x0000040c,0x9f658633);
}

static void emi_sw_setting(void)
{
	//int emi_isu;
	int emi_dcm;
	//char *str;

	enable_infra_emi_broadcast(1);


	*((volatile unsigned int *) EMI_CONH) = *((volatile unsigned int *) EMI_CONH) | 0xC0;


	*((volatile unsigned int *) EMI_BWCT0) = 0x05000305;
	*((volatile unsigned int *) EMI_BWCT0_6TH) = 0x08FF0705;
	*((volatile unsigned int *) EMI_BWCT0_3RD) = 0x0DFF0A05;
	*((volatile unsigned int *) EMI_BWCT0_4TH) = 0x7FFF0F05;

	enable_infra_emi_broadcast(0);

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

	str = dconfig_getenv("emi_isu");
	emi_isu = (str) ? atoi(str) : 0;
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


	emi_dcm = 0;
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
	//unsigned int emi_mpu_slverr = 0;
	//unsigned int domain = 0;
	//char *str;

	mt_emi_sync_write(EMI_BASE+0x000007a4, 0xC0000000);
#ifdef SUB_EMI_BASE
	mt_emi_sync_write(SUB_EMI_BASE+0x000007a4, 0xD0000000);
#endif

	enable_infra_emi_broadcast(1);
 
   emi_cen_config();
   emi_chn_config();
 
	enable_infra_emi_broadcast(0);
}

void emi_init2(void)
{
  unsigned int emi_temp_data;

  enable_infra_emi_broadcast(1);

  mt_emi_sync_write_or(CHN0_EMI_BASE+0x00000010, 0x00000001);
  mt_emi_sync_write_or(EMI_BASE+0x00000060, 0x00000400);

  #ifdef REAL_CHIP_EMI_GOLDEN_SETTING


  mt_emi_sync_write_or(EMI_MPU_BASE+0x00000000,0x00000010);


  emi_temp_data = mt_emi_sync_read(EMI_CHANNEL_APB_BASE+0x000001b0);
  emi_temp_data = emi_temp_data & ~(0x1);
  mt_emi_sync_write(EMI_CHANNEL_APB_BASE+0x000001b0, emi_temp_data);

  emi_temp_data = mt_emi_sync_read(EMI_CHANNEL_APB_BASE+0x00000000);
  emi_temp_data = emi_temp_data & 0x1;
  mt_emi_sync_write_or(EMI_CHANNEL_APB_BASE+0x000001b0, emi_temp_data);

  enable_infra_emi_broadcast(0);


  mt_emi_sync_write(INFRACFG_AO_MEM_BASE+0x100, 0xFFFFFFFF);
  mt_emi_sync_write(INFRACFG_AO_MEM_BASE+0x104, 0xFFFFFFFF);
  mt_emi_sync_write(INFRACFG_AO_MEM_BASE+0x108, 0xFFFFFFFF);
  mt_emi_sync_write(INFRACFG_AO_MEM_BASE+0x10C, 0xFFFFFFFF);
  mt_emi_sync_write(INFRACFG_AO_MEM_BASE+0x110, 0x06000003);
  mt_emi_sync_write(INFRACFG_AO_MEM_BASE+0x114, 0x40000000);
  mt_emi_sync_write(INFRACFG_AO_MEM_BASE+0x118, 0xC0201800);
  mt_emi_sync_write(INFRACFG_AO_MEM_BASE+0x11C, 0x00000000);
  mt_emi_sync_write(INFRACFG_AO_MEM_BASE+0x120, 0x180003FC);
  mt_emi_sync_write(INFRACFG_AO_MEM_BASE+0x124, 0x80000000);
  mt_emi_sync_write(INFRACFG_AO_MEM_BASE+0x128, 0x06000001);
  mt_emi_sync_write(INFRACFG_AO_MEM_BASE+0x12C, 0x00000600);
  mt_emi_sync_write(INFRACFG_AO_MEM_BASE+0x130, 0x20003C00);
  mt_emi_sync_write(INFRACFG_AO_MEM_BASE+0x134, 0x00000000);
  mt_emi_sync_write(INFRACFG_AO_MEM_BASE+0x138, 0x00080000);
  mt_emi_sync_write(INFRACFG_AO_MEM_BASE+0x13C, 0x00000000);
  //mt_emi_sync_write(INFRACFG_AO_MEM_BASE+0x140, 0x60003C00);
  //mt_emi_sync_write(INFRACFG_AO_MEM_BASE+0x144, 0x10000006);
  //mt_emi_sync_write(INFRACFG_AO_MEM_BASE+0x148, 0x00090000);
  //mt_emi_sync_write(INFRACFG_AO_MEM_BASE+0x14C, 0x02100800);
  mt_emi_sync_write(INFRACFG_AO_MEM_BASE+0x150, 0x80FF8000);
  mt_emi_sync_write(INFRACFG_AO_MEM_BASE+0x154, 0x00000001);
  mt_emi_sync_write(INFRACFG_AO_MEM_BASE+0x158, 0x3000E018);
  mt_emi_sync_write(INFRACFG_AO_MEM_BASE+0x15C, 0x0106019F);
  //mt_emi_sync_write(INFRACFG_AO_MEM_BASE+0x160, 0x00000000);
  //mt_emi_sync_write(INFRACFG_AO_MEM_BASE+0x164, 0x011FFFF8);
  //mt_emi_sync_write(INFRACFG_AO_MEM_BASE+0x168, 0x084007E0);
  //mt_emi_sync_write(INFRACFG_AO_MEM_BASE+0x16C, 0x00600006);
  //mt_emi_sync_write(INFRACFG_AO_MEM_BASE+0x170, 0x00000000);
  //mt_emi_sync_write(INFRACFG_AO_MEM_BASE+0x174, 0x00000000);
  //mt_emi_sync_write(INFRACFG_AO_MEM_BASE+0x178, 0x000001A0);
  //mt_emi_sync_write(INFRACFG_AO_MEM_BASE+0x17C, 0x1C800000);

  mt_emi_sync_write(SUB_INFRACFG_AO_MEM_BASE+0x020, 0xFFFFFFFF);
  mt_emi_sync_write(SUB_INFRACFG_AO_MEM_BASE+0x024, 0xFFFFFFFF);
  mt_emi_sync_write(SUB_INFRACFG_AO_MEM_BASE+0x210, 0x00000000);
  mt_emi_sync_write(SUB_INFRACFG_AO_MEM_BASE+0x214, 0x06007FE0);
  mt_emi_sync_write(SUB_INFRACFG_AO_MEM_BASE+0x218, 0x0000003F);
  mt_emi_sync_write(SUB_INFRACFG_AO_MEM_BASE+0x21C, 0x0003000C);
  mt_emi_sync_write(SUB_INFRACFG_AO_MEM_BASE+0x220, 0x00000380);
  mt_emi_sync_write(SUB_INFRACFG_AO_MEM_BASE+0x224, 0x00000000);
  mt_emi_sync_write(SUB_INFRACFG_AO_MEM_BASE+0x228, 0x00C08000);
  mt_emi_sync_write(SUB_INFRACFG_AO_MEM_BASE+0x22C, 0x01000000);
  mt_emi_sync_write(SUB_INFRACFG_AO_MEM_BASE+0x230, 0xCF3F3000);
  mt_emi_sync_write(SUB_INFRACFG_AO_MEM_BASE+0x234, 0x30008013);
  mt_emi_sync_write(SUB_INFRACFG_AO_MEM_BASE+0x238, 0x00000000);
  mt_emi_sync_write(SUB_INFRACFG_AO_MEM_BASE+0x23C, 0x00000000);

  mt_emi_sync_write(INFRACFG_AO_MEM_BASE+0x028, 0x0018002F);
  mt_emi_sync_write(INFRACFG_AO_MEM_BASE+0x02C, 0x40000000);
  mt_emi_sync_write(INFRACFG_AO_MEM_BASE+0x030, 0x00000044);
  mt_emi_sync_write(INFRACFG_AO_MEM_BASE+0x034, 0x00000000);

  mt_emi_sync_write(SUB_INFRACFG_AO_MEM_BASE+0x028, 0x000D007F);
  mt_emi_sync_write(SUB_INFRACFG_AO_MEM_BASE+0x02C, 0x40000000);
  mt_emi_sync_write(SUB_INFRACFG_AO_MEM_BASE+0x030, 0x00000044);
  mt_emi_sync_write(SUB_INFRACFG_AO_MEM_BASE+0x034, 0x00000000);

  mt_emi_sync_write(INFRACFG_AO_MEM_BASE+0x038, 0xA00001FF);

  mt_emi_sync_write_or(INFRACFG_AO_BASE+0x00000078, 0x08000000);

  #ifdef EMI_MP_SETTING

  mt_emi_sync_write_or(EMI_APB_BASE+0x00000068,0x00400000);


  emi_temp_data = mt_emi_sync_read(0x40000000);
  mt_emi_sync_write(0x40000000, emi_temp_data);
  emi_temp_data = mt_emi_sync_read(0x40000100);
  mt_emi_sync_write(0x40000100, emi_temp_data);
  emi_temp_data = mt_emi_sync_read(0x40000200);
  mt_emi_sync_write(0x40000200, emi_temp_data);
  emi_temp_data = mt_emi_sync_read(0x40000300);
  mt_emi_sync_write(0x40000300, emi_temp_data);

  mt_emi_sync_write_or(EMI_CHANNEL_APB_BASE+0x00000050,0x00000004);


  mt_emi_sync_write(INFRACFG_AO_MEM_BASE+0x00000050, 0x00000021);
  mt_emi_sync_write(INFRACFG_AO_MEM_BASE+0x00000050, 0x80000021);


  emi_temp_data = mt_emi_sync_read(INFRACFG_AO_MEM_BASE+0x050);
  emi_temp_data = emi_temp_data & 0xf;
  mt_emi_sync_write_or(EMI_BASE+0x07A4, emi_temp_data);


  mt_emi_sync_write_or(EMI_APB_BASE+0x00000068,0x00200000);
  mt_emi_sync_write_or(EMI_CHANNEL_APB_BASE+0x00000050,0x00000010);
  #else
  // MP_dsim_v02 test (from v01) - all fr
  //mt_emi_sync_write(INFRACFG_AO_MEM_BASE+0x028, 0x003F0000);
  //mt_emi_sync_write(INFRACFG_AO_MEM_BASE+0x02C, 0xA0000000);
  #endif

  #endif



  if (channel_num_auxadc == CHANNEL_FOURTH)
  	{
      mt_emi_sync_write(INFRACFG_AO_MEM_BASE+0x00000050, 0x00000021);
      mt_emi_sync_write(INFRACFG_AO_MEM_BASE+0x00000050, 0x80000021);
  	}
  else
  	{
      mt_emi_sync_write(INFRACFG_AO_MEM_BASE+0x00000050, 0x00000007);
      mt_emi_sync_write(INFRACFG_AO_MEM_BASE+0x00000050, 0x80000007);
  	}



  emi_temp_data = mt_emi_sync_read(INFRACFG_AO_MEM_BASE+0x050);
  emi_temp_data = emi_temp_data & 0xf;

  enable_infra_emi_broadcast(1);
  mt_emi_sync_write_or(EMI_BASE+0x07A4, emi_temp_data);

  mt_emi_sync_write(CHN0_EMI_BASE+0x0020, 0x00000040);
  enable_infra_emi_broadcast(0);

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

#ifdef SUB_EMI_BASE
	channel_nr *= 2;
#endif
#if DRAM_AUXADC_CONFIG
	U32 ret = 0, voltage = 0;
	ret = iio_read_channel_processed(5, &voltage);
	if (ret == 0)
		if (voltage < 700)
			channel_nr = CHANNEL_FOURTH;
		else
			channel_nr = CHANNEL_DUAL;
	else
		emi_log("Error! Read AUXADC value fail\n");
#endif

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

	quad_ch_ratio = (get_channel_nr_by_emi() == 4)? 2 : 1;

	ch0_rank0_size = (ch0_rank0_size * quad_ch_ratio) << 28;
	ch0_rank1_size = (ch0_rank1_size * quad_ch_ratio) << 28;
	ch1_rank0_size = (ch1_rank0_size * quad_ch_ratio) << 28;
	ch1_rank1_size = (ch1_rank1_size * quad_ch_ratio) << 28;

	if(ch0_rank0_size == 0) {
		emi_log("[EMI] undefined CONH for CH0 RANK0\n");
		ASSERT(0);
	}
	dram_rank_size[0] += ch0_rank0_size;

	if (get_rank_nr_by_emi() > 1) {
		if(ch0_rank1_size == 0) {
			emi_log("[EMI] undefined CONH for CH0 RANK1\n");
			ASSERT(0);
		}
		dram_rank_size[1] += ch0_rank1_size;
	}

	if(get_channel_nr_by_emi() > 1) {
		if(ch1_rank0_size == 0) {
			emi_log("[EMI] undefined CONH for CH1 RANK0\n");
			ASSERT(0);
		}
		dram_rank_size[0] += ch1_rank0_size;

		if (get_rank_nr_by_emi() > 1) {
			if(ch1_rank1_size == 0) {
				emi_log("[EMI] undefined CONH for CH1 RANK1\n");
				ASSERT(0);
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

void enable_infra_emi_broadcast(unsigned int enable)
{
	if (enable)
		mt_emi_sync_write(INFRA_DRAMC_REG_CONFIG, 0x00027f7f);
	else
		mt_emi_sync_write(INFRA_DRAMC_REG_CONFIG, 0x00000000);
}

unsigned int get_cen_emi_cona(void)
{
	return mt_emi_sync_read(EMI_CONA);
}


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


unsigned int set_emi_before_rank1_mem_test(void)
{
	cen_emi_conh_backup = mt_emi_sync_read(EMI_CONH);
	chn_emi_cona_backup = get_chn_emi_cona();

	enable_infra_emi_broadcast(1);
	if (get_rank_nr_by_emi() == 2) {

		mt_emi_sync_write(EMI_CONH,
			(cen_emi_conh_backup & 0x0000ffff) | 0x22220000);
		set_chn_emi_cona(
			(chn_emi_cona_backup & 0xff00ffff) | 0x00220000);
	} else {

		mt_emi_sync_write(EMI_CONH,
			(cen_emi_conh_backup & 0x0000ffff) | 0x44440000);
		set_chn_emi_cona(
			(chn_emi_cona_backup & 0xff00ffff) | 0x00440000);
	}
	enable_infra_emi_broadcast(0);

	return 0x40000000;
}

void restore_emi_after_rank1_mem_test(void)
{
	enable_infra_emi_broadcast(1);
	mt_emi_sync_write(EMI_CONH, cen_emi_conh_backup);
	set_chn_emi_cona(chn_emi_cona_backup);
	enable_infra_emi_broadcast(0);
}

void get_emi_isu_info(struct isu_info_t *isu_info_ptr)
{
#ifdef LAST_EMI
	last_emi_info_ptr = (LAST_EMI_INFO_T *) get_dbg_info_base(KEY_LAST_EMI);
	isu_info_ptr->buf_size = EMI_ISU_BUF_SIZE;
	isu_info_ptr->buf_addr = (unsigned long long)last_emi_info_ptr->isu_buf_h;
	isu_info_ptr->buf_addr <<= 32;
	isu_info_ptr->buf_addr += (unsigned long long)last_emi_info_ptr->isu_buf_l;
	isu_info_ptr->ver_addr = (unsigned long long)(&(last_emi_info_ptr->isu_version));
	isu_info_ptr->con_addr = (unsigned long long)(&(last_emi_info_ptr->isu_ctrl));
#endif
}

void clr_emi_mpu_prot(void)
{
}

void dis_emi_apb_prot(void)
{
	mt_emi_sync_write(EMI_MPU_CTRL, 0xFFFFFFFE);

	emi_log("[EMI] EMI_MPU_CTRL 0x%x\n", mt_emi_sync_read(EMI_MPU_CTRL));
}

int update_emi_setting(EMI_SETTINGS *default_emi_setting, EMI_INFO_T *emi_info)
{
	unsigned int ddr_type;
	unsigned int cen_cona_val, cen_conf_val, cen_conh_val, cen_conk_val;
	unsigned int chn_cona_val;
	unsigned int cen_conf_shf;
	unsigned int col, row, row_ext, rk_size, rk_size_ext, rk_size_chn_ext;
	unsigned int temp_val;
	int i;

	ddr_type = default_emi_setting->type & 0xFF;
	cen_conh_val = 0x00000003;
	cen_conk_val = mt_emi_sync_read(EMI_CONK) & 0x0000FFFF;
	if (u1IsLP4Family(ddr_type)) {
		cen_cona_val = 0x00000104;
		chn_cona_val = 0x04000000;
	} else {
		cen_cona_val = 0x00000006;
		chn_cona_val = 0x00000002;
	}

	for (i = 0; i < emi_info->rk_num; i++) {
		row = emi_info->row_width[i] - 13;
		row_ext = row >> 2;
		row &= 0x3;

		col = emi_info->col_width[i] - 9;

		rk_size = emi_info->rank_size[i] >> 28;
		if (u1IsLP4Family(ddr_type))
			rk_size >>= 1;
		rk_size_ext = rk_size >> 4;
		rk_size_chn_ext = rk_size_ext & 0x1;
		rk_size &= 0xF;

		if (i == 1) {
			col <<= 2;
			row <<= 2;
			row_ext <<= 1;
			rk_size <<= 4;
			rk_size_ext <<= 4;
			rk_size_chn_ext <<= 1;
			cen_cona_val |= 0x00030000;
			chn_cona_val |= 0x00000001;
		}

		cen_cona_val |=
			(col << 4) | (row << 12) | (col << 20) |
			(row_ext << 24) | (row << 28);
		cen_conh_val |=
			(row_ext << 4) | (rk_size << 16) | (rk_size << 24);
		cen_conk_val |=
			(rk_size_ext << 16) | (rk_size_ext << 24);
		chn_cona_val |=
			(row_ext << 2) | (col << 4) | (row << 12) |
			(rk_size << 16) | (rk_size_chn_ext << 8);
	}

	cen_conf_shf = 0;
	for (i = 0; i < emi_info->rk_num; i++) {
		temp_val = 2 + emi_info->col_width[i];
		if (cen_conf_shf < temp_val)
			cen_conf_shf = temp_val;
	}

	if (((cen_conf_shf - 9) * 4) < 21)
		cen_conf_val = 0x421 << ((cen_conf_shf - 9) * 4);
	else
		return -1;

	emi_log("[EMI] %s(0x%x),%s(0x%x),%s(0x%x),%s(0x%x),%s(0x%x)\n",
			"CEN_CONA", cen_cona_val,
			"CEN_CONF", cen_conf_val,
			"CEN_CONH", cen_conh_val,
			"CEN_CONK", cen_conk_val,
			"CHN_CONA", chn_cona_val);

	default_emi_setting->EMI_CONA_VAL = cen_cona_val;
	default_emi_setting->EMI_CONF_VAL = cen_conf_val;
	default_emi_setting->EMI_CONH_VAL = cen_conh_val;
	default_emi_setting->EMI_CONK_VAL = cen_conk_val;
	default_emi_setting->CHN0_EMI_CONA_VAL = chn_cona_val;
	default_emi_setting->CHN1_EMI_CONA_VAL = chn_cona_val;

	return 0;
}

#if !__ETT__
void record_emi_snst(void)
{
#ifdef LAST_EMI
	if (!last_emi_info_ptr)
		last_emi_info_ptr =
			(LAST_EMI_INFO_T *) get_dbg_info_base(KEY_LAST_EMI);

	last_emi_info_ptr->snst_past = last_emi_info_ptr->snst_last;
	last_emi_info_ptr->snst_last = mt_emi_sync_read(EMI_SNST);
	emi_log("[EMI] SNST: 0x%x\n", last_emi_info_ptr->snst_last);


	mt_emi_sync_write(EMI_SNST, 0x85000000);
#endif
}

unsigned long long platform_memory_size(void)
{
	static unsigned long long mem_size = 0;
	int nr_rank;
	int i;
	unsigned long long rank_size[DRAMC_MAX_RK] = {0};

	if (!mem_size) {
		nr_rank = get_dram_rank_nr();

		get_dram_rank_size(rank_size);

		for (i = 0; i < nr_rank; i++)
			mem_size += rank_size[i];
	}

	return mem_size;
}
#endif

