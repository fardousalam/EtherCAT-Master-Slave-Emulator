#include <Arduino.h>
#include <SPI.h>         // needed for Arduino versions later than 0018
#include <Ethernet.h>

#include "ec_slave_stack/xgeneral.h"
#include "ec_slave_stack/ethercattype.h"
#include "ec_slave_stack/ecs_slave.h"
#include "ec_slave_stack/ec_device.h"
#include "ec_slave_stack/fsm_slave.h"
#include "ec_slave_stack/ec_sii.h"
#include "ec_slave_stack/ec_regs.h"
#include "ec_slave_stack/ec_net.h"
#include "ec_slave_stack/ec_cmd.h"
#include "ec_slave_stack/ec_com.h"
#include "w5200.h"


static byte mac[6] = { 0xfa, 0xeb, 0x2c, 0xff, 0xde, 0xab };
#define BUFFER_SIZE 600
uint8_t buf[BUFFER_SIZE];
SOCKET s;

extern "C" int ethernetSetup()
{
	W5100.init();
  	W5100.writeSnMR(s, SnMR::MACRAW); 
  	W5100.execCmdSn(s, Sock_OPEN);
	return 0;
}


extern "C" uint8_t ecat_rcv(ecat_slave  *ecs)
{
	int16_t len;
	uint8_t *ecat;

	len = W5100.getRXReceivedSize(s);
	if (len <= 0){
		return 0x11;
	}
	W5100.recv_data_processing(s, buf, len);
	W5100.execCmdSn(s, Sock_RECV);
	ecat = (uint8_t *)&buf[2];

	if (eth_hdr(ecat)->ether_type !=  htons(ETH_P_ECAT)){
		return 0x22;
	}
	ec_process_datagrams(ecs, len - 2, ecat);
	return 0xFF;
}

extern "C" void ec_tx_pkt(uint8_t *buf, int sz, struct ec_device *txdev)
{
 	W5100.send_data_processing(s, buf, sz);
 	W5100.execCmdSn(s, Sock_SEND_MAC);
}



