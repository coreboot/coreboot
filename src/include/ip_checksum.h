#ifndef IP_CHECKSUM_H
#define IP_CHECKSUM_H

unsigned long compute_ip_checksum(void *addr, unsigned long length);

#endif /* IP_CHECKSUM_H */
