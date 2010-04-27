/*****************************************************************************\
 * ip_checksum.h
\*****************************************************************************/

#ifndef IP_CHECKSUM_H
#define IP_CHECKSUM_H

/* Note: The contents of this file were borrowed from the coreboot source
 *       code which may be obtained from http://www.coreboot.org.
 *       Specifically, this code was obtained from coreboot (LinuxBIOS)
 *       version 1.0.0.8.
 */

unsigned long compute_ip_checksum(void *addr, unsigned long length);

#endif				/* IP_CHECKSUM_H */
