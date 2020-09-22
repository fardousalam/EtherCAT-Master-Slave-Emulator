#ifndef  __EC_COE_SDO_MAPPING_H__
#define  __EC_COE_SDO_MAPPING_H__

int 	ec_nr_sdos(void);
void	ec_populate_sdos(coe_sdo_service_data *srvdata);
void 	ec_sdo_action(ecat_slave* ecs, 
		uint16_t index, uint16_t subindex,
		uint8_t* data, int datalen);

#endif
