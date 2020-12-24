/** @file
 * @defgroup blinky_example_main main.c
 * @{
 * @ingroup blinky_example_freertos
 *
 * @brief Blinky FreeRTOS Example Application main file.
 *
 * This file contains the source code for a sample application using FreeRTOS to blink LEDs.
 *
 */

#include <stdbool.h>
#include <stdint.h>

/*FreeRTOS kernel dependencies */
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
/* nRF dependencies */
#include "pca10059.h"
#include "nrf_gpio.h"
#include "nrf_drv_clock.h"
#include "nrf_delay.h"

#define TASK_DELAY        500          /** Task delay. Delays the task for x ms */
#define TIMER_PERIOD      1000          /** Timer period. Timer will expire after x ms */

TaskHandle_t  task_handle;   /** Reference to FreeRTOS task. */
TimerHandle_t timer_handle;  /** Reference to FreeRTOS timer. */

static void led_toggle_task_function (void * pvParameter);
static void led_toggle_timer_callback (void * pvParameter);
static void start_error_mode(void);

int main(void)
{
    uint32_t err_code;

    /* Configure LED-pins as outputs */
    // nrf_dfu_trigger_usb_init();  // Initialise the USB DFU trigger library
    // gpio_output_voltage_setup(); //  Increase GPIO voltage to 3.0 V in order to be able to use all LEDs
    nrf_gpio_cfg_output(LED1_G); nrf_gpio_pin_write(LED1_G, 1); // Initialise LED1_G and turn led OFF
    nrf_gpio_cfg_output(LED2_G); nrf_gpio_pin_write(LED2_G, 1); // Initialise LED2_G and turn led OFF
    nrf_gpio_cfg_output(LED2_R); nrf_gpio_pin_write(LED2_R, 1); // Initialise LED2_R and turn led OFF

    /* Initialize clock driver for better time accuracy in FREERTOS */
    err_code = nrf_drv_clock_init();
    //APP_ERROR_CHECK(err_code);
    if (err_code != 0) {
        start_error_mode();
    }

    /* Create task for LED2 blinking with priority set to 2 */
    xTaskCreate(
        led_toggle_task_function,        // Task callback function
        "LED0",                          // Task Name
        configMINIMAL_STACK_SIZE + 200,  // Stack size
        NULL,                            // Task Parameters
        2,                               // Task Priority
        &task_handle                     // Task Handle (by which the created task can be referenced)
    );

    /* Start timer for LED1 blinking */
    timer_handle = xTimerCreate(
        "LED1",                    // Timer Name
        TIMER_PERIOD,              // Timer Period in ticks
        pdTRUE,                    // Auto Reload
        NULL,                      // Timer ID
        led_toggle_timer_callback  // Timer Callback function
    );
    xTimerStart(timer_handle, 0);

    /* Activate deep sleep mode */
    SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;

    /* Start FreeRTOS scheduler. */
    vTaskStartScheduler();

    
    /* FreeRTOS won't get here unless there is insufficient RAM, 
        * because it goes back to the start of stack in vTaskStartScheduler function.
        * https://www.freertos.org/a00111.html */
    /* If the program were to get here, start error mode */
    start_error_mode();
}


/** Enter Error mode, which blinks the Red LED 
 * 
 */
static void start_error_mode(void)
{
    while (true)
    {
        nrf_gpio_pin_toggle(LED2_R);
        nrf_delay_ms(200);
    }
}


/**@brief LED0 task entry function.
 *
 * @param[in] pvParameter   Pointer that will be used as the parameter for the task.
 */
static void led_toggle_task_function (void * pvParameter)
{
    while (true)
    {
        nrf_gpio_pin_toggle(LED1_G);

        /* Delay a task for a given number of ticks */
        vTaskDelay(TASK_DELAY);

        /* Tasks must be implemented to never return... */
    }
}

/**@brief The function to call when the LED1 FreeRTOS timer expires.
 *
 * @param[in] pvParameter   Pointer that will be used as the parameter for the timer.
 */
static void led_toggle_timer_callback (void * pvParameter)
{
    nrf_gpio_pin_toggle(LED2_G);
}

/** @} */
