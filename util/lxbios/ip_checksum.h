/*****************************************************************************\
 * ip_checksum.h
 * $Id: ip_checksum.h,v 1.1.1.1 2005/12/02 22:35:19 dsp_llnl Exp $
\*****************************************************************************/

#ifndef IP_CHECKSUM_H
#define IP_CHECKSUM_H

/* Note: The contents of this file were borrowed from the LinuxBIOS source
 *       code which may be obtained from http://www.linuxbios.org.
 *       Specifically, this code was obtained from LinuxBIOS version 1.0.0.8.
 */

unsigned long compute_ip_checksum(void *addr, unsigned long length);

#endif /* IP_CHECKSUM_H */
