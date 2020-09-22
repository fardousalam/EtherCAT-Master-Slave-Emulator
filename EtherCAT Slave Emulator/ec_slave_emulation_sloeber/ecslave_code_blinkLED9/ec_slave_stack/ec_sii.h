#ifndef __EC_SII__
#define __EC_SII__

#ifdef __cplusplus
extern "C" {
#endif

void init_sii(ecat_slave *);
void ec_sii_rw(uint8_t * data, int datalen);
int ec_sii_start_read(uint8_t * data, int datalen);
void ec_sii_syncm(int reg, uint8_t* data, int datalen);
int set_process_data(uint8_t *data, uint16_t offset, uint16_t datalen);


#define ARDUINO_DIGITAL_IOS_PINS	14
#define ARDUINO_ANALOG_INPUT_PINS	6
#define NR_PDOS	     (ARDUINO_DIGITAL_IOS_PINS + ARDUINO_ANALOG_INPUT_PINS + 1 )
#define CMD_PDO_IDX  NR_PDOS -1
#define STAT_PDO_IDX  NR_PDOS -1

#ifdef __cplusplus
}
#endif

#endif
