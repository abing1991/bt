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

#ifndef __YOC_AVRC_API_H__
#define __YOC_AVRC_API_H__

#include <stdint.h>
#include <stdbool.h>
#include "yoc_err.h"
#include "yoc_bt_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

/// AVRC feature bit mask
typedef enum {
    YOC_AVRC_FEAT_RCTG = 0x0001,                 /*!< remote control target */
    YOC_AVRC_FEAT_RCCT = 0x0002,                 /*!< remote control controller */
    YOC_AVRC_FEAT_VENDOR = 0x0008,               /*!< remote control vendor dependent commands */
    YOC_AVRC_FEAT_BROWSE = 0x0010,               /*!< use browsing channel */
    YOC_AVRC_FEAT_META_DATA = 0x0040,            /*!< remote control metadata transfer command/response */
    YOC_AVRC_FEAT_ADV_CTRL = 0x0200,             /*!< remote control advanced control commmand/response */
} yoc_avrc_features_t;

/// AVRC passthrough command code
typedef enum {
    YOC_AVRC_PT_CMD_PLAY = 0x44,                 /*!< play */
    YOC_AVRC_PT_CMD_STOP = 0x45,                 /*!< stop */
    YOC_AVRC_PT_CMD_PAUSE = 0x46,                /*!< pause */
    YOC_AVRC_PT_CMD_FORWARD = 0x4B,              /*!< forward */
    YOC_AVRC_PT_CMD_BACKWARD = 0x4C,             /*!< backward */
    YOC_AVRC_PT_CMD_REWIND = 0x48,               /*!< rewind */
    YOC_AVRC_PT_CMD_FAST_FORWARD = 0x49          /*!< fast forward */
} yoc_avrc_pt_cmd_t;

/// AVRC passthrough command state
typedef enum {
    YOC_AVRC_PT_CMD_STATE_PRESSED = 0,           /*!< key pressed */
    YOC_AVRC_PT_CMD_STATE_RELEASED = 1           /*!< key released */
} yoc_avrc_pt_cmd_state_t;

/// AVRC Controller callback events
typedef enum {
    YOC_AVRC_CT_CONNECTION_STATE_EVT = 0,        /*!< connection state changed event */
    YOC_AVRC_CT_PASSTHROUGH_RSP_EVT = 1,         /*!< passthrough response event */
    YOC_AVRC_CT_METADATA_RSP_EVT = 2,            /*!< metadata response event */
    YOC_AVRC_CT_PLAY_STATUS_RSP_EVT = 3,         /*!< play status response event */
    YOC_AVRC_CT_CHANGE_NOTIFY_EVT = 4,           /*!< notification event */
    YOC_AVRC_CT_REMOTE_FEATURES_EVT = 5,         /*!< feature of remote device indication event */
} yoc_avrc_ct_cb_event_t;

/// AVRC metadata attribute mask
typedef enum {
    YOC_AVRC_MD_ATTR_TITLE = 0x1,                 /*!< title of the playing track */
    YOC_AVRC_MD_ATTR_ARTIST = 0x2,                /*!< track artist */
    YOC_AVRC_MD_ATTR_ALBUM = 0x4,                 /*!< album name */
    YOC_AVRC_MD_ATTR_TRACK_NUM = 0x8,             /*!< track position on the album */
    YOC_AVRC_MD_ATTR_NUM_TRACKS = 0x10,           /*!< number of tracks on the album */
    YOC_AVRC_MD_ATTR_GENRE = 0x20,                /*!< track genre */
    YOC_AVRC_MD_ATTR_PLAYING_TIME = 0x40          /*!< total album playing time in miliseconds */
} yoc_avrc_md_attr_mask_t;

/// AVRC event notification ids
typedef enum {
    YOC_AVRC_RN_PLAY_STATUS_CHANGE = 0x01,        /*!< track status change, eg. from playing to paused */
    YOC_AVRC_RN_TRACK_CHANGE = 0x02,              /*!< new track is loaded */
    YOC_AVRC_RN_TRACK_REACHED_END = 0x03,         /*!< current track reached end */
    YOC_AVRC_RN_TRACK_REACHED_START = 0x04,       /*!< current track reached start position */
    YOC_AVRC_RN_PLAY_POS_CHANGED = 0x05,          /*!< track playing position changed */
    YOC_AVRC_RN_BATTERY_STATUS_CHANGE = 0x06,     /*!< battery status changed */
    YOC_AVRC_RN_SYSTEM_STATUS_CHANGE = 0x07,      /*!< system status changed */
    YOC_AVRC_RN_APP_SETTING_CHANGE = 0x08,        /*!< application settings changed */
    YOC_AVRC_RN_MAX_EVT
} yoc_avrc_rn_event_ids_t;

/// AVRC player setting ids
typedef enum {
    YOC_AVRC_PS_EQUALIZER = 0x01,                 /*!< equalizer, on or off */
    YOC_AVRC_PS_REPEAT_MODE = 0x02,               /*!< repeat mode */
    YOC_AVRC_PS_SHUFFLE_MODE = 0x03,              /*!< shuffle mode */
    YOC_AVRC_PS_SCAN_MODE = 0x04,                 /*!< scan mode on or off */
    YOC_AVRC_PS_MAX_ATTR
} yoc_avrc_ps_attr_ids_t;

/// AVRC equalizer modes
typedef enum {
    YOC_AVRC_PS_EQUALIZER_OFF = 0x1,              /*!< equalizer OFF */
    YOC_AVRC_PS_EQUALIZER_ON = 0x2                /*!< equalizer ON */
} yoc_avrc_ps_eq_value_ids_t;

/// AVRC repeat modes
typedef enum {
    YOC_AVRC_PS_REPEAT_OFF = 0x1,                 /*!< repeat mode off */
    YOC_AVRC_PS_REPEAT_SINGLE = 0x2,              /*!< single track repeat */
    YOC_AVRC_PS_REPEAT_GROUP = 0x3                /*!< group repeat */
} yoc_avrc_ps_rpt_value_ids_t;


/// AVRC shuffle modes
typedef enum {
    YOC_AVRC_PS_SHUFFLE_OFF = 0x1,                /*<! shuffle off */
    YOC_AVRC_PS_SHUFFLE_ALL = 0x2,                /*<! all trackes shuffle */
    YOC_AVRC_PS_SHUFFLE_GROUP = 0x3               /*<! group shuffle */
} yoc_avrc_ps_shf_value_ids_t;

/// AVRC scan modes
typedef enum {
    YOC_AVRC_PS_SCAN_OFF = 0x1,                   /*!< scan off */
    YOC_AVRC_PS_SCAN_ALL = 0x2,                   /*!< all tracks scan */
    YOC_AVRC_PS_SCAN_GROUP = 0x3                  /*!< group scan */
} yoc_avrc_ps_scn_value_ids_t;

/// AVRC controller callback parameters
typedef union {
    /**
     * @brief YOC_AVRC_CT_CONNECTION_STATE_EVT
     */
    struct avrc_ct_conn_stat_param {
        bool connected;                          /*!< whether AVRC connection is set up */
        yoc_bd_addr_t remote_bda;                /*!< remote bluetooth device address */
    } conn_stat;                                 /*!< AVRC connection status */

    /**
     * @brief YOC_AVRC_CT_PASSTHROUGH_RSP_EVT
     */
    struct avrc_ct_psth_rsp_param {
        uint8_t tl;                              /*!< transaction label, 0 to 15 */
        uint8_t key_code;                        /*!< passthrough command code */
        uint8_t key_state;                       /*!< 0 for PRESSED, 1 for RELEASED */
    } psth_rsp;                                  /*!< passthrough command response */

    /**
     * @brief YOC_AVRC_CT_METADATA_RSP_EVT
     */
    struct avrc_ct_meta_rsp_param {
        uint8_t attr_id;                         /*!< id of metadata attribute */
        uint8_t *attr_text;                      /*!< attribute itself */
        int attr_length;                         /*!< attribute character length */
    } meta_rsp;                                  /*!< metadata attributes response */

    /**
     * @brief YOC_AVRC_CT_CHANGE_NOTIFY_EVT
     */
    struct avrc_ct_change_notify_param {
        uint8_t event_id;                        /*!< id of AVRC event notification */
        uint32_t event_parameter;                /*!< event notification parameter */
    } change_ntf;                                /*!< notifications */

    /**
     * @brief YOC_AVRC_CT_REMOTE_FEATURES_EVT
     */
    struct avrc_ct_rmt_feats_param {
        uint32_t feat_mask;                      /*!< AVRC feature mask of remote device */
        yoc_bd_addr_t remote_bda;                /*!< remote bluetooth device address */
    } rmt_feats;                                 /*!< AVRC features discovered from remote SDP server */
    
} yoc_avrc_ct_cb_param_t;


/**
 * @brief           AVRCP controller callback function type
 * @param           event : Event type
 * @param           param : Pointer to callback parameter union
 */
typedef void (* yoc_avrc_ct_cb_t)(yoc_avrc_ct_cb_event_t event, yoc_avrc_ct_cb_param_t *param);


/**
 * @brief           Register application callbacks to AVRCP module; for now only AVRCP Controller
 *                  role is supported. This function should be called after yoc_bluedroid_enable()
 *                  completes successfully
 *
 * @param[in]       callback: AVRCP controller callback function
 *
 * @return
 *                  - YOC_OK: success
 *                  - YOC_INVALID_STATE: if bluetooth stack is not yet enabled
 *                  - YOC_FAIL: others
 *
 */
yoc_err_t yoc_avrc_ct_register_callback(yoc_avrc_ct_cb_t callback);


/**
 *
 * @brief           Initialize the bluetooth AVRCP controller module, This function should be called
 *                  after yoc_bluedroid_enable() completes successfully
 *
 * @return
 *                  - YOC_OK: success
 *                  - YOC_INVALID_STATE: if bluetooth stack is not yet enabled
 *                  - YOC_FAIL: others
 *
 */
yoc_err_t yoc_avrc_ct_init(void);


/**
 *
 * @brief           De-initialize AVRCP controller module. This function should be called after
 *                  after yoc_bluedroid_enable() completes successfully
 *
 * @return
 *                  - YOC_OK: success
 *                  - YOC_INVALID_STATE: if bluetooth stack is not yet enabled
 *                  - YOC_FAIL: others
 */
yoc_err_t yoc_avrc_ct_deinit(void);

/**
 *
 * @brief           Send player application settings command to AVRCP target. This function should be called
 *                  after YOC_AVRC_CT_CONNECTION_STATE_EVT is received and AVRCP connection is established.
 *
 * @param[in]       tl : transaction label, 0 to 15, consecutive commands should use different values.
 * @param[in]       attr_id : player application setting attribute IDs from one of yoc_avrc_ps_attr_ids_t
 * @param[in]       value_id : attribute value defined for the specific player application setting attribute
 * @return
 *                  - YOC_OK: success
 *                  - YOC_INVALID_STATE: if bluetooth stack is not yet enabled
 *                  - YOC_FAIL: others
 */
yoc_err_t yoc_avrc_ct_send_set_player_value_cmd(uint8_t tl, uint8_t attr_id, uint8_t value_id);

/**
 * @brief           Send register notification command to AVRCP target, This function should be called after
 *                  YOC_AVRC_CT_CONNECTION_STATE_EVT is received and AVRCP connection is established
 *
 * @param[in]       tl : transaction label, 0 to 15, consecutive commands should use different values.
 * @param[in]       event_id : id of events, e.g. YOC_AVRC_RN_PLAY_STATUS_CHANGE, YOC_AVRC_RN_TRACK_CHANGE, etc.
 * @param[in]       event_parameter : special parameters, eg. playback interval for YOC_AVRC_RN_PLAY_POS_CHANGED
 * @return
 *                  - YOC_OK: success
 *                  - YOC_INVALID_STATE: if bluetooth stack is not yet enabled
 *                  - YOC_FAIL: others
 */
yoc_err_t yoc_avrc_ct_send_register_notification_cmd(uint8_t tl, uint8_t event_id, uint32_t event_parameter);


/**
 * @brief           Send metadata command to AVRCP target, This function should be called after
 *                  YOC_AVRC_CT_CONNECTION_STATE_EVT is received and AVRCP connection is established
 *
 * @param[in]       tl : transaction label, 0 to 15, consecutive commands should use different values.
 * @param[in]       attr_mask : mask of attributes, e.g. YOC_AVRC_MD_ATTR_ID_TITLE | YOC_AVRC_MD_ATTR_ID_ARTIST.
 *
 * @return
 *                  - YOC_OK: success
 *                  - YOC_INVALID_STATE: if bluetooth stack is not yet enabled
 *                  - YOC_FAIL: others
 */
yoc_err_t yoc_avrc_ct_send_metadata_cmd(uint8_t tl, uint8_t attr_mask);


/**
 * @brief           Send passthrough command to AVRCP target, This function should be called after
 *                  YOC_AVRC_CT_CONNECTION_STATE_EVT is received and AVRCP connection is established
 *
 * @param[in]       tl : transaction label, 0 to 15, consecutive commands should use different values.
 * @param[in]       key_code : passthrough command code, e.g. YOC_AVRC_PT_CMD_PLAY, YOC_AVRC_PT_CMD_STOP, etc.
 * @param[in]       key_state : passthrough command key state, YOC_AVRC_PT_CMD_STATE_PRESSED or
 *                  YOC_AVRC_PT_CMD_STATE_RELEASED
 *
 * @return
 *                  - YOC_OK: success
 *                  - YOC_INVALID_STATE: if bluetooth stack is not yet enabled
 *                  - YOC_FAIL: others
 */
yoc_err_t yoc_avrc_ct_send_passthrough_cmd(uint8_t tl, uint8_t key_code, uint8_t key_state);


#ifdef __cplusplus
}
#endif

#endif /* __YOC_AVRC_API_H__ */
