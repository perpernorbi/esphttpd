
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

#define PWM_W_OUT_IO_MUX  PERIPHS_IO_MUX_GPIO2_U
#define PWM_W_OUT_IO_NUM  2
#define PWM_W_OUT_IO_FUNC FUNC_GPIO2
#define PWM_W_CHANNEL 0

#define PWM_R_OUT_IO_MUX  PERIPHS_IO_MUX_MTDO_U
#define PWM_R_OUT_IO_NUM  15
#define PWM_R_OUT_IO_FUNC FUNC_GPIO15
#define PWM_R_CHANNEL 1

#define PWM_G_OUT_IO_MUX  PERIPHS_IO_MUX_MTCK_U
#define PWM_G_OUT_IO_NUM  13
#define PWM_G_OUT_IO_FUNC FUNC_GPIO13
#define PWM_G_CHANNEL 2

#define PWM_B_OUT_IO_MUX  PERIPHS_IO_MUX_MTDI_U
#define PWM_B_OUT_IO_NUM  12
#define PWM_B_OUT_IO_FUNC FUNC_GPIO12
#define PWM_B_CHANNEL 3

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
        pwm_set_duty(PWM_MAX, PWM_W_CHANNEL);
        pwm_set_duty(PWM_MAX, PWM_R_CHANNEL);
        pwm_set_duty(PWM_MAX, PWM_G_CHANNEL);
        pwm_set_duty(PWM_MAX, PWM_B_CHANNEL);
        ledState = 1;
    } else {
        pwm_set_duty(0, PWM_W_CHANNEL);
        pwm_set_duty(0, PWM_R_CHANNEL);
        pwm_set_duty(0, PWM_G_CHANNEL);
        pwm_set_duty(0, PWM_B_CHANNEL);
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

int dr,dg,db;

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

void ICACHE_FLASH_ATTR ioRGB(int r, int g, int b)
{
    dr = PWM_MAX*r/255;
    dg = PWM_MAX*g/255;
    db = PWM_MAX*b/255;
    ledState = 1;
    //pwm_set_duty(PWM_MAX, PWM_W_CHANNEL);
    pwm_set_duty(dr, PWM_R_CHANNEL);
    pwm_set_duty(dg, PWM_G_CHANNEL);
    pwm_set_duty(db, PWM_B_CHANNEL);
    pwm_start();}


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
        {PWM_W_OUT_IO_MUX,PWM_W_OUT_IO_FUNC,PWM_W_OUT_IO_NUM},
        {PWM_R_OUT_IO_MUX,PWM_R_OUT_IO_FUNC,PWM_R_OUT_IO_NUM},
        {PWM_G_OUT_IO_MUX,PWM_G_OUT_IO_FUNC,PWM_G_OUT_IO_NUM},
        {PWM_B_OUT_IO_MUX,PWM_B_OUT_IO_FUNC,PWM_B_OUT_IO_NUM}
    };
    uint32 pwm_duty_init[5] = {1, 20, 30, 1};
    pwm_init(PWM_PERIOD,  pwm_duty_init, 4, io_info);
    pwm_start();
}

void ICACHE_FLASH_ATTR ioInit()
{
    ioInitButton();
    ioInitPwm();
}

