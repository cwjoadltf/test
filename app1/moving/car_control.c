#include <stdio.h>

#include <unistd.h>

#include "ohos_init.h"
#include "cmsis_os2.h"

#include <iot_pwm.h>
#include <iot_gpio.h>

#include <unistd.h>
#include <hi_types_base.h>
#include <hi_early_debug.h>

#include <hi_pwm.h>
#include <hi_gpio.h>
#include <hi_io.h>
#include "car_control.h"

#include "iot_pwm.h"

#define GPIO0 0
#define GPIO1 1
#define GPIO9 9
#define GPIO10 10
#define GPIOFUNC 0
#define PWM_FREQ_FREQUENCY  (60000)
#define CAR_SPEED_CHANGE_VALUE   (500)
#define PWM_DUTY_STOP            (100)

struct car_sys_info car_info;


void gpio_init(hi_gpio_idx id, hi_gpio_dir  dir, hi_u8 func)
{
    hi_io_set_func(id, func);
    IoTGpioSetDir(id, IOT_GPIO_DIR_OUT);
    IoTGpioSetOutputVal(id, IOT_GPIO_VALUE0);
    IoTGpioInit(id);
}

void gpio_control(hi_gpio_idx id, hi_gpio_dir  dir,hi_gpio_value gpio_val)
{
    hi_io_set_func(id, 0);
    IoTGpioSetDir(id, dir);
    IoTGpioSetOutputVal(id, gpio_val);
}

void pwm_control(hi_gpio_idx id, hi_u16 duty)
{  
    hi_pwm_port port;
    switch(id)
    {
        case HI_GPIO_IDX_0: port=HI_PWM_PORT_PWM3;break;
        case HI_GPIO_IDX_1: port=HI_PWM_PORT_PWM4;break;
        case HI_GPIO_IDX_9: port=HI_PWM_PORT_PWM0;break;
        case HI_GPIO_IDX_10: port=HI_PWM_PORT_PWM1;break;
        default: port=HI_PWM_PORT_PWM3;
    }
    hi_io_set_func(id, 5);
    IoTPwmStart(port, duty, PWM_FREQ_FREQUENCY);
}

void io_init(void)
{
	gpio_init(HI_GPIO_IDX_0,HI_GPIO_DIR_OUT,HI_IO_FUNC_GPIO_0_GPIO);
    gpio_init(HI_GPIO_IDX_1,HI_GPIO_DIR_OUT,HI_IO_FUNC_GPIO_1_PWM4_OUT);
    gpio_init(HI_GPIO_IDX_9,HI_GPIO_DIR_OUT,HI_IO_FUNC_GPIO_9_GPIO);
    gpio_init(HI_GPIO_IDX_10,HI_GPIO_DIR_OUT,HI_IO_FUNC_GPIO_10_PWM1_OUT);
    IoTPwmInit(HI_PWM_PORT_PWM4);
    IoTPwmInit(HI_PWM_PORT_PWM1);
    IoTPwmInit(HI_PWM_PORT_PWM0);
    IoTPwmInit(HI_PWM_PORT_PWM3);
}

//CarStatus carstatus = CAR_STATUS_STOP;
//CarMode carmode = CAR_MODE_STEP;

void car_info_init(void)
{
	car_info.cur_status = CAR_STATUS_STOP;
    car_info.change_flag=0;
	car_info.speed = 50;
}
void car_init(void)
{
    io_init();
    car_info_init();
}

/*correct car speed*/
void correct_car_speed(hi_void)
{
    if (car_info.speed > 90) {
        car_info.speed = 90;
    }

    if (car_info.speed < 10) {
        car_info.speed = 10;
    }
}

//设置车速
void car_speed_set(int speed)
{
    if(speed>=10 && speed <= 90)
        car_info.speed = 100-speed;
    correct_car_speed();
    car_info.change_flag=1;
}


/*car stop */
hi_void car_stop(hi_void)
{
    correct_car_speed();
    // IoTPwmStop(HI_PWM_PORT_PWM4);
    // IoTPwmStop(HI_PWM_PORT_PWM1);
    // IoTPwmStop(HI_PWM_PORT_PWM0);
    // IoTPwmStop(HI_PWM_PORT_PWM3);
    gpio_control(HI_GPIO_IDX_0,HI_GPIO_DIR_OUT,HI_GPIO_VALUE0);
    gpio_control(HI_GPIO_IDX_9,HI_GPIO_DIR_OUT,HI_GPIO_VALUE0);
    gpio_control(HI_GPIO_IDX_1,HI_GPIO_DIR_OUT,HI_GPIO_VALUE0);
    gpio_control(HI_GPIO_IDX_10,HI_GPIO_DIR_OUT,HI_GPIO_VALUE0);
}

/*car turn right */
hi_void car_turn_left(hi_void)
{
    correct_car_speed();
    gpio_control(HI_GPIO_IDX_9,HI_GPIO_DIR_OUT,HI_GPIO_VALUE1);//右轮走
    pwm_control(HI_GPIO_IDX_0,car_info.speed);
    pwm_control(HI_GPIO_IDX_10,car_info.speed);
    gpio_control(HI_GPIO_IDX_1,HI_GPIO_DIR_OUT,HI_GPIO_VALUE1);//左轮停下
}

/*car go back */
hi_void car_go_back(hi_void)
{
    correct_car_speed();
    pwm_control(HI_GPIO_IDX_0, car_info.speed); 
    gpio_control(HI_GPIO_IDX_1, HI_GPIO_DIR_OUT,HI_GPIO_VALUE1);
    pwm_control(HI_GPIO_IDX_9, car_info.speed); 
    gpio_control( HI_GPIO_IDX_10, HI_GPIO_DIR_OUT,HI_GPIO_VALUE1);
}

/*car turn left */
hi_void car_turn_right(hi_void)
{
    correct_car_speed();
    pwm_control(HI_GPIO_IDX_1, car_info.speed); //右轮停下
    gpio_control(HI_GPIO_IDX_0, HI_GPIO_DIR_OUT,HI_GPIO_VALUE1);
    gpio_control(HI_GPIO_IDX_10, HI_GPIO_DIR_OUT,HI_GPIO_VALUE1);//左轮走
    pwm_control(HI_GPIO_IDX_9, car_info.speed);
}

void car_dubug(hi_void)
{
    printf("state:%d\n",car_info.cur_status);
    printf("change:%d\n",car_info.change_flag);
    printf("speed:%d\n",car_info.speed);
}


/*car go forward */
hi_void car_go_forward(hi_void)
{
    correct_car_speed();
    gpio_control(HI_GPIO_IDX_0, HI_GPIO_DIR_OUT,HI_GPIO_VALUE1);
    gpio_control(HI_GPIO_IDX_9, HI_GPIO_DIR_OUT,HI_GPIO_VALUE1);
    pwm_control(HI_GPIO_IDX_1, car_info.speed);
    pwm_control(HI_GPIO_IDX_10, car_info.speed);
}


void CarTask(void *arg)
{
    (void)arg;
    car_init();

    printf("hhhhhhhhhhh\n");
	while(1)
	{
		if(car_info.change_flag)
		{
			car_info.change_flag = 0;
            car_dubug();

			switch(car_info.cur_status)
			{
				case CAR_STATUS_STOP:
					car_stop();
					break;
				
				case CAR_STATUS_FORWARD:
					car_go_forward();
					break;

				case CAR_STATUS_BACKWARD:
					car_go_back();
					break;

				case CAR_STATUS_LEFT:
					car_turn_left();
					break;

				case CAR_STATUS_RIGHT:
					car_turn_left();
					break;

				default:

					break;
			}
		}
		
		usleep(10000);
	}
}

void CarExampleEntry(void)
{
    osThreadAttr_t attr;

    attr.name = "CarTask";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = 10240;
    attr.priority = osPriorityNormal;

    if (osThreadNew(CarTask, NULL, &attr) == NULL) {
        printf("[CarExample] Falied to create CarTask!\n");
    }
}

APP_FEATURE_INIT(CarExampleEntry);

