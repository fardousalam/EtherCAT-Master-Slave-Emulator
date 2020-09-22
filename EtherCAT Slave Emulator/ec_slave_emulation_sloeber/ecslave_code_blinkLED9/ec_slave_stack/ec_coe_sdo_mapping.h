#ifndef  __EC_COE_SDO_MAPPING_H__
#define  __EC_COE_SDO_MAPPING_H__

#include "xgeneral.h"
#include "ethercattype.h"
#include "ecs_slave.h"
#include "ec_mbox.h"
#include "ec_regs.h"
#include "ec_coe_sdo_mapping.h"
#include "ec_coe.h"

int 	ec_nr_sdos(void);
void	ec_populate_sdos(coe_sdo_service_data *srvdata);
void 	ec_sdo_action(ecat_slave* ecs,
		uint16_t index, uint16_t subindex,
		uint8_t* data, int datalen);

#endif
