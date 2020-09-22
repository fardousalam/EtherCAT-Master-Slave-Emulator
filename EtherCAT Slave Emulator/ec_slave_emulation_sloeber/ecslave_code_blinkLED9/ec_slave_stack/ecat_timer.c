//kernel
#ifdef kernel
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/hrtimer.h>
#include <linux/hardirq.h>
#include <linux/ktime.h>

#include "xgeneral.h"
#include "ethercattype.h"
#include "ecs_slave.h"
#include "ec_regs.h"
#include "ecat_timer.h"

DEFINE_SPINLOCK(timer_sync);
static struct list_head ecat_events;
static struct hrtimer ecat_hrtimer;
static int ecat_delay_ns = 0;
static ktime_t ecat_ktime;

void (*stop_timer)(void) = 0;

enum hrtimer_restart ecat_timer(struct hrtimer *timer)
{
	struct ecat_event *ev;
	struct list_head* temp;
	struct list_head* temp2;

	hrtimer_add_expires_ns(&ecat_hrtimer, ecat_delay_ns);
	list_for_each_safe(temp, temp2, &ecat_events){
		ev = list_entry(temp, struct ecat_event, list);
		ev->action(ev->__private);
		list_del_init(&ev->list);
	}
	return HRTIMER_RESTART;
}

void ecat_schedule_timed_event(void *private,struct ecat_event *ev, void (*action)(void *))
{
	ev->action = action;
	ev->__private = private;

	spin_lock_bh(&timer_sync);
        list_add_tail(&ecat_events, &ev->list);
	spin_unlock_bh(&timer_sync);
}

void ecat_hrtimer_exit(void)
{
	int ret;

	ret = hrtimer_cancel(&ecat_hrtimer);
	if (ret) {
		printk("The timer was still in use...\n");
		return;
	}
	printk("HR Timer module uninstalling\n");
}


void ecat_create_timer(void)
{
	
	long nsec, sec;
	
	INIT_LIST_HEAD(&ecat_events);
	sec = ecat_get_dcstart(0);
	nsec = do_div(sec, NSEC_PER_SEC);

	ecat_ktime = ktime_set(sec, nsec);
	hrtimer_init(&ecat_hrtimer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
	ecat_hrtimer.function = ecat_timer;
	ecat_delay_ns = nsec;
	stop_timer = ecat_hrtimer_exit;
}

void ecat_wake_timer()
{
//	hrtimer_start(&ecat_hrtimer, ecat_ktime, HRTIMER_MODE_REL);
	printk("skipping hrtimer\n");
}

void ecat_timer_exit(void)
{
	if (stop_timer)
		stop_timer();
}
#endif 

//user


#ifdef user
#include "xgeneral.h"
#include "ethercattype.h"
#include "ecs_slave.h"
#include "ec_mbox.h"
#include "ec_net.h"
#include "ec_sii.h"
#include "ecat_timer.h"
#include "ec_regs.h"
#include "ec_device.h"
#include <semaphore.h>
#include <pthread.h>

static sem_t timersem;
static int ecat_timer_run  = 0;
static pthread_mutex_t timer_sync;

LIST_HEAD(events_head, ecat_event) ecat_events;

void *ecat_timer(void *dummy __attribute__ ((unused)) )
{
	uint32_t t;
	struct timespec tm ={0};
	struct timespec rem = {0};
	struct ecat_event *ev;
	uint32_t local_starttime;

	ecat_timer_run = 1;
	sem_wait(&timersem);
	/* 
	 * get starting time delay 
	*/
	local_starttime = ecat_get_dcstart(0) + ecat_systime_offset();
	tm.tv_nsec = local_starttime % NSEC_PER_SEC;
	tm.tv_sec = local_starttime/NSEC_PER_SEC;

	nanosleep(&tm, &rem);
	t = ecat_local_time();
	ecat_set_dcstart(0,(uint8_t *)&t, sizeof(t));

	while(1) {
		pthread_mutex_lock(&timer_sync);
		while (LIST_FIRST(&ecat_events) != NULL) {
		      	ev = LIST_FIRST(&ecat_events);
			ev->action(ev->__private);
			LIST_REMOVE(ev, list);
			ev->action = 0; /* for debug purpose*/
		}
		pthread_mutex_unlock(&timer_sync);
		/* calculate next interval */
		tm.tv_sec  = 0;
		tm.tv_nsec = ecat_cyclic_interval_ns();
		nanosleep(&tm, &rem);
		t = ecat_local_time();
		ecat_set_dcstart(0, (uint8_t *)&t,sizeof(t));
	}
}

void  ecat_schedule_timed_event(void *private,struct ecat_event *event, void (*action)(void *))
{
	event->action = action;
	event->__private = private;
	pthread_mutex_lock(&timer_sync);
        LIST_INSERT_HEAD(&ecat_events, event, list);
	pthread_mutex_unlock(&timer_sync);
}

void ecat_create_timer(void)
{
	int ret;
	pthread_t thread;
	pthread_attr_t attr;
	pthread_mutexattr_t mta;

	if (ecat_timer_run){
		return;
	}
	sem_init(&timersem, 0, 0);
	pthread_mutexattr_init(&mta);
	pthread_mutex_init(&timer_sync, &mta);


	pthread_attr_init(&attr);
	LIST_INIT(&ecat_events);
	ret = pthread_create(&thread , &attr, &ecat_timer, 0);
	if (ret) {
		printf("failed to create"
		" timer thread %s\n",strerror(ret));
		sleep(1);
		exit(0);
	}
}

void ecat_wake_timer(void)
{
	uint32_t interval_ns;

	if (ecat_timer_run == 0){
		printf("Insane 980 before 9a0\n");
		return;
	}
	interval_ns = ecat_cyclic_interval_ns();
	printf("interval %dns\n", interval_ns);
	if (interval_ns < 1000000){
		printf("WARNING: user space ecat cannot "
			"handle less than 1ms intervals");
	}
	sem_post(&timersem);
}
#endif
