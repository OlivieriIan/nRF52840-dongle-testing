/** @file
 * @defgroup nrf_dev_timer_example_main main.c
 * @{
 * @ingroup nrf_dev_timer_example
 * @brief Timer Example Application main file.
 *
 * This file contains the source code for a sample application using Timer0.
 *
 */

#include <stdbool.h>
#include <stdint.h>
#include "nrf.h"
#include "pca10059.h"
#include "nrf_gpio.h"
#include "nrf_drv_timer.h"
#include "app_error.h"
#include "nrf_delay.h"

static const uint8_t g_leds[] = LEDS_LIST;
const nrf_drv_timer_t TIMER_INST = NRF_DRV_TIMER_INSTANCE(0);

static void start_error_mode(void);
void timer_led_event_handler(nrf_timer_event_t event_type, void* p_context);

/**
 * @brief Function for main application entry.
 */
int main(void)
{
    /* Timer program exmplanation
     * We want a blink rate of time ms @ frequency hz.
     * Given a timer frequency = 16MHZ => 1 tick = 62.5ns
     * If the blink time = 500 ms => the timer needs 8 million ticks to trigger the interruption
     * at the correct time.
     * For 8M ticks, we will need a timer count resolution of minimum 24 bits (max ~16M ticks)
     */
    uint32_t time_ms = 500; //Time(in miliseconds) between consecutive compare events.
    uint32_t time_ticks = 0;
    uint32_t err_code = NRF_SUCCESS;

    //Configure all leds on board as outputs and set them off by default
    nrf_gpio_cfg_output(LED1_G); nrf_gpio_pin_write(LED1_G, 1);
    nrf_gpio_cfg_output(LED2_R); nrf_gpio_pin_write(LED2_R, 1);
    nrf_gpio_cfg_output(LED2_G); nrf_gpio_pin_write(LED2_G, 1);
    nrf_gpio_cfg_output(LED2_B); nrf_gpio_pin_write(LED2_B, 1);

    //Configure and initialise the timer
    nrf_drv_timer_config_t timer_cfg = 
    {
        .frequency          = NRF_TIMER_FREQ_16MHz,
        .mode               = NRF_TIMER_MODE_TIMER,
        .bit_width          = NRF_TIMER_BIT_WIDTH_24, // Minimum bits needed for the timer to
                                                      // count up to the required ticks to generate
                                                      // the interrupt
        .interrupt_priority = NRFX_TIMER_DEFAULT_CONFIG_IRQ_PRIORITY,
        .p_context          = NULL
    };

    err_code = nrfx_timer_init(&TIMER_INST, &timer_cfg, timer_led_event_handler);
    if(err_code != 0) start_error_mode();

    time_ticks = nrfx_timer_ms_to_ticks(&TIMER_INST, time_ms); // 8 million ticks for 500ms @ 16MHz

    nrfx_timer_extended_compare(
        &TIMER_INST,                         // Timer instance
        NRF_TIMER_CC_CHANNEL0,               // Timer compare channel number
        time_ticks,                          // Compare value in ticks
        NRF_TIMER_SHORT_COMPARE0_CLEAR_MASK, // Shortcut between the compare event on the channel
                                             // and the timer task
        true                                 // Enable interrupt
    );

    nrfx_timer_enable(&TIMER_INST);

    while (1)
    {
        __WFI(); // Wait for interrupt
    }
}

/** 
 * @brief: Enter Error mode, where the program gets stuck forever
 */
static void start_error_mode(void)
{
    while (true)
    {
        nrf_gpio_pin_toggle(LED2_R);
        nrf_delay_ms(200);
    }
}

/**
 * @brief Handler for timer events.
 */
void timer_led_event_handler(nrf_timer_event_t event_type, void* p_context)
{
    static uint32_t i = 0;
    if(++i >= LEDS_NUMBER) i = 0;

    switch (event_type)
    {
        case NRF_TIMER_EVENT_COMPARE0:
            nrf_gpio_pin_toggle(g_leds[i]);
            break;
        default:
            //Do nothing.
            break;
    }
}

/** @} */
