/*
 * Copyright (c) 2020 HiSilicon (Shanghai) Technologies CO., LIMITED.
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
 * limitations under the License.
 */

#ifndef __CAR_TEST_H__
#define __CAR_TEST_H__

#define CAR_STEP_COUNT      150

typedef enum {
    /*停止*/
    CAR_STATUS_STOP,

    /*前进*/
    CAR_STATUS_FORWARD,

    /*后退*/
    CAR_STATUS_BACKWARD,

    /*左转*/
    CAR_STATUS_LEFT,

    /*右转*/
    CAR_STATUS_RIGHT,

    /** Maximum value */
    CAR_STATUS_MAX
} CarStatus;



struct car_sys_info {

    CarStatus cur_status;
    
    int change_flag;
    int speed;
} ;

extern struct car_sys_info car_info;

void car_dubug(void);
void car_turn_right(void);
void car_stop(void);
void car_turn_left(void);
void car_go_back(void);
void car_go_forward(void);
void car_init(void);

#define IO_NAME_GPIO_0		0
#define IO_NAME_GPIO_1		1
#define IO_NAME_GPIO_9		9
#define IO_NAME_GPIO_10		10

#define IO_FUNC_GPIO_0_PWM3_OUT	HI_IO_FUNC_GPIO_0_PWM3_OUT
#define IO_FUNC_GPIO_1_PWM4_OUT	HI_IO_FUNC_GPIO_1_PWM4_OUT
#define IO_FUNC_GPIO_9_PWM0_OUT	HI_IO_FUNC_GPIO_9_PWM0_OUT
#define IO_FUNC_GPIO_10_PWM1_OUT	HI_IO_FUNC_GPIO_10_PWM1_OUT

#define PWM_PORT_PWM3	HI_PWM_PORT_PWM3
#define PWM_PORT_PWM4	HI_PWM_PORT_PWM4
#define PWM_PORT_PWM0	HI_PWM_PORT_PWM0
#define PWM_PORT_PWM1	HI_PWM_PORT_PWM1

#endif /* __CAR_TEST_H__ */