#include "etm_decoder.h"
#include "config.h"
#include "common.h"
#include "JLINK_API.h"
#include "device.h"
#include <string.h>
#define branch_scheme 1 // 0: origin, 1: alternative

// ROR & ROL for ETM calculate with branch and necount
#define CROR(value, bits) (((value) >> (bits)) | ((value) << (32 - (bits))))
#define CROL(value, bits) (((value) << (bits)) | ((value) >> (32 - (bits))))
// #define NEHASH(value, bits) CROR(value, bits)

#if DEBUG_DECODER
	FILE* fp;
#endif
#if TIME_EVA
    extern struct timeval start, end;
    extern FILE *fp_time;
#endif

char* bitmap_ptr = 0x0;
uint32_t branches[TRACESIZE/4] = {0x0};
int branch_num = 0;
#if HASH_VERSION==0
    int necount = 0;
#else
    char bit_stream_buffer[MAX_STREAM_SIZE] = {0x0};
    int bit_stream_size = 0;
#endif
uint32_t etm_prev_branch = 0x0;
uint32_t base_addr = 0x0; // used for I-Sync to update the whole current PC value. And to decompress branch address in branch packet.
// uint32_t prev_basic_addr = 0x0; // used to nearest known address when exit exception as prev branch addr
int in_exception = 0; // set 1 when meet exception, set 0 when meet exception exit
int just_exit = 0; // used to drop previous task final

int error_flag = 0;
uint32_t prev_branch = 0x0;

void init_bitmap(void *ptr){
    bitmap_ptr = ptr;
}

void init_decoder(){
    branch_num = 0;
#if HASH_VERSION==0
    necount = 0;
#else
    bit_stream_size = 0;
#endif
    etm_prev_branch = 0;
    base_addr = 0;
    in_exception = 0;
    just_exit = 0;
    error_flag = 0;
    prev_branch = 0x0;
}

void init_start_addr(uint32_t value){
    base_addr = etm_prev_branch = value;
}

static void AFL_ERROR_OUT(const uint8_t* sError) {
	// printf("\nERROR: %s\n", sError);
    // printf("press any key to exit.\n");
    // system("pause");
    // exit(-1);
    error_flag = 1;
}

static void AFL_maybe_log(uint32_t curr_branch){
    #if DEBUG_DECODER
        fprintf(fp, "prev_branch=0x%x,curr_branch=0x%x\n",prev_branch,curr_branch);
    #endif
	curr_branch ^= curr_branch >> 16;
	curr_branch *= 0x85ebca6b;
	curr_branch ^= curr_branch >> 13;
	curr_branch *= 0xc2b2ae35;
	curr_branch ^= curr_branch >> 16;
	curr_branch = (curr_branch >> 4) ^ (curr_branch << 8);
	curr_branch &= MAP_SIZE - 1;
	bitmap_ptr[curr_branch ^ prev_branch]++;
    #if DEBUG_DECODER
        fprintf(fp, "bitmap[%d]=%d\n",curr_branch ^ prev_branch,bitmap_ptr[curr_branch ^ prev_branch]);
    #endif
	prev_branch = curr_branch >> 1;
}

uint32_t NEHASH(uint32_t value, int bits){
#if HASH_VERSION==0    
    value += bits;
    return CROR(value, bits);
#else
    uint32_t t = 0;
    uint32_t t_5b = 0;
    uint32_t t_5b_size = 0;
    for(int i = 0; i < bit_stream_size; i++){
        t_5b = t_5b << 1;
        t_5b_size++;
        if(bit_stream_buffer[i] == 1){
            t_5b |= 0x1;
        }
        if(t_5b_size == 5){
            t ^= t_5b;
            t_5b = 0;
            t_5b_size = 0;
        }
    }
    value = value + t;
    uint32_t left = (value << (32 - t)) | (value >> t);
    uint32_t right = (value << t) | (value >> (32 - t));
    return (left | right);
#endif
}

int is_from_branch(uint32_t branch_addr){
    uint32_t inst_addr = etm_prev_branch;
    uint8_t inst_value = 0;
    int temp_necount;
    #if HASH_VERSION==0
        temp_necount = necount;
    #else
        temp_necount = bit_stream_size;
    #endif
    int i = 0;
#if TIME_EVA
	gettimeofday(&end, 0);
	fprintf(fp_time, "before is_from_branch:%d\t", (int)(1000 * (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000));
	gettimeofday(&start, 0);
#endif
    while(temp_necount != 0 && error_flag == 0){
        i = 3;
        do{
            if(JLINKARM_ReadMemU8(inst_addr, 1, &inst_value, NULL) >= 0){
                #if DEBUG_DECODER
                    fprintf(fp, "instruction at %x is %x\n", inst_addr, inst_value);
                #endif
                if((inst_value & 0xE0) == 0xE000 && (inst_value & 0x18) != 0x0){ // 32 bits
                    #if DEBUG_DECODER
                        fprintf(fp, "32 bit instruction\n");
                    #endif
                    inst_addr += 4;
                }
                else{ // 16 bits
                    #if DEBUG_DECODER
                        fprintf(fp, "16 bit instruction\n");
                    #endif
                    inst_addr += 2;
                }
                temp_necount -= 1;
                break;
            }
            i--;
        }while(i);
        if(i <= 0){
            // printf("You shouldn't reach here\n");
            // exit(-1);
            error_flag = 1;
        }
    }
    #if DEBUG_DECODER
        fprintf(fp, "compare with 0x%x\n",branch_addr);
    #endif
#if TIME_EVA
	gettimeofday(&end, 0);
	fprintf(fp_time, "is_from_branch:%d\t", (int)(1000 * (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000));
	gettimeofday(&start, 0);
#endif
    if(inst_addr == branch_addr){
        #if DEBUG_DECODER
            fprintf(fp, "exception not from branch\n");
        #endif
        return 0; // exception not from branch
    }else{
        #if DEBUG_DECODER
            fprintf(fp, "exception from branch\n");
        #endif
        return 1; // exception from branch
    }
}

static uint8_t* Decode_ASync(uint8_t* p, uint8_t* end){
    int i = 4;
    #if DEBUG_DECODER
        // fprintf(fp, "Decode_ASync\n");
    #endif
    while(i--){ // 2th to 5th
        p++;
        if(p >= end){
            return p;
        }
        if(*p != 0x0){  // 2th
            AFL_ERROR_OUT("Async error 1\n");
            #if DEBUG_DECODER
                fprintf(fp, "Async error 1\n");
            #endif
        }
    }
    do{
        p++;
        if(p >= end){
            return p;
        }
    }while(*p == 0x0); // more
    if(p >= end){
        return p;
    }
    if(*p != 0x80){
        AFL_ERROR_OUT("Async error 5\n");
        #if DEBUG_DECODER
            fprintf(fp, "Async error 5\n");
        #endif
    }
    p++;
    return p;
}

static uint8_t* Decode_ISync(uint8_t* p, uint8_t* end){
    p++; // point to information byte due to cortex-M no ContextID
    if(p >= end){
        return p;
    }
    // do what you want about information byte
    p++;
    // address
    if(p >= end){
        return p;
    }
    base_addr = 0x0;
    base_addr |= *p >> 1 << 1;
    p++;
    if(p >= end){
        return p;
    }
    base_addr |= *p << 8;
    p++;
    if(p >= end){
        return p;
    }
    base_addr |= *p << 16;
    p++;
    if(p >= end){
        return p;
    }
    base_addr |= *p << 24;
    // if(!in_exception)
    //     prev_basic_addr = base_addr;
    #if DEBUG_DECODER
        // fprintf(fp, "Decode_ISync = 0x%x\n", base_addr);
    #endif
    p++;
    return p;
}

#if HASH_VERSION==0
static uint8_t* Decode_Pheader(uint8_t* p){
    if(!in_exception){ // not in exception
        if((*p & 0x02) == 0){// Format 1
            necount += (*p >> 2) & 0x0F;
            necount += (*p >> 6) & 0x01;
        }else{// Format 2
            necount += ((*p >> 2) & 0x01) + ((*p >> 3) & 0x01);
        }
    }
#if DEBUG_DECODER
    else{ // in exception now
        fprintf(fp, "do not handle necount in exception\n");
    }
    // fprintf(fp, "Decode_Pheader, necount = %d\n", necount);
#endif
    p++;
    return p;
}
#else
static uint8_t* Decode_Pheader(uint8_t* p){
    if(!in_exception){ // not in exception
        if((*p & 0x02) == 0){// Format 1
            int e_num = (*p >> 2) & 0x0F;
            int n_num = (*p >> 6) & 0x01;
            memset(bit_stream_buffer + bit_stream_size, 0x1, e_num);
            bit_stream_size += e_num;
            if(n_num == 1){
                bit_stream_size++;
            }
            if(bit_stream_size >= MAX_STREAM_SIZE){
                assert(!"bit stream buffer is too small, please increase MAX_STREAM_SIZE");
            }
        }else{// Format 2
            int num_1 = (*p >> 3) & 0x01;
            int num_2 = (*p >> 2) & 0x01;
            if(num_1 == 0){
                bit_stream_buffer[bit_stream_size] = 1;
            }
            bit_stream_size++;
            
            if(num_2 == 0){
                bit_stream_buffer[bit_stream_size] = 1;
            }
            bit_stream_size++;
        }
    }
    p++;
    return p;
}
#endif

static void Write_Branch(uint32_t tar_addr, int bits){
#if HASH_VERSION==0
    #if DEBUG_DECODER
        fprintf(fp, "etm_prev_branch = 0x%x, necount = %d\n", etm_prev_branch, necount);
        fprintf(fp, "bits = 0x%x, tar_addr = 0x%x, new etm_prev_branch = 0x%x\n", bits, tar_addr, (base_addr >> bits << bits) | tar_addr);
        fprintf(fp, "hash result = 0x%x\n", NEHASH(etm_prev_branch, necount));
    #endif
    // branches[branch_num++] = NEHASH(etm_prev_branch, necount);
    AFL_maybe_log(NEHASH(etm_prev_branch, necount));
    necount = 0;
#else
    #if DEBUG_DECODER
        fprintf(fp, "etm_prev_branch = 0x%x, necount = %d\n", etm_prev_branch, bit_stream_size);
        fprintf(fp, "bits = 0x%x, tar_addr = 0x%x, new etm_prev_branch = 0x%x\n", bits, tar_addr, (base_addr >> bits << bits) | tar_addr);
        fprintf(fp, "hash result = 0x%x\n", NEHASH(etm_prev_branch, bit_stream_size));
    #endif
    AFL_maybe_log(NEHASH(etm_prev_branch, bit_stream_size));
    bit_stream_size = 0;
#endif
    etm_prev_branch = (base_addr >> bits << bits) | tar_addr;
}

static uint8_t* Decode_Branch(uint8_t* p, uint8_t* end){
#if (branch_scheme == 1)
    #if DEBUG_DECODER
        fprintf(fp, "Decode_Branch\n");
    #endif
    uint32_t tar_addr = 0x0;
    int bits = 0;
    tar_addr |= (*p & 0x7E); // address[6:1]
    if((*p & 0x80) == 0){ // byte 0 in address
        #if DEBUG_DECODER
            fprintf(fp, "byte0 in address\n");
        #endif
        if(!in_exception){   
            Write_Branch(tar_addr, 7);
        }
    #if DEBUG_DECODER
        else{
            fprintf(fp, "do not handle branch in exception\n");
        }
    #endif
        p++;
        return p;
    }
    p++;
    if((*p & 0x80)){ // byte 1 in address
        #if DEBUG_DECODER
            fprintf(fp, "byte1 in address\n");
        #endif
        tar_addr |= (*p & 0x7F) << 7; // address[13:7]
    }else{
        #if DEBUG_DECODER
            fprintf(fp, "byte1 not in address\n");
        #endif
        tar_addr |= (*p & 0x3F) << 7; // address[12:7]
        bits = 13;
        goto exception;
    }
    p++;
    if((*p & 0x80)){ // byte 2 in address
        tar_addr |= (*p & 0x7F) << 14; // address[20:14]
        #if DEBUG_DECODER
            fprintf(fp, "byte2 in address\n");
        #endif
    }else{
        tar_addr |= (*p & 0x3F) << 14; // address[19:14]
        bits = 20;
        #if DEBUG_DECODER
            fprintf(fp, "byte2 not in address\n");
        #endif
        goto exception;
    }
    p++;
    if((*p & 0x80)){ // byte 3 in address
        tar_addr |= (*p & 0x7F) << 21; // address[27:21]
        #if DEBUG_DECODER
            fprintf(fp, "byte3 in address\n");
        #endif
    }else{
        tar_addr |= (*p & 0x3F) << 21; // address[26:21]
        bits = 27;
        #if DEBUG_DECODER
            fprintf(fp, "byte3 not in address\n");
        #endif
        goto exception;
    }
    p++;
    tar_addr |= (*p & 0x0F) << 28; // address[31:28]
    bits = 32;

exception:
    if((*p & 0x40)){
        #if DEBUG_DECODER
            fprintf(fp, "hit exception\n");
        #endif
        in_exception = 1;
        p++;
        if((*p & 0x20)){ // Cancel is set
        #if HASH_VERSION==0
            necount--;
        #else
            bit_stream_size--;
        #endif
        }
        if((*p & 0x80)){ // byte 0 in exception
            p++;
            #if DEBUG_DECODER
                fprintf(fp, "byte 0 in exception\n");
            #endif
            if((*p & 0x80)){ // byte 1 in exception
                p++;
                #if DEBUG_DECODER
                    fprintf(fp, "byte 1 in exception\n");
                #endif
            }
        }
    }
    if(!in_exception){ // may just enter an exception or branch is generated inside an exception
        // prev_basic_addr = (base_addr >> bits << bits) | tar_addr;
        if(just_exit == 2){
            #if DEBUG_DECODER
                fprintf(fp, " old just_exits 2 and now is 1\n");
            #endif
            if(is_from_branch((base_addr >> bits << bits) | tar_addr)){
                Write_Branch(tar_addr, bits);
            }
            just_exit--;
        }else if(just_exit == 1){
            // if(is_from_branch((base_addr >> bits << bits) | tar_addr)){
                Write_Branch(tar_addr, bits);
            // }
            just_exit--;
            #if DEBUG_DECODER
                fprintf(fp, "just_exit is zero\n");
            #endif
        }
        else{
            Write_Branch(tar_addr, bits);
        }
    }
    base_addr = (base_addr >> bits << bits) | tar_addr;
    #if DEBUG_DECODER
        fprintf(fp, "Decode_Branch in_exception = %d\n", in_exception);
        // fprintf(fp, "Decode_Branch just_exit = %d\n", just_exit);
        fprintf(fp, "Decode_Branch base_addr = 0x%x\n", base_addr);
    #endif
    p++;
    return p;
#else
#endif
}

static uint8_t* Decode_ExcepExit(uint8_t* p){
    in_exception = 0;
    just_exit = 2;
    p++;
    return p;
}
#if DEBUG_DECODER
    unsigned long ed_file_idx = 0;
    char ed_log_file[128] = {0x0};
#endif
int DecodeETM(uint8_t* payload, int size){
#if HASH_VERSION==0
    necount = 0;
#else
    bit_stream_size = 0;
#endif
    branch_num = 0;
    in_exception = 0;
    just_exit = 0;

    uint8_t* p = payload;
    uint8_t* end = payload + size;
    #if DEBUG_DECODER
        if(fp != NULL)
            fclose(fp);
        fp = NULL;
        memset(ed_log_file, 0x0, 128);
        sprintf(ed_log_file,"%lu_ed.txt",ed_file_idx++);
        fp = fopen(ed_log_file, "w");
    #endif
    while(p < end){
        if(*p == 0x00){ // A-Sync
            p = Decode_ASync(p, end);
        }
        else if(*p == 0x08){ // I-Sync
            p = Decode_ISync(p, end);
        }
        else if((*p & 0x81) == 0x80 ){ // P-header
            p = Decode_Pheader(p);
        }
        else if((*p &0x01) == 0x1){ // Branch
            p = Decode_Branch(p, end);
        }
        else if(*p == 0x76){ // Exception Exit
            p = Decode_ExcepExit(p);
        }
        else{
            AFL_ERROR_OUT("Unknow packet type.");
            error_flag = 1;
            #if DEBUG_DECODER
                fprintf(fp, "Unknown packet type: %x\n", *p);
            #endif
        }
        if(error_flag){
            #if DEBUG_DECODER
                fprintf(fp, "error_flag set\n");
            #endif
            return -1;
        }
    }
    #if DEBUG_DECODER
        fprintf(fp, "Last Write_Branch\n");
    #endif
    if(!in_exception && just_exit==0){
        Write_Branch(0, 0);
    }
    #if DEBUG_DECODER
        fprintf(fp, "\nbitmap:");
		for (int i = 0; i < MAP_SIZE; i++)
		{
			if (bitmap_ptr[i] != 0)
			{
                char c = (char)(bitmap_ptr[i]);
				fprintf(fp, "[%d:%d]\t", i, (int)c);
			}
		}
		fprintf(fp, "\n\n");
    #endif
    return 0;
}