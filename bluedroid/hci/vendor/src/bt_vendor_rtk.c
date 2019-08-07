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
 *  Filename:      bt_vendor_rtk.c
 *
 *  Description:   Realtek vendor specific library implementation
 *
 ******************************************************************************/

#undef NDEBUG
#define LOG_TAG "bt_vendor_uart"

#include "bt_vendor_rtk.h"
#include "upio.h"
#include <aos/kernel.h>
#include "userial_vendor.h"

/******************************************************************************
**  Externs
******************************************************************************/

void hw_config_start(void);

#if (HW_END_WITH_HCI_RESET == TRUE)
    void hw_epilog_process(void);
#endif

/******************************************************************************
**  Variables
******************************************************************************/

bt_vendor_callbacks_t *bt_vendor_cbacks = NULL;
uint8_t vnd_local_bd_addr[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

/******************************************************************************
**  Local type definitions
******************************************************************************/

/******************************************************************************
**  Static Variables
******************************************************************************/

static const tUSERIAL_CFG userial_init_cfg = {
    (USERIAL_DATABITS_8 | USERIAL_PARITY_EVEN | USERIAL_STOPBITS_1),
    USERIAL_BAUD_115200,
    USERIAL_HW_FLOW_CTRL_OFF
};

/******************************************************************************
**  Functions
******************************************************************************/

/*****************************************************************************
**
**   BLUETOOTH VENDOR INTERFACE LIBRARY FUNCTIONS
**
*****************************************************************************/

static int init(const bt_vendor_callbacks_t *p_cb, unsigned char *local_bdaddr)
{
    HCI_TRACE_DEBUG("init");

    if (p_cb == NULL) {
        LOG_ERROR("init failed with no user callbacks!");
        return -1;
    }

    userial_vendor_init();
    upio_init();
    HCI_TRACE_DEBUG("bt_wake_up_host_mode_set(1)");
    bt_wake_up_host_mode_set(1);

    /* store reference to user callbacks */
    bt_vendor_cbacks = (bt_vendor_callbacks_t *) p_cb;

    /* This is handed over from the stack */
    //memcpy(vnd_local_bd_addr, local_bdaddr, 6);

    return 0;
}



/** Requested operations */
static int op(bt_vendor_opcode_t opcode, void *param)
{
    int retval = 0;

    HCI_TRACE_DEBUG("op for %d", opcode);

    switch (opcode) {
        case BT_VND_OP_POWER_CTRL: {
            int *state = (int *) param;

            if (*state == BT_VND_PWR_OFF) {
                upio_set_bluetooth_power(UPIO_BT_POWER_OFF);
                aos_msleep(300);
                HCI_TRACE_DEBUG("set power off and delay 200ms");

            } else if (*state == BT_VND_PWR_ON) {
                upio_set_bluetooth_power(UPIO_BT_POWER_ON);
                aos_msleep(500);
                HCI_TRACE_DEBUG("set power on and delay 500ms");

            }
        }
        break;

        case BT_VND_OP_FW_CFG: {
            hw_config_start();
        }
        break;

        case BT_VND_OP_SCO_CFG: {
            retval = -1;
        }
        break;

        case BT_VND_OP_USERIAL_OPEN: {
            //int (*fd_array)[] = (int (*)[]) param;
            //int fd, idx;
            retval = userial_vendor_open((tUSERIAL_CFG *) &userial_init_cfg, param);

#if 0
            if (fd != -1) {
                for (idx = 0; idx < CH_MAX; idx++) {
                    (*fd_array)[idx] = fd;
                }

                retval = 1;
            }
#endif
            /* retval contains numbers of open fd of HCI channels */
        }
        break;

        case BT_VND_OP_USERIAL_CLOSE: {
            userial_vendor_close();
        }
        break;

        case BT_VND_OP_GET_LPM_IDLE_TIMEOUT: {

        }
        break;

        case BT_VND_OP_LPM_SET_MODE: {

        }
        break;

        case BT_VND_OP_LPM_WAKE_SET_STATE: {

        }
        break;

        case BT_VND_OP_EPILOG: {
#if (HW_END_WITH_HCI_RESET == FALSE)

            if (bt_vendor_cbacks) {
                bt_vendor_cbacks->epilog_cb(BT_VND_OP_RESULT_SUCCESS);
            }

#else
            hw_epilog_process();
#endif
        }
        break;
        default:
        break;
    }

    return retval;
}

/** Closes the interface */
static void cleanup(void)
{
    HCI_TRACE_DEBUG("cleanup");

    upio_cleanup();
    LOG_ERROR("bt_wake_up_host_mode_set(0)");
    bt_wake_up_host_mode_set(0);

    bt_vendor_cbacks = NULL;
}

// Entry point of DLib
const bt_vendor_interface_t rtk_vendor_interface = {
    sizeof(bt_vendor_interface_t),
    init,
    op,
    cleanup
};

const bt_vendor_interface_t *bt_vendor_get_interface(void)
{
    return &rtk_vendor_interface;
}
