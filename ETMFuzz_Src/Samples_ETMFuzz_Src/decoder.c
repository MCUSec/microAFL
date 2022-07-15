#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "decoder.h"
#include "common.h"
#include "device.h"
#include "etm_decoder.h"

#define SEPARATOR_SIZE 4
#if TRACE_TO_FILE
const char SEPARATOR[SEPARATOR_SIZE] = {0xFF, 0xFF, 0xFF, 0x7F};
#else
const char SEPARATOR[SEPARATOR_SIZE] = {0x7F, 0xFF, 0xFF, 0xFF};
#endif

#define MASK 0x1
#define ID 1
#define DATA 0
#define RESP_NEW 0x0

#if DEBUG_MODE
    extern FILE* fp;
#endif

char payload[TRACESIZE] = {0x0};
int payload_size = 0;

int DecodeFrame(char* raw_data, int raw_data_size){
#if TRACE_TO_FILE
    char* local_raw_data = raw_data;
    char* raw_data_end = raw_data + raw_data_size -1 - 15;
    int i = 0;
    char auxiliary = 0;
    char curr_id = 0x0;
    payload_size = 0;
    while(1){
        if(strncmp(local_raw_data, SEPARATOR, SEPARATOR_SIZE) == 0){
            local_raw_data += SEPARATOR_SIZE;
        }
        if(local_raw_data > raw_data_end){
            break;
        }
        auxiliary = *(local_raw_data + 15);
        for(i=0; i < 7; i++){
            if((*local_raw_data & MASK) == ID){ // ID
                if((auxiliary & (1 << i)) == RESP_NEW){
                    curr_id = *local_raw_data >> 1;
                    local_raw_data++;
                    if(curr_id == ETMTRACEIDR_ID){
                        payload[payload_size++] = *local_raw_data;
                    }
                    local_raw_data++;
                }else{
                    if(curr_id == ETMTRACEIDR_ID)
                        payload[payload_size++] = *(local_raw_data + 1);
                    curr_id = *local_raw_data >> 1;
                    local_raw_data++;
                    local_raw_data++;
                }
            }else{ // Data
                if(curr_id == ETMTRACEIDR_ID){
                    payload[payload_size++] = (*local_raw_data & 0xFE) | ((auxiliary >> i) & 1);
                    payload[payload_size++] = *(local_raw_data + 1);
                }
                local_raw_data++;
                local_raw_data++;
            }
        }
        if((*local_raw_data & MASK) == ID){
            curr_id = *local_raw_data >> 1;
        }else{
            if(curr_id == ETMTRACEIDR_ID)
                payload[payload_size++] = (*local_raw_data & 0xFE) | ((auxiliary >> i) & 1);
        }
        local_raw_data++;
        local_raw_data++;
    }
#else
    char* local_raw_data = raw_data + raw_data_size - 1;
    char* end = raw_data + 15;
    char auxiliary = 0x0;
    int i = 0;
    char curr_id = 0x0;
    payload_size = 0;
    while(1){
        // skip seprator string
        if(strncmp(local_raw_data - SEPARATOR_SIZE + 1, SEPARATOR, SEPARATOR_SIZE) == 0){
            local_raw_data -= SEPARATOR_SIZE;
        }
        if(local_raw_data < end){
            break;
        }
        auxiliary = *(local_raw_data - 15);
        for(i = 0; i < 7; i++){
            if((*local_raw_data & MASK) == ID){ // ID
                if((auxiliary & (1 << i)) == RESP_NEW){
                    curr_id = *local_raw_data >> 1;
                    local_raw_data--;
                    if(curr_id == ETMTRACEIDR_ID){
                        payload[payload_size++] = *local_raw_data;  
                    }
                    local_raw_data--;
                }else{
                        if(curr_id == ETMTRACEIDR_ID)
                            payload[payload_size++] = *(local_raw_data - 1);
                        curr_id = *local_raw_data >> 1;
                        local_raw_data --;
                        local_raw_data --;
                }
            }else{ // Data
                if(curr_id == ETMTRACEIDR_ID){
                    payload[payload_size++] = (*local_raw_data & 0xFE) | ((auxiliary >> i) & 1);
                    payload[payload_size++] = *(local_raw_data-1);
                }
                local_raw_data--;
                local_raw_data--;
            }
        }
        if((*local_raw_data & MASK) == ID){
             curr_id =  *local_raw_data >> 1;
        }else{
            if(curr_id == ETMTRACEIDR_ID)
                payload[payload_size++] = (*local_raw_data & 0xFE) | ((auxiliary >> i) & 1);
        }
        local_raw_data--;
        local_raw_data--;
    }
#endif
    #if DEBUG_MODE
    // fprintf(fp, "payload_size=%d\tpayload:",payload_size);
    // int tmp = 0;
    // unsigned char tmp_chr = 0;
    // for(i=0;i<payload_size;i++){
    //     tmp_chr = payload[i];
    //     // int tmp = (int)tmp_chr;
    //     fprintf(fp, "0x%x,",tmp_chr);
    // }
    // fprintf(fp,"\n");
    #endif
    #if TIME_EVA
        gettimeofday(&end, 0);
        fprintf(fp_time, "DecodeFrame() decode TPIU:%d\t", (int)(1000 * (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000));
        gettimeofday(&start, 0);
    #endif
    i = DecodeETM(payload, payload_size);
    #if TIME_EVA
        gettimeofday(&end, 0);
        fprintf(fp_time, "DecodeFrame() decode ETM:%d\t", (int)(1000 * (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000));
        gettimeofday(&start, 0);
    #endif 
    return i;
}