#ifndef IP_CHECKSUM_H
#define IP_CHECKSUM_H

#ifndef __ROMCC__
unsigned long compute_ip_checksum(void *addr, unsigned long length);
unsigned long add_ip_checksums(unsigned long offset, unsigned long sum, unsigned long new);
#endif
#endif /* IP_CHECKSUM_H */
