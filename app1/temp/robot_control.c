/*
 * Copyright (C) 2021 HiHope Open Source Organization .
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 *
 * limitations under the License.
 */

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

#include "ohos_init.h"
#include "cmsis_os2.h"
#include "iot_gpio.h"
#include "hi_io.h"
#include "hi_time.h"
#include "robot_control.h"
#include "hi_adc.h"
#include "iot_errno.h"

#define GPIO_0 0
#define GPIO_1 1
#define GPIO_9 9
#define GPIO_10 10

#define GPIO11 11
#define GPIO12 12

#define GPIO_0_PWM3 5
#define GPIO_1_PWM4 5
#define GPIO_9_PWM0 5
#define GPIO_10_PWM1 5
#define PWM0 0
#define PWM1 1
#define PWM3 3
#define PWM4 4

#define     PWM_DUTY_LEFT_RIGHT               (5000)
#define     MAX_SPEED                         (100)
#define     MIN_SPEED                         (30000)

#define LEFT 1
#define RIGHT 2
#define ALL 3
hi_u16 clean_step=1;
hi_u32  g_car_speed = 1000;

/*correct car speed*/
hi_void correct_car_speed(hi_void)
{
    if (g_car_speed < MAX_SPEED) {
        g_car_speed = MAX_SPEED;
    }

    if (g_car_speed > MIN_SPEED) {
        g_car_speed = MIN_SPEED;
    }
}

#define     GPIO5   5
#define     FUNC_GPIO   0
#define     ADC_TEST_LENGTH             (20)
#define     VLT_MIN                     (100)
#define     OLED_FALG_ON                ((unsigned char)0x01)
#define     OLED_FALG_OFF               ((unsigned char)0x00)

osThreadId_t led_task_id;

unsigned short  g_adc_buf[ADC_TEST_LENGTH] = { 0 };
unsigned short  g_gpio5_adc_buf[ADC_TEST_LENGTH] = { 0 };
unsigned int  g_gpio5_tick = 0;
unsigned char   g_car_control_mode = 0;
unsigned char   g_car_speed_control = 0;
unsigned int  g_car_control_demo_task_id = 0;
unsigned char   g_car_status = CAR_STOP_STATUS;
unsigned char   g_wifi_status =  CAR_WIFI_CONNECT_ING;

extern float GetDistance(void);
extern void trace_module(void);
extern void car_go_back(void);
extern void car_go_forward(void);
extern void car_turn_left(void);
extern void car_turn_right(void);
extern void car_stop(void);
extern void engine_turn_left(void);
extern void engine_turn_right(void);
extern void regress_middle(void);


void switch_init(void)
{
    IoTGpioInit(5);
    hi_io_set_func(5, 0);
    IoTGpioSetDir(5, IOT_GPIO_DIR_IN);
    hi_io_set_pull(5, 1);

    IoTGpioInit(11);
    hi_io_set_func(11, 0);
    IoTGpioSetDir(11, IOT_GPIO_DIR_IN);
    hi_io_set_pull(11, 1);

    IoTGpioInit(12);
    hi_io_set_func(12, 0);
    IoTGpioSetDir(12, IOT_GPIO_DIR_IN);
    hi_io_set_pull(12, 1);
}

//????????????????????????
void gpio5_isr_func_mode(void)
{
    printf("gpio5_isr_func_mode start\n");
    unsigned int tick_interval = 0;
    unsigned int current_gpio5_tick = 0; 

    current_gpio5_tick = hi_get_tick();
    tick_interval = current_gpio5_tick - g_gpio5_tick;
    
    if (tick_interval < KEY_INTERRUPT_PROTECT_TIME) {  
        return NULL;
    }
    g_gpio5_tick = current_gpio5_tick;

    if (g_car_status == CAR_STOP_STATUS) {                
        g_car_status = CAR_TRACE_STATUS;                    //??????        
        printf("_______1______\n");
    } else if (g_car_status == CAR_TRACE_STATUS) {       
        g_car_status = CAR_STOP_STATUS; 
        printf("_______2______\n");
    } 
}

unsigned char get_gpio5_voltage(void *param)
{
    int i;
    unsigned short data;
    unsigned int ret;
    unsigned short vlt;
    float voltage;
    float vlt_max = 0;
    float vlt_min = VLT_MIN;

    hi_unref_param(param);
    memset_s(g_gpio5_adc_buf, sizeof(g_gpio5_adc_buf), 0x0, sizeof(g_gpio5_adc_buf));
    for (i = 0; i < ADC_TEST_LENGTH; i++) {
        ret = hi_adc_read(HI_ADC_CHANNEL_2, &data, HI_ADC_EQU_MODEL_4, HI_ADC_CUR_BAIS_DEFAULT, 0xF0); //ADC_Channal_2  ??????????????????  CNcomment:4????????????????????? CNend */
        if (ret != IOT_SUCCESS) {
            printf("ADC Read Fail\n");
            return  NULL;
        }    
        g_gpio5_adc_buf[i] = data;
    }

    for (i = 0; i < ADC_TEST_LENGTH; i++) {  
        vlt = g_gpio5_adc_buf[i]; 
        voltage = (float)vlt * 1.8 * 4 / 4096.0;  /* vlt * 1.8* 4 / 4096.0??????????????????????????? */
        vlt_max = (voltage > vlt_max) ? voltage : vlt_max;
        vlt_min = (voltage < vlt_min) ? voltage : vlt_min;
    }
    printf("vlt_max is %f\r\n", vlt_max);
    if (vlt_max > 0.6 && vlt_max < 1.0) {
        gpio5_isr_func_mode();
    } 
    else if(vlt_max > 1.0  &&  vlt_max < 2.0) 
    {
        return 1;
    }
    return 0;
}

//????????????
void interrupt_monitor(void)
{
    unsigned int  ret = 0;
    /*gpio5 switch2 mode*/
    g_gpio5_tick = hi_get_tick();
    ret = IoTGpioRegisterIsrFunc(GPIO5, IOT_INT_TYPE_EDGE, IOT_GPIO_EDGE_FALL_LEVEL_LOW, get_gpio5_voltage, NULL);
    if (ret == IOT_SUCCESS) {
        printf(" register gpio5\r\n");
    }
}

/*Judge steering gear*/
static unsigned int engine_go_where(void)
{
    float left_distance = 0;
    float right_distance = 0;
    /*????????????????????????????????????????????????*/

    engine_turn_left();
    hi_sleep(100);
    left_distance = GetDistance();
    hi_sleep(100);

    /*??????*/
    regress_middle();
    hi_sleep(100);

    /*????????????????????????????????????????????????*/
    engine_turn_right();
    hi_sleep(100);
    right_distance = GetDistance();
    hi_sleep(100);

    /*??????*/
    regress_middle();
    if (left_distance > right_distance) {
        return CAR_TURN_LEFT;
    } else {
        return CAR_TURN_RIGHT;
    }
}

/*??????????????????????????????????????????????????????
1?????????????????????20cm????????????
2???????????????20cm?????????????????????0.5s,?????????????????????,???????????????
*/
/*Judge the direction of the car*/
static void car_where_to_go(float distance)
{
    if (distance < DISTANCE_BETWEEN_CAR_AND_OBSTACLE) {
        car_stop();
        hi_sleep(500);
        car_go_back();
        hi_sleep(500);
        car_stop();
        unsigned int ret = engine_go_where();
        printf("ret is %d\r\n", ret);
        if (ret == CAR_TURN_LEFT) {
            car_turn_left();
            hi_sleep(500);
        } else if (ret == CAR_TURN_RIGHT) {
            car_turn_right();
            hi_sleep(500);
        }
        car_stop();
    } else {
        car_go_forward();
        } 
}

/*car mode control func*/
static void car_mode_control_func(void)
{
    float m_distance = 0.0;
    regress_middle();
    while (1) {
        if (g_car_status != CAR_OBSTACLE_AVOIDANCE_STATUS) {
            printf("car_mode_control_func 1 module changed\n");
            regress_middle();
            break;
        }

        /*???????????????????????????*/
        m_distance = GetDistance();
        car_where_to_go(m_distance);
        hi_sleep(20);
    }
}

void *RobotCarTestTask(void* param)
{
    switch_init();
    interrupt_monitor();

    while (1) {
        switch (g_car_status) {
            case CAR_STOP_STATUS:
                car_stop();
                break;
            case CAR_OBSTACLE_AVOIDANCE_STATUS:
                car_mode_control_func();
                break;
            case CAR_TRACE_STATUS:
                clean_module();//trace_module();
                break;
            default:
                break;
        }
        IoTWatchDogDisable();
        osDelay(10);        
    }
}



void RobotCarDemo(void)
{
    osThreadAttr_t attr;

    attr.name = "RobotCarTestTask";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = 10240;
    attr.priority = 25;

    if (osThreadNew(RobotCarTestTask, NULL, &attr) == NULL) {
        printf("[Ssd1306TestDemo] Falied to create RobotCarTestTask!\n");
    }
}
APP_FEATURE_INIT(RobotCarDemo);