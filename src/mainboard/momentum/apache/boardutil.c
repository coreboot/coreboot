/*
 * Copyright (C) 2003, Greg Watson <gwatson@lanl.gov>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

/*
 * Do very early board initialization:
 *
 * - Configure External Bus (EBC)
 * - Setup Flash
 * - Setup NVRTC
 * - Setup Board Control and Status Registers (BCSR)
 * - Enable UART0 for debugging
 */

#include "boardutil.h"
#include "ppc970lib.h"
#include "ppc970.h"
#include "stddef.h"
#include "string.h"

/*----------------------------------------------------------------------------+
| What_platform.
+----------------------------------------------------------------------------*/
int what_platform()
{

   #ifdef PPC970FX_EVB_LITE
      return(PLATFORM_EVB_LITE);
   #endif
   #ifdef PPC970FX_EVB
      return(PLATFORM_EVB_FINAL);
   #endif
}

/*----------------------------------------------------------------------------+
| Get_system_info.  Cannot access any global variables in this function.
+----------------------------------------------------------------------------*/
void get_system_info(board_cfg_data_t *board_cfg)
{

   unsigned long	msr;
   unsigned long	data;

   if (board_cfg==NULL) {
      (void)ppcHalt();
   }
   msr=ppcAndMsr((unsigned long)~MSR_EE);
   board_cfg->usr_config_ver[0]='1';
   board_cfg->usr_config_ver[1]='.';
   board_cfg->usr_config_ver[2]='0';
   board_cfg->usr_config_ver[3]='\0';
   /*-------------------------------------------------------------------------+
   | Read power status register.
   +-------------------------------------------------------------------------*/
   data=read_psr()&SCOM_PSR_FREQ_MASK;
   if (data==SCOM_PSR_FREQ_FULL) {
      board_cfg->freq_ratio=1;
   } else if (data==SCOM_PSR_FREQ_HALF) {
      board_cfg->freq_ratio=2;
   } else if (data==SCOM_PSR_FREQ_QUARTER) {
      board_cfg->freq_ratio=4;
   } else {
      board_cfg->freq_ratio=0;
   }
   /*-------------------------------------------------------------------------+
   | Read information passed from service processor.
   +-------------------------------------------------------------------------*/
   if (get_ei_ratio(&data)==0) {
      board_cfg->ei_ratio=data;
   } else {
      board_cfg->ei_ratio=0;
   }
   if (get_sys_clk(&data)==0) {
      board_cfg->sys_freq=data;
   } else {
      board_cfg->sys_freq=0;
   }
   if (get_pll_mult(&data)==0) {
      if (board_cfg->freq_ratio!=0) {
         board_cfg->cpu_freq=(board_cfg->sys_freq* data)/ board_cfg->freq_ratio;
      } else {
         board_cfg->cpu_freq=0;
      }
   } else {
      data=0;
      board_cfg->cpu_freq=0;
   }
   /*-------------------------------------------------------------------------+
   | On some boards we have to execute with timers running on internal clock.
   +-------------------------------------------------------------------------*/
   if ((ppcMfspr_any(SPR_HID0)&HID0_EXT_TB_EN)==0) {
      board_cfg->tmr_freq=(board_cfg->sys_freq* data)/ PPC970_TB_RATIO;
   } else {
      board_cfg->tmr_freq=EXT_TIME_BASE_FREQ;
   }
   /*-------------------------------------------------------------------------+
   | If the above calculation did not yield valid timer speed try to estimate
   | it.
   +-------------------------------------------------------------------------*/
   if (board_cfg->tmr_freq==0) {
      board_cfg->tmr_freq=timebase_speed_calc(UART1_MMIO_BASE);
   }
   /*-------------------------------------------------------------------------+
   | Read information passed from service processor.
   +-------------------------------------------------------------------------*/
   board_cfg->mem_size=sdram_size();
   /*-------------------------------------------------------------------------+
   | Assign rest of the information.
   +-------------------------------------------------------------------------*/
   board_cfg->ser_freq=UART_INPUT_CLOCK;
   board_cfg->procver=ppcMfspr_any(SPR_PVR);
   board_cfg->hid0=ppcMfspr_any(SPR_HID0);
   board_cfg->hid1=ppcMfspr_any(SPR_HID1);
   board_cfg->hid4=ppcMfspr_any(SPR_HID4);
   board_cfg->hid5=ppcMfspr_any(SPR_HID5);
   board_cfg->hior=ppcMfspr_any(SPR_HIOR);
   board_cfg->sdr1=ppcMfspr_any(SPR_SDR1);
   board_cfg->procstr[0]='9';
   board_cfg->procstr[1]='7';
   board_cfg->procstr[2]='0';
   board_cfg->procstr[3]='F';
   board_cfg->procstr[4]='X';
   board_cfg->procstr[5]='\0';
   board_cfg->reserved[0]='\0';
   (void)get_hwd_addr((char *)board_cfg->hwaddr0, 0);
   (void)ppcMtmsr(msr);
   return;
}

/*----------------------------------------------------------------------------+
| Get_hwd_addr.
+----------------------------------------------------------------------------*/
int get_hwd_addr(char *dest,
    int ethernet_num)
{

   bios_data_struct_t   *bios_data;
   char                 *src;
   unsigned char        nc;
   int                  len;
   int                  num;

   bios_data=(bios_data_struct_t *)PIBS_DATABASE_ADDR;
   if (ethernet_num!=0) {
      for(len=0;len<ETHERNET_HW_ADDR_LEN;len++) {
         dest[len]=(char)0xFF;
      }
      return(-1);
   } else {
      src=bios_data->bios_eth_hwd0;
   }
   len=0;
   while((src[len]!='\0') && (len<(ETHERNET_HW_ADDR_LEN* 3))) {
      len++;
   }
   if (len!=(ETHERNET_HW_ADDR_LEN* 2)) {
      for(len=0;len<ETHERNET_HW_ADDR_LEN;len++) {
         dest[len]=(char)0xFF;
      }
      return(-1);
   }
   for(len=0;len<(ETHERNET_HW_ADDR_LEN* 2);len++) {
      nc=toupper((int)src[len]);
      if ((nc>='0') && (nc<='9')) {
         num=nc- '0';
      } else if ((nc>='A') && (nc<='F')) {
         num=nc- 'A'+ 0xA;
      } else {
         for(len=0;len<ETHERNET_HW_ADDR_LEN;len++) {
            dest[len]=(char)0xFF;
         }
         return(-1);
      }
      if ((len%2)==0) {
         dest[len/ 2]=(char)num;
      } else {
         dest[len/ 2]=(char)((dest[len/ 2]* 0x10)+ num);
      }
   }
   return(0);
}

/*----------------------------------------------------------------------------+
| Get_sys_clk.
+----------------------------------------------------------------------------*/
int get_sys_clk(unsigned long *value)
{

   unsigned long	data;

   if (read_sp_data(SUPER_IO_NVRAM_DATA_VALID, 4, &data)!=0) {
      return(-1);
   }
   if (data!=SUPER_IO_VALID_VALUE) {
      return(-1);
   }
   if (read_sp_data(SUPER_IO_NVRAM_SYS_CLK, 4, &data)!=0) {
      return(-1);
   }
   *value=data;
   return(0);
}

/*----------------------------------------------------------------------------+
| Get_pll_mult.
+----------------------------------------------------------------------------*/
int get_pll_mult(unsigned long *value)
{

   unsigned long	data;

   if (read_sp_data(SUPER_IO_NVRAM_DATA_VALID, 4, &data)!=0) {
      return(-1);
   }
   if (data!=SUPER_IO_VALID_VALUE) {
      return(-1);
   }
   if (read_sp_data(SUPER_IO_NVRAM_CLK_MULT, 1, value)!=0) {
      return(-1);
   }
   return(0);
}

/*----------------------------------------------------------------------------+
| Get_ei_ratio.
+----------------------------------------------------------------------------*/
int get_ei_ratio(unsigned long *value)
{

   unsigned long	data;

   if (read_sp_data(SUPER_IO_NVRAM_DATA_VALID, 4, &data)!=0) {
      return(-1);
   }
   if (data!=SUPER_IO_VALID_VALUE) {
      return(-1);
   }
   if (read_sp_data(SUPER_IO_NVRAM_EI_RATIO, 1, &data)!=0) {
      return(-1);
   }

   if (data==0x0000000000000000) data=PPC970_EI_RATIO_000;
   else if (data==0x0000000000000001) data=PPC970_EI_RATIO_001;
   else if (data==0x0000000000000002) data=PPC970_EI_RATIO_010;
   else if (data==0x0000000000000003) data=PPC970_EI_RATIO_011;
   else if (data==0x0000000000000004) data=PPC970_EI_RATIO_100;
   else if (data==0x0000000000000005) data=PPC970_EI_RATIO_101;
   else if (data==0x0000000000000006) data=PPC970_EI_RATIO_110;
   else return(-1);

   *value=data;
   return(0);
}

/*----------------------------------------------------------------------------+
| Read_sp_data.
+----------------------------------------------------------------------------*/
int read_sp_data(unsigned int offset,
    unsigned int count, unsigned long *data)
{

   unsigned long	addr_index;
   unsigned long	addr_data;
   unsigned long	addr;
   unsigned int		new_data;
   unsigned int		i;

   /*-------------------------------------------------------------------------+
   | If this is not a JS20 or EVB platform then just return.
   +-------------------------------------------------------------------------*/
   if (what_platform()==PLATFORM_EVB_FINAL) {
      addr_index=NB_HT_IO_BASE_CPU+ SUPER_IO_ADDR_NVRAM;
      addr_data=NB_HT_IO_BASE_CPU+ SUPER_IO_ADDR_NVRAM+ 1;
      *data=0x0000000000000000;
      for(i=0;i<count;i++) {
         (void)outbyte(addr_index, offset+ i);
         new_data=inbyte(addr_data);
         *data|=new_data<<((count- i- 1)* 8);
      }
      return(0);
   } else if (what_platform()==PLATFORM_EVB_LITE) {
      addr=SB_NVRAM_ADDR;
      *data=0x0000000000000000;
      for(i=0;i<count;i++) {
         new_data=inbyte(addr+ i+ offset);
         *data|=new_data<<((count- i- 1)* 8);
      }
      return(0);
   }
   return(-1);
}

/*----------------------------------------------------------------------------+
| Write_sp_data.
+----------------------------------------------------------------------------*/
int write_sp_data(unsigned int offset,
    unsigned int data)
{

   unsigned long	addr_index;
   unsigned long	addr_data;
   unsigned long	addr;

   /*-------------------------------------------------------------------------+
   | If this is not a JS20 or EVB platform then just return.
   +-------------------------------------------------------------------------*/
   if (what_platform()==PLATFORM_EVB_FINAL) {
      addr_index=NB_HT_IO_BASE_CPU+ SUPER_IO_ADDR_NVRAM;
      addr_data=NB_HT_IO_BASE_CPU+ SUPER_IO_ADDR_NVRAM+ 1;
      (void)outbyte(addr_index, offset);
      (void)outbyte(addr_data, data);
      return(0);
   } else if (what_platform()==PLATFORM_EVB_LITE) {
      addr=SB_NVRAM_ADDR;
      (void)outbyte(addr+ offset, data);
      return(0);
   }
   return(-1);
}

/*----------------------------------------------------------------------------+
| Read_psr.
+----------------------------------------------------------------------------*/
unsigned long read_psr()
{

   unsigned long	msr;
   unsigned long	value;

   msr=ppcAndMsr((unsigned long)~MSR_EE);
   (void)ppcMtspr_any(SPR_SCOMC, SCOM_ADDR_PSR_READ);
   (void)ppcIsync();
   value=ppcMfspr_any(SPR_SCOMD);
   (void)ppcIsync();
   (void)ppcMtmsr(msr);
   return(value);
}

/*----------------------------------------------------------------------------+
| Write_pcr_pcrh.
+----------------------------------------------------------------------------*/
void write_pcr_pcrh(unsigned long data)
{

   unsigned long	msr;

   msr=ppcAndMsr((unsigned long)~MSR_EE);
   /*-------------------------------------------------------------------------+
   | First write to PCR with all 0 (errata).
   +-------------------------------------------------------------------------*/
   (void)ppcMtspr_any(SPR_SCOMD, SCOM_ADDR_PCR_DATA_MASK);
   (void)ppcIsync();
   (void)ppcMtspr_any(SPR_SCOMC, SCOM_ADDR_PCR_WRITE);
   (void)ppcIsync();
   /*-------------------------------------------------------------------------+
   | Write to PCRH.
   +-------------------------------------------------------------------------*/
   (void)ppcMtspr_any(SPR_SCOMD, 0x0000000000000000UL);
   (void)ppcIsync();
   (void)ppcMtspr_any(SPR_SCOMC, SCOM_ADDR_PCR_WRITE);
   (void)ppcIsync();
   /*-------------------------------------------------------------------------+
   | Write to PCR.
   +-------------------------------------------------------------------------*/
   (void)ppcMtspr_any(SPR_SCOMD, data|SCOM_ADDR_PCR_DATA_MASK);
   (void)ppcIsync();
   (void)ppcMtspr_any(SPR_SCOMC, SCOM_ADDR_PCR_WRITE);
   (void)ppcIsync();
   (void)ppcMtmsr(msr);
   return;
}

/*----------------------------------------------------------------------------+
| Is_writable.
+----------------------------------------------------------------------------*/
int is_writable(unsigned long addr,
    unsigned long len)
{

   if ((addr>=BOOT_BASE) && (addr<SDRAM_UPPER_BASE)) {
      return(0);
   }
   if ((addr+ len)>=BOOT_BASE) {
      return(0);
   }
   return(1);
}

