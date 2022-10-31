#include <stdio.h>
#include "ohos_init.h"
#include "ohos_types.h"
#include "car_control.h"
#include <unistd.h>

#include "cmsis_os2.h"
#include "softbus_bus_center.h"
#include <unistd.h>
#include <hi_types_base.h>
#include <hi_early_debug.h>

void TestTask(void *arg)
{
    while(1)
    {
        sleep(1);
    }
    
}

void TestExampleEntry(void)
{
    osThreadAttr_t attr;

    attr.name = "TestTask";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = 1024;
    attr.priority = osPriorityNormal;

    if (osThreadNew(TestTask, NULL, &attr) == NULL) {
        printf("[CarExample] Falied to create CarTask!\n");
    }
}

APP_FEATURE_INIT(TestExampleEntry);

