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
 * - Setup SIO
 */
#include "ppc970.h"
#include "boardutil.h"

/*----------------------------------------------------------------------------+
| Local defines.
+----------------------------------------------------------------------------*/
#define BASE_MASK			0xFFFFFFFF

void
board_init(void)
{
  super_io_setup();
}


/*----------------------------------------------------------------------------+
| Super_io_setup.
+----------------------------------------------------------------------------*/
void super_io_setup()
{

   unsigned long	io_base;
   unsigned long	sio_index;
   unsigned long	sio_data;
   int			platform;
   unsigned int		i;

   /*-------------------------------------------------------------------------+
   | If this is not a JS20 or EVB platform then just return.
   +-------------------------------------------------------------------------*/
   platform=what_platform();
   if (platform==PLATFORM_EVB_FINAL) {
      /*----------------------------------------------------------------------+
      | Assign addresses.
      +----------------------------------------------------------------------*/
      io_base=(unsigned long)(NB_HT_IO_BASE_BYTE<<NB_HT_IO_BASE_BYTE_SH);
      io_base&=BASE_MASK;
      sio_index=io_base+ SUPER_IO_INDEX_OFF;
      sio_data=io_base+ SUPER_IO_DATA_OFF;
      /*----------------------------------------------------------------------+
      | Serial 1 setup/enable.
      +----------------------------------------------------------------------*/
      (void)outbyte(sio_index, SUPER_IO_DEVICE_SEL);
      (void)outbyte(sio_data, SUPER_IO_DEVICE_S1);
      (void)outbyte(sio_index, SUPER_IO_BASE_DEV_MSB);
      (void)outbyte(sio_data, (unsigned int)((UART0_MMIO_BASE>>8)&0xFF));
      (void)outbyte(sio_index, SUPER_IO_BASE_DEV_LSB);
      (void)outbyte(sio_data, (unsigned int)((UART0_MMIO_BASE>>0)&0xFF));
      (void)outbyte(sio_index, SUPER_IO_DEVICE_CTRL);
      (void)outbyte(sio_data, SUPER_IO_DEVICE_ENABLE);
      /*----------------------------------------------------------------------+
      | Serial 2 setup/enable.
      +----------------------------------------------------------------------*/
      (void)outbyte(sio_index, SUPER_IO_DEVICE_SEL);
      (void)outbyte(sio_data, SUPER_IO_DEVICE_S2);
      (void)outbyte(sio_index, SUPER_IO_BASE_DEV_MSB);
      (void)outbyte(sio_data, (unsigned int)((UART1_MMIO_BASE>>8)&0xFF));
      (void)outbyte(sio_index, SUPER_IO_BASE_DEV_LSB);
      (void)outbyte(sio_data, (unsigned int)((UART1_MMIO_BASE>>0)&0xFF));
      (void)outbyte(sio_index, SUPER_IO_DEVICE_CTRL);
      (void)outbyte(sio_data, SUPER_IO_DEVICE_ENABLE);
      /*----------------------------------------------------------------------+
      | X-bus setup/enable.
      +----------------------------------------------------------------------*/
      (void)outbyte(sio_index, SUPER_IO_DEVICE_SEL);
      (void)outbyte(sio_data, SUPER_IO_DEVICE_XBUS);
      (void)outbyte(sio_index, SUPER_IO_BASE_DEV_MSB);
      (void)outbyte(sio_data, (SUPER_IO_ADDR_XBUS>>8)&0xFF);
      (void)outbyte(sio_index, SUPER_IO_BASE_DEV_LSB);
      (void)outbyte(sio_data, (SUPER_IO_ADDR_XBUS>>0)&0xFF);
      (void)outbyte(sio_index, SUPER_IO_XBUS_CONFIG);
      (void)outbyte(sio_data, SUPER_IO_BIOS_SIZE_1M);
      (void)outbyte(sio_index, SUPER_IO_DEVICE_CTRL);
      (void)outbyte(sio_data, SUPER_IO_DEVICE_ENABLE);
      for(i=0;i<16;i++) {
         (void)outbyte(io_base+ SUPER_IO_XBUS_HOST_ACCESS, i);
      }
      /*----------------------------------------------------------------------+
      | RTC setup/enable.
      +----------------------------------------------------------------------*/
      (void)outbyte(sio_index, SUPER_IO_DEVICE_SEL);
      (void)outbyte(sio_data, SUPER_IO_DEVICE_RTC);
      (void)outbyte(sio_index, SUPER_IO_BASE_DEV_MSB);
      (void)outbyte(sio_data, (SUPER_IO_ADDR_RTC>>8)&0xFF);
      (void)outbyte(sio_index, SUPER_IO_BASE_DEV_LSB);
      (void)outbyte(sio_data, (SUPER_IO_ADDR_RTC>>0)&0xFF);
      (void)outbyte(sio_index, SUPER_IO_EXT_DEV_MSB);
      (void)outbyte(sio_data, (SUPER_IO_ADDR_NVRAM>>8)&0xFF);
      (void)outbyte(sio_index, SUPER_IO_EXT_DEV_LSB);
      (void)outbyte(sio_data, (SUPER_IO_ADDR_NVRAM>>0)&0xFF);
      (void)outbyte(sio_index, SUPER_IO_RTC_DATE_ALARM_OFF);
      (void)outbyte(sio_data, SUPER_IO_RTC_DATE_ALARM_LOC);
      (void)outbyte(sio_index, SUPER_IO_RTC_MONTH_ALARM_OFF);
      (void)outbyte(sio_data, SUPER_IO_RTC_MONTH_ALARM_LOC);
      (void)outbyte(sio_index, SUPER_IO_RTC_CENTURY_ALARM_OFF);
      (void)outbyte(sio_data, SUPER_IO_RTC_CENTURY_ALARM_LOC);
      (void)outbyte(sio_index, SUPER_IO_DEVICE_CTRL);
      (void)outbyte(sio_data, SUPER_IO_DEVICE_ENABLE);
   }
   return;
}

void
board_init2(void)
{
}
