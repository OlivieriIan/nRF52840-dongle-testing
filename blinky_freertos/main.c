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

#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "bsp.h"
#include "nordic_common.h"
#include "nrf_drv_clock.h"
#include "app_error.h"
#include "nrf_delay.h"

#define TASK_DELAY        1000           /**< Task delay. Delays a LED0 task for 200 ms */
#define TIMER_PERIOD      2000          /**< Timer period. LED1 timer will expire after 1000 ms */

TaskHandle_t  led_toggle_task_handle;   /**< Reference to LED0 toggling FreeRTOS task. */
TimerHandle_t led_toggle_timer_handle;  /**< Reference to LED1 toggling FreeRTOS timer. */

static void led_toggle_task_function (void * pvParameter);
static void led_toggle_timer_callback (void * pvParameter);

int main(void)
{
    ret_code_t err_code;

    /* Initialize clock driver for better time accuracy in FREERTOS */
    err_code = nrf_drv_clock_init();
    APP_ERROR_CHECK(err_code);

    /* Configure LED-pins as outputs */
    bsp_board_init(BSP_INIT_LEDS);

    /* Create task for LED2 blinking with priority set to 2 */
    xTaskCreate(
        led_toggle_task_function,        // Task callback function
        "LED0",                          // Task Name
        configMINIMAL_STACK_SIZE + 200,  // Stack size
        NULL,                            // Task Parameters
        2,                               // Task Priority
        &led_toggle_task_handle          // Task Handle (by which the created task can be referenced)
    );

    /* Start timer for LED1 blinking */
    led_toggle_timer_handle = xTimerCreate(
        "LED1",                    // Timer Name
        TIMER_PERIOD,              // Timer Period in ticks
        pdTRUE,                    // Auto Reload
        NULL,                      // Timer ID
        led_toggle_timer_callback  // Timer Callback function
    );
    xTimerStart(led_toggle_timer_handle, 0);

    /* Activate deep sleep mode */
    SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;

    /* Start FreeRTOS scheduler. */
    vTaskStartScheduler();

    while (true)
    {
        /* FreeRTOS won't get here unless there is insufficient RAM, 
         * because it goes back to the start of stack in vTaskStartScheduler function.
         * https://www.freertos.org/a00111.html */
        /* If the program were to get here, blink the board's red LED */
        bsp_board_led_invert(BSP_BOARD_LED_1);
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
        bsp_board_led_invert(BSP_BOARD_LED_0);

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
    bsp_board_led_invert(BSP_BOARD_LED_2);
}
