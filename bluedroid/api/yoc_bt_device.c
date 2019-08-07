// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <stdlib.h>
#include <string.h>
#include "yoc_bt_device.h"
#include "yoc_bt_main.h"
#include "device/controller.h"
#include "btc/btc_task.h"
#include "btc/btc_dev.h"

const uint8_t *yoc_bt_dev_get_address(void)
{
    if (yoc_bluedroid_get_status() != YOC_BLUEDROID_STATUS_ENABLED) {
	return NULL;
    }
    return controller_get_interface()->get_address()->address;
}

yoc_err_t yoc_bt_dev_set_device_name(const char *name)
{
    btc_msg_t msg;
    btc_dev_args_t arg;

    if (yoc_bluedroid_get_status() != YOC_BLUEDROID_STATUS_ENABLED) {
        return YOC_ERR_INVALID_STATE;
    }
    if (!name){
        return YOC_ERR_INVALID_ARG;
    }
    if (strlen(name) > YOC_DEV_DEVICE_NAME_MAX) {
        return YOC_ERR_INVALID_ARG;
    }

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_DEV;
    msg.act = BTC_DEV_ACT_SET_DEVICE_NAME;
    strcpy(arg.set_dev_name.device_name, name);

    return (btc_transfer_context(&msg, &arg, sizeof(btc_dev_args_t), NULL) == BT_STATUS_SUCCESS ? YOC_OK : YOC_FAIL);
}
