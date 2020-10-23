/*
 * update_mr33.c
 *
 *  Created on: 2020��10��15��
 *      Author: gaolongfei
 */

#include <stddef.h>

#include "update_type.h"
#include "rf_handle.h"
#include "debug.h"

#include "thread_trans.h"
#include "cc2640r2_rf.h"
#include "timer.h"
#include "mr33_update.h"
#include "memery.h"

enum{
    UPDATA_START = (uint8_t)0,
    UPDATA_DOING,
    UPDATA_BEGIN,
    UPDATA_END
}em_updata_status;


static group_data_st* updata_para = NULL;
static uint8_t updata_status = UPDATA_START;


trans_struct *buffer1 = NULL;
trans_struct *buffer2 = NULL;
List_Elem * current_list = NULL;


int8_t updata_handle(uint8_t** addr, uint8_t n, rf_parse_st* info, void * extra)
{
    group_data_st* data_addr =  (group_data_st*)addr;
    mr33_data_st *basic_data = (mr33_data_st *)data_addr->data;
    uint8_t timer = 0;
    uint16_t timeout = 0;


    switch(updata_status){
        case UPDATA_START:
            updata_para = ap_malloc(sizeof(group_data_st));
            if (NULL == updata_para){
                //todo: error
                break;
            }
            memcpy(updata_para, data_addr, sizeof(group_data_st));
            set_power_rate(updata_para->power, updata_para->tx_rate);
            set_frequence(updata_para->channel);
            current_list = (List_Elem *)extra;

            buffer1 = (trans_struct *)current_list->next;
            buffer2 = (trans_struct *)current_list;
            rf_queue_init(buffer1->buf, buffer1->data_len, buffer2->buf, buffer2->data_len);
#if 0
            if((timer=TIM_Open(100, timeout, TIMER_UP_CNT, TIMER_ONCE)) == TIMER_UNKNOW)
            {
                //todo: error
                break;
            }
#endif
            updata_status = UPDATA_BEGIN;
            break;
        case UPDATA_BEGIN:
            rf_infinite_post_send();
            updata_status = UPDATA_DOING;
            break;
        case UPDATA_DOING:

            break;
        case UPDATA_END:        //1.elinker send cmd stop; 2.timeout stop
            if (NULL != updata_para){
                ap_free(updata_para, sizeof(group_data_st));
            }
            updata_para = NULL;
            break;
        default:
            break;
    }

    return 0;
}


