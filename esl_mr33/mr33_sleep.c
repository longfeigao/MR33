/*
 * mr33_sleep.c
 *
 *  Created on: 2020��10��21��
 *      Author: gaolongfei
 */

#include "update_type.h"
#include "rf_handle.h"
#include "debug.h"

#include "cc2640r2_rf.h"
#include "timer.h"
#include "crc16.h"

#define CTRL_SLEEP          (uint8_t)(7 << 5)

static int8_t sleep_mode0(void* addr);
uint16_t cal_crc16(uint8_t ctrl, const uint8_t *eslid, const uint8_t *pdata, uint8_t len);
int8_t make_sleep_data(uint8_t *eslid, uint8_t x, uint8_t *pdata, uint8_t len);

int8_t sleep_handle(uint8_t** addr, uint8_t n, rf_parse_st* info, void * extra)
{
    int8_t ret = 0;
    sleep_st* sleep =  (sleep_st*)addr[n];

    pdebug("sleep:rate=%d,power=%d,mode=%d,interval=%d\n", sleep->rate, sleep->power, sleep->mode, sleep->interval);
    pdebug("idx=%d,times=%d,num=%d,default_len=%d\n", sleep->idx, sleep->times, sleep->num, sleep->default_len);

    if(sleep->num == 0) {
        goto done;
    }

    set_power_rate(sleep->power, sleep->rate);

    switch(sleep->mode) {
        case 0: // 0 is default
        default:
            if(sleep_mode0(sleep) < 0) {
                ret = -2;
            }
        break;
    }
done:
    return ret;
}


static int8_t sleep_mode0(void* addr)
{
    sleep_st* sleep =  (sleep_st*)addr;
    int8_t ret = 0;
    int16_t i, j;
    basic_data_st *cur = (basic_data_st*)sleep->data;
    uint8_t sleep_data_len  = 0;
    uint8_t sleep_times = 1;

    volatile uint8_t prev_channel=0;

    for(j = 0; j < sleep_times; j++) {
        if(Core_GetQuitStatus() == 1) {
            pdebug("sleep_mode0 quit2\r\n");
            break;
        }

        ret = 0;

        for( i = 0; i < sleep->num; i++) {
            if(Core_GetQuitStatus() == 1){
                pdebug("sleep_mode0 quit1\r\n");
                break;
            }

            if(0 == cur->len) {
                sleep->default_len = 0==sleep->default_len ? 26 : sleep->default_len;
                sleep_data_len = sleep->default_len;
                make_sleep_data(cur->id, sleep->idx, cur->data, sleep_data_len);
            }else {
                sleep_data_len = cur->len;
            }
            if (cur->channel != prev_channel){
                set_frequence(cur->channel);
            }
            prev_channel = cur->channel;
            send_data(cur->id, cur->data, sleep_data_len, 1000);

            pdebug("sleep %02X-%02X-%02X-%02X, channel=%d, datalen=%d: ", \
                   cur->id[0], cur->id[1], cur->id[2], cur->id[3], cur->channel, sleep_data_len);
            pdebughex(sleep->data, sleep_data_len);

            cur = (basic_data_st *)((uint8_t*)cur + offsetof(basic_data_st, data) + cur->len);
        }
    }

    return ret;
}

int8_t make_sleep_data(uint8_t *eslid, uint8_t x, uint8_t *pdata, uint8_t len)
{
    uint16_t crc = 0;

    if(len < 3) {
        return -1;
    }

    memset(pdata, 0, len);
    pdata[0] = CTRL_SLEEP | (eslid[x] & 0x1F);
    crc = cal_crc16(pdata[0], eslid, pdata+1, len-3); // 3 = 1 byte ctrl + 2 bytes crc
    pdata[len-2] = crc % 256;
    pdata[len-1] = crc / 256;

    return (int8_t)len;
}

uint16_t cal_crc16(uint8_t ctrl, const uint8_t *eslid, const uint8_t *pdata, uint8_t len)
{
    uint16_t crc = 0;

    crc = CRC16_CaculateStepByStep(crc, &ctrl, sizeof(ctrl));
    crc = CRC16_CaculateStepByStep(crc, pdata, len);
    crc = CRC16_CaculateStepByStep(crc, eslid, 4);

    return crc;
}


//todo: elinker do it
#if 0
int32_t sleep_init(sleep_st* slp, void* tmp)
{
    int16_t valid_slp_num;

    if((addr == 0) || (len == 0))
    {
        return -1;
    }

    sleep_addr = addr;
    sleep_len = len;

    Flash_Read(addr, (uint8_t *)&sleep_datarate, 2);
    Flash_Read(addr+2, &sleep_power, 1);
    Flash_Read(addr+3, &sleep_mode, 1);
    Flash_Read(addr+4, &sleep_interval, 1);
    Flash_Read(addr+5, &sleep_idx, 1);
    Flash_Read(addr+6, &sleep_times, 1);
    Flash_Read(addr+7, &sleep_default_len, 1);
    if((sleep_default_len==0) || (sleep_default_len > SIZE_MAX_ESL_BUF))
    {
        sleep_default_len = 26;
    }
    Flash_Read(addr+15, (uint8_t *)&sleep_num, 2);
    if (table->esl_num < 6){
        valid_slp_num = (table->esl_work_duration*1000 - \
                        FRAME1_TIME -\
                        table->max_esl_pkg_num*(table->tx_interval+1) - \
                        table->esl_num*(table->deal_duration + SEND_QUERY_TIME)*RETRY_TIMES -\
                        table->esl_num*SEND_SLEEP_TIME  \
                        )*2/3;      //1.5ms send sleep
//      pinfo("para:%d,%d,%d,%d,%d\r\n", table->esl_work_duration, table->esl_num, table->max_esl_pkg_num, table->deal_duration,table->tx_interval);
//      pinfo("cal:%d-%d-%d-%d-%d\r\n", table->esl_work_duration*1000, FRAME1_TIME, \
//                                   table->max_esl_pkg_num*(table->tx_interval+1), \
//                                   table->esl_num*(table->deal_duration + SEND_QUERY_TIME)*RETRY_TIMES, \
//                                   table->esl_num*SEND_SLEEP_TIME);
    }else {
        valid_slp_num = (table->esl_work_duration*1000 - \
                        FRAME1_TIME - \
                        (table->esl_num * table->max_esl_pkg_num*SEND_DATA_TIME+50)/100 - \
                        table->esl_num*(table->deal_duration + SEND_QUERY_TIME)*RETRY_TIMES -\
                        table->esl_num*SEND_SLEEP_TIME    \
                        )*2/3;      //1.5ms send sleep
//      pinfo("para:%d,%d,%d,%d\r\n", table->esl_work_duration, table->esl_num, table->max_esl_pkg_num, table->deal_duration);
//      pinfo("cal:%d-%d-%d-%d-%d\r\n", table->esl_work_duration*1000, FRAME1_TIME, \
//                                      (table->esl_num * table->max_esl_pkg_num*SEND_DATA_TIME+50)/100, \
//                                      table->esl_num*(table->deal_duration + SEND_QUERY_TIME)*RETRY_TIMES, \
//                                      table->esl_num*SEND_SLEEP_TIME);

    }

    if (valid_slp_num > 0){
        sleep_num = valid_slp_num<sleep_num ? valid_slp_num : sleep_num;
    }else{
        sleep_num = 0;
    }
    return 0;
}
#endif
