/*
 * Network protocols
 *
 */

#define	NET_PROTO_IPV4		/* IPv4 protocol */
#define	NET_PROTO_IPV6		/* IPv6 protocol */
#undef	NET_PROTO_FCOE		/* Fibre Channel over Ethernet protocol */
#define	NET_PROTO_STP		/* Spanning Tree protocol */

/*
 * PXE support
 *
 */
#define	PXE_STACK		/* PXE stack in iPXE - you want this! */
#define	PXE_MENU		/* PXE menu booting */

/*
 * Download protocols
 *
 */

#define	DOWNLOAD_PROTO_TFTP	/* Trivial File Transfer Protocol */
#define	DOWNLOAD_PROTO_HTTP	/* Hypertext Transfer Protocol */
#define	DOWNLOAD_PROTO_HTTPS	/* Secure Hypertext Transfer Protocol */
#define	DOWNLOAD_PROTO_FTP	/* File Transfer Protocol */
#define	DOWNLOAD_PROTO_SLAM	/* Scalable Local Area Multicast */
#define	DOWNLOAD_PROTO_NFS	/* Network File System Protocol */

/*
 * SAN boot protocols
 *
 */

#define	SANBOOT_PROTO_ISCSI	/* iSCSI protocol */
#define	SANBOOT_PROTO_AOE	/* AoE protocol */
#undef	SANBOOT_PROTO_IB_SRP	/* Infiniband SCSI RDMA protocol */
#undef	SANBOOT_PROTO_FCP	/* Fibre Channel protocol */
#define	SANBOOT_PROTO_HTTP	/* HTTP SAN protocol */

/*
 * HTTP extensions
 *
 */
#define HTTP_AUTH_BASIC		/* Basic authentication */
#define HTTP_AUTH_DIGEST	/* Digest authentication */
#define HTTP_ENC_PEERDIST	/* PeerDist content encoding */

/*
 * 802.11 cryptosystems and handshaking protocols
 *
 */
#define	CRYPTO_80211_WEP	/* WEP encryption (deprecated and insecure!) */
#define	CRYPTO_80211_WPA	/* WPA Personal, authenticating with passphrase */
#define	CRYPTO_80211_WPA2	/* Add support for stronger WPA cryptography */

/*
 * Name resolution modules
 *
 */

#define	DNS_RESOLVER		/* DNS resolver */
/*
 * Image types
 *
 * Etherboot supports various image formats.  Select whichever ones
 * you want to use.
 *
 */
#define	IMAGE_NBI		/* NBI image support */
#define	IMAGE_ELF		/* ELF image support */
#define	IMAGE_MULTIBOOT		/* MultiBoot image support */
#define	IMAGE_PXE		/* PXE image support */
#define	IMAGE_SCRIPT		/* iPXE script image support */
#define	IMAGE_BZIMAGE		/* Linux bzImage image support */
#define	IMAGE_COMBOOT		/* SYSLINUX COMBOOT image support */
#undef	IMAGE_EFI		/* EFI image support */
#define	IMAGE_SDI		/* SDI image support */
#define	IMAGE_PNM		/* PNM image support */
#define	IMAGE_PNG		/* PNG image support */

/*
 * Command-line commands to include
 *
 */
#define	AUTOBOOT_CMD		/* Automatic booting */
#define	NVO_CMD			/* Non-volatile option storage commands */
#define	CONFIG_CMD		/* Option configuration console */
#define	IFMGMT_CMD		/* Interface management commands */
#define	IWMGMT_CMD		/* Wireless interface management commands */
#define IBMGMT_CMD		/* Infiniband management commands */
#define FCMGMT_CMD		/* Fibre Channel management commands */
#define	ROUTE_CMD		/* Routing table management commands */
#define IMAGE_CMD		/* Image management commands */
#define DHCP_CMD		/* DHCP management commands */
#define SANBOOT_CMD		/* SAN boot commands */
#define MENU_CMD		/* Menu commands */
#define LOGIN_CMD		/* Login command */
#define SYNC_CMD		/* Sync command */
#define NSLOOKUP_CMD		/* DNS resolving command */
#define TIME_CMD		/* Time commands */
#define DIGEST_CMD		/* Image crypto digest commands */
#define LOTEST_CMD		/* Loopback testing commands */
#define VLAN_CMD		/* VLAN commands */
#define PXE_CMD		/* PXE commands */
#define REBOOT_CMD		/* Reboot command */
#define POWEROFF_CMD		/* Power off command */
#define IMAGE_TRUST_CMD	/* Image trust management commands */
#define PCI_CMD		/* PCI commands */
#define PARAM_CMD		/* Form parameter commands */
#define NEIGHBOUR_CMD		/* Neighbour management commands */
#define PING_CMD		/* Ping command */
#define CONSOLE_CMD		/* Console command */
#define IPSTAT_CMD		/* IP statistics commands */
#define PROFSTAT_CMD		/* Profiling commands */


/*
 * ROM-specific options
 *
 */
#undef	NONPNP_HOOK_INT19	/* Hook INT19 on non-PnP BIOSes */
#define	AUTOBOOT_ROM_FILTER	/* Autoboot only devices matching our ROM */

/*
 * Virtual network devices
 *
 */
#define VNIC_IPOIB		/* Infiniband IPoIB virtual NICs */
//#define VNIC_XSIGO		/* Infiniband Xsigo virtual NICs */

/*
 * Error message tables to include
 *
 */
#define	ERRMSG_80211		/* All 802.11 error descriptions (~3.3kb) */
