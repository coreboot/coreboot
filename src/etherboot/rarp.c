#include "etherboot.h"
#include "nic.h"

static unsigned long xid;

#define eth_poll() nic.poll(&nic)
#define eth_transmit(a,b,c,d) nic.transmit(&nic,a,b,c, (char *) d)

int decode_rfc1533(unsigned char *p, int block, int len, int eof)
{
	return(-1);	// don't support it yet, just define it!
}

#define NO_DHCP_SUPPORT
#define EMERGENCYDISKBOOT

#define getchar() ttys0_rx_char()

static const unsigned char broadcast[] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
static unsigned long     netmask;

struct arptable_t        arptable[MAX_ARP];

struct bootpd_t bootp_data;

/***************************************************************************
getdec, a simple sort of atoi
***************************************************************************/
int getdec(char **ptr)
{
	char *p = *ptr;
	int ret=0;
	if ((*p < '0') || (*p > '9')) return(-1);
	while ((*p >= '0') && (*p <= '9')) {
		ret = ret*10 + (*p - '0');
		p++;
	}
	*ptr = p;
	return(ret);
}

/**************************************************************************
STRCASECMP (not entirely correct, but this will do for our purposes)
**************************************************************************/
int strcasecmp(char *a, char *b)
{
	while (*a && *b && (*a & ~0x20) == (*b & ~0x20)) {a++; b++; }
	return((*a & ~0x20) - (*b & ~0x20));
}


/**************************************************************************
DEFAULT_NETMASK - Return default netmask for IP address
**************************************************************************/
static inline unsigned long default_netmask(void)
{
	int net = ntohl(arptable[ARP_CLIENT].ipaddr.s_addr) >> 24;
	if (net <= 127)
		return(htonl(0xff000000));
	else if (net < 192)
		return(htonl(0xffff0000));
	else
		return(htonl(0xffffff00));
}

/**************************************************************************
IPCHKSUM - Checksum IP Header
**************************************************************************/
static unsigned short ipchksum(unsigned short *ip, int len)
{
	unsigned long sum = 0;
	len >>= 1;
	while (len--) {
		sum += *(ip++);
		if (sum > 0xFFFF)
			sum -= 0xFFFF;
	}
	return((~sum) & 0x0000FFFF);
}

/**************************************************************************
RFC951_SLEEP - sleep for expotentially longer times
**************************************************************************/
void rfc951_sleep(int exp)
{
	static long seed = 0;
	long q;
	unsigned long tmo;

#ifdef BACKOFF_LIMIT
	if (exp > BACKOFF_LIMIT)
		exp = BACKOFF_LIMIT;
#endif
	if (!seed) /* Initialize linear congruential generator */
		seed = currticks() + *(long *)&arptable[ARP_CLIENT].node
		       + ((short *)arptable[ARP_CLIENT].node)[2];
	/* simplified version of the LCG given in Bruce Scheier's
	   "Applied Cryptography" */
	q = seed/53668;
	if ((seed = 40014*(seed-53668*q) - 12211*q) < 0) seed += 2147483563l;
	/* compute mask */
	for (tmo = 63; tmo <= 60*TICKS_PER_SEC && --exp > 0; tmo = 2*tmo+1);
	/* sleep */
	printk_info("<sleep>\n");
	for (tmo = (tmo&seed)+currticks(); currticks() < tmo; )
	return;
}

/**************************************************************************
AWAIT_REPLY - Wait until we get a response for our request
**************************************************************************/
int await_reply(int type, int ival, void *ptr, int timeout)
{
	unsigned long time;
	struct	iphdr *ip;
	struct	udphdr *udp;
	struct	arprequest *arpreply;
	struct	bootp_t *bootpreply;
	struct	rpc_t *rpc;
	unsigned short ptype;

	unsigned int protohdrlen = ETH_HLEN + sizeof(struct iphdr) +
				sizeof(struct udphdr);
	time = timeout + currticks();
	/* The timeout check is done below.  The timeout is only checked if
	 * there is no packet in the Rx queue.  This assumes that eth_poll()
	 * needs a negligible amount of time.  */
	for (;;) {
		if (eth_poll()) {	/* We have something! */
					/* Check for ARP - No IP hdr */
			if (nic.packetlen >= ETH_HLEN) {
				ptype = ((unsigned short) nic.packet[12]) << 8
					| ((unsigned short) nic.packet[13]);
			} else continue; /* what else could we do with it? */
			if ((nic.packetlen >= ETH_HLEN +
				sizeof(struct arprequest)) &&
			   (ptype == ARP) ) {
				unsigned long tmp;

				arpreply = (struct arprequest *)
					&nic.packet[ETH_HLEN];
				if ((arpreply->opcode == htons(ARP_REPLY)) &&
				   !memcmp(arpreply->sipaddr, ptr, sizeof(in_addr)) &&
				   (type == AWAIT_ARP)) {
					memcpy(arptable[ival].node, arpreply->shwaddr, ETH_ALEN);
					return(1);
				}
				memcpy(&tmp, arpreply->tipaddr, sizeof(in_addr));
				if ((arpreply->opcode == htons(ARP_REQUEST)) &&
					(tmp == arptable[ARP_CLIENT].ipaddr.s_addr)) {
					arpreply->opcode = htons(ARP_REPLY);
					memcpy(arpreply->tipaddr, arpreply->sipaddr, sizeof(in_addr));
					memcpy(arpreply->thwaddr, arpreply->shwaddr, ETH_ALEN);
					memcpy(arpreply->sipaddr, &arptable[ARP_CLIENT].ipaddr, sizeof(in_addr));
					memcpy(arpreply->shwaddr, arptable[ARP_CLIENT].node, ETH_ALEN);
					eth_transmit(arpreply->thwaddr, ARP,
						sizeof(struct  arprequest),
						(char *) arpreply);
#ifdef	MDEBUG
					memcpy(&tmp, arpreply->tipaddr, sizeof(in_addr));
					printf("Sent ARP reply to: %I\n",tmp);
#endif	/* MDEBUG */
				}
				continue;
			}

			if (type == AWAIT_QDRAIN) {
				continue;
			}

					/* Check for RARP - No IP hdr */
			if ((type == AWAIT_RARP) &&
			   (nic.packetlen >= ETH_HLEN +
				sizeof(struct arprequest)) &&
			   (ptype == RARP)) {
				arpreply = (struct arprequest *)
					&nic.packet[ETH_HLEN];
				if ((arpreply->opcode == htons(RARP_REPLY)) &&
				   !memcmp(arpreply->thwaddr, ptr, ETH_ALEN)) {
					memcpy(arptable[ARP_SERVER].node, arpreply->shwaddr, ETH_ALEN);
					memcpy(& arptable[ARP_SERVER].ipaddr, arpreply->sipaddr, sizeof(in_addr));
					memcpy(& arptable[ARP_CLIENT].ipaddr, arpreply->tipaddr, sizeof(in_addr));
					return(1);
				}
				continue;
			}

					/* Anything else has IP header */
			if ((nic.packetlen < protohdrlen) ||
			   (ptype != IP) ) continue;
			ip = (struct iphdr *)&nic.packet[ETH_HLEN];
			if ((ip->verhdrlen != 0x45) ||
				ipchksum((unsigned short *)ip, sizeof(struct iphdr)) ||
				(ip->protocol != IP_UDP)) continue;
			udp = (struct udphdr *)&nic.packet[ETH_HLEN +
				sizeof(struct iphdr)];

					/* BOOTP ? */
			bootpreply = (struct bootp_t *)&nic.packet[ETH_HLEN + sizeof(struct iphdr) + sizeof(struct udphdr)];
			if ((type == AWAIT_BOOTP) &&
			   (nic.packetlen >= (ETH_HLEN + sizeof(struct iphdr) + sizeof(struct udphdr) +
#ifdef	NO_DHCP_SUPPORT
			     sizeof(struct bootp_t))) &&
#else
			     sizeof(struct bootp_t))-DHCP_OPT_LEN) &&
#endif	/* NO_DHCP_SUPPORT */
			   (udp->dest == htons(BOOTP_CLIENT)) &&
			   (bootpreply->bp_op == BOOTP_REPLY) &&
			   (bootpreply->bp_xid == xid) &&
			   (memcmp(broadcast, bootpreply->bp_hwaddr, ETH_ALEN) ==0 ||
			    memcmp(arptable[ARP_CLIENT].node, bootpreply->bp_hwaddr, ETH_ALEN) == 0)) {
				arptable[ARP_CLIENT].ipaddr.s_addr =
					bootpreply->bp_yiaddr.s_addr;
#ifndef	NO_DHCP_SUPPORT
				dhcp_addr.s_addr = bootpreply->bp_yiaddr.s_addr;
#endif	/* NO_DHCP_SUPPORT */
				netmask = default_netmask();
				arptable[ARP_SERVER].ipaddr.s_addr =
					bootpreply->bp_siaddr.s_addr;
				memset(arptable[ARP_SERVER].node, 0, ETH_ALEN);  /* Kill arp */
				arptable[ARP_GATEWAY].ipaddr.s_addr =
					bootpreply->bp_giaddr.s_addr;
				memset(arptable[ARP_GATEWAY].node, 0, ETH_ALEN);  /* Kill arp */
				/* bootpreply->bp_file will be copied to KERNEL_BUF in the memcpy */
				memcpy((char *)BOOTP_DATA_ADDR, (char *)bootpreply, sizeof(struct bootpd_t));
				decode_rfc1533(BOOTP_DATA_ADDR->bootp_reply.bp_vend,
#ifdef	NO_DHCP_SUPPORT
				       0, BOOTP_VENDOR_LEN + MAX_BOOTP_EXTLEN, 1);
#else
				       0, DHCP_OPT_LEN + MAX_BOOTP_EXTLEN, 1);
#endif	/* NO_DHCP_SUPPORT */
#ifdef	REQUIRE_VCI_ETHERBOOT
				if (!vci_etherboot)
					return (0);
#endif

				return(1);
			}

#ifdef	DOWNLOAD_PROTO_TFTP
					/* TFTP ? */
			if ((type == AWAIT_TFTP) &&
				(ntohs(udp->dest) == ival)) return(1);
#endif	/* DOWNLOAD_PROTO_TFTP */

#ifdef	DOWNLOAD_PROTO_NFS
					/* RPC ? */
			rpc = (struct rpc_t *)&nic.packet[ETH_HLEN];
			if ((type == AWAIT_RPC) &&
			    (ntohs(udp->dest) == ival) &&
			    (*(unsigned long *)ptr == ntohl(rpc->u.reply.id)) &&
			    (ntohl(rpc->u.reply.type) == MSG_REPLY)) {
				return (1);
			}
#endif	/* DOWNLOAD_PROTO_NFS */

		} else {
			/* Check for abort key only if the Rx queue is empty -
			 * as long as we have something to process, don't
			 * assume that something failed.  It is unlikely that
			 * we have no processing time left between packets.  */
			if (iskey() && (getchar() == ESC))
#ifdef	EMERGENCYDISKBOOT
				return(-1);
#else
				longjmp(jmp_bootmenu, -1);
#endif
			/* Do the timeout after at least a full queue walk.  */
			if ((timeout == 0) || (currticks() > time)) {
				break;
			}
		}
	}
	return(0);
}


/**************************************************************************
RARP - Get my IP address and load information
**************************************************************************/
int rarp(void)
{
	int retry;

	/* arp and rarp requests share the same packet structure. */
	struct arprequest rarpreq;

	memset(&rarpreq, 0, sizeof(rarpreq));

	rarpreq.hwtype = htons(1);
	rarpreq.protocol = htons(IP);
	rarpreq.hwlen = ETH_ALEN;
	rarpreq.protolen = 4;
	rarpreq.opcode = htons(RARP_REQUEST);
	memcpy(&rarpreq.shwaddr, arptable[ARP_CLIENT].node, ETH_ALEN);
	/* sipaddr is already zeroed out */
	memcpy(&rarpreq.thwaddr, arptable[ARP_CLIENT].node, ETH_ALEN);
	/* tipaddr is already zeroed out */

	for (retry = 0; retry < MAX_ARP_RETRIES; rfc951_sleep(++retry)) {
		eth_transmit(broadcast, RARP, sizeof(rarpreq), &rarpreq);

		if (await_reply(AWAIT_RARP, 0, rarpreq.shwaddr, TIMEOUT))
			break;
	}

	if (retry < MAX_ARP_RETRIES) {
		(void)sprintf(KERNEL_BUF, DEFAULT_KERNELPATH, arptable[ARP_CLIENT].ipaddr);

		return (1);
	}
	return (0);
}

/**************************************************************************
UDP_TRANSMIT - Send a UDP datagram
**************************************************************************/
int udp_transmit(unsigned long destip, unsigned int srcsock,
	unsigned int destsock, int len, const void *buf)
{
	struct iphdr *ip;
	struct udphdr *udp;
	struct arprequest arpreq;
	int arpentry, i;
	int retry;

	ip = (struct iphdr *)buf;
	udp = (struct udphdr *)((char *)buf + sizeof(struct iphdr));
	ip->verhdrlen = 0x45;
	ip->service = 0;
	ip->len = htons(len);
	ip->ident = 0;
	ip->frags = 0;
	ip->ttl = 60;
	ip->protocol = IP_UDP;
	ip->chksum = 0;
	ip->src.s_addr = arptable[ARP_CLIENT].ipaddr.s_addr;
	ip->dest.s_addr = destip;
	ip->chksum = ipchksum((unsigned short *)buf, sizeof(struct iphdr));
	udp->src = htons(srcsock);
	udp->dest = htons(destsock);
	udp->len = htons(len - sizeof(struct iphdr));
	udp->chksum = 0;
	if (destip == IP_BROADCAST) {
		eth_transmit(broadcast, IP, len, buf);
	} else {
		if (((destip & netmask) !=
			(arptable[ARP_CLIENT].ipaddr.s_addr & netmask)) &&
			arptable[ARP_GATEWAY].ipaddr.s_addr)
				destip = arptable[ARP_GATEWAY].ipaddr.s_addr;
		for(arpentry = 0; arpentry<MAX_ARP; arpentry++)
			if (arptable[arpentry].ipaddr.s_addr == destip) break;
		if (arpentry == MAX_ARP) {
			printk_info("%I is not in my arp table!\n", destip);
			return(0);
		}
		for (i = 0; i < ETH_ALEN; i++)
			if (arptable[arpentry].node[i])
				break;
		if (i == ETH_ALEN) {	/* Need to do arp request */
			arpreq.hwtype = htons(1);
			arpreq.protocol = htons(IP);
			arpreq.hwlen = ETH_ALEN;
			arpreq.protolen = 4;
			arpreq.opcode = htons(ARP_REQUEST);
			memcpy(arpreq.shwaddr, arptable[ARP_CLIENT].node, ETH_ALEN);
			memcpy(arpreq.sipaddr, &arptable[ARP_CLIENT].ipaddr, sizeof(in_addr));
			memset(arpreq.thwaddr, 0, ETH_ALEN);
			memcpy(arpreq.tipaddr, &destip, sizeof(in_addr));
			for (retry = 1; retry <= MAX_ARP_RETRIES; retry++) {
				eth_transmit(broadcast, ARP, sizeof(arpreq),
					&arpreq);
				if (await_reply(AWAIT_ARP, arpentry,
					arpreq.tipaddr, TIMEOUT)) goto xmit;
				rfc951_sleep(retry);
				/* We have slept for a while - the packet may
				 * have arrived by now.  If not, we have at
				 * least some room in the Rx buffer for the
				 * next reply.  */
				if (await_reply(AWAIT_ARP, arpentry,
					arpreq.tipaddr, 0)) goto xmit;
			}
			return(0);
		}
xmit:
		eth_transmit(arptable[arpentry].node, IP, len, buf);
	}
	return(1);
}

/**************************************************************************
TFTP - Download extended BOOTP data, or kernel image
**************************************************************************/
int tftp(const char *name, int (*fnc)(unsigned char *, int, int, int))
{
	int             retry = 0;
	static unsigned short iport = 2000;
	unsigned short  oport;
	unsigned short  len, block = 0, prevblock = 0;
	int		bcounter = 0;
	struct tftp_t  *tr;
	struct tftpreq_t tp;
	int		rc;
	int		packetsize = TFTP_DEFAULTSIZE_PACKET;

	/* Clear out the Rx queue first.  It contains nothing of interest,
	 * except possibly ARP requests from the DHCP/TFTP server.  We use
	 * polling throughout Etherboot, so some time may have passed since we
	 * last polled the receive queue, which may now be filled with
	 * broadcast packets.  This will cause the reply to the packets we are
	 * about to send to be lost immediately.  Not very clever.  */
	await_reply(AWAIT_QDRAIN, 0, NULL, 0);

	tp.opcode = htons(TFTP_RRQ);
	/* Warning: the following assumes the layout of bootp_t.
	   But that's fixed by the IP, UDP and BOOTP specs. */
	len = sizeof(tp.ip) + sizeof(tp.udp) + sizeof(tp.opcode) +
		sprintf((char *)tp.u.rrq, "%s%coctet%cblksize%c%d",
		name, 0, 0, 0, TFTP_MAX_PACKET) + 1;
	if (!udp_transmit(arptable[ARP_SERVER].ipaddr.s_addr, ++iport,
		TFTP_PORT, len, &tp))
		return (0);
	for (;;)
	{
#ifdef	CONGESTED
		if (!await_reply(AWAIT_TFTP, iport, NULL, (block ? TFTP_REXMT : TIMEOUT)))
#else
		if (!await_reply(AWAIT_TFTP, iport, NULL, TIMEOUT))
#endif
		{
			if (!block && retry++ < MAX_TFTP_RETRIES)
			{	/* maybe initial request was lost */
				rfc951_sleep(retry);
				if (!udp_transmit(arptable[ARP_SERVER].ipaddr.s_addr,
					++iport, TFTP_PORT, len, &tp))
					return (0);
				continue;
			}
#ifdef	CONGESTED
			if (block && ((retry += TFTP_REXMT) < TFTP_TIMEOUT))
			{	/* we resend our last ack */
#ifdef	MDEBUG
				printk_info("<REXMT>\n");
#endif
				udp_transmit(arptable[ARP_SERVER].ipaddr.s_addr,
					iport, oport,
					TFTP_MIN_PACKET, &tp);
				continue;
			}
#endif
			break;	/* timeout */
		}
		tr = (struct tftp_t *)&nic.packet[ETH_HLEN];
		if (tr->opcode == ntohs(TFTP_ERROR))
		{
			printk_info("TFTP error %d (%s)\n",
			       ntohs(tr->u.err.errcode),
			       tr->u.err.errmsg);
			break;
		}

		if (tr->opcode == ntohs(TFTP_OACK)) {
			char *p = tr->u.oack.data, *e;

			if (prevblock)		/* shouldn't happen */
				continue;	/* ignore it */
			len = ntohs(tr->udp.len) - sizeof(struct udphdr) - 2;
			if (len > TFTP_MAX_PACKET)
				goto noak;
			e = p + len;
			while (*p != '\000' && p < e) {
				if (!strcasecmp("blksize", p)) {
					p += 8;
					if ((packetsize = getdec(&p)) <
					    TFTP_DEFAULTSIZE_PACKET)
						goto noak;
					while (p < e && *p) p++;
					if (p < e)
						p++;
				}
				else {
				noak:
					tp.opcode = htons(TFTP_ERROR);
					tp.u.err.errcode = 8;
/*
 *	Warning: the following assumes the layout of bootp_t.
 *	But that's fixed by the IP, UDP and BOOTP specs.
 */
					len = sizeof(tp.ip) + sizeof(tp.udp) + sizeof(tp.opcode) + sizeof(tp.u.err.errcode) +
						sprintf((char *)tp.u.err.errmsg,
						"RFC1782 error") + 1;
					udp_transmit(arptable[ARP_SERVER].ipaddr.s_addr,
						     iport, ntohs(tr->udp.src),
						     len, &tp);
					return (0);
				}
			}
			if (p > e)
				goto noak;
			block = tp.u.ack.block = 0; /* this ensures, that */
						/* the packet does not get */
						/* processed as data! */
		}
		else if (tr->opcode == htons(TFTP_DATA)) {
			len = ntohs(tr->udp.len) - sizeof(struct udphdr) - 4;
			if (len > packetsize)	/* shouldn't happen */
				continue;	/* ignore it */
			block = ntohs(tp.u.ack.block = tr->u.data.block); }
		else /* neither TFTP_OACK nor TFTP_DATA */
			break;

		if ((block || bcounter) && (block != prevblock+1)) {
			/* Block order should be continuous */
			tp.u.ack.block = htons(block = prevblock);
		}
		tp.opcode = htons(TFTP_ACK);
		oport = ntohs(tr->udp.src);
		udp_transmit(arptable[ARP_SERVER].ipaddr.s_addr, iport,
			oport, TFTP_MIN_PACKET, &tp);	/* ack */
		if ((unsigned short)(block-prevblock) != 1) {
			/* Retransmission or OACK, don't process via callback
			 * and don't change the value of prevblock.  */
			continue;
		}
		prevblock = block;
		retry = 0;	/* It's the right place to zero the timer? */
		if ((rc = fnc(tr->u.data.download,
			      ++bcounter, len, len < packetsize)) >= 0)
			return(rc);
		if (len < packetsize)		/* End of data */
			return (1);
	}
	return (0);
}

int             retry = 0;
static unsigned short iport = 2000;
unsigned short  oport;
unsigned short  len, block = 0, prevblock = 0;
int		bcounter = 0;
struct tftp_t  *tr;
struct tftpreq_t tp;
int		rc;
int		packetsize = TFTP_DEFAULTSIZE_PACKET;

int tftp_init(const char *name)
{
	retry = 0;
	iport = 2000;
	block = 0;
	prevblock = 0;
	bcounter = 0;
	packetsize = TFTP_DEFAULTSIZE_PACKET;

	/* Clear out the Rx queue first.  It contains nothing of interest,
	 * except possibly ARP requests from the DHCP/TFTP server.  We use
	 * polling throughout Etherboot, so some time may have passed since we
	 * last polled the receive queue, which may now be filled with
	 * broadcast packets.  This will cause the reply to the packets we are
	 * about to send to be lost immediately.  Not very clever.  */
	await_reply(AWAIT_QDRAIN, 0, NULL, 0);

	tp.opcode = htons(TFTP_RRQ);
	/* Warning: the following assumes the layout of bootp_t.
	   But that's fixed by the IP, UDP and BOOTP specs. */
	len = sizeof(tp.ip) + sizeof(tp.udp) + sizeof(tp.opcode) +
		sprintf((char *)tp.u.rrq, "%s%coctet%cblksize%c%d",
		name, 0, 0, 0, TFTP_MAX_PACKET) + 1;
	if (!udp_transmit(arptable[ARP_SERVER].ipaddr.s_addr, ++iport,
		TFTP_PORT, len, &tp))
		return (-1);
#ifdef	CONGESTED
	while (!await_reply(AWAIT_TFTP, iport, NULL, (block ? TFTP_REXMT : TIMEOUT)))
#else
	while (!await_reply(AWAIT_TFTP, iport, NULL, TIMEOUT))
#endif
	{
		if (!block && retry++ < MAX_TFTP_RETRIES)
		{	/* maybe initial request was lost */
			rfc951_sleep(retry);
			if (!udp_transmit(arptable[ARP_SERVER].ipaddr.s_addr,
				++iport, TFTP_PORT, len, &tp))
				return (-1);
			continue;
		}
#ifdef	CONGESTED
		if (block && ((retry += TFTP_REXMT) < TFTP_TIMEOUT))
		{	/* we resend our last ack */
#ifdef	MDEBUG
			printk_info("<REXMT>\n");
#endif
			udp_transmit(arptable[ARP_SERVER].ipaddr.s_addr,
				iport, oport,
				TFTP_MIN_PACKET, &tp);
			continue;
		}
#endif
		break;	/* timeout */
	}

// we have recieved reply. see if it's any good.

	tr = (struct tftp_t *)&nic.packet[ETH_HLEN];
	if (tr->opcode == ntohs(TFTP_ERROR)) {
		printk_info("TFTP error %d (%s)\n",
		       ntohs(tr->u.err.errcode),
		       tr->u.err.errmsg);

		return(-ntohs(tr->u.err.errcode));
	}

	if (tr->opcode == ntohs(TFTP_OACK)) {
		char *p = tr->u.oack.data, *e;

		if (prevblock)		/* shouldn't happen */
			return(-1);	/* ignore it */
		len = ntohs(tr->udp.len) - sizeof(struct udphdr) - 2;
		if (len > TFTP_MAX_PACKET)
			goto noak;
		e = p + len;
		while (*p != '\000' && p < e) {
			if (!strcasecmp("blksize", p)) {
				p += 8;
				if ((packetsize = getdec(&p)) <
				    TFTP_DEFAULTSIZE_PACKET)
					goto noak;
				while (p < e && *p) p++;
				if (p < e)
					p++;
			}
			else {
			noak:
				tp.opcode = htons(TFTP_ERROR);
				tp.u.err.errcode = 8;
/*
 *	Warning: the following assumes the layout of bootp_t.
 *	But that's fixed by the IP, UDP and BOOTP specs.
 */
				len = sizeof(tp.ip) + sizeof(tp.udp) + sizeof(tp.opcode) + sizeof(tp.u.err.errcode) +
					sprintf((char *)tp.u.err.errmsg,
					"RFC1782 error") + 1;
				udp_transmit(arptable[ARP_SERVER].ipaddr.s_addr,
					     iport, ntohs(tr->udp.src),
					     len, &tp);
				return (-1);
			}
		}
		if (p > e)
			goto noak;
		block = tp.u.ack.block = 0; /* this ensures, that */
					/* the packet does not get */
					/* processed as data! */
	}
	return(0);
}
int tftp_fetchone(char *buffer)
{
	/* Clear out the Rx queue first.  It contains nothing of interest,
	 * except possibly ARP requests from the DHCP/TFTP server.  We use
	 * polling throughout Etherboot, so some time may have passed since we
	 * last polled the receive queue, which may now be filled with
	 * broadcast packets.  This will cause the reply to the packets we are
	 * about to send to be lost immediately.  Not very clever.  */
retry:
	await_reply(AWAIT_QDRAIN, 0, NULL, 0);

	tp.opcode = htons(TFTP_ACK);
	oport = ntohs(tr->udp.src);
	udp_transmit(arptable[ARP_SERVER].ipaddr.s_addr, iport,
		oport, TFTP_MIN_PACKET, &tp);	/* ack */

#ifdef	CONGESTED
	while (!await_reply(AWAIT_TFTP, iport, NULL, (block ? TFTP_REXMT : TIMEOUT)))
#else
	while (!await_reply(AWAIT_TFTP, iport, NULL, TIMEOUT))
#endif
	{
		if (!block && retry++ < MAX_TFTP_RETRIES)
		{	/* maybe initial request was lost */
			rfc951_sleep(retry);
			if (!udp_transmit(arptable[ARP_SERVER].ipaddr.s_addr,
				++iport, TFTP_PORT, len, &tp))
				return (-1);
			continue;
		}
#ifdef	CONGESTED
		if (block && ((retry += TFTP_REXMT) < TFTP_TIMEOUT))
		{	/* we resend our last ack */
#ifdef	MDEBUG
			printk_info("<REXMT>\n");
#endif
			udp_transmit(arptable[ARP_SERVER].ipaddr.s_addr,
				iport, oport,
				TFTP_MIN_PACKET, &tp);
			continue;
		}
#endif
		break;	/* timeout */
	}

	tr = (struct tftp_t *)&nic.packet[ETH_HLEN];
	if (tr->opcode == ntohs(TFTP_ERROR))
	{
		printk_info("TFTP error %d (%s)\n",
		       ntohs(tr->u.err.errcode),
		       tr->u.err.errmsg);
		return(-tr->u.err.errcode);
	}

	if (tr->opcode == ntohs(TFTP_OACK))
		goto retry;	// session is already started! try again.

	else if (tr->opcode == htons(TFTP_DATA)) {
		len = ntohs(tr->udp.len) - sizeof(struct udphdr) - 4;
		if (len > packetsize)	/* shouldn't happen */
			goto retry; 	/* bad thing, skip it and try again! */
		block = ntohs(tp.u.ack.block = tr->u.data.block); 

	} else /* neither TFTP_OACK nor TFTP_DATA */
		goto retry; // whatever it is, it's invalid! try again.

	if ((block || bcounter) && (block != prevblock+1)) {
		/* Block order should be continuous */
		tp.u.ack.block = htons(block = prevblock);
		goto retry;
	}

	prevblock = block;
	retry = 0;	/* It's the right place to zero the timer? */
	
	
	memcpy(buffer,tr->u.data.download, len);
	return(len);
}
