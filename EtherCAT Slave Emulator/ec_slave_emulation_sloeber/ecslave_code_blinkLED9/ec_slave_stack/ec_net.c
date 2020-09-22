
// Arduino
#ifdef ARDUINO
#include "xgeneral.h"
#include "ethercattype.h"
#include "ecs_slave.h"
#include "ec_device.h"
#include "fsm_slave.h"
#include "ec_sii.h"
#include "ec_regs.h"
#include "ec_net.h"
#include "ec_process_data.h"
#include "ec_com.h"

void ethernetSetup();
static struct ec_device etherDev[2];

/*
 * scan network device list, find the interfaces by mac and 
 * grab them.
*/
int  ecs_net_init(int __attribute__ ((unused)) dummy,char __attribute__ ((unused)) *argv[], ecat_slave *ecs)
{
	struct ec_device *device;

	device = &etherDev[0];
	ecs->intr[RX_INT_INDEX] = device;
	ecs->interfaces_nr = 1;
	ecs->intr[TX_INT_INDEX] = ecs->intr[RX_INT_INDEX];
        ethernetSetup();
	return 0;
}

int ec_is_nic_link_up(struct __ecat_slave__* __attribute__ ((unused)) dummy,struct ec_device __attribute__ ((unused)) *d)
{ 
	return 1;
}
#endif //ARDUINO

//kernel

#ifdef kernel
#include "xgeneral.h"
#include "globals.h"
#include "ethercattype.h"
#include "ecs_slave.h"
#include "ec_device.h"
#include "fsm_slave.h"
#include "ec_sii.h"
#include "ec_regs.h"
#include "ec_net.h"
#include "ec_process_data.h"
#include "ec_com.h"
#include "ec_debug.h"
#include <linux/netdevice.h>

int ec_mac_equal(const uint8_t * mac1, /**< First MAC address. */
		 const uint8_t * mac2 /**< Second MAC address. */
    		)
{
	unsigned int i;

	for (i = 0; i < ETH_ALEN; i++)
		if (mac1[i] != mac2[i])
			return 0;

	return 1;
}

struct net_device *ec_find_dev_by_mac(const uint8_t *mac)
{
 	char *endp;
	int i = 0;
	int j = 0;
	int len = strlen(mac);
	struct net_device *netdev;
	uint8_t tempmac[ETH_ALEN];
	
	for ( ; j <  len; i++){
		if ( j + 2 >= len )
			 j = len-2;
		endp = (char *)&mac[j+2]; 
		tempmac[i] = simple_strtol( &mac[j], &endp, 16);
		j += 3;
	}

	for_each_netdev(&init_net, netdev) {
		if (ec_mac_equal(tempmac, netdev->dev_addr)) {
			return netdev;
		}
	}
	return NULL;
}

/*
 * scan network device list, find the interfaces by mac and 
 * grab them.
*/
int ec_net_init(ecat_slave * ecs, char *rxmac, char *txmac)
{
	struct net_device *netdev;
	struct ec_device *device;

	if (!rxmac) {
		EC_NODE_ERR(ecs, "Must provide receive interface MAC\n");
		return -1;
	}
	netdev = ec_find_dev_by_mac(rxmac);
	if (!netdev) {
		EC_NODE_ERR(ecs, "Receive mac %s is invalid\n", rxmac);
		return -1;
	}
	device = kmalloc(sizeof(struct ec_device), GFP_KERNEL);
	if (!device) {
		EC_NODE_ERR(ecs, "out of memory for %s\n", rxmac);
		return -1;
	}
	device->processed_skb = 0;
	ec_device_init(device, ecs);
	ec_device_attach(device, netdev);
	ecs->intr[RX_INT_INDEX] = device;
	EC_NODE_INFO(ecs, "Loaded RX listener on device %s\n",netdev->name);
	ecs->interfaces_nr = 1;
	/* closed loop */
	if (!txmac) {
		ecs->intr[TX_INT_INDEX] = ecs->intr[RX_INT_INDEX];
		EC_NODE_INFO(ecs, "Last slave . TX is %s",netdev->name);
		return 0;
	}
	netdev = ec_find_dev_by_mac(txmac);
	if (!netdev) {
		EC_NODE_ERR(ecs, "Transmit interface MAC %s is invalid\n",
			    txmac);
		return -1;
	}
	device = kmalloc(sizeof(struct ec_device), GFP_KERNEL);
	if (!device) {
		EC_NODE_ERR(ecs, "out of memory for %s\n", txmac);
		return -1;
	}
	device->processed_skb = 0;
	ec_device_init(device, ecs);
	ec_device_attach(device, netdev);
	ecs->intr[TX_INT_INDEX] = device;
	ecs->interfaces_nr = 2;
	EC_NODE_INFO(ecs, "Loaded Tx listener on device %s\n", netdev->name);
	return 0;
}

void ec_tx_pkt(	uint8_t *buf __attribute__ ((unused)),
		int size __attribute__ ((unused)), 
		struct ec_device *txdev)
{
	struct ec_device *rxdev = txdev->ecat_node->intr[RX_INT_INDEX];

	ec_device_send(txdev, rxdev->processed_skb);
	rxdev->processed_skb = 0;
}

int ec_is_nic_link_up(ecat_slave * ecs, struct ec_device *intr)
{
	return intr->dev->ethtool_ops->get_link(intr->dev);
}
#endif

//user
#ifdef user
#include "xgeneral.h"
#include "ec_device.h"
#include "ethercattype.h"
#include "ecs_slave.h"
#include "ec_net.h"

#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <net/if.h>

#include <linux/sockios.h>
#include <linux/ethtool.h>

/* use the ethtool way to determine whether link is up*/
int ec_is_nic_link_up(ecat_slave *esv,struct ec_device *intr)
{
    int sock;
    struct ifreq ifr;
    struct ethtool_value edata;
    int rc;

    sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);
    if (sock < 0) {
    	return 0;
    }
    strncpy(ifr.ifr_name, intr->name, sizeof(ifr.ifr_name));
    ifr.ifr_data = (void *)&edata;

    edata.cmd = ETHTOOL_GLINK;

    rc = ioctl(sock, SIOCETHTOOL, &ifr);
    close(sock);
    if (rc < 0) {
        return 0;
    }

    return  edata.data  ? 1 : 0;
}

int ecs_get_intr_conf(struct ec_device * intr)
{
	struct ifreq ifr;
	int ret;
	char temp_str[20];
	struct sockaddr_in sin_ip;
	struct sockaddr_in sin_mask;
	struct sockaddr *sa;

	/* Get host's ip */
	strcpy(ifr.ifr_name, intr->name);
	ret = ioctl(intr->sock, SIOCGIFADDR, &ifr);
	if (ret < 0) {
		/* no IP. put all zeros */
		memset(&sin_ip, 0, sizeof(struct sockaddr));
	} else {
		memcpy(&sin_ip, &ifr.ifr_addr, sizeof(struct sockaddr));
	}

	inet_ntop(AF_INET, &sin_ip.sin_addr, intr->ip, sizeof(intr->ip));

	ec_printf("%s:\nLOCAL IP %s\n", ifr.ifr_name, intr->ip);
	/*
	 * get host's subnet mask
	 */
	strcpy(ifr.ifr_name, intr->name);
	ret = ioctl(intr->sock, SIOCGIFNETMASK, &ifr);
	if (ret < 0) {
		/* no mask. put all zeros */
		memset(&sin_mask, 0, sizeof(struct sockaddr));
		ec_printf("LOCAL SUBNET MASK 0.0.0.0\n");
	} else{
		memcpy(&sin_mask, &ifr.ifr_netmask,
		       sizeof(struct sockaddr));
		ec_printf("LOCAL SUBNET MASK %s\n",
			  inet_ntop(AF_INET, &sin_mask.sin_addr, temp_str,
				    sizeof(temp_str)));
	}
	intr->subnet_mask = sin_mask.sin_addr.s_addr;
	/* get mac address */
	strcpy(ifr.ifr_name, intr->name);
	ret = ioctl(intr->sock, SIOCGIFHWADDR, &ifr);
	if (ret < 0) {
		perror("failed to get interface address\n");
		return -1;
	}
	sa = &ifr.ifr_hwaddr;
	if (sa->sa_family != ARPHRD_ETHER) {
		perror("interface without ARPHRD_ETHER");
		return -1;
	}
	/* first byte is type */
	intr->mac.ether_type = HTYPE_ETHER;
	/* other six bytes the actual addresses */
	memcpy(&intr->mac.ether_shost, sa->sa_data, 6);

	sprintf(intr->macaddr,
		  "%2.2X:%2.2X:%2.2X:%2.2X:%2.2X:%2.2X",
		  intr->mac.ether_shost[0],
		  intr->mac.ether_shost[1],
		  intr->mac.ether_shost[2],
		  intr->mac.ether_shost[3],
		  intr->mac.ether_shost[4], intr->mac.ether_shost[5]);
	if (ioctl(intr->sock, SIOCGIFINDEX, &ifr) == -1) {
		return (-1);
	}
	intr->index = ifr.ifr_ifindex;
	ec_printf("LOCAL MAC %s\n", intr->macaddr);
	return 0;
}

int ecs_sock(struct ec_device * intr)
{
	struct ifreq ifr;

	intr->sock = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
	if (intr->sock < 0) {
		perror("socket failed:");
		return -1;
	}
	memset(&ifr,0,sizeof(ifr));
	strcpy(ifr.ifr_name, intr->name);
	if (setsockopt(intr->sock, SOL_SOCKET, 
			SO_BINDTODEVICE, &ifr, sizeof(ifr)) < 0){
		perror("failed to bind socket to interface\n");
		return -1;
	}
	return 0;
}

int ecs_net_init(int argc, char *argv[], ecat_slave * esv)
{
	int i;
	int k;
	struct ec_device *intr;

	esv->interfaces_nr = 0;
	for (i = 0, k = 1; k < argc; k++, i++) {
		intr = esv->intr[i] = malloc(sizeof(struct ec_device));
		strncpy(intr->name,
			argv[k], sizeof(intr->name));
		if (ecs_sock(intr))
			return -1;
		if (ecs_get_intr_conf(intr))
			return -1;
		printf("LINK %d %s  %s\n", i, intr->name,
			  ec_is_nic_link_up(esv, intr) ? "UP" : "DOWN");
		/*if (!ec_is_nic_link_up(esv, esv->intr[i])) {
			free(intr);
			esv->intr[i] = 0;
			return -1;
		}*/
		ec_init_device(intr);
		esv->interfaces_nr++;	
	}
	if (esv->interfaces_nr == 1) {
		/* closed loop */
		esv->intr[TX_INT_INDEX] = esv->intr[RX_INT_INDEX];
	}
	return 0;
}

#endif
