
/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * Jeroen Domburg <jeroen@spritesmods.com> wrote this file. As long as you retain
 * this notice you can do whatever you want with this stuff. If we meet some day,
 * and you think this stuff is worth it, you can buy me a beer in return.
 * ----------------------------------------------------------------------------
 */


#include <esp8266.h>
#include <pwm.h>

#define BTN_GPIO_MUX  PERIPHS_IO_MUX_GPIO0_U
#define BTN_GPIO_BIT  BIT0
#define BTN_GPIO_FUNC FUNC_GPIO0

#define PWM_0_OUT_IO_MUX  PERIPHS_IO_MUX_GPIO2_U
#define PWM_0_OUT_IO_NUM  2
#define PWM_0_OUT_IO_FUNC FUNC_GPIO2
#define PWM_0_CHANNEL 0

#define PWM_1_OUT_IO_MUX  PERIPHS_IO_MUX_MTDO_U
#define PWM_1_OUT_IO_NUM  15
#define PWM_1_OUT_IO_FUNC FUNC_GPIO15
#define PWM_1_CHANNEL 1

#define PWM_2_OUT_IO_MUX  PERIPHS_IO_MUX_MTCK_U
#define PWM_2_OUT_IO_NUM  13
#define PWM_2_OUT_IO_FUNC FUNC_GPIO13
#define PWM_2_CHANNEL 2

#define PWM_3_OUT_IO_MUX  PERIPHS_IO_MUX_MTDI_U
#define PWM_3_OUT_IO_NUM  12
#define PWM_3_OUT_IO_FUNC FUNC_GPIO12
#define PWM_3_CHANNEL 3

#define PWM_PERIOD (1000)
#define PWM_MAX (PWM_PERIOD)

static ETSTimer resetBtntimer;

static uint8_t ledState = 0;

static void (*statusChangeHandler)(void) = NULL;
void ICACHE_FLASH_ATTR ioLedChangeHandler(void (*f)(void))
{
    statusChangeHandler = f;
}

void ICACHE_FLASH_ATTR ioLed(int ena) {
    if (ena) {
        pwm_set_duty(PWM_MAX, PWM_0_CHANNEL);
        pwm_set_duty(PWM_MAX, PWM_1_CHANNEL);
        pwm_set_duty(PWM_MAX, PWM_2_CHANNEL);
        pwm_set_duty(PWM_MAX, PWM_3_CHANNEL);
        ledState = 1;
    } else {
        pwm_set_duty(0, PWM_0_CHANNEL);
        pwm_set_duty(0, PWM_1_CHANNEL);
        pwm_set_duty(0, PWM_2_CHANNEL);
        pwm_set_duty(0, PWM_3_CHANNEL);
        ledState = 0;
    }
    pwm_start();
    if (statusChangeHandler != NULL)
        statusChangeHandler();
}

uint8_t ICACHE_FLASH_ATTR ioGetLed()
{
    return ledState;
}

void ICACHE_FLASH_ATTR ioLedToggle()
{
    ioLed((ledState)?0:1);
}

static void ICACHE_FLASH_ATTR resetBtnTimerCb(void *arg)
{
    static int resetCnt=0;
    if (!(gpio_input_get() & BTN_GPIO_BIT)) {
        resetCnt++;
    } else {
        if (resetCnt>=30) { //3 sec pressed
            wifi_station_disconnect();
            wifi_set_opmode(0x3); //reset to AP+STA mode
            os_printf("Reset to AP mode. Restarting system...\n");
            system_restart();
        }
        resetCnt=0;
    }
    if (resetCnt == 2) {
        ioLedToggle();
    }
}

void ICACHE_FLASH_ATTR ioPWM(const uint32_t* pwm)
{
    uint32_t pwm0 = PWM_MAX*pwm[0]/255;
    uint32_t pwm1 = PWM_MAX*pwm[1]/255;
    uint32_t pwm2 = PWM_MAX*pwm[2]/255;
    uint32_t pwm3 = PWM_MAX*pwm[3]/255;
    ledState = pwm0 || pwm1 || pwm2 || pwm3;
    pwm_set_duty(pwm0, PWM_0_CHANNEL);
    pwm_set_duty(pwm1, PWM_1_CHANNEL);
    pwm_set_duty(pwm2, PWM_2_CHANNEL);
    pwm_set_duty(pwm3, PWM_3_CHANNEL);
    pwm_start();
}


void ICACHE_FLASH_ATTR ioInitButton()
{
    PIN_FUNC_SELECT(BTN_GPIO_MUX, BTN_GPIO_FUNC);
    gpio_output_set(0, 0, 0, BTN_GPIO_BIT);
    os_timer_disarm(&resetBtntimer);
    os_timer_setfn(&resetBtntimer, resetBtnTimerCb, NULL);
    os_timer_arm(&resetBtntimer, 100, 1);
}

void ICACHE_FLASH_ATTR ioInitPwm()
{
    uint32 io_info[][3] = {
        {PWM_0_OUT_IO_MUX,PWM_0_OUT_IO_FUNC,PWM_0_OUT_IO_NUM},
        {PWM_1_OUT_IO_MUX,PWM_1_OUT_IO_FUNC,PWM_1_OUT_IO_NUM},
        {PWM_2_OUT_IO_MUX,PWM_2_OUT_IO_FUNC,PWM_2_OUT_IO_NUM},
        {PWM_3_OUT_IO_MUX,PWM_3_OUT_IO_FUNC,PWM_3_OUT_IO_NUM}
    };
    uint32 pwm_duty_init[5] = {0, 0, 0, 0};
    pwm_init(PWM_PERIOD,  pwm_duty_init, 4, io_info);
    pwm_start();
}

void ICACHE_FLASH_ATTR ioInit()
{
    ioInitButton();
    ioInitPwm();
}

