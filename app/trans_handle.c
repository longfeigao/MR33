#include <stdint.h>
#include <trans_handle.h>

#include "thread_rf.h"
#include "thread_trans.h"
#include "debug.h"
#include "task_id.h"


int8_t downlink_data_handler(uint16_t id, uint8_t* data, uint32_t length,
    uint32_t size, uint32_t storage)
{
    TRACE();
    pinfo("%s,id:%x,data:%x,length:%d\n", __func__,
         id, data, length);

    uint8_t temp[16];
    switch(id){
    case CORE_CMD_ESL_UPDATA_REQUEST:
        //forward_msg_rfthread(id, temp, length, size, storage);
        break;
    case CORE_CMD_ESL_HB_REQUEST:
        forward_msg_rfthread(id, temp, length, size, storage);
        break;
    case CORE_CMD_RCREQ_REQUEST:
        break;
    case CORE_CMD_SOFT_REBOOT:
        //uart_data_send(id, data, length);
        break;
    case CORE_CMD_FT_RR_TXNULL:
        forward_msg_rfthread(id, temp, length, size, storage);
        break;
    case CORE_CMD_SCAN_BG:
        forward_msg_rfthread(id, temp, length, size, storage);
        break;
    case CORE_CMD_FT_RF_BER:
        //uart_data_send(id, data, length);
        break;
    case CORE_CMD_SET_DEBUG_LEVEL:
        //uart_data_send(id, data, length);
        break;
    case CORE_CMD_SET_RF_LOG:
        //uart_data_send(id, data, length);
        break;
    case CORE_CMD_SCAN_WKUP:
        forward_msg_rfthread(id, temp, length, size, storage);
        break;
    case CORE_CMD_RF_TXRX:
        forward_msg_rfthread(id, temp, length, size, storage);
        break;
    case CORE_CMD_SCAN_DEVICE:
        //uart_data_send(id, data, length);
        break;
    case CORE_CMD_CALIBRATE_POWER:
        break;
    case CORE_CMD_CALIBRATE_FREQ:
        break;
    default:
        break;
    }
    return 0;
}