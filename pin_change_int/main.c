
/** @file
 * @defgroup pin_change_int_example_main main.c
 * @{
 * @ingroup pin_change_int_example
 * @brief Set the GPIOTE on an input pin to trigger an interrupt when toggled.
 * An event handler function will toggle the pin PIN_OUT_INT, while the pin PIN_OUT_NORMAL
 * will be toggled on the main loop as soon as any interrupt is triggered (because of the _WFI
 * instruction)
 *
 */

#include <stdbool.h>
#include "pca10059.h"
#include "nrf.h"
#include "nrf_drv_gpiote.h"
#include "nrf_gpio.h"

#define PIN_IN         BUTTON_1
#define PIN_OUT_INT    LED2_B
#define PIN_OUT_NORMAL LED1_G

/**
 * @brief Input pin change event handler function. When called, toggles PIN_OUT_INT state
 */
void in_pin_event_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
    nrf_gpio_pin_toggle(PIN_OUT_INT);
}

/**
 * @brief Main entry to the program
 */
int main(void)
{
    // Configure output GPIO peripheral
    nrfx_gpiote_init();
    nrf_gpio_cfg_output(PIN_OUT_INT); nrf_gpio_pin_write(PIN_OUT_INT, 1);
    nrf_gpio_cfg_output(PIN_OUT_NORMAL); nrf_gpio_pin_write(PIN_OUT_NORMAL, 1);

    // Configure input with GPIOTE (task event) peripheral. This is necessary to trigger
    // the input change interrupt
    nrf_drv_gpiote_in_config_t in_config = 
    {
        .sense           = NRF_GPIOTE_POLARITY_TOGGLE,
        .pull            = NRF_GPIO_PIN_PULLUP,
        .is_watcher      = false,
        .hi_accuracy     = true,
        .skip_gpio_setup = false,
    };
    nrfx_gpiote_in_init(PIN_IN, &in_config, in_pin_event_handler);

    nrfx_gpiote_in_event_enable(PIN_IN, true);

    while (true)
    {
        __WFI(); // Wait for interrupt
        nrf_gpio_pin_toggle(PIN_OUT_NORMAL);
    }
}


/** @} */
