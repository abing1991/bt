/******************************************************************************
 *
 *  Copyright (C) 2009-2012 Realtek Corporation
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at:
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 ******************************************************************************/

/******************************************************************************
 *
 *  Filename:      userial_vendor.c
 *
 *  Description:   Contains vendor-specific userial functions
 *
 ******************************************************************************/
#undef NDEBUG
#define LOG_TAG "bt_userial_vendor"

//#include <utils/Log.h>
#include <termios.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include "bt_vendor_rtk.h"
//#include "userial.h"
#include "userial_vendor.h"
#include <osi/thread.h>

#include <devices/uart.h>

/******************************************************************************
**  Constants & Macros
******************************************************************************/
#ifndef CONFIG_HCI_UART_ID
#define CONFIG_HCI_UART_ID 0
#endif

#ifndef VNDUSERIAL_DBG
    #define VNDUSERIAL_DBG TRUE
#endif

#if (VNDUSERIAL_DBG == TRUE)
    #define VNDUSERIALDBG(param, ...) {ALOGD(param, ## __VA_ARGS__);}
#else
    #define VNDUSERIALDBG(param, ...) {}
#endif

#define VND_PORT_NAME_MAXLEN    256

#ifndef BT_CHIP_HW_FLOW_CTRL_ON
    #define BT_CHIP_HW_FLOW_CTRL_ON TRUE
#endif



/******************************************************************************
**  Local type definitions
******************************************************************************/

/* vendor serial control block */
typedef struct {
    dev_t* uart_dev;
    uart_config_t config;
    //struct termios termios;     /* serial terminal of BT port */
    //char port_name[VND_PORT_NAME_MAXLEN];
} vnd_userial_cb_t;

/******************************************************************************
**  Static variables
******************************************************************************/

static vnd_userial_cb_t vnd_userial;


static void uart_event(dev_t *dev, int event_id, void *priv)
{
    if (event_id == USART_EVENT_READ || event_id == USART_OVERFLOW) {
        hci_hal_h5_task_post(SIG_HCI_HAL_RECV_AVAILABLE, 100);
    }
}

/*****************************************************************************
**   Helper Functions
*****************************************************************************/

/*******************************************************************************
**
** Function        userial_to_tcio_baud
**
** Description     helper function converts USERIAL baud rates into TCIO
**                  conforming baud rates
**
** Returns         TRUE/FALSE
**
*******************************************************************************/
uint8_t userial_to_tcio_baud(uint8_t cfg_baud, uint32_t *baud)
{
    if (cfg_baud == USERIAL_BAUD_115200) {
        *baud = 115200;
    } else if (cfg_baud == USERIAL_BAUD_4M) {
        *baud = 4000000;
    } else if (cfg_baud == USERIAL_BAUD_3M) {
        *baud = 3000000;
    } else if (cfg_baud == USERIAL_BAUD_2M) {
        *baud = 2000000;
    } else if (cfg_baud == USERIAL_BAUD_1M) {
        *baud = 1000000;
    } else if (cfg_baud == USERIAL_BAUD_1_5M) {
        *baud = 1500000;
    } else if (cfg_baud == USERIAL_BAUD_921600) {
        *baud = 921600;
    } else if (cfg_baud == USERIAL_BAUD_460800) {
        *baud = 460800;
    } else if (cfg_baud == USERIAL_BAUD_230400) {
        *baud = 230400;
    } else if (cfg_baud == USERIAL_BAUD_57600) {
        *baud = 57600;
    } else if (cfg_baud == USERIAL_BAUD_19200) {
        *baud = 19200;
    } else if (cfg_baud == USERIAL_BAUD_9600) {
        *baud = 9600;
    } else if (cfg_baud == USERIAL_BAUD_1200) {
        *baud = 1200;
    } else if (cfg_baud == USERIAL_BAUD_600) {
        *baud = 600;
    } else {
        LOG_ERROR("userial vendor open: unsupported baud idx %i", cfg_baud);
        *baud = 115200;
        return FALSE;
    }

    return TRUE;
}

#if (BT_WAKE_VIA_USERIAL_IOCTL==TRUE)
/*******************************************************************************
**
** Function        userial_ioctl_init_bt_wake
**
** Description     helper function to set the open state of the bt_wake if ioctl
**                  is used. it should not hurt in the rfkill case but it might
**                  be better to compile it out.
**
** Returns         none
**
*******************************************************************************/
void userial_ioctl_init_bt_wake(int fd)
{
    uint32_t bt_wake_state;

    /* assert BT_WAKE through ioctl */
    ioctl(fd, USERIAL_IOCTL_BT_WAKE_ASSERT, NULL);
    ioctl(fd, USERIAL_IOCTL_BT_WAKE_GET_ST, &bt_wake_state);
    VNDUSERIALDBG("userial_ioctl_init_bt_wake read back BT_WAKE state=%i", \
                  bt_wake_state);
}
#endif // (BT_WAKE_VIA_USERIAL_IOCTL==TRUE)


/*****************************************************************************
**   Userial Vendor API Functions
*****************************************************************************/

/*******************************************************************************
**
** Function        userial_vendor_init
**
** Description     Initialize userial vendor-specific control block
**
** Returns         None
**
*******************************************************************************/
void userial_vendor_init()
{
    //vnd_userial.fd = -1;
    //snprintf(vnd_userial.port_name, VND_PORT_NAME_MAXLEN, "%s",
    //        bt_device_node);
}


/*******************************************************************************
**
** Function        userial_vendor_open
**
** Description     Open the serial port with the given configuration
**
** Returns         device fd
**
*******************************************************************************/
int userial_vendor_open(tUSERIAL_CFG *p_cfg, void *arg)
{
    uint32_t baud;
    hal_uart_data_width_t data_bits;
    hal_uart_parity_t parity;
    hal_uart_stop_bits_t stop_bits;
    dev_t **dev = (dev_t **)arg;

    if (!userial_to_tcio_baud(p_cfg->baud, &baud)) {
        return -1;
    }

    if (p_cfg->fmt & USERIAL_DATABITS_8) {
        data_bits = DATA_WIDTH_8BIT;
    } else if (p_cfg->fmt & USERIAL_DATABITS_7) {
        data_bits = DATA_WIDTH_7BIT;
    } else if (p_cfg->fmt & USERIAL_DATABITS_6) {
        data_bits = DATA_WIDTH_6BIT;
    } else if (p_cfg->fmt & USERIAL_DATABITS_5) {
        data_bits = DATA_WIDTH_5BIT;
    } else {
        LOG_ERROR("userial vendor open: unsupported data bits");
        return -1;
    }

    if (p_cfg->fmt & USERIAL_PARITY_NONE) {
        parity = PARITY_NONE;
    } else if (p_cfg->fmt & USERIAL_PARITY_EVEN) {
        parity = PARITY_EVEN;
    } else if (p_cfg->fmt & USERIAL_PARITY_ODD) {
        parity = PARITY_ODD;
    } else {
        LOG_ERROR("userial vendor open: unsupported parity bit mode");
        return -1;
    }

    if (p_cfg->fmt & USERIAL_STOPBITS_1) {
        stop_bits = STOP_BITS_1;
    } else if (p_cfg->fmt & USERIAL_STOPBITS_2) {
        stop_bits = STOP_BITS_2;
    } else {
        LOG_ERROR("userial vendor open: unsupported stop bits");
        return -1;
    }

    vnd_userial.uart_dev = uart_open_id("uart", CONFIG_HCI_UART_ID);

    if (vnd_userial.uart_dev == NULL) {
        return -1;
    }

    uart_config_default(&vnd_userial.config);
    vnd_userial.config.baud_rate = 115200;
    vnd_userial.config.parity = parity;
    vnd_userial.config.data_width = data_bits;
    vnd_userial.config.stop_bits = stop_bits;
    uart_config(vnd_userial.uart_dev, &vnd_userial.config);

    uart_set_event(vnd_userial.uart_dev, uart_event, NULL);

    //uart_send(vnd_userial.uart_dev, "hello world", 9);
    
    *dev = vnd_userial.uart_dev;

    HCI_TRACE_DEBUG("device fd open");

    return 0;
}

/*******************************************************************************
**
** Function        userial_vendor_close
**
** Description     Conduct vendor-specific close work
**
** Returns         None
**
*******************************************************************************/
void userial_vendor_close(void)
{
    aos_check_param(vnd_userial.uart_dev);

    uart_close(vnd_userial.uart_dev);
    vnd_userial.uart_dev = NULL;

}

/*******************************************************************************
**
** Function        userial_vendor_set_baud
**
** Description     Set new baud rate
**
** Returns         None
**
*******************************************************************************/
void userial_vendor_set_baud(uint8_t userial_baud)
{
    aos_check_param(vnd_userial.uart_dev);
    uint32_t tcio_baud;
    HCI_TRACE_DEBUG("userial_vendor_set_baud++");
    userial_to_tcio_baud(userial_baud, &tcio_baud);

    vnd_userial.config.baud_rate = tcio_baud;
    uart_config(vnd_userial.uart_dev, &vnd_userial.config);
    uart_set_buffer_size(vnd_userial.uart_dev, 10240);
}

/*******************************************************************************
**
** Function        userial_vendor_ioctl
**
** Description     ioctl inteface
**
** Returns         None
**
*******************************************************************************/
void userial_vendor_ioctl(userial_vendor_ioctl_op_t op, void *p_data)
{
    switch (op) {
#if (BT_WAKE_VIA_USERIAL_IOCTL==TRUE)

        case USERIAL_OP_ASSERT_BT_WAKE:
            VNDUSERIALDBG("## userial_vendor_ioctl: Asserting BT_Wake ##");
            ioctl(vnd_userial.fd, USERIAL_IOCTL_BT_WAKE_ASSERT, NULL);
            break;

        case USERIAL_OP_DEASSERT_BT_WAKE:
            VNDUSERIALDBG("## userial_vendor_ioctl: De-asserting BT_Wake ##");
            ioctl(vnd_userial.fd, USERIAL_IOCTL_BT_WAKE_DEASSERT, NULL);
            break;

        case USERIAL_OP_GET_BT_WAKE_STATE:
            ioctl(vnd_userial.fd, USERIAL_IOCTL_BT_WAKE_GET_ST, p_data);
            break;
#endif  //  (BT_WAKE_VIA_USERIAL_IOCTL==TRUE)

        default:
            break;
    }
}

/*******************************************************************************
**
** Function        userial_set_port
**
** Description     Configure UART port name
**
** Returns         0 : Success
**                 Otherwise : Fail
**
*******************************************************************************/
int userial_set_port(char *p_conf_name, char *p_conf_value, int param)
{
    return 0;
}

/*******************************************************************************
**
** Function        userial_vendor_set_hw_fctrl
**
** Description     Conduct vendor-specific close work
**
** Returns         None
**
*******************************************************************************/
void userial_vendor_set_hw_fctrl(uint8_t hw_fctrl)
{
    aos_check_param(vnd_userial.uart_dev);

    if (hw_fctrl) {
        vnd_userial.config.flow_control = FLOW_CONTROL_CTS_RTS;
    } else {
        vnd_userial.config.flow_control = FLOW_CONTROL_DISABLED;
    }
}
