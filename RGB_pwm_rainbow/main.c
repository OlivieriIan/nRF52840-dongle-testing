/** @file
 * @defgroup Rainbow RGB LED PWM example main.c
 * @{
 * @ingroup pwm_example
 *
 * @brief  PWM Example Application main file.
 *
 * Based on <SDK>/peripheral/pwm_library/main.c
 */

#include <stdbool.h>
#include <stdint.h>
#include "nrf.h"
#include "app_error.h"
#include "bsp.h"
#include "nrf_delay.h"
#include "app_pwm.h"

#include "color.h"

// Note: Timers are enabled in config/sdk_config.h
APP_PWM_INSTANCE(PWM_R, 1);              // Create the instance "PWM_R" using TIMER1.
APP_PWM_INSTANCE(PWM_G, 2);              // Create the instance "PWM_G" using TIMER2.
APP_PWM_INSTANCE(PWM_B, 3);              // Create the instance "PWM_B" using TIMER3.

/* ================ Function Declaration ======================================================== */
static void init_PWM(app_pwm_t const * const p_PWM, uint32_t pin, uint32_t period);
static void update_rainbow_effect(uint32_t speed);
static void set_RGB_PWM(uint32_t R, uint32_t G, uint32_t B);
static void start_error_mode(void);
void pwm_ready_callback(uint32_t pwm_id);

/* ================ Function Definition ========================================================= */
int main(void)
{
    const uint32_t pwm_period_us = 1000L; // period [us] == 1KHz

    /* Initialise error LED and and turn it OFF by default */
    nrf_gpio_cfg_output(LED1_G); nrf_gpio_pin_write(LED1_G, 1);

    /* Initialise RGB LED PWMs */
    init_PWM(&PWM_R, LED2_R, pwm_period_us);
    init_PWM(&PWM_G, LED2_G, pwm_period_us);
    init_PWM(&PWM_B, LED2_B, pwm_period_us);

    while (true)
    {
        update_rainbow_effect(1);
        nrf_delay_ms(100);
    }
}

/**@brief Enter Error mode, which blinks the Red LED */
static void start_error_mode(void)
{
    while (true)
    {
        nrf_gpio_pin_toggle(LED1_G);
        nrf_delay_ms(500);
    }
}

/**@brief Initialize PWM peripheral with 1CH connected to the given pin */
static void init_PWM(app_pwm_t const * const p_PWM, uint32_t pin, uint32_t period)
{
    ret_code_t err_code;
    app_pwm_config_t pwm_cfg = {
        .pins            = {pin, APP_PWM_NOPIN},
        .pin_polarity    = {APP_PWM_POLARITY_ACTIVE_LOW, APP_PWM_POLARITY_ACTIVE_LOW},
        .num_of_channels = 1,
        .period_us       = period
    };
    err_code = app_pwm_init(p_PWM, &pwm_cfg, pwm_ready_callback);
    if (err_code != 0) start_error_mode();
    app_pwm_enable(p_PWM);
}

/**@brief Set RGB LEDs PWM values based on the angle in the colorwheel. 
 *
 * @param[in] angle_delta   Rate of change of the colorwheel rotation in %. 
 * Value should fall between -100 and 100;
 */
static void update_rainbow_effect(uint32_t angle_delta)
{
    static HsvColor hsv= {
        .h = 0,
        .s = 100,
        .v = 100
    };

    hsv.h += angle_delta;
    if (hsv.h >= 360) hsv.h = hsv.h - 360;

    RgbColor rgb = HsvToRgb(hsv);

    // Update LEDs PWM values
    set_RGB_PWM(rgb.r, rgb.g, rgb.b);
}

/**@brief Set R, G and B related PWMs values */
static void set_RGB_PWM(uint32_t R, uint32_t G, uint32_t B)
{
    while (app_pwm_channel_duty_set(&PWM_R, 0, R) == NRF_ERROR_BUSY);
    while (app_pwm_channel_duty_set(&PWM_G, 0, G) == NRF_ERROR_BUSY);
    while (app_pwm_channel_duty_set(&PWM_B, 0, B) == NRF_ERROR_BUSY);
}

/**@brief PWM ready callback function */
void pwm_ready_callback(uint32_t pwm_id)
{

}

/** @} */
