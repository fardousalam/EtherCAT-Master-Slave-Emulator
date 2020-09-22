/*****************************************************************************
 *
 *  $Id$
 *
 *  Copyright (C) 2007-2009  Florian Pose, Ingenieurgemeinschaft IgH
 *
 *  This file is part of the IgH EtherCAT Master.
 *
 *  The IgH EtherCAT Master is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License version 2, as
 *  published by the Free Software Foundation.
 *
 *  The IgH EtherCAT Master is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
 *  Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with the IgH EtherCAT Master; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 *  ---
 *
 *  The license mentioned above concerns the source code only. Using the
 *  EtherCAT technology and brand is only permitted in compliance with the
 *  industrial property and similar rights of Beckhoff Automation GmbH.
 *
 ****************************************************************************/

#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h> /* clock_gettime() */
#include <sys/mman.h> /* mlockall() */
#include <sched.h> /* sched_setscheduler() */

/****************************************************************************/

#include "ecrt.h" //this is the library that we need

/****************************************************************************


/** Task period in ns*/

// Application parameters
#define FREQUENCY 1000
#define CLOCK_TO_USE CLOCK_MONOTONIC
#define MEASURE_TIMING
#define PERIOD_NS   (1000000)

#define MAX_SAFE_STACK (8 * 1024) /* The maximum stack size which is
                                     guranteed safe to access without
                                     faulting */

/****************************************************************************/
/****************************************************************************/
#define NSEC_PER_SEC (1000000000)
#define PERIOD_NS (NSEC_PER_SEC / FREQUENCY)

#define DIFF_NS(A, B) (((B).tv_sec - (A).tv_sec) * NSEC_PER_SEC + \
        (B).tv_nsec - (A).tv_nsec)

#define TIMESPEC2NS(T) ((uint64_t) (T).tv_sec * NSEC_PER_SEC + (T).tv_nsec)

/* Constants 
#define NSEC_PER_SEC (1000000)
#define FREQUENCY (NSEC_PER_SEC / PERIOD_NS)


/****************************************************************************/

// EtherCAT
static ec_master_t *master = NULL;
static ec_master_state_t master_state = {};

static ec_domain_t *domain1 = NULL;
static ec_domain_state_t domain1_state = {};

static ec_slave_config_t *sc_ana_in = NULL;
static ec_slave_config_t *dig = NULL;
static ec_slave_config_t *lauf = NULL;
static ec_slave_config_t *dreh = NULL;
static ec_slave_config_state_t sc_ana_in_state = {};

/****************************************************************************/
#define NUMSLAVES 2 

// process data
static uint8_t *domain1_pd = NULL;/*process data*/
//HERE WE BEGIN TO DEFINE THE PDO ENTRIES AND WHERE THEY WILL APPEAR IN THE PROCESS DATA DOMAIN 
//definitions for the bus connections and expected slave positions

#define MCSLaufbandslavepos 1001,0
#define MCSDrehbandslavepos 1002,0
#define AnaInSlavePos 0,0
#define DigOutSlavePos 1004,0

#define MCS 0x00000147, 0x00002d51 
#define Beckhoff_EP4374_0002 0x00000002, 0x11164052 
#define Beckhoff_EP2318_0001 0x00000002, 0x090e4052 




// offsets for PDO entries

static unsigned int off_dig_out;
static unsigned int off_ana_in;
static unsigned int off_dig_in;
static unsigned int off_MCS_Lauf;
static unsigned int off_MCS_Dreh;
static int16_t ultraschallsensor= 0;
static unsigned int a= 192;
static int16_t digital_input = 0;
static unsigned int bit_position_4 = 4;
static int16_t Motorsensor= 0;
static int32_t Motorsensor1=0;
//sync manager configuration
static uint8_t sync_index=1;
static ec_direction_t direction=EC_DIR_INPUT;
static ec_watchdog_mode_t  watchdog=EC_WD_DISABLE;
// from dc_user
static unsigned int counter = 0;
static unsigned int blink = 0;
static unsigned int sync_ref_counter = 0;
const struct timespec cycletime = {0, PERIOD_NS};
struct timespec timespec_add(struct timespec time1, struct timespec time2)
{
    struct timespec result;

    if ((time1.tv_nsec + time2.tv_nsec) >= NSEC_PER_SEC) {
        result.tv_sec = time1.tv_sec + time2.tv_sec + 1;
        result.tv_nsec = time1.tv_nsec + time2.tv_nsec - NSEC_PER_SEC;
    } else {
        result.tv_sec = time1.tv_sec + time2.tv_sec;
        result.tv_nsec = time1.tv_nsec + time2.tv_nsec;
    }

    return result;
}

const static ec_pdo_entry_reg_t domain1_regs[] = {
    {DigOutSlavePos, Beckhoff_EP2318_0001, 0x7040,0x01, &off_dig_out,&bit_position_4},
    {DigOutSlavePos, Beckhoff_EP2318_0001, 0x6000,0x01, &off_dig_in,NULL},
    {AnaInSlavePos, Beckhoff_EP4374_0002,0x6010,0x11, &off_ana_in,NULL},
    {MCSLaufbandslavepos,MCS ,0x2320,0x00, &off_MCS_Lauf,NULL},
    {MCSDrehbandslavepos,MCS ,0x6064,0x00, &off_MCS_Dreh,NULL},
    
    {}
};

static char slaves_up = 0; 



/*****************************************************************************/




/* Master 0, Slave 0, "MCS"
 * Vendor ID:       0x00000147
 * Product code:    0x00002d51
 * Revision number: 0x00000007
 */
ec_pdo_entry_info_t slave_0_pdo_entries[] = {
    {0x6060, 0x00, 8},
    {0x6060, 0x00, 8},
    {0x6040, 0x00, 16},// Controlword
    {0x607a, 0x00, 32},// Target position
    {0x60ff, 0x00, 32},// Target velocity
    {0x6061, 0x00, 8},
    {0x6061, 0x00, 8},
    {0x2320, 0x00, 16},
    {0x6041, 0x00, 16},//statusword
    {0x6064, 0x00, 32},//Position actual value
    {0x606c, 0x00, 32},//Actual speed 
};
ec_pdo_info_t slave_0_pdos[] = {
    {0x1600, 2, slave_0_pdo_entries + 0},
    {0x1601, 1, slave_0_pdo_entries + 2},
    {0x1602, 1, slave_0_pdo_entries + 3},
    {0x1603, 1, slave_0_pdo_entries + 4},
    {0x1a00, 3, slave_0_pdo_entries + 5},
    {0x1a01, 1, slave_0_pdo_entries + 8},
    {0x1a02, 1, slave_0_pdo_entries + 9},
    {0x1a03, 1, slave_0_pdo_entries + 10},
};


ec_sync_info_t slave_0_syncs[] = {
    {0, EC_DIR_OUTPUT, 0, NULL, EC_WD_DISABLE},
    {1, EC_DIR_INPUT, 0, NULL, EC_WD_DISABLE},
    {2, EC_DIR_OUTPUT, 4, slave_0_pdos + 0, EC_WD_ENABLE},
    {3, EC_DIR_INPUT, 4, slave_0_pdos + 4, EC_WD_DISABLE},
    {0xff}
};

/* Master 0, Slave 1, "MCS"
 * Vendor ID:       0x00000147
 * Product code:    0x00002d51
 * Revision number: 0x00000007
 */
ec_pdo_entry_info_t slave_1_pdo_entries[] = {
    {0x6060, 0x00, 8},
    {0x6060, 0x00, 8},
    {0x6040, 0x00, 16},
    {0x607a, 0x00, 32},
    {0x6081, 0x00, 32},
    {0x6061, 0x00, 8},
    {0x6061, 0x00, 8},
    {0x2320, 0x00, 16},
    {0x6041, 0x00, 16},
    {0x6064, 0x00, 32},
    {0x606c, 0x00, 32},
};

ec_pdo_info_t slave_1_pdos[] = {
    {0x1600, 2, slave_1_pdo_entries + 0},
    {0x1601, 1, slave_1_pdo_entries + 2},
    {0x1602, 1, slave_1_pdo_entries + 3},
    {0x1603, 1, slave_1_pdo_entries + 4},
    {0x1a00, 3, slave_1_pdo_entries + 5},
    {0x1a01, 1, slave_1_pdo_entries + 8},
    {0x1a02, 1, slave_1_pdo_entries + 9},
    {0x1a03, 1, slave_1_pdo_entries + 10},
};

ec_sync_info_t slave_1_syncs[] = {
    {0, EC_DIR_OUTPUT, 0, NULL, EC_WD_DISABLE},
    {1, EC_DIR_INPUT, 0, NULL, EC_WD_DISABLE},
    {2, EC_DIR_OUTPUT, 4, slave_1_pdos + 0, EC_WD_ENABLE},
    {3, EC_DIR_INPUT, 4, slave_1_pdos + 4, EC_WD_DISABLE},
    {0xff}
};
/* Master 0, Slave 2, "EP4374-0002"
 * Vendor ID:       0x00000002
 * Product code:    0x11164052
 * Revision number: 0x00130002
 */

ec_pdo_entry_info_t slave_2_pdo_entries[] = {
    {0x7020, 0x11, 16}, /* Analog output */
    {0x7030, 0x11, 16}, /* Analog output */
    {0x6000, 0x01, 1}, /* Underrange */
    {0x6000, 0x02, 1}, /* Overrange */
    {0x6000, 0x03, 2}, /* Limit 1 */
    {0x6000, 0x05, 2}, /* Limit 2 */
    {0x6000, 0x07, 1}, /* Error */
    {0x0000, 0x00, 1}, /* Gap */
    {0x0000, 0x00, 5}, /* Gap */
    {0x6000, 0x0e, 1}, /* Sync error */
    {0x6000, 0x0f, 1}, /* TxPDO State */
    {0x6000, 0x10, 1}, /* TxPDO Toggle */
    {0x6000, 0x11, 16}, /* Value */
    {0x6010, 0x01, 1}, /* Underrange */
    {0x6010, 0x02, 1}, /* Overrange */
    {0x6010, 0x03, 2}, /* Limit 1 */
    {0x6010, 0x05, 2}, /* Limit 2 */
    {0x6010, 0x07, 1}, /* Error */
    {0x0000, 0x00, 1}, /* Gap */
    {0x0000, 0x00, 5}, /* Gap */
    {0x6010, 0x0e, 1}, /* Sync error */ 
    {0x6010, 0x0f, 1}, /* TxPDO State */
    {0x6010, 0x10, 1}, /* TxPDO Toggle */
    {0x6010, 0x11, 16}, /* Value */
};
ec_pdo_info_t slave_2_pdos[] = {
    {0x1600, 1, slave_2_pdo_entries + 0}, /* AO Outputs Ch.3 */
    {0x1601, 1, slave_2_pdo_entries + 1}, /* AO Outputs Ch.4 */
    {0x1a00, 11, slave_2_pdo_entries + 2}, /* AI Inputs Ch.1 */
    {0x1a02, 11, slave_2_pdo_entries + 13}, /* AI Inputs Ch.2 */
};
ec_sync_info_t slave_2_syncs[] = {
    {0, EC_DIR_OUTPUT, 0, NULL, EC_WD_DISABLE},//{sync manager index, sync manager direction, number of pdos in pdos, arrays with pdos to assign, watchdog mode}
    {1, EC_DIR_INPUT, 0, NULL, EC_WD_DISABLE},
    {2, EC_DIR_OUTPUT, 2, slave_2_pdos + 0, EC_WD_DISABLE},
    {3, EC_DIR_INPUT, 2, slave_2_pdos + 2, EC_WD_DISABLE},
    {0xff}
};
/* Master 0, Slave 3, "EP2318-0001"
 * Vendor ID:       0x00000002
 * Product code:    0x090e4052
 * Revision number: 0x00110001
 */
ec_pdo_entry_info_t slave_3_pdo_entries[] = {
    {0x0000, 0x00, 4},
    {0x7040, 0x01, 1}, /* Output */
    {0x7050, 0x01, 1}, /* Output */
    {0x7060, 0x01, 1}, /* Output */
    {0x7070, 0x01, 1}, /* Output */
    {0x6000, 0x01, 1}, /* Input */
    {0x6010, 0x01, 1}, /* Input */
    {0x6020, 0x01, 1}, /* Input */
    {0x6030, 0x01, 1}, /* Input */
    {0x0000, 0x00, 4},
};
ec_pdo_info_t slave_3_pdos[] = {
    {0x1604, 2, slave_3_pdo_entries + 0}, /* Channel 5 */
    {0x1605, 1, slave_3_pdo_entries + 2}, /* Channel 6 */
    {0x1606, 1, slave_3_pdo_entries + 3}, /* Channel 7 */
    {0x1607, 1, slave_3_pdo_entries + 4}, /* Channel 8 */
    {0x1a00, 1, slave_3_pdo_entries + 5}, /* Channel 1 */
    {0x1a01, 1, slave_3_pdo_entries + 6}, /* Channel 2 */
    {0x1a02, 1, slave_3_pdo_entries + 7}, /* Channel 3 */
    {0x1a03, 2, slave_3_pdo_entries + 8}, /* Channel 4 */
};
ec_sync_info_t slave_3_syncs[] = {
    {0, EC_DIR_OUTPUT, 4, slave_3_pdos + 0, EC_WD_ENABLE},
    {1, EC_DIR_INPUT, 4, slave_3_pdos + 4, EC_WD_DISABLE},
    {0xff}
};


static ec_sdo_request_t *sdo;



/*****************************************************************************/

void check_domain1_state(void)
{
    ec_domain_state_t ds;

    ecrt_domain_state(domain1, &ds);
 

    if (ds.working_counter != domain1_state.working_counter) {
        printf("Domain1: WC %u.\n", ds.working_counter);
    }
    if (ds.wc_state != domain1_state.wc_state) {
        printf("Domain1: State %u.\n", ds.wc_state);
    }


    domain1_state = ds;
}

/*****************************************************************************/

void check_master_state(void)
{
    ec_master_state_t ms;

    ecrt_master_state(master, &ms);

    if (ms.slaves_responding != master_state.slaves_responding) {
        printf("%u slave(s).\n", ms.slaves_responding);
    }
    if (ms.al_states != master_state.al_states) {
        printf("AL states: 0x%02X.\n", ms.al_states);
    }
    if (ms.link_up != master_state.link_up) {
        printf("Link is %s.\n", ms.link_up ? "up" : "down");                       
    }

    master_state = ms;
}

/*****************************************************************************/

void check_slave_config_states(void)
{
 ec_slave_config_state_t s;   
 ecrt_slave_config_state(sc_ana_in, &s);   
 

    if (s.al_state !=  0x02) {
        printf("AnaIn: State 0x%02X.\n", s.al_state);
    }
    if (s.online != sc_ana_in_state.online) {
        printf("AnaIn: %s.\n", s.online ? "online" : "offline");
    }
    if (s.operational != sc_ana_in_state.operational) {
        printf("AnaIn: %soperational.\n", s.operational ? "" : "Not ");
    }

    sc_ana_in_state = s;


}
void read_sdo(void)
{
    switch (ecrt_sdo_request_state(sdo)) {
        case EC_REQUEST_UNUSED: // request was not used yet
            ecrt_sdo_request_read(sdo); // trigger first read
            break;
        case EC_REQUEST_BUSY:
            printf("Still busy...\n");
            break;
        case EC_REQUEST_SUCCESS:
            printf("SDO value: 0x%04X\n",
                    EC_READ_U16(ecrt_sdo_request_data(sdo)));
            ecrt_sdo_request_read(sdo); // trigger next read
            break;
        case EC_REQUEST_ERROR:
            printf("Failed to read SDO!\n");
            ecrt_sdo_request_read(sdo); // retry reading
            break;
    }
}


/*****************************************************************************/
void cyclic_task()
{ 
    struct timespec wakeupTime, time;
#ifdef MEASURE_TIMING
    struct timespec startTime, endTime, lastStartTime = {};
    uint32_t period_ns = 0, exec_ns = 0, latency_ns = 0,
             latency_min_ns = 0, latency_max_ns = 0,
             period_min_ns = 0, period_max_ns = 0,
             exec_min_ns = 0, exec_max_ns = 0;
#endif
 // get current time
    clock_gettime(CLOCK_TO_USE, &wakeupTime);
     while(1) {
        wakeupTime = timespec_add(wakeupTime, cycletime);
        clock_nanosleep(CLOCK_TO_USE, TIMER_ABSTIME, &wakeupTime, NULL);

        // Write application time to master
        //
        // It is a good idea to use the target time (not the measured time) as
        // application time, because it is more stable.
        //
        ecrt_master_application_time(master, TIMESPEC2NS(wakeupTime));
        #ifdef MEASURE_TIMING
        clock_gettime(CLOCK_TO_USE, &startTime);
        latency_ns = DIFF_NS(wakeupTime, startTime);
        period_ns = DIFF_NS(lastStartTime, startTime);
        exec_ns = DIFF_NS(lastStartTime, endTime);
        lastStartTime = startTime;

        if (latency_ns > latency_max_ns) {
            latency_max_ns = latency_ns;
        }
        if (latency_ns < latency_min_ns) {
            latency_min_ns = latency_ns;
        }
        if (period_ns > period_max_ns) {
            period_max_ns = period_ns;
        }
        if (period_ns < period_min_ns) {
            period_min_ns = period_ns;
        }
        if (exec_ns > exec_max_ns) {
            exec_max_ns = exec_ns;
        }
        if (exec_ns < exec_min_ns) {
            exec_min_ns = exec_ns;
        }
#endif
    
    
    // receive process data
    ecrt_master_receive(master);
    ecrt_domain_process(domain1);

    // check process data state
    check_domain1_state();

    if (counter) {
        counter--;
    } 
    else 
    { // do this at 1 Hz
        counter = FREQUENCY;

        

        // check for master state (optional)
        check_master_state();

        // check for slave configuration state(s) (optional)
        check_slave_config_states();
         
        // read process data SDO
         read_sdo();
        
        #ifdef MEASURE_TIMING
            // output timing stats
            printf("period     %10u ... %10u\n",
                    period_min_ns, period_max_ns);
            printf("exec       %10u ... %10u\n",
                    exec_min_ns, exec_max_ns);
            printf("latency    %10u ... %10u\n",
                    latency_min_ns, latency_max_ns);
            period_max_ns = 0;
            period_min_ns = 0xffffffff;
            exec_max_ns = 0;
            exec_min_ns = 0xffffffff;
            latency_max_ns = 0;
            latency_min_ns = 0xffffffff;
#endif

            // calculate new process data
            blink = !blink;
        
    }  
          


   //  read digital input data
        digital_input = EC_READ_BIT(domain1_pd + off_dig_in, 3); 
         printf("Digital Input: %i\n", digital_input); 
 
  //   read analog input data
       ultraschallsensor = EC_READ_U16(domain1_pd + off_ana_in); 
        printf("ultraschallsensor distance in mm: %i\n ",ultraschallsensor/a); 


    // write digital output data
       blink = !blink; 
       EC_WRITE_BIT(domain1_pd + off_dig_out, 4, blink);
       printf("Digital output: %i\n",blink); 
       
       // read motor input data
       Motorsensor = EC_READ_S16(domain1_pd + off_MCS_Lauf); 
        printf("Motorsensor values: %i\n ",Motorsensor);
       //read motor input data
       Motorsensor1 = EC_READ_S16(domain1_pd + off_MCS_Dreh); 
        printf("Motor1 values: %i\n ",Motorsensor1);
        
         if (sync_ref_counter) {
            sync_ref_counter--;
        } else {
            sync_ref_counter = 1; // sync every cycle

            clock_gettime(CLOCK_TO_USE, &time);
            ecrt_master_sync_reference_clock_to(master, TIMESPEC2NS(time));
        }
        
        ecrt_master_sync_slave_clocks(master);
        
    


    // send process data
    ecrt_domain_queue(domain1);
    ecrt_master_send(master);
    #ifdef MEASURE_TIMING
        clock_gettime(CLOCK_TO_USE, &endTime);
#endif
    }
}




void stack_prefault(void)
{
    unsigned char dummy[MAX_SAFE_STACK];

    memset(dummy, 0, MAX_SAFE_STACK);
}




int main(int argc, char **argv)
{
    unsigned int j, op_flag; 
    ec_slave_config_t *sc;
     ec_master_state_t ms; 
      static const int enable_realtime = 1;

    if (enable_realtime) {
	struct sched_param param;
	param.sched_priority = 49;
	if ( sched_setscheduler( 0, SCHED_FIFO, &param ) == -1) {
	    perror("sched_setscheduler failed");
	}
     if (mlockall(MCL_CURRENT | MCL_FUTURE) == -1) {
        perror("mlockall failed");
        return -1;
    }
}
 
 

  //FIRST, REQUEST A MASTER INSTANCE

    master = ecrt_request_master(0);
    if (!master) { fprintf(stderr, "Unable to get requested master.\n");
        return -1;
    }
   //THEN, CREATE A DOMAIN 

    domain1 = ecrt_master_create_domain(master);
    if (!domain1) {fprintf(stderr, "Unable to create process data domain.\n");
        return -1;
    }
    
    if (!(sc_ana_in = ecrt_master_slave_config(
                    master, AnaInSlavePos, Beckhoff_EP4374_0002))) {
        fprintf(stderr, "Failed to get slave configuration.\n");
        return -1;
    }
    //configuration
    ecrt_slave_config_sdo16( sc_ana_in, 0xF800, 0x02, 0x02);
    //ecrt_slave_config_sdo16( sc_ana_in, 0x1C12, 0x01, 0x1600);
    //ecrt_slave_config_sdo16( sc_ana_in, 0x1C12, 0x02, 0x1601);
    //ecrt_slave_config_sdo16( sc_ana_in, 0x1C13, 0x02, 0x1A02);
    ecrt_slave_config_sdo16( sc_ana_in, 0x1C33, 0x01, 0x00);
    printf("Configuring PDOs...\n");
    if (ecrt_slave_config_pdos(sc_ana_in, EC_END,  slave_2_syncs)) {
        fprintf(stderr, "Failed to configure PDOs.\n");
        return -1;
    }
    
    printf("Configuring Sync manager...\n");
    if ( ecrt_slave_config_sync_manager(sc_ana_in, sync_index, direction, watchdog)) {
        fprintf(stderr, "Failed to configure sync manager\n");
        return -1;
     } 
    
    printf( "Creating SDO requests...\n");
    if (!(sdo = ecrt_slave_config_create_sdo_request(sc_ana_in,0x6010, 0x11, 2))) {
        fprintf(stderr, "Failed to create SDO request.\n");
        return -1;
    }
    ecrt_sdo_request_timeout(sdo, 500); // ms  

    if (!(dig = ecrt_master_slave_config(
                    master,DigOutSlavePos ,Beckhoff_EP2318_0001 ))) {
        fprintf(stderr, "Failed to get slave configuration.\n");
        return -1;
    }

    if (ecrt_slave_config_pdos(dig, EC_END, slave_3_syncs)) {
        fprintf(stderr, "Failed to configure PDOs.\n");
        return -1;
    }
    if (!(lauf = ecrt_master_slave_config(
                    master,MCSLaufbandslavepos,MCS))) {
        fprintf(stderr, "Failed to get slave configuration.\n");
        return -1;
    }

    if (ecrt_slave_config_pdos(lauf, EC_END, slave_0_syncs)) {
        fprintf(stderr, "Failed to configure PDOs.\n");
        return -1;
    }
    if (!(dreh = ecrt_master_slave_config(
                    master,MCSDrehbandslavepos ,MCS))) {
        fprintf(stderr, "Failed to get slave configuration.\n");
        return -1;
    }

    if (ecrt_slave_config_pdos(dreh, EC_END, slave_1_syncs)) {
        fprintf(stderr, "Failed to configure PDOs.\n");
        return -1;
    }

    if (ecrt_domain_reg_pdo_entry_list(domain1, domain1_regs)) {
        fprintf(stderr, "PDO entry registration failed!\n");
        return -1;
    }

     
       // configure SYNC signals for this slave
    ecrt_slave_config_dc(sc_ana_in, 0x6010, PERIOD_NS, 4400000, 0, 0);
    //ACTIVATE THE MASTER. DO NOT APPLY ANY CONFIGURATION AFTER THIS, IT WON'T WORK
    printf("Activating master...\n");
    if (ecrt_master_activate(master)) {
        fprintf(stderr,"activation failed.\n"); 
        return -1;
    }
      printf("ok!\n"); 
   //INITIALIZE THE PROCESS DOMAIN MEMORY (FOR USER-SPACE APPS) 

    if (!(domain1_pd = ecrt_domain_data(domain1))) {
          fprintf(stderr,"Domain data initialization failed.\n"); 
        return -1;
    }
    
    printf("Domain data registered ok.\n"); 
         check_master_state();  
         check_domain1_state(); 
 
         
         op_flag = 0;
//ONCE THE MASTER IS ACTIVATED, THE APP IS IN CHARGE OF EXCHANGING DATA THROUGH EXPLICIT CALLS TO THE ECRT LIBRARY (DONE IN THE IDLE STATE BY THE MASTER) 
#if 1   
  
         ecrt_master_receive(master); //RECEIVE A FRAME       
         ecrt_domain_process(domain1); //DETERMINE THE DATAGRAM STATES      
        // check_slave_config_states();
          if (!op_flag)
           {check_domain1_state(); 
           }
      #if 0        
      if (slaves_up == 2 && !op_flag) 
        {  printf("All slaves reached OP state at %d cycles.\n", j);          
           op_flag = 1;        
         }  
         #endif 
      if (domain1_state.wc_state == EC_WC_COMPLETE && !op_flag) 
         {          
         printf("Domain is up at %d cycles.\n", j);          
         op_flag = 1;        
         } 
      usleep(10000); //wait 1000 ms
      printf("Starting cyclic function.\n");
      cyclic_task();
    
    //dummy to keep abrt from seeing the last datagram (which won't match) 
      ecrt_master_receive(master);    
      ecrt_domain_process(domain1); 
#endif 
     printf("...Done. Releasing the master!\n");   
     ecrt_release_master(master); // RELEASE THE MASTER INSTANCE   
     return 0; 
 }    
    
 
   

    

/****************************************************************************/
