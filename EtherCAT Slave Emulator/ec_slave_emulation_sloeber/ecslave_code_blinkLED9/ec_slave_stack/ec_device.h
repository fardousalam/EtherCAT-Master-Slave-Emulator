#ifndef __EC_DEVICE_H__

#define __EC_DEVICE_H__

//kernel

#ifdef kernel

#include "ecdev.h"



struct ec_device

{

    ecat_node_t *ecat_node; /**< EtherCAT master/slave */

    struct net_device *dev; /**< pointer to the assigned net_device */

    struct sk_buff *processed_skb;

    struct list_head events;

    spinlock_t events_sync;

    struct ecat_event rx_time;

    // Frame statistics

    u64 tx_count; /**< Number of frames sent. */

    u64 last_tx_count; /**< Number of frames sent of last statistics cycle. */

    u64 rx_count; /**< Number of frames received. */

    u64 tx_bytes; /**< Number of frames sent. */

    u64 last_tx_bytes; /**< Number of bytes sent of last statistics cycle. */

    u64 tx_errors; /**< Number of transmit errors. */

    u64 last_loss; /**< Tx/Rx difference of last statistics cycle. */



};



/*****************************************************************************/



int ec_device_init(

        ec_device_t *device, /**< EtherCAT device */

        ecat_node_t *ecat_node /**< ethercat node owning the device */

        );



void ec_device_attach(

        ec_device_t *device, /**< EtherCAT device */

        struct net_device *net_dev /**< net_device structure */

        );

/*****************************************************************************/

void ec_device_detach(

        ec_device_t *device /**< EtherCAT device */

        );



/*****************************************************************************/

void ec_device_clear_stats(

        ec_device_t *device /**< EtherCAT device */

        );



/*****************************************************************************/

void ec_device_send(

	ec_device_t *device,

	struct sk_buff *skb

	);
	
#endif

//user

#ifdef user


#include "ecat_timer.h"



struct ec_device {

	pthread_mutex_t events_sync;

    LIST_HEAD(porteve, ecat_event) events;

 	struct ecat_event rx_time;

	int index;

	struct ether_header mac;

	char name[16];

	char ip[32];

	char macaddr[32];

	int sock;

	int subnet_mask;

	int link_up;

};



void ec_init_device(struct ec_device *ec);

void ecat_add_event_to_device(struct ec_device *ec, 

		struct ecat_event* ev,

		void (*action)(void *), 

		void *private);

#endif

//arduino

#ifdef ARDUINO
struct ec_device {

        int index;

        struct ecat_event rx_time;

};





#endif
#endif

