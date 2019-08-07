/******************************************************************************
 *
 *  Copyright (C) 2014 Google, Inc.
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

#define LOG_TAG "bt_hci_h5"

#include <assert.h>
#include <errno.h>
#include <string.h>

//#include "osi/include/eager_reader.h"
#include "hci/hci_hal.h"
#include "osi/osi.h"
//#include "osi/log.h"
//#include "osi/reactor.h"
#include "osi/thread.h"
#include "hci/vendor.h"

#include "hci/hci_layer.h"
#include "hci/hci_h5_int.h"


#define HCI_HAL_SERIAL_BUFFER_SIZE 1026
#define HCI_BLE_EVENT 0x3e

// Increased HCI thread priority to keep up with the audio sub-system
// when streaming time sensitive data (A2DP).
#define HCI_THREAD_PRIORITY -19

// Our interface and modules we import
static const hci_hal_t h5_interface;
static const hci_h5_t *h5_int_interface;
static const hci_hal_callbacks_t *h5_hal_callbacks;

static aos_task_t hcih5_task_handler;
static int task_run;
static aos_queue_t hcih5_queue;
static char queue_buf[HCI_H5_QUEUE_LEN * sizeof(BtTaskEvt_t)];

static serial_data_type_t current_data_type;
static bool stream_has_interpretation;
static bool stream_corruption_detected;
static uint8_t stream_corruption_bytes_to_ignore;

static void h5_data_ready_cb(BT_HDR *packet);
//static uint16_t h5_int_transmit_data_cb(serial_data_type_t type, uint8_t *data, uint16_t length) ;
static void h5_event_uart_has_bytes();

static  hci_h5_hal_callbacks_t h5_int_hal_callbacks = {
    .h5_data_ready_cb = h5_data_ready_cb,
};


// Interface functions
#if 0
static bool h5_hal_init(const hci_hal_callbacks_t *upper_callbacks)
{
    assert(upper_callbacks != NULL);
  
    h5_hal_callbacks = upper_callbacks;
    h5_int_interface = hci_get_h5_int_interface();
    h5_int_interface->h5_int_init(&h5_int_hal_callbacks);
    return true;
}
#endif

task_post_status_t hci_hal_h5_task_post(SIG_HCI_HAL_t type, task_post_t timeout)
{
    BtTaskEvt_t evt;

    evt.sig = type;
    evt.par = 0;

    if (aos_queue_send(&hcih5_queue, &evt, sizeof(BtTaskEvt_t)) == 0) {
        return TASK_POST_SUCCESS;
    }

    return TASK_POST_FAIL;
}

static void hci_hal_h5_rx_handler(void *arg)
{
    BtTaskEvt_t e;
    unsigned int len;

    while (task_run) {
        if (0 == aos_queue_recv(&hcih5_queue, AOS_WAIT_FOREVER, &e, &len)) {
#if 0
            if (e.sig == SIG_HCI_HAL_RECV_PACKET) {
                fixed_queue_process(hci_hal_env.rx_q);

            } else 
#endif
            //HCI_TRACE_ERROR("%s", __func__);
            if (e.sig == SIG_HCI_HAL_RECV_AVAILABLE) {
                h5_event_uart_has_bytes();
            }
        }
    }
}

static bool h5_hal_open(const hci_hal_callbacks_t *upper_callbacks)
{
    HCI_TRACE_DEBUG("%s", __func__);

    assert(upper_callbacks != NULL);

    int ret = aos_queue_new(&hcih5_queue, queue_buf, HCI_H5_QUEUE_LEN * sizeof(BtTaskEvt_t), sizeof(BtTaskEvt_t));

    aos_check_return_einval(!ret);
    task_run = 1;

    ret = aos_task_new_ext(&hcih5_task_handler, HCI_H5_TASK_NAME, hci_hal_h5_rx_handler,  NULL,    HCI_H5_TASK_STACK_SIZE, HCI_H5_TASK_PRIO);

    aos_check_return_einval(!ret);
  
    h5_hal_callbacks = upper_callbacks;
    h5_int_interface = hci_get_h5_int_interface();
    h5_int_interface->h5_int_init(&h5_int_hal_callbacks);
    // TODO(zachoverflow): close if already open / or don't reopen (maybe at the hci layer level)

    return true;
}

static void h5_hal_close()
{
    HCI_TRACE_DEBUG("%s", __func__);

    h5_int_interface->h5_int_cleanup();
}

static size_t h5_read_data(serial_data_type_t type, uint8_t *buffer, size_t max_size)
{
    int size = 0;

    if (type < DATA_TYPE_ACL || type > DATA_TYPE_EVENT) {
        HCI_TRACE_ERROR("%s invalid data type: %d", __func__, type);
        return 0;
    } else if (!stream_has_interpretation) {
        HCI_TRACE_ERROR("%s with no valid stream intepretation.", __func__);
        return 0;
    } else if (current_data_type != type) {
        HCI_TRACE_ERROR("%s with different type than existing interpretation.", __func__);
        return 0;
    }

    if (max_size == 0) {
        return 0;
    }

    size = h5_int_interface->h5_int_read_data(buffer, max_size);

    if (size == -1) {
        HCI_TRACE_ERROR("Error there is no data to be read, stack error or fw error!");
        return 0;
    }

    return size;
}

static void h5_packet_finished(serial_data_type_t type)
{
    if (!stream_has_interpretation) {
        HCI_TRACE_ERROR("%s with no existing stream interpretation.", __func__);
    } else if (current_data_type != type) {
        HCI_TRACE_ERROR("%s with different type than existing interpretation.", __func__);
    }

    stream_has_interpretation = false;
}

static void h5_data_ready_cb(BT_HDR *packet)
{
    stream_has_interpretation = true;
    //current_data_type = type;
    h5_hal_callbacks->packet_ready(packet);
}

static uint16_t h5_transmit_data(serial_data_type_t type, uint8_t *data, uint16_t length)
{
    assert(data != NULL);
    assert(length > 0);

    uint16_t transmitted_length = 0;
    uint16_t opcode;
    uint8_t  *data_temp = data;
    //LOG_DEBUG("hci_hal_h5 %s, data type: %d", __func__, type);

    if (type < DATA_TYPE_COMMAND || type > DATA_TYPE_SCO) {
        HCI_TRACE_ERROR("%s invalid data type: %d", __func__, type);
        return 0;
    }

    switch (type) {
        case DATA_TYPE_COMMAND:
            STREAM_TO_UINT16(opcode, data_temp);
            //LOG_DEBUG("cmd opcode  = 0x%0x", opcode);

            if (opcode == HCI_VSC_H5_INIT) {
                transmitted_length = length;
                h5_int_interface->h5_send_sync_cmd(opcode, NULL, length);
                break;
            }

            transmitted_length = h5_int_interface->h5_send_cmd(type, data, length);
            break;

        case DATA_TYPE_ACL:
            transmitted_length = h5_int_interface->h5_send_acl_data(type, data, length);
            break;

        case DATA_TYPE_SCO:
            transmitted_length = h5_int_interface->h5_send_sco_data(type, data, length);
            break;

        default:
            break;
    }

    return transmitted_length;
}

// Internal functions

// WORKAROUND:
// As exhibited by b/23934838, during result-heavy LE scans, the UART byte
// stream can get corrupted, leading to assertions caused by mis-interpreting
// the bytes following the corruption.
// This workaround looks for tell-tale signs of a BLE event and attempts to
// skip the correct amount of bytes in the stream to re-synchronize onto
// a packet boundary.
// Function returns true if |byte_read| has been processed by the workaround.
static bool stream_corrupted_during_le_scan_workaround(const uint8_t byte_read)
{
    if (!stream_corruption_detected && byte_read == HCI_BLE_EVENT) {
        HCI_TRACE_ERROR("%s HCI stream corrupted (message type 0x3E)!", __func__);
        stream_corruption_detected = true;
        return true;
    }

    if (stream_corruption_detected) {
        if (stream_corruption_bytes_to_ignore == 0) {
            stream_corruption_bytes_to_ignore = byte_read;
            HCI_TRACE_ERROR("%s About to skip %d bytes...", __func__, stream_corruption_bytes_to_ignore);
        } else {
            --stream_corruption_bytes_to_ignore;
        }

        if (stream_corruption_bytes_to_ignore == 0) {
            HCI_TRACE_ERROR("%s Back to our regularly scheduled program...", __func__);
            stream_corruption_detected = false;
        }

        return true;
    }

    return false;
}



// See what data is waiting, and notify the upper layer
static void h5_event_uart_has_bytes()
{
         h5_int_interface->h5_recv_msg(NULL, 0);
}

static const hci_hal_t h5_interface = {
    //h5_hal_init,

    h5_hal_open,
    h5_hal_close,

    h5_read_data,
    h5_packet_finished,
    h5_transmit_data,
};

const hci_hal_t *hci_hal_h5_get_interface()
{
    return &h5_interface;
}

const hci_hal_t *hci_hal_h5_get_test_interface(vendor_t *vendor_interface)
{
    return &h5_interface;
}
