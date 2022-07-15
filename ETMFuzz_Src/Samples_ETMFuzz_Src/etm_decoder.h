#ifndef __ETM_DECODER_H__
#define __ETM_DECODER_H__
#include <stdint.h>

void init_decoder();
void init_bitmap(void *ptr);
void init_start_addr(uint32_t value);
int DecodeETM(uint8_t* payload, int size);

#endif