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

#include <string.h>
#include "yoc_bt_device.h"
#include "yoc_bt_main.h"
#include "yoc_gap_ble_api.h"
#include "bta/bta_api.h"
#include "common/bt_trace.h"
#include "btc/btc_manage.h"
#include "btc_gap_ble.h"
#include "btc/btc_ble_storage.h"


yoc_err_t yoc_ble_gap_register_callback(yoc_gap_ble_cb_t callback)
{
    YOC_BLUEDROID_STATUS_CHECK(YOC_BLUEDROID_STATUS_ENABLED);

    return (btc_profile_cb_set(BTC_PID_GAP_BLE, callback) == 0 ? YOC_OK : YOC_FAIL);
}


yoc_err_t yoc_ble_gap_config_adv_data(yoc_ble_adv_data_t *adv_data)
{
    btc_msg_t msg;
    btc_ble_gap_args_t arg;

    YOC_BLUEDROID_STATUS_CHECK(YOC_BLUEDROID_STATUS_ENABLED);

    if (adv_data == NULL) {
        return YOC_ERR_INVALID_ARG;
    }

    if (adv_data->service_uuid_len & 0xf) { //not 16*n
        return YOC_ERR_INVALID_ARG;
    }

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GAP_BLE;
    msg.act = BTC_GAP_BLE_ACT_CFG_ADV_DATA;
    memcpy(&arg.cfg_adv_data.adv_data, adv_data, sizeof(yoc_ble_adv_data_t));

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gap_args_t), btc_gap_ble_arg_deep_copy) == BT_STATUS_SUCCESS ? YOC_OK : YOC_FAIL);

}


yoc_err_t yoc_ble_gap_set_scan_params(yoc_ble_scan_params_t *scan_params)
{
    btc_msg_t msg;
    btc_ble_gap_args_t arg;

    YOC_BLUEDROID_STATUS_CHECK(YOC_BLUEDROID_STATUS_ENABLED);

    if (scan_params == NULL) {
        return YOC_ERR_INVALID_ARG;
    }

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GAP_BLE;
    msg.act = BTC_GAP_BLE_ACT_SET_SCAN_PARAM;
    memcpy(&arg.set_scan_param.scan_params, scan_params, sizeof(yoc_ble_scan_params_t));

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gap_args_t), NULL) == BT_STATUS_SUCCESS ? YOC_OK : YOC_FAIL);
}

yoc_err_t yoc_ble_gap_start_scanning(uint32_t duration)
{
    btc_msg_t msg;
    btc_ble_gap_args_t arg;

    YOC_BLUEDROID_STATUS_CHECK(YOC_BLUEDROID_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GAP_BLE;
    msg.act = BTC_GAP_BLE_ACT_START_SCAN;
    arg.start_scan.duration = duration;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gap_args_t), NULL) == BT_STATUS_SUCCESS ? YOC_OK : YOC_FAIL);
}


yoc_err_t yoc_ble_gap_stop_scanning(void)
{
    btc_msg_t msg;

    YOC_BLUEDROID_STATUS_CHECK(YOC_BLUEDROID_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GAP_BLE;
    msg.act = BTC_GAP_BLE_ACT_STOP_SCAN;
    return (btc_transfer_context(&msg, NULL, 0, NULL) == BT_STATUS_SUCCESS ? YOC_OK : YOC_FAIL);
}

yoc_err_t yoc_ble_gap_start_advertising(yoc_ble_adv_params_t *adv_params)
{
    btc_msg_t msg;
    btc_ble_gap_args_t arg;

    YOC_BLUEDROID_STATUS_CHECK(YOC_BLUEDROID_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GAP_BLE;
    msg.act = BTC_GAP_BLE_ACT_START_ADV;
    memcpy(&arg.start_adv.adv_params, adv_params, sizeof(yoc_ble_adv_params_t));

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gap_args_t), NULL) == BT_STATUS_SUCCESS ? YOC_OK : YOC_FAIL);
}

yoc_err_t yoc_ble_gap_stop_advertising(void)
{
    btc_msg_t msg;

    YOC_BLUEDROID_STATUS_CHECK(YOC_BLUEDROID_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GAP_BLE;
    msg.act = BTC_GAP_BLE_ACT_STOP_ADV;

    return (btc_transfer_context(&msg, NULL, 0, NULL) == BT_STATUS_SUCCESS ? YOC_OK : YOC_FAIL);
}


yoc_err_t yoc_ble_gap_update_conn_params(yoc_ble_conn_update_params_t *params)
{
    btc_msg_t msg;
    btc_ble_gap_args_t arg;

    YOC_BLUEDROID_STATUS_CHECK(YOC_BLUEDROID_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GAP_BLE;
    msg.act = BTC_GAP_BLE_ACT_UPDATE_CONN_PARAM;
    memcpy(&arg.conn_update_params.conn_params, params, sizeof(yoc_ble_conn_update_params_t));

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gap_args_t), NULL) == BT_STATUS_SUCCESS ? YOC_OK : YOC_FAIL);
}

yoc_err_t yoc_ble_gap_set_pkt_data_len(yoc_bd_addr_t remote_device, uint16_t tx_data_length)
{
    btc_msg_t msg;
    btc_ble_gap_args_t arg;

    YOC_BLUEDROID_STATUS_CHECK(YOC_BLUEDROID_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GAP_BLE;
    msg.act = BTC_GAP_BLE_ACT_SET_PKT_DATA_LEN;
    arg.set_pkt_data_len.tx_data_length = tx_data_length;
    memcpy(arg.set_pkt_data_len.remote_device, remote_device, YOC_BD_ADDR_LEN);

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gap_args_t), NULL) == BT_STATUS_SUCCESS ? YOC_OK : YOC_FAIL);
}


yoc_err_t yoc_ble_gap_set_rand_addr(yoc_bd_addr_t rand_addr)
{
    btc_msg_t msg;
    btc_ble_gap_args_t arg;

    YOC_BLUEDROID_STATUS_CHECK(YOC_BLUEDROID_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GAP_BLE;
    msg.act = BTC_GAP_BLE_ACT_SET_RAND_ADDRESS;
    memcpy(arg.set_rand_addr.rand_addr, rand_addr, YOC_BD_ADDR_LEN);

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gap_args_t), NULL) == BT_STATUS_SUCCESS ? YOC_OK : YOC_FAIL);
}


yoc_err_t yoc_ble_gap_config_local_privacy (bool privacy_enable)
{
    btc_msg_t msg;
    btc_ble_gap_args_t arg;

    YOC_BLUEDROID_STATUS_CHECK(YOC_BLUEDROID_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GAP_BLE;
    msg.act = BTC_GAP_BLE_ACT_CONFIG_LOCAL_PRIVACY;
    arg.cfg_local_privacy.privacy_enable = privacy_enable;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gap_args_t), NULL) == BT_STATUS_SUCCESS ? YOC_OK : YOC_FAIL);
}

yoc_err_t yoc_ble_gap_config_local_icon (uint16_t icon)
{
    yoc_err_t ret;
    btc_msg_t msg;
    btc_ble_gap_args_t arg;

    YOC_BLUEDROID_STATUS_CHECK(YOC_BLUEDROID_STATUS_ENABLED);

    switch (icon) {
    case YOC_BLE_APPEARANCE_GENERIC_PHONE:
    case YOC_BLE_APPEARANCE_GENERIC_COMPUTER:
    case YOC_BLE_APPEARANCE_GENERIC_REMOTE:
    case YOC_BLE_APPEARANCE_GENERIC_THERMOMETER:
    case YOC_BLE_APPEARANCE_THERMOMETER_EAR:
    case YOC_BLE_APPEARANCE_GENERIC_HEART_RATE:
    case YOC_BLE_APPEARANCE_HEART_RATE_BELT:
    case YOC_BLE_APPEARANCE_GENERIC_BLOOD_PRESSURE:
    case YOC_BLE_APPEARANCE_BLOOD_PRESSURE_ARM:
    case YOC_BLE_APPEARANCE_BLOOD_PRESSURE_WRIST:
    case YOC_BLE_APPEARANCE_GENERIC_PULSE_OXIMETER:
    case YOC_BLE_APPEARANCE_PULSE_OXIMETER_FINGERTIP:
    case YOC_BLE_APPEARANCE_PULSE_OXIMETER_WRIST:
    case YOC_BLE_APPEARANCE_GENERIC_GLUCOSE:
    case YOC_BLE_APPEARANCE_GENERIC_WEIGHT:
    case YOC_BLE_APPEARANCE_GENERIC_WALKING:
    case YOC_BLE_APPEARANCE_WALKING_IN_SHOE:
    case YOC_BLE_APPEARANCE_WALKING_ON_SHOE:
    case YOC_BLE_APPEARANCE_WALKING_ON_HIP:
    case YOC_BLE_APPEARANCE_GENERIC_WATCH:
    case YOC_BLE_APPEARANCE_SPORTS_WATCH:
    case YOC_BLE_APPEARANCE_GENERIC_EYEGLASSES:
    case YOC_BLE_APPEARANCE_GENERIC_DISPLAY:
    case YOC_BLE_APPEARANCE_GENERIC_MEDIA_PLAYER:
    case YOC_BLE_APPEARANCE_GENERIC_BARCODE_SCANNER:
    case YOC_BLE_APPEARANCE_HID_BARCODE_SCANNER:
    case YOC_BLE_APPEARANCE_GENERIC_HID:
    case YOC_BLE_APPEARANCE_HID_KEYBOARD:
    case YOC_BLE_APPEARANCE_HID_MOUSE:
    case YOC_BLE_APPEARANCE_HID_JOYSTICK:
    case YOC_BLE_APPEARANCE_HID_GAMEPAD:
    case YOC_BLE_APPEARANCE_HID_DIGITIZER_TABLET:
    case YOC_BLE_APPEARANCE_HID_CARD_READER:
    case YOC_BLE_APPEARANCE_HID_DIGITAL_PEN:
    case YOC_BLE_APPEARANCE_UNKNOWN:
    case YOC_BLE_APPEARANCE_GENERIC_CLOCK:
    case YOC_BLE_APPEARANCE_GENERIC_TAG:
    case YOC_BLE_APPEARANCE_GENERIC_KEYRING:
    case YOC_BLE_APPEARANCE_GENERIC_CYCLING:
    case YOC_BLE_APPEARANCE_CYCLING_COMPUTER:
    case YOC_BLE_APPEARANCE_CYCLING_SPEED:
    case YOC_BLE_APPEARANCE_CYCLING_CADENCE:
    case YOC_BLE_APPEARANCE_CYCLING_POWER:
    case YOC_BLE_APPEARANCE_CYCLING_SPEED_CADENCE:
    case YOC_BLE_APPEARANCE_GENERIC_PERSONAL_MOBILITY_DEVICE:
    case YOC_BLE_APPEARANCE_POWERED_WHEELCHAIR:
    case YOC_BLE_APPEARANCE_MOBILITY_SCOOTER:
    case YOC_BLE_APPEARANCE_GENERIC_CONTINUOUS_GLUCOSE_MONITOR:
    case YOC_BLE_APPEARANCE_GENERIC_INSULIN_PUMP:
    case YOC_BLE_APPEARANCE_INSULIN_PUMP_DURABLE_PUMP:
    case YOC_BLE_APPEARANCE_INSULIN_PUMP_PATCH_PUMP:
    case YOC_BLE_APPEARANCE_INSULIN_PEN:
    case YOC_BLE_APPEARANCE_GENERIC_MEDICATION_DELIVERY:
    case YOC_BLE_APPEARANCE_GENERIC_OUTDOOR_SPORTS:
    case YOC_BLE_APPEARANCE_OUTDOOR_SPORTS_LOCATION:
    case YOC_BLE_APPEARANCE_OUTDOOR_SPORTS_LOCATION_AND_NAV:
    case YOC_BLE_APPEARANCE_OUTDOOR_SPORTS_LOCATION_POD:
    case YOC_BLE_APPEARANCE_OUTDOOR_SPORTS_LOCATION_POD_AND_NAV:
        msg.sig = BTC_SIG_API_CALL;
        msg.pid = BTC_PID_GAP_BLE;
        msg.act = BTC_GAP_BLE_ACT_CONFIG_LOCAL_ICON;
        arg.cfg_local_icon.icon = icon;
        ret = (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gap_args_t), NULL) == BT_STATUS_SUCCESS ? YOC_OK : YOC_FAIL);
        break;
    default:
        ret = YOC_ERR_INVALID_ARG;
        break;
    }
    return ret;
}

yoc_err_t yoc_ble_gap_update_whitelist(bool add_remove, yoc_bd_addr_t remote_bda)
{
    btc_msg_t msg;
    btc_ble_gap_args_t arg;

    if (yoc_bluedroid_get_status() != YOC_BLUEDROID_STATUS_ENABLED) {
        return YOC_ERR_INVALID_STATE;
    }
    if (!remote_bda){
        return YOC_ERR_INVALID_SIZE;
    }
    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GAP_BLE;
    msg.act = BTC_GAP_BLE_ACT_UPDATE_WHITE_LIST;
    arg.update_white_list.add_remove = add_remove;
    memcpy(arg.update_white_list.remote_bda, remote_bda, sizeof(yoc_bd_addr_t));

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gap_args_t), NULL)
                == BT_STATUS_SUCCESS ? YOC_OK : YOC_FAIL);
}

yoc_err_t yoc_ble_gap_get_whitelist_size(uint16_t *length)
{
    if (length == NULL) {
        return YOC_FAIL;
    }
    btc_get_whitelist_size(length);

    return YOC_OK;
}

yoc_err_t yoc_ble_gap_set_prefer_conn_params(yoc_bd_addr_t bd_addr,
                                                                 uint16_t min_conn_int, uint16_t max_conn_int,
                                                                 uint16_t slave_latency, uint16_t supervision_tout)
{
    btc_msg_t msg;
    btc_ble_gap_args_t arg;

    if (yoc_bluedroid_get_status() != YOC_BLUEDROID_STATUS_ENABLED) {
        return YOC_ERR_INVALID_STATE;
    }

    if (YOC_BLE_IS_VALID_PARAM(min_conn_int, YOC_BLE_CONN_INT_MIN, YOC_BLE_CONN_INT_MAX) &&
        YOC_BLE_IS_VALID_PARAM(max_conn_int, YOC_BLE_CONN_INT_MIN, YOC_BLE_CONN_INT_MAX) &&
        YOC_BLE_IS_VALID_PARAM(supervision_tout, YOC_BLE_CONN_SUP_TOUT_MIN, YOC_BLE_CONN_SUP_TOUT_MAX) &&
        (slave_latency <= YOC_BLE_CONN_LATENCY_MAX || slave_latency == YOC_BLE_CONN_PARAM_UNDEF) &&
        ((supervision_tout * 10) >= ((1 + slave_latency) * ((max_conn_int * 5) >> 1))) && min_conn_int <= max_conn_int) {

        msg.sig = BTC_SIG_API_CALL;
        msg.pid = BTC_PID_GAP_BLE;
        msg.act = BTC_GAP_BLE_ACT_SET_CONN_PARAMS;
        arg.set_conn_params.min_conn_int = min_conn_int;
        arg.set_conn_params.max_conn_int = max_conn_int;
        arg.set_conn_params.slave_latency = slave_latency;
        arg.set_conn_params.supervision_tout = supervision_tout;
        memcpy(arg.set_conn_params.bd_addr, bd_addr, sizeof(yoc_bd_addr_t));

        return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gap_args_t), NULL)
                    == BT_STATUS_SUCCESS ? YOC_OK : YOC_FAIL);
    } else {
        LOG_ERROR("%s,invalid connection params:min_int = %d, max_int = %d, latency = %d, timeout = %d",\
                            __func__, min_conn_int, max_conn_int, slave_latency, supervision_tout);
        return YOC_FAIL;
    }
}

yoc_err_t yoc_ble_gap_set_device_name(const char *name)
{
    YOC_BLUEDROID_STATUS_CHECK(YOC_BLUEDROID_STATUS_ENABLED);

    return yoc_bt_dev_set_device_name(name);
}

yoc_err_t yoc_ble_gap_get_local_used_addr(yoc_bd_addr_t local_used_addr, uint8_t * addr_type)
{
    if(yoc_bluedroid_get_status() != (YOC_BLUEDROID_STATUS_ENABLED)) {
        LOG_ERROR("%s, bluedroid status error", __func__);
        return YOC_FAIL;
    }
    if(!BTM_BleGetCurrentAddress(local_used_addr, addr_type)) {
        return YOC_FAIL;
    }
    return YOC_OK;
}

uint8_t *yoc_ble_resolve_adv_data( uint8_t *adv_data, uint8_t type, uint8_t *length)
{
    if (((type < YOC_BLE_AD_TYPE_FLAG) || (type > YOC_BLE_AD_TYPE_128SERVICE_DATA)) &&
            (type != YOC_BLE_AD_MANUFACTURER_SPECIFIC_TYPE)) {
        LOG_ERROR("the eir type not define, type = %x\n", type);
        return NULL;
    }

    if (adv_data == NULL) {
        LOG_ERROR("Invalid p_eir data.\n");
        return NULL;
    }

    return (BTM_CheckAdvData( adv_data, type, length));
}

yoc_err_t yoc_ble_gap_config_adv_data_raw(uint8_t *raw_data, uint32_t raw_data_len)
{
    btc_msg_t msg;
    btc_ble_gap_args_t arg;

    YOC_BLUEDROID_STATUS_CHECK(YOC_BLUEDROID_STATUS_ENABLED);

    if (raw_data == NULL
            || (raw_data_len <= 0 || raw_data_len > YOC_BLE_ADV_DATA_LEN_MAX)) {
        return YOC_ERR_INVALID_ARG;
    }

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GAP_BLE;
    msg.act = BTC_GAP_BLE_ACT_CFG_ADV_DATA_RAW;
    arg.cfg_adv_data_raw.raw_adv = raw_data;
    arg.cfg_adv_data_raw.raw_adv_len = raw_data_len;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gap_args_t), btc_gap_ble_arg_deep_copy) == BT_STATUS_SUCCESS ? YOC_OK : YOC_FAIL);

}

yoc_err_t yoc_ble_gap_read_rssi(yoc_bd_addr_t remote_addr)
{
    btc_msg_t msg;
    btc_ble_gap_args_t arg;

    if (yoc_bluedroid_get_status() != YOC_BLUEDROID_STATUS_ENABLED) {
        return YOC_ERR_INVALID_STATE;
    }

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GAP_BLE;
    msg.act = BTC_GAP_BLE_ACT_READ_RSSI;
    memcpy(arg.read_rssi.remote_addr, remote_addr, sizeof(yoc_bd_addr_t));

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gap_args_t), NULL) == BT_STATUS_SUCCESS ? YOC_OK : YOC_FAIL);
}

yoc_err_t yoc_ble_gap_config_scan_rsp_data_raw(uint8_t *raw_data, uint32_t raw_data_len)
{
    btc_msg_t msg;
    btc_ble_gap_args_t arg;

    YOC_BLUEDROID_STATUS_CHECK(YOC_BLUEDROID_STATUS_ENABLED);

    if (raw_data == NULL
            || (raw_data_len <= 0 || raw_data_len > YOC_BLE_SCAN_RSP_DATA_LEN_MAX)) {
        return YOC_ERR_INVALID_ARG;
    }

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GAP_BLE;
    msg.act = BTC_GAP_BLE_ACT_CFG_SCAN_RSP_DATA_RAW;
    arg.cfg_scan_rsp_data_raw.raw_scan_rsp = raw_data;
    arg.cfg_scan_rsp_data_raw.raw_scan_rsp_len = raw_data_len;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gap_args_t), btc_gap_ble_arg_deep_copy) == BT_STATUS_SUCCESS ? YOC_OK : YOC_FAIL);

}

#if (SMP_INCLUDED == TRUE)
yoc_err_t yoc_ble_gap_set_security_param(yoc_ble_sm_param_t param_type,
        void *value, uint8_t len)
{
    if(param_type >= YOC_BLE_SM_MAX_PARAM) {
        return YOC_ERR_INVALID_ARG;
    }
    if((param_type != YOC_BLE_SM_CLEAR_STATIC_PASSKEY) && ( value == NULL || len < sizeof(uint8_t) || len > sizeof(uint32_t))) {
        return YOC_ERR_INVALID_ARG;
    }
    if((param_type == YOC_BLE_SM_SET_STATIC_PASSKEY)) {
        uint32_t passkey = 0;
        for(uint8_t i = 0; i < len; i++)
        {
            passkey += (((uint8_t *)value)[i]<<(8*i));
        }
        if(passkey > 999999) {
            return YOC_ERR_INVALID_ARG;
        }
    }

    btc_msg_t msg;
    btc_ble_gap_args_t arg;

    YOC_BLUEDROID_STATUS_CHECK(YOC_BLUEDROID_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GAP_BLE;
    msg.act = BTC_GAP_BLE_SET_SECURITY_PARAM_EVT;
    arg.set_security_param.param_type = param_type;
    arg.set_security_param.len = len;
    arg.set_security_param.value = value;

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gap_args_t), btc_gap_ble_arg_deep_copy)
            == BT_STATUS_SUCCESS ? YOC_OK : YOC_FAIL);
}

yoc_err_t yoc_ble_set_encryption(yoc_bd_addr_t bd_addr, yoc_ble_sec_act_t sec_act)
{
    btc_msg_t msg;
    btc_ble_gap_args_t arg;

    YOC_BLUEDROID_STATUS_CHECK(YOC_BLUEDROID_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GAP_BLE;
    msg.act = BTC_GAP_BLE_SET_ENCRYPTION_EVT;
    arg.set_encryption.sec_act = sec_act;
    memcpy(arg.set_encryption.bd_addr, bd_addr, YOC_BD_ADDR_LEN);

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gap_args_t), NULL)
            == BT_STATUS_SUCCESS ? YOC_OK : YOC_FAIL);
}

yoc_err_t yoc_ble_gap_security_rsp(yoc_bd_addr_t bd_addr, bool accept)
{
    btc_msg_t msg;
    btc_ble_gap_args_t arg;

    YOC_BLUEDROID_STATUS_CHECK(YOC_BLUEDROID_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GAP_BLE;
    msg.act = BTC_GAP_BLE_SECURITY_RSP_EVT;
    arg.sec_rsp.accept = accept;
    memcpy(arg.sec_rsp.bd_addr, bd_addr, YOC_BD_ADDR_LEN);

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gap_args_t), NULL)
            == BT_STATUS_SUCCESS ? YOC_OK : YOC_FAIL);

}

yoc_err_t yoc_ble_passkey_reply(yoc_bd_addr_t bd_addr, bool accept, uint32_t passkey)
{
    btc_msg_t msg;
    btc_ble_gap_args_t arg;

    YOC_BLUEDROID_STATUS_CHECK(YOC_BLUEDROID_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GAP_BLE;
    msg.act = BTC_GAP_BLE_PASSKEY_REPLY_EVT;
    arg.enc_passkey_replay.accept = accept;
    arg.enc_passkey_replay.passkey = passkey;
    memcpy(arg.enc_passkey_replay.bd_addr, bd_addr, YOC_BD_ADDR_LEN);

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gap_args_t), NULL)
            == BT_STATUS_SUCCESS ? YOC_OK : YOC_FAIL);
}

yoc_err_t yoc_ble_confirm_reply(yoc_bd_addr_t bd_addr, bool accept)
{
    btc_msg_t msg;
    btc_ble_gap_args_t arg;

    YOC_BLUEDROID_STATUS_CHECK(YOC_BLUEDROID_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GAP_BLE;
    msg.act = BTC_GAP_BLE_CONFIRM_REPLY_EVT;
    arg.enc_comfirm_replay.accept = accept;
    memcpy(arg.enc_comfirm_replay.bd_addr, bd_addr, YOC_BD_ADDR_LEN);

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gap_args_t), NULL)
            == BT_STATUS_SUCCESS ? YOC_OK : YOC_FAIL);
}

yoc_err_t yoc_ble_remove_bond_device(yoc_bd_addr_t bd_addr)
{
    btc_msg_t msg;
    btc_ble_gap_args_t arg;
    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GAP_BLE;
    msg.act = BTC_GAP_BLE_REMOVE_BOND_DEV_EVT;
    memcpy(arg.remove_bond_device.bd_addr, bd_addr, YOC_BD_ADDR_LEN);

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gap_args_t), NULL)
            == BT_STATUS_SUCCESS ? YOC_OK : YOC_FAIL);
}

int yoc_ble_get_bond_device_num(void)
{
    if (yoc_bluedroid_get_status() != YOC_BLUEDROID_STATUS_ENABLED) {
        return YOC_FAIL;
    }

    return btc_storage_get_num_ble_bond_devices();
}

yoc_err_t yoc_ble_get_bond_device_list(int *dev_num, yoc_ble_bond_dev_t *dev_list)
{
    int ret;
    int dev_num_total;

    if (dev_num == NULL || dev_list == NULL) {
        return YOC_ERR_INVALID_ARG;
    }

    YOC_BLUEDROID_STATUS_CHECK(YOC_BLUEDROID_STATUS_ENABLED);

    dev_num_total = btc_storage_get_num_ble_bond_devices();
    if (*dev_num > dev_num_total) {
        *dev_num = dev_num_total;
    }

    ret = btc_storage_get_bonded_ble_devices_list(dev_list, *dev_num);

    return (ret == BT_STATUS_SUCCESS ? YOC_OK : YOC_FAIL);
}
#endif /* #if (SMP_INCLUDED == TRUE) */

yoc_err_t yoc_ble_gap_disconnect(yoc_bd_addr_t remote_device)
{
    btc_msg_t msg;
    btc_ble_gap_args_t arg;

    YOC_BLUEDROID_STATUS_CHECK(YOC_BLUEDROID_STATUS_ENABLED);

    msg.sig = BTC_SIG_API_CALL;
    msg.pid = BTC_PID_GAP_BLE;
    msg.act = BTC_GAP_BLE_DISCONNECT_EVT;
    memcpy(arg.disconnect.remote_device, remote_device, YOC_BD_ADDR_LEN);

    return (btc_transfer_context(&msg, &arg, sizeof(btc_ble_gap_args_t), NULL) == BT_STATUS_SUCCESS ? YOC_OK : YOC_FAIL);
}




