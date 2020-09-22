#include <Arduino.h>
#include <Ethernet.h>
#include <SoftPWM.h>
#include <LibPrintf.h>

#include "ec_slave_stack/xgeneral.h"
#include "ec_slave_stack/ethercattype.h"
#include "ec_slave_stack/fsm_slave.h"
#include "ec_slave_stack/ecs_slave.h"
#include "ec_slave_stack/ec_sii.h"
#include "ec_slave_stack/ec_regs.h"
#include "ec_slave_stack/ec_net.h"
#include "ec_slave_stack/ec_process_data.h"
#include "ec_slave_stack/ec_com.h"
#include "ec_slave_stack/ec_cmd.h"

extern "C" uint8_t ecat_rcv(ecat_slave  *ecs);



#define ARDUINO_ANALOG_WRITE 0x1
#define ARDUINO_ANALOG_READ  0x2
#define ARDUINO_DIGITAL_WRITE 0x3
#define ARDUINO_DIGITAL_READ  0x4

int DIG_IN_1 = 0;     //digital input 1
int DIG_IN_2 = 5;     //Digital input 2
int ANA_IN_1 = A4;    //Analoge input 1
int ANA_IN_2 = A5;    //Analoge input 2
int PWM_PIN = 7;    // Analoge output
int DIG_OUT_1= A3;   //Digital output 1
int button_1 = 0;
int button_2=0;
int sensor_1 = 0;
int sensor_2 = 0;

struct fsm_slave fsm_slave;
ecat_slave ecs;

uint8_t err_code = 0x00;


void arduinoAction(int pdoe,uint32_t value)
{
	uint8_t cmd = value  & 0x00FF;
	uint8_t val = (value & 0xFF00) >> 8;

	switch(cmd) {
		case ARDUINO_ANALOG_WRITE:
			analogWrite(pdoe, val);
			break;
		case ARDUINO_DIGITAL_WRITE:
			digitalWrite(pdoe, val);
			break;
		case ARDUINO_ANALOG_READ:
		    analogRead(pdoe);
		    break;
		case ARDUINO_DIGITAL_READ:
		    digitalRead(pdoe);
		    break;

	}

}

void setup()
{
	int ret;
        Serial.begin(9600);
        Serial.println("Arduino Ethercat slave");

          //pinMode(PWM_PIN,OUTPUT);
   		pinMode(DIG_OUT_1,OUTPUT);
   		pinMode(DIG_IN_1,INPUT);
   		pinMode(DIG_IN_2,INPUT);
   		pinMode(ANA_IN_1,INPUT);
   		pinMode(ANA_IN_2,INPUT);

	 SoftPWMBegin();

	 SoftPWMSet(PWM_PIN, 0);

	  SoftPWMSetFadeTime(PWM_PIN, 1000, 1000);


	ecs.fsm = 0; /* act as flag */
	ret = ecs_net_init(0 , 0, &ecs);
	if (ret < 0) {
		Serial.print("Error init network");
		Serial.println(ret);
		return;
	}
	ret  = ec_init_regs(&ecs) ;
	if (ret < 0){
		Serial.println("Error init registers");
		Serial.println(ret);
		return;
	}

	init_sii(&ecs);

	if (init_process_data(&ecs) < 0) {
		Serial.println("illegal pdo configuration\n");
		return;
	}
	ecs.fsm = &fsm_slave;
	ecs.dgram_processed = 0;
	ecs.dgrams_cnt = 0;
}

void loop()
{

	if (ecs.fsm)
		{
			err_code = ecat_rcv(&ecs);
			switch(err_code)
			{
			case 0x11:
				Serial.println("0x11");
				break;
			case 0x22:
				Serial.println("0x22");
				break;
			case 0xFF:
				Serial.println("0xFF");
				break;
			default:
				Serial.println("0x00");
				break;
			}
	        }
		else
		{


		digitalWrite(DIG_OUT_1,HIGH);
		delay(100);
		digitalWrite(DIG_OUT_1,LOW);
		delay (100);

		button_1 = digitalRead(DIG_IN_1);
		button_2 = digitalRead(DIG_IN_2);
		sensor_1 = analogRead(ANA_IN_1);
		sensor_2 = analogRead(ANA_IN_2);


		Serial.print("STATUS Button 1: ");
		if (button_1 == 1)
			Serial.print("HIGH");
		else
			Serial.print("LOW");
		Serial.print("   ");
		Serial.print("STATUS Button 2: ");
		if (button_2 == 1)
			Serial.print("HIGH");
		else
			Serial.print("LOW");
		Serial.print("   ");
		Serial.print("STATUS Analog_1: "); Serial.print(sensor_1);Serial.print("   ");
		Serial.print("STATUS Analog_2: "); Serial.print(sensor_2);
		Serial.println();

		SoftPWMSet(PWM_PIN, (uint8_t)(sensor_1 >> 2));
		//delay(1000);
		}


}


/*
 * process data  operations for Arduino UNO only
 *  Each pin may be used for a dedicated  purpose.
 *
*/
extern "C" int set_process_data(uint8_t *data, uint16_t offset, uint16_t datalen)
{
	int pdoe_idx = 0;
	uint32_t cmd ;
	int should_act = 0;
	uint32_t* pdos = (uint32_t *)data;

	memcpy( (void *)&cmd, (void *)&pdos[CMD_PDO_IDX], 4);

	for (;pdoe_idx < NR_PDOS; pdoe_idx++)
	{

		should_act = cmd & (1 << pdoe_idx);
		if (!should_act) {
			continue;
		}
		arduinoAction(pdoe_idx, pdos[pdoe_idx]);
	}
	return 0;
}

