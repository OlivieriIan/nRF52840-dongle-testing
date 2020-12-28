/**@file
 * @defgroup usbd_cdc_acm_example main.c
 * @{
 * @ingroup usbd_cdc_acm_example
 * @brief USBD CDC ACM example
 *
 */

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

#include "nrf.h"
#include "nrf_drv_usbd.h"
#include "nrf_drv_clock.h"
#include "nrf_gpio.h"
#include "pca10059.h"
#include "nrf_delay.h"
#include "nrf_drv_power.h"

#include "app_error.h"
#include "app_util.h"
#include "app_usbd_core.h"
#include "app_usbd.h"
#include "app_usbd_string_desc.h"
#include "app_usbd_cdc_acm.h"
#include "app_usbd_serial_num.h"

#include "bsp.h"

#define LED_USB_RESUME      (LED1_G)
#define LED_CDC_ACM_OPEN    (LED2_G)
#define LED_CDC_ACM_RX      (LED2_B)
#define LED_CDC_ACM_TX      (LED2_R)

static void cdc_acm_user_ev_handler(app_usbd_class_inst_t const * p_inst,
                                    app_usbd_cdc_acm_user_event_t event);


/**
 * @brief CDC_ACM class instance
 * */
APP_USBD_CDC_ACM_GLOBAL_DEF(
    m_app_cdc_acm,                           // USBD_CDC_ACM instance name
    cdc_acm_user_ev_handler,                 // User event handler
    0,                                       // Interface number of cdc_acm control
    1,                                       // Interface number of cdc_acm DATA
    NRFX_USBD_EPIN2,                         // COMM subclass IN endpoint
    NRFX_USBD_EPIN1,                         // DATA subclass IN endpoint
    NRFX_USBD_EPOUT1,                        // DATA subclass OUT endpoint
    APP_USBD_CDC_COMM_PROTOCOL_AT_V250       // CDC protocol (see app_usbd_cdc_comm_protocol_t)
);

#define READ_SIZE 1

static char m_rx_buffer[READ_SIZE];
static char m_tx_buffer[NRFX_USBD_EPSIZE];

/**
 * @brief User defined CDC ACM event handler
 */
static void cdc_acm_user_ev_handler(app_usbd_class_inst_t const * p_inst,
                                    app_usbd_cdc_acm_user_event_t event)
{
    app_usbd_cdc_acm_t const * p_cdc_acm = app_usbd_cdc_acm_class_get(p_inst);

    switch (event)
    {
        case APP_USBD_CDC_ACM_USER_EVT_PORT_OPEN:
        {
            nrf_gpio_pin_clear(LED_CDC_ACM_OPEN); // Turn ON

            /*Setup first transfer*/
            app_usbd_cdc_acm_read(&m_app_cdc_acm, m_rx_buffer, READ_SIZE);
            size_t size = sprintf(m_tx_buffer, "USB Connected!\r\n");
            app_usbd_cdc_acm_write(&m_app_cdc_acm, m_tx_buffer, size);
            break;
        }
        case APP_USBD_CDC_ACM_USER_EVT_PORT_CLOSE:
            nrf_gpio_pin_set(LED_CDC_ACM_OPEN); // Turn OFF
            break;
        case APP_USBD_CDC_ACM_USER_EVT_TX_DONE:
            nrf_gpio_pin_toggle(LED_CDC_ACM_TX);
            break;
        case APP_USBD_CDC_ACM_USER_EVT_RX_DONE:
        {
            nrf_gpio_pin_toggle(LED_CDC_ACM_RX);
            ret_code_t ret = NRF_SUCCESS;
            do
            {
                /*Get amount of data transfered*/
                size_t size = app_usbd_cdc_acm_rx_size(p_cdc_acm);

                /* Fetch data until internal buffer is empty */
                ret = app_usbd_cdc_acm_read(&m_app_cdc_acm, m_rx_buffer, size);
                app_usbd_cdc_acm_write(&m_app_cdc_acm, m_rx_buffer, size);
            } while (ret == NRF_SUCCESS);
            break;
        }
        default:
            break;
    }
}

/**
 * @brief User defined USBD event handler
 */
static void usbd_user_ev_handler(app_usbd_event_type_t event)
{
    switch (event)
    {
        case APP_USBD_EVT_DRV_SUSPEND:
            nrf_gpio_pin_set(LED_USB_RESUME); // Turn OFF
            break;
        case APP_USBD_EVT_DRV_RESUME:
            nrf_gpio_pin_clear(LED_USB_RESUME); // Turn ON
            break;
        case APP_USBD_EVT_STARTED:
            break;
        case APP_USBD_EVT_STOPPED:
            app_usbd_disable();
            // Turn off all LEDs
            nrf_gpio_pin_set(LED_USB_RESUME  );
            nrf_gpio_pin_set(LED_CDC_ACM_OPEN);
            nrf_gpio_pin_set(LED_CDC_ACM_RX  );
            nrf_gpio_pin_set(LED_CDC_ACM_TX  );
            break;
        case APP_USBD_EVT_POWER_DETECTED:
            if (!nrf_drv_usbd_is_enabled())
            {
                app_usbd_enable();
            }
            break;
        case APP_USBD_EVT_POWER_REMOVED:
            app_usbd_stop();
            break;
        case APP_USBD_EVT_POWER_READY:
            app_usbd_start();
            break;
        default:
            break;
    }
}

static void init_board(void)
{
    nrf_gpio_cfg_output(LED_USB_RESUME);   nrf_gpio_pin_write(LED_USB_RESUME, 1);
    nrf_gpio_cfg_output(LED_CDC_ACM_OPEN); nrf_gpio_pin_write(LED_CDC_ACM_OPEN, 1);
    nrf_gpio_cfg_output(LED_CDC_ACM_RX);   nrf_gpio_pin_write(LED_CDC_ACM_RX, 1);
    nrf_gpio_cfg_output(LED_CDC_ACM_TX);   nrf_gpio_pin_write(LED_CDC_ACM_TX, 1);
}

int main(void)
{
    ret_code_t ret = NRF_SUCCESS;
    static const app_usbd_config_t usbd_config = {
        .ev_state_proc = usbd_user_ev_handler
    };

    ret = nrf_drv_clock_init();
    APP_ERROR_CHECK(ret);
    
    nrf_drv_clock_lfclk_request(NULL);

    while(!nrf_drv_clock_lfclk_is_running()) { } /* Just waiting */

    init_board();

    app_usbd_serial_num_generate();

    ret = app_usbd_init(&usbd_config);
    APP_ERROR_CHECK(ret);

    app_usbd_class_inst_t const * class_cdc_acm = app_usbd_cdc_acm_class_inst_get(&m_app_cdc_acm);
    ret = app_usbd_class_append(class_cdc_acm);
    APP_ERROR_CHECK(ret);

    ret = app_usbd_power_events_enable();
    APP_ERROR_CHECK(ret);

    while (true)
    {
        while (app_usbd_event_queue_process()) { }/* Nothing to do */

        /* Sleep CPU until an event is triggered */
        __WFE();
    }
}

/** @} */
