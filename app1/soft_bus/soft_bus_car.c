#include <stdio.h>

#include <unistd.h>

#include "ohos_init.h"
#include "cmsis_os2.h"

#include "hi_wifi_api.h"
#include "lwip/ip_addr.h"
#include "lwip/netifapi.h"

#include "softbus_bus_center.h"
#include "softbus_common.h"
#include "discovery_service.h"
#include "session.h"

#include <unistd.h>
#include <hi_types_base.h>
#include <hi_early_debug.h>

#include <hi_gpio.h>
#include <hi_io.h>




void Softbus_car_Task(void *arg)
{
    (void)arg;
    

	while(1)
	{
		
	}
		
		usleep(1000);
	
}

void Softbus_car_ExampleEntry(void)
{
    osThreadAttr_t attr;

    attr.name = "Softbus_car_Task";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = 10240;
    attr.priority = osPriorityNormal;

    if (osThreadNew(Softbus_car_Task, NULL, &attr) == NULL) {
        printf("[CarExample] Falied to create CarTask!\n");
    }
}

APP_FEATURE_INIT(Softbus_car_ExampleEntry);