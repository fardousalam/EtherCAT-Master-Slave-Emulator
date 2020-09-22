#include <Arduino.h>
#include "../include/xgeneral.h"
#include "ethercattype.h"
#include "ecs_slave.h"
#include "ec_mbox.h"
#include "ec_regs.h"
#include "ec_coe.h"
#include "ec_coe_sdo_mapping.h"


#define NR_SDOS 1
#define SDO1 0x1234
#define SDO2 0x5678

void ec_populate_sdos(coe_sdo_service_data *srvdata)
{
	srvdata->index[0] = SDO1;
	srvdata->index[NR_SDOS] = 0x00;
}

int ec_nr_sdos(void)
{
	return NR_SDOS;
}

void ec_sdo_action(ecat_slave* ecs, 
		uint16_t index, uint16_t subindex,
		uint8_t* data, int datalen)
{	
	Serial.println("ec sdo action"); 
	Serial.println(index); 

	switch (index) {
	case SDO1:
		break;
	case SDO2:
		break;
	default:
		break;
	}
}
