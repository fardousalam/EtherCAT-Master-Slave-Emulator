#include "xgeneral.h"
#include "ethercattype.h"
#include "fsm_slave.h"
#include "ecs_slave.h"
#include "ec_regs.h"
#include "ec_process_data.h"

/** Logical Write */
void ec_cmd_lwr(ecat_slave * ecs,uint8_t *dgram_ec)
{
	uint16_t datalen = __ec_dgram_dlength(dgram_ec);
	uint8_t *data = __ec_dgram_data(dgram_ec);
	uint32_t offset = __ec_dgram_laddr(dgram_ec);

	set_process_data(data + offset, offset, datalen);
	__ec_inc_wkc__(dgram_ec);
     	__set_fsm_state(ecs, ecs_process_next_dgram);
}
