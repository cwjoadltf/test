#include <stdio.h>
#include <securec.h>
#include <stdint.h>
#include <unistd.h>

#include "ohos_init.h"
#include "cmsis_os2.h"

#include "hi_wifi_api.h"
#include "lwip/netif.h"
#include "lwip/ip_addr.h"
#include "lwip/netifapi.h"

#include "softbus_bus_center.h"
#include "softbus_common.h"
#include "discovery_service.h"
#include "softbus_errcode.h"
#include "softbus_server_frame.h"
#include "session.h"

#include <hi_types_base.h>
#include <hi_early_debug.h>

#include "wifi_sta.h"

extern int wifi_ready;
#define SERVICE_NAME "sensor"


// static void TestDeviceFound(const DeviceInfo *device)
// {
//     printf("[client]TestDeviceFound\n");
//     OnDeviceFound(device);
// }
// static void OnDeviceFound(const DeviceInfo *device)
// {
//     if (device == NULL) {
//         printf("ondevice found device is null\n");
//         return;
//     }
//     printf("***********OnDeviceFound!!!!!******************************************\n");
//     printf("id : %s.\n", device->devId);
//     printf("name : %s.\n", device->devName);
//     printf("device type : %u.\n", device->devType);
//     printf("capNum : %u.\n", device->capabilityBitmapNum);
//     for (uint32_t i = 0; i < device->capabilityBitmapNum; i++) {
//         printf("capBitmap[%u] : %u.\n", i, device->capabilityBitmap[i]);
//     }
//     printf("addr num : %u.\n", device->addrNum);
//     printf("ip : %s.\n", device->addr[0].info.ip.ip);
//     printf("port : %d.\n", device->addr[0].info.ip.port);
//     printf("connect type : %d.\n", device->addr[0].type);
//     printf("peerUid : %s.\n", device->addr[0].peerUid);
//     printf("**********************************************************************\n");
//     return;
// }
// static void TestDiscoverFailed(int subscribeId, DiscoveryFailReason failReason)
// {
//     printf("[client]TestDiscoverFailed, subscribeId = %d, failReason = %d\n", subscribeId, failReason);
// }

// static void TestDiscoverySuccess(int subscribeId)
// {
//     printf("[client]TestDiscoverySuccess, subscribeId = %d\n", subscribeId);
// }

static void SensorPublishSuccess(int publishId)
{
    printf("[client]SensorPublishSuccess, publishId = %d\n", publishId);
}

static void SensorPublishFail(int publishId, PublishFailReason reason)
{
    printf("[client]SensorPublishSuccess, publishId = %d, PublishFailReason = %d\n", publishId, reason);
}

static PublishInfo sensor_info={
    .publishId=1,
    .mode=DISCOVER_MODE_ACTIVE,
    .medium=COAP,
    .freq=MID,
    .capability="dvKit",
    .capabilityData="sensor_date",
    .dataLen=sizeof("sensor_date"),
};//发布任务信息

static IPublishCallback sensor_callback={
    .OnPublishSuccess = SensorPublishSuccess,
    .OnPublishFail = SensorPublishFail
};//发布回调函数，printf

void Service_init()
{
    InitSoftBusServer();
    sleep(5);//等待初始化
    while(!wifi_ready)
    {
        sleep(5);
    }

    if (PublishService(SERVICE_NAME, &sensor_info, &sensor_callback) != SOFTBUS_OK) {
        printf("test failed, [%s].\n", __FUNCTION__);
        return;
    }

}


void Sb_s_Task(void *arg)
{
    (void)arg;
    
    Service_init();
	while(1)
	{
		usleep(1000);
	}
		
		usleep(1000);
	
}

// void Sb_s_Entry(void)
// {
//     osThreadAttr_t attr;

//     attr.name = "Sb_s_Task";
//     attr.attr_bits = 0U;
//     attr.cb_mem = NULL;
//     attr.cb_size = 0U;
//     attr.stack_mem = NULL;
//     attr.stack_size = 2048;
//     attr.priority = osPriorityNormal;

//     if (osThreadNew((osThreadFunc_t)Sb_s_Task, NULL, &attr) == NULL) {
//         printf("[CarExample] Falied to create CarTask!\n");
//     }
// }

// APP_FEATURE_INIT(Sb_s_Entry);


static void DSoftBus(void)
{
    printf("[%s:%d]: %s\n", __FILE__, __LINE__, __func__);

    osThreadAttr_t attr;
    attr.name = "dsoftbus task";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = 65536;
    attr.priority = 24;

    extern void InitSoftBusServer(void);
    if (osThreadNew((osThreadFunc_t) InitSoftBusServer, NULL, &attr) == NULL) {
        printf("Falied to create WifiSTATask!\n");
    }
}

APP_FEATURE_INIT(DSoftBus);