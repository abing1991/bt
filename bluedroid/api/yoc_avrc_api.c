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

#include "common/bt_target.h"
#include <string.h>
#include "yoc_err.h"
#include "yoc_avrc_api.h"
#include "yoc_bt_main.h"
#include "btc/btc_manage.h"
#include "btc_avrc.h"

#if BTC_AV_INCLUDED

yoc_err_t yoc_avrc_ct_register_callback(yoc_avrc_ct_cb_t callback)
{
    if (yoc_bluedroid_get_status() != YOC_BLUEDROID_STATUS_ENABLED) {
        return YOC_ERR_INVALID_STATE;
    }

    if (callback == NULL) {
        return YOC_FAIL;
    }

    btc_profile_cb_set(BTC_PID_AVRC, callback);
    return YOC_OK;
}

yoc_err_t yoc_avrc_ct_init(void)
{
    if (yoc_bluedroid_get_status() != YOC_BLUEDROID_STATUS_ENABLED) {
        return YOC_ERR_INVALID_STATE;
    }

    btc_msg_t msg;

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_AVRC;
    msg.act = BTC_AVRC_CTRL_API_INIT_EVT;

    /* Switch to BTC context */
    bt_status_t stat = btc_transfer_context(&msg, NULL, 0, NULL);
    return (stat == BT_STATUS_SUCCESS) ? YOC_OK : YOC_FAIL;
}

yoc_err_t yoc_avrc_ct_deinit(void)
{
    if (yoc_bluedroid_get_status() != YOC_BLUEDROID_STATUS_ENABLED) {
        return YOC_ERR_INVALID_STATE;
    }

    btc_msg_t msg;

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_AVRC;
    msg.act = BTC_AVRC_CTRL_API_DEINIT_EVT;

    /* Switch to BTC context */
    bt_status_t stat = btc_transfer_context(&msg, NULL, 0, NULL);
    return (stat == BT_STATUS_SUCCESS) ? YOC_OK : YOC_FAIL;
}

yoc_err_t yoc_avrc_ct_send_set_player_value_cmd(uint8_t tl, uint8_t attr_id, uint8_t value_id)
{
    if (yoc_bluedroid_get_status() != YOC_BLUEDROID_STATUS_ENABLED) {
        return YOC_ERR_INVALID_STATE;
    }

    if (tl >= 16 || attr_id > YOC_AVRC_PS_MAX_ATTR - 1) {
        return YOC_FAIL;
    }

    btc_msg_t msg;
    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_AVRC;
    msg.act = BTC_AVRC_CTRL_API_SET_PLAYER_SETTING_EVT;

    btc_avrc_args_t arg;
    memset(&arg, 0, sizeof(btc_avrc_args_t));

    arg.ps_cmd.tl = tl;
    arg.ps_cmd.attr_id = attr_id;
    arg.ps_cmd.value_id = value_id;

    /* Switch to BTC context */
    bt_status_t stat = btc_transfer_context(&msg, &arg, sizeof(btc_avrc_args_t), NULL);
    return (stat == BT_STATUS_SUCCESS) ? YOC_OK : YOC_FAIL;
}


yoc_err_t yoc_avrc_ct_send_register_notification_cmd(uint8_t tl, uint8_t event_id, uint32_t event_parameter)
{
    if (yoc_bluedroid_get_status() != YOC_BLUEDROID_STATUS_ENABLED) {
        return YOC_ERR_INVALID_STATE;
    }

    if (tl >= 16 || event_id > YOC_AVRC_RN_MAX_EVT - 1) {
        return YOC_FAIL;
    }

    btc_msg_t msg;
    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_AVRC;
    msg.act = BTC_AVRC_NOTIFY_API_SND_REG_NOTIFY_EVT;

    btc_avrc_args_t arg;
    memset(&arg, 0, sizeof(btc_avrc_args_t));

    arg.rn_cmd.tl = tl;
    arg.rn_cmd.event_id = event_id;
    arg.rn_cmd.event_parameter = event_parameter;

    /* Switch to BTC context */
    bt_status_t stat = btc_transfer_context(&msg, &arg, sizeof(btc_avrc_args_t), NULL);
    return (stat == BT_STATUS_SUCCESS) ? YOC_OK : YOC_FAIL;
}

yoc_err_t yoc_avrc_ct_send_metadata_cmd(uint8_t tl, uint8_t attr_mask)
{
    if (yoc_bluedroid_get_status() != YOC_BLUEDROID_STATUS_ENABLED) {
        return YOC_ERR_INVALID_STATE;
    }

    if (tl >= 16) {
        return YOC_FAIL;
    }

    btc_msg_t msg;
    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_AVRC;
    msg.act = BTC_AVRC_STATUS_API_SND_META_EVT;

    btc_avrc_args_t arg;
    memset(&arg, 0, sizeof(btc_avrc_args_t));

    arg.md_cmd.tl = tl;
    arg.md_cmd.attr_mask = attr_mask;

    /* Switch to BTC context */
    bt_status_t stat = btc_transfer_context(&msg, &arg, sizeof(btc_avrc_args_t), NULL);
    return (stat == BT_STATUS_SUCCESS) ? YOC_OK : YOC_FAIL;
}

yoc_err_t yoc_avrc_ct_send_passthrough_cmd(uint8_t tl, uint8_t key_code, uint8_t key_state)
{
    if (yoc_bluedroid_get_status() != YOC_BLUEDROID_STATUS_ENABLED) {
        return YOC_ERR_INVALID_STATE;
    }

    if (tl >= 16 || key_state > YOC_AVRC_PT_CMD_STATE_RELEASED) {
        return YOC_FAIL;
    }

    btc_msg_t msg;
    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_AVRC;
    msg.act = BTC_AVRC_CTRL_API_SND_PTCMD_EVT;

    btc_avrc_args_t arg;
    memset(&arg, 0, sizeof(btc_avrc_args_t));

    arg.pt_cmd.tl = tl;
    arg.pt_cmd.key_code = key_code;
    arg.pt_cmd.key_state = key_state;

    /* Switch to BTC context */
    bt_status_t stat = btc_transfer_context(&msg, &arg, sizeof(btc_avrc_args_t), NULL);
    return (stat == BT_STATUS_SUCCESS) ? YOC_OK : YOC_FAIL;
}

#endif /* #if BTC_AV_INCLUDED */
