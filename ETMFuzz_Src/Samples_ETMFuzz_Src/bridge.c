#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "JLINK_API.h"


#include "config.h"
#include "bridge.h"
#include "device.h"
#include "decoder.h"
#include "common.h"
#include "trace.h"
#include "Init.h"
#include "etm_decoder.h"

// #define ACSIZE 0x400
#define TIME_INTERVAL 1 // usec for usleep / microseconds for wait API
// #define TRACENUM 1000

#define NO_BRANCH 0xffffffff

static U32 prev_branch = 0x0;

static int need_power_reset = 1;

U8 global_trace_buf[TRACESIZE];

struct timeval starttime, endtime;
#ifdef TIME_EVA
struct timeval start, end;
FILE *fp_time = NULL;
U8 tmpbuf[0xFFFF];
#endif
#if DEBUG_MODE
FILE *fp = NULL;
#endif

//=========================Static functions===================================

static void AFL_ERROR_OUT(const char *sError)
{
	#if DEBUG_MODE
		fprintf(fp, sError);
		fprintf(fp, "\n");
	#endif
	printf("\nERROR: %s\n", sError);
	printf("press any key to exit.\n");
	system("pause");
	exit(-1);
}
static U8 padded_tc[TESTCASE_LIMIT_SIZE];
static int WriteTestCase(U8 *testcase, U32 size)
{
	int len = 0;
	int i = 3;
	// This operation is due to Segger SDK inner bug of magic length 1013
	// if (size == 1013)
	// {
	// 	#if DEBUG_MODE
	// 		fprintf(fp, "size is 1013\n");
	// 	#endif
	// 	size = 1012;
	// 	testcase[1012] = 0x0;
	// }
	// End
	int remain;
	if(size >= 4)
		remain = size % 4;
	else
		remain = size;
	int padded_size = size;
	if(remain != 0){
		padded_size = 4 * (size / 4 + 1);
	}
	if(padded_size > TESTCASE_LIMIT_SIZE){
		padded_size = TESTCASE_LIMIT_SIZE;
		memcpy(padded_tc, testcase, TESTCASE_LIMIT_SIZE);
	}else{
		memcpy(padded_tc, testcase, size);
	}
	#if DEBUG_MODE
		fprintf(fp, "real size = %u, padded tc size=%u\n",size, padded_size);
	#endif
	do
	{
		len = JLINKARM_WriteMem(TESTCASEADDR, padded_size, padded_tc);
		if (len == padded_size)
		{
			break;
		}
	} while (i--);
	if(i<=0){
		#if DEBUG_MODE
			fprintf(fp, "Failed size:%u, return size:%u\n",size, len);
		#endif
		// AFL_ERROR_OUT("Write testcase fails\n");
	}
	return len;
}

static void WriteTestCaseLen(U32 size)
{
	if(size > TESTCASE_LIMIT_SIZE)
		size = TESTCASE_LIMIT_SIZE;
	if (JLINKARM_WriteU32(TESTCASELENADDR, size))
		AFL_ERROR_OUT("Failt to write testcase length into target device.");
}

static void SetCrashCatch()
{
	U32 reg = 0x0;
	JLINKARM_ReadMemU32(DEMCR, 1, &reg, NULL);
#if DEBUG_MODE
	fprintf(fp, "Before Modification DEMCR is 0x%x\n", reg);
#endif
	if ((reg & 0x7F0) != 0x7F0)
	{
		reg |= 0x7F0;
		JLINKARM_WriteU32(DEMCR, reg);
		JLINKARM_ReadMemU32(DEMCR, 1, &reg, NULL);
		if ((reg & 0x7F0) != 0x7F0)
		{
			AFL_ERROR_OUT("Fail to set crash catch. Please check the program.");
		}
	}
#if DEBUG_MODE
	fprintf(fp, "After Modification DEMCR is 0x%x\n", reg);
#endif
}

static int IsCrashed()
{
	U32 reg = 0x0;
	JLINKARM_ReadMemU32(DFSR, 1, &reg, NULL);
	if (reg & 0x8)
	{
#if DEBUG_MODE
		fprintf(fp, "Crash Reason: 0x%x\n", reg);
#endif
		return 1;
	}
	else
	{
		return 0;
	}
	// if(JLINKARM_ReadMemU32(ICSR, 1, &reg, NULL) < 0)
	// reg &= 0x1FF;
	// ICSR[8:0] means The exception number of the current executing exception
	// A value of 0 indicates that the processor is in Thread mode
	// This also could be used to catch crash
	// But I'm wondering exeception should be catched all of them?
}

extern uint32_t etm_prev_branch;
static int Processor_Reset(U8 *testcase, U32 size)
{
	U32 reg;
	U32 value;
	int r = 0;

#ifdef TIME_EVA
	gettimeofday(&start, 0);
#endif
#if (PERSISTENT_MODE == 1)
	if (need_power_reset)
	{
		JLINKARM_Reset();
		// Set ETM Configure
		ConfigTraceRegs();
		// Set the Vector Catch for crash
		SetCrashCatch();
		need_power_reset = 0;
		// StartTrace();
		JLINKARM_Go();
	}
#else
	if (need_power_reset)
	{
// 		do
// 		{
// 			r = JLINKARM_Halt();
// #if DEBUG_MODE
// 			fprintf(fp, "init halt\n");
// #endif
// 		} while (r != 0);
// 		StopTrace();
		// do
		// { // clear trace buf
		// 	r = JLINKARM_RAWTRACE_Read(global_trace_buf, TRACESIZE);
		// } while (r != 0);

		r = JLINKARM_Reset();
		if(r < 0){
			AFL_ERROR_OUT("JLINKARM_Reset failed\n");
		}
		// Execute JLINK Script
		OneTimeTraceStart();
		// Set ETM Configure
		ConfigTraceRegs();
		// Set the Vector Catch for crash
		SetCrashCatch();
		need_power_reset = 0;
		// StartTrace();
		JLINKARM_Go();
	}
	else
	{
// trigger reset register
#if (DEVICE == 0)
		JLINKARM_ReadMemU32(AIRCR, 1, &reg, NULL);
		reg &= 0x0000FFFF;
		// VECTORRESET
		// reg |= 0x05FA0001;
		// SYSRESETREQ
		reg |= 0x05FA0004;
		JLINKARM_WriteU32(AIRCR, reg);
#elif (DEVICE == 1)
		JLINKARM_ReadMemU32(AIRCR_S, 1, &reg, NULL);
		reg &= 0x0000FFFF;
		reg |= 0x05FA0004;
		JLINKARM_WriteU32(AIRCR_S, reg);
#endif
		JLINKARM_WriteBits(); // confirm effective
	}
#endif
#ifdef TIME_EVA
	gettimeofday(&end, 0);
	fprintf(fp_time, "0:%d\t", (int)(1000 * (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000));
	gettimeofday(&start, 0);
#endif
#if (FEED_WITH_RESET == 1)
	if(WriteTestCase(testcase, size) < 0){
		return -1;
	}
#endif
// 	if (size > TESTCASE_LIMIT_SIZE)
// 	{
// 		r = WriteTestCase(testcase, TESTCASE_LIMIT_SIZE);
// 		if (r != TESTCASE_LIMIT_SIZE)
// 		{
// #if DEBUG_MODE
// 			fprintf(fp, "WriteTestCase 1 failed.\n");
// #endif
// 			return -1;
// 		}
// 	}
// 	else
// 	{
// 		r = WriteTestCase(testcase, size);
// 		if (r != size)
// 		{
// #if DEBUG_MODE
// 			fprintf(fp, "WriteTestCase 2 failed.\n");
// #endif
// 			return -1;
// 		}
// 	}

#if (PERSISTENT_MODE == 1)
	JLINKARM_WriteBits();
#endif
#ifdef TIME_EVA
	gettimeofday(&end, 0);
	fprintf(fp_time, "1:%d\t", (int)(1000 * (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000));
	gettimeofday(&start, 0);
#endif
	r = JLINKARM_WaitForHalt(FIRST_WAIT);
	if (r == 1)
	{
#ifdef TIME_EVA
		gettimeofday(&end, 0);
		fprintf(fp_time, "2:%d\t", (int)(1000 * (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000));
		gettimeofday(&start, 0);
#endif
		reg = JLINKARM_ReadReg(PC_REG);
		JLINKARM_ReadMemU32(reg, 1, &value, NULL);
		if ((value & 0xFF) == MAIN_FLAG)
		{
			init_start_addr(reg);
			#if (FEED_WITH_RESET==0)
				if(WriteTestCase(testcase, size) < 0){
					return -1;
				}
			#endif
			WriteTestCaseLen(size);
			// if (size > TESTCASE_LIMIT_SIZE)
			// {
			// 	WriteTestCaseLen(TESTCASE_LIMIT_SIZE);
			// }
			// else
			// {
			// 	WriteTestCaseLen(size);
			// }
#ifdef TIME_EVA
			gettimeofday(&end, 0);
			fprintf(fp_time, "3:%d\t", (int)(1000 * (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000));
			gettimeofday(&start, 0);
#endif
			// Test: check if testcase and size writed into the memory
			// #if DEBUG_MODE
			// 	U32 size;
			// 	U8 testcase[1024] = {0};
			// 	JLINKARM_ReadMemU32(TESTCASELENADDR, 1, &size, NULL);
			// 	JLINKARM_ReadMemU8(TESTCASEADDR, size, testcase, NULL);
			// #endif
			// Test end
			JLINKARM_Step();
#ifdef TIME_EVA
			gettimeofday(&end, 0);
			fprintf(fp_time, "4:%d\t", (int)(1000 * (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000));
			gettimeofday(&start, 0);
#endif
			// #ifdef TIME_EVA
			// 	int cnt = 0;
			// 	do{
			// 		cnt = JLINKARM_RAWTRACE_Read(tmpbuf,0xFFFF);
			// 	}while(cnt!=0);
			// #endif
			if(StartTrace() == -1)
				return -1;
#ifdef TIME_EVA
			gettimeofday(&end, 0);
			fprintf(fp_time, "5:%d\t", (int)(1000 * (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000));
			gettimeofday(&start, 0);
#endif
			JLINKARM_Go();
#ifdef TIME_EVA
			gettimeofday(&end, 0);
			fprintf(fp_time, "6:%d\t", (int)(1000 * (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000));
			gettimeofday(&start, 0);
#endif
			return 0;
		}
		else
		{
#if DEBUG_MODE
			fprintf(fp, "JLINKARM_WaitForHalt failed.\n");
			fprintf(fp, "current pc = 0x%x\n", reg);
#endif
		}
	}
	return -1;
}

static int GetHaltReason()
{
	U32 reg = 0x0;
	U32 value = 0x0;
#ifdef TIME_EVA
	gettimeofday(&end, 0);
	fprintf(fp_time, "9:%d\t", (int)(1000 * (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000));
	gettimeofday(&start, 0);
#endif

	if (IsCrashed())
	{
#if DEBUG_MODE
		fprintf(fp, "crashed\n");
#endif
		return CRASH;
	}
	else
	{
		reg = JLINKARM_ReadReg(PC_REG);
		if (JLINKARM_ReadMemU32(reg, 1, &value, NULL) < 0)
		{
			// AFL_ERROR_OUT("Fail to read PC at the end of testcase execution.");
			return -1;
		}
#ifdef TIME_EVA
		gettimeofday(&end, 0);
		fprintf(fp_time, "10:%d\t", (int)(1000 * (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000));
		gettimeofday(&start, 0);
#endif
		// if(reg == BKPT_END_OF_TESTCASE )
		// 	return END_OF_TESTCASE;
		// else
		// 	AFL_ERROR_OUT("Unexpected when get halt reason.");
		if ((value & 0xFF) == END_FLAG)
		{
			return END_OF_TESTCASE;
		}
		else
		{
			// AFL_ERROR_OUT("Unexpected when get halt reason.");
			return -1;
		}
	}
}
static int ExecOnBoard(U32 timeout, U8 *testcase, U32 size)
{
	int r = 0;
	int ExecOnBoard_Retry = 3;
	int i = 0;
	do
	{
#if DEBUG_MODE
		fprintf(fp, "New Round.\n");
#endif
		init_decoder();
		r = Processor_Reset(testcase, size);
		if (r == 0)
		{
			r = JLINKARM_WaitForHalt(timeout);
#ifdef TIME_EVA
			gettimeofday(&end, 0);
			fprintf(fp_time, "7:%d\t", (int)(1000 * (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000));
			gettimeofday(&start, 0);
#endif
			switch (r)
			{
			case 0: // CPU not halted (timeout reached)
				// printf("round not stop\n");
				if(JLINKARM_Halt()==1){
					// AFL_ERROR_OUT("Fail to halt CPU 1.");
				}else{
					StopTrace();
				}
				// usleep(1000);
				need_power_reset = 1;
#if DEBUG_MODE
				fprintf(fp, "round not stop\n");
#endif
				return TIMEOUT;
			case 1: // CPU halted
#if DEBUG_MODE
				fprintf(fp, "round stop normally\n");
#endif
#ifdef TIME_EVA
				gettimeofday(&end, 0);
				fprintf(fp_time, "8:%d\t", (int)(1000 * (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000));
				gettimeofday(&start, 0);
#endif
				r = GetHaltReason();
				if (r < 0)
				{
#if DEBUG_MODE
					fprintf(fp, "halt unknown reason\n");
#endif
					need_power_reset = 1;
				}
				else
				{
					if(StopTrace() == -1){
						need_power_reset = 1;
					}else
						return r;
				}
			default: // < 0, Error
#if DEBUG_MODE
				fprintf(fp, "round stop with error\n");
#endif
				if(JLINKARM_Halt()==1){
					// AFL_ERROR_OUT("Fail to halt CPU 2.");
				}else{
					StopTrace();
				}
				need_power_reset = 1;
			}
		}
		else
		{
#if DEBUG_MODE
			fprintf(fp, "ExecOnBoard_Retry = %d\n", ExecOnBoard_Retry);
#endif
			if(JLINKARM_Halt()==1){
				// AFL_ERROR_OUT("Fail to halt CPU 3.");
			}else{
				StopTrace();
			}
			need_power_reset = 1;
		}
	} while (ExecOnBoard_Retry--);
	return -1;
}

static int not_mapped = 1;
void *mapped_trace_buf = NULL;
static int GetBranchs(U8 * global_trace_buf)
{
	long r = 0;
	int i = 0;
	void *trace_buf = global_trace_buf;
	usleep(100);

#if TRACE_TO_FILE
	/*
	// mmap trace file
	int trace_fd = 0;
	struct stat statbuf;
    int err = 0;
	trace_fd = open(RAW_TRACE_FILE, O_RDONLY);
	if(trace_fd < 0){
		perror(NULL);
		AFL_ERROR_OUT("Fail to open trace log file\n");
	}
	err = fstat(trace_fd, &statbuf);
	if(err < 0){
		perror(NULL);
		AFL_ERROR_OUT("Fail to get status of the trace log file\n");
	}
	r = statbuf.st_size;
	if(not_mapped){
		trace_buf = mmap(trace_buf, TRACESIZE, PROT_READ, MAP_SHARED, trace_fd, 0);
		if(trace_buf == MAP_FAILED){
			perror(NULL);
			AFL_ERROR_OUT("Fail to map trace log file\n");
		}
		not_mapped = 0;
	}
	close(trace_fd);
*/

	// read trace file directly
	FILE *trace_file = fopen(RAW_TRACE_FILE, "rb");
	if (trace_file == NULL)
	{
		perror(NULL);
		AFL_ERROR_OUT("Fail to open trace log file\n");
	}
	fseek(trace_file, 0, SEEK_END);
	r = ftell(trace_file);
#if DEBUG_MODE
	// if (timeout_flag)
	// 	fprintf(fp, "trace file size = %ld\n", r);
#endif
	fseek(trace_file, 0, SEEK_SET);
	if (r > TRACESIZE)
	{
		return -1;
	}
	r = fread(trace_buf, 1, r, trace_file);
	fclose(trace_file);

#if DEBUG_MODE
	// fprintf(fp, "read out %ld bytes\n", r);
	// for (i = 0; i < r; i++)
	// {
	// 	fprintf(fp, "0x%x,", (char*)(trace_buf+i));
	// }
	// fprintf(fp, "\n");
#endif
#else
	r = JLINKARM_RAWTRACE_Read(trace_buf, TRACESIZE);
#if DEBUG_MODE
	fprintf(fp, "JLINKARM_RAWTRACE_Read %ld bytes:\n", r);
	for (i = 0; i < r; i++)
	{
		fprintf(fp, "0x%x,", trace_buf[i]);
	}
	fprintf(fp, "\n");
#endif
#endif
#if TIME_EVA
	gettimeofday(&end, 0);
	fprintf(fp_time, "read trace file():%d\t", (int)(1000 * (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000));
	gettimeofday(&start, 0);
#endif
	if (r <= 0)
	{
		return -1;
	}
	else
	{
		r = DecodeFrame(trace_buf, r);
		return r;
	}
}

// static void AFL_maybe_log(U8* bitmap, U32 curr_branch){
// 	curr_branch ^= curr_branch >> 16;
// 	curr_branch *= 0x85ebca6b;
// 	curr_branch ^= curr_branch >> 13;
// 	curr_branch *= 0xc2b2ae35;
// 	curr_branch ^= curr_branch >> 16;
// 	curr_branch = (curr_branch >> 4) ^ (curr_branch << 8);
// 	curr_branch &= MAP_SIZE - 1;
// 	bitmap[curr_branch ^ prev_branch]++;
// 	prev_branch = curr_branch >> 1;
// }

extern INIT_PARAS _Paras;

void AFL_JLinkInit(char *target_bin)
{
	int r;
#ifdef TIME_EVA
	fp_time = fopen("time_eva.txt", "w");
#endif
// #if DEBUG_MODE
// 	fp = fopen("branches.txt", "w");
// #endif
	memset(&_Paras, 0, sizeof(INIT_PARAS));
	_InitConfig();
	if (_InitDebugSession(&_Paras) < 0)
	{
		AFL_ERROR_OUT("Initialization of debug session failed.");
	}
#if (NEED_DOWNLOAD == 1)
	r = JLINK_DownloadFile(target_bin, IMAGEPOS);
	if (r < 0)
	{
		AFL_ERROR_OUT("Download file failed.");
	}
#endif

	need_power_reset = 1;
	// do{
	// 	r = JLINKARM_Halt();
	// 	#if DEBUG_MODE
	// 		fprintf(fp, "init halt\n");
	// 	#endif
	// }while(r != 0);
	// StopTrace();
	// do{ // clear trace buf
	// 	r = JLINKARM_RAWTRACE_Read(global_trace_buf, TRACESIZE);
	// }while(r != 0);
	// JLINKARM_Reset();
}

extern U32 branches[TRACESIZE / 4];
extern U32 branch_num;
unsigned long file_idx = 0;
char log_file[128] = {0x0};
int AFL_GetBitmap(U8 *bitmap, U32 timeout, U8 *testcase, U32 size)
{
	int i = 0;
	U32 reg = 0x0;
	int r = 0;
	int exe_res = 0;
exec_retry:
#if DEBUG_MODE
	if(fp != NULL)
		fclose(fp);
	fp = NULL;
	memset(log_file, 0x0, 128);
	sprintf(log_file,"%lu.txt",file_idx++);
	fp = fopen(log_file, "w");
	fprintf(fp, "\ntestcase size:%u\n testcase:", size);
	int tmp = 0;
	unsigned char tmp_chr = 0;
	for (i = 0; i < size; i++)
	{
		tmp_chr = testcase[i];
		// tmp = (int)tmp_chr;
		fprintf(fp, "0x%x,", tmp_chr);
	}
	fprintf(fp, "\n");
#endif
	init_bitmap(bitmap);
	gettimeofday(&starttime, 0);
	exe_res = ExecOnBoard(timeout, testcase, size);
	if (exe_res != -1)
	{
		gettimeofday(&endtime, 0);
#ifdef TIME_EVA
		gettimeofday(&end, 0);
		fprintf(fp_time, "11:%d\t", (int)(1000 * (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000));
		gettimeofday(&start, 0);
#endif
#ifdef TIME_EVA
		gettimeofday(&end, 0);
		fprintf(fp_time, "11.5:%d\t", (int)(1000 * (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000));
		gettimeofday(&start, 0);
#endif
		r = GetBranchs(global_trace_buf);
		if (r == -1)
		{
			need_power_reset = 1;
			memset(bitmap, 0x0, MAP_SIZE);
			#if DEBUG_MODE
				fprintf(fp, "Discard and rerun\n");
			#endif
			goto exec_retry;
		}
#ifdef TIME_EVA
		gettimeofday(&end, 0);
		fprintf(fp_time, "12:%d\t", (int)(1000 * (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000));
		gettimeofday(&start, 0);
#endif
// for(i = 0; i < branch_num; i++){
// 	AFL_maybe_log(bitmap, branches[i]);
// }
#if DEBUG_MODE
		// fprintf(fp, "branch size:%d\t", branch_num);
		// fprintf(fp, "branches:");
		// for(i = 0; i < branch_num; i++){
		// 	fprintf(fp, "0x%x,",branches[i]);
		// }
		fprintf(fp, "\nbitmap:");
		for (i = 0; i < MAP_SIZE; i++)
		{
			if (bitmap[i] != 0)
			{
				fprintf(fp, "[%d:%d]\t", i, bitmap[i]);
			}
		}
		fprintf(fp, "\n\n");
		// exit(0);
#endif
#ifdef TIME_EVA
		gettimeofday(&end, 0);
		fprintf(fp_time, "13:%d\t", (int)(1000 * (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000));
		gettimeofday(&start, 0);
#endif
		prev_branch = 0;
		switch (exe_res)
		{
		case END_OF_TESTCASE:
// JLINKARM_Step();
#ifdef TIME_EVA
			gettimeofday(&end, 0);
			fprintf(fp_time, "14:%d\t", (int)(1000 * (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000));
			gettimeofday(&start, 0);
#endif
			JLINKARM_Go();
#ifdef TIME_EVA
			gettimeofday(&end, 0);
			fprintf(fp_time, "15:%d\n", (int)(1000 * (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000));
			gettimeofday(&start, 0);
#endif
#if DEBUG_MODE
			volatile int ms = (int)(1000 * (endtime.tv_sec - starttime.tv_sec) + (endtime.tv_usec - starttime.tv_usec) / 1000);
#endif
			return (int)(1000 * (endtime.tv_sec - starttime.tv_sec) + (endtime.tv_usec - starttime.tv_usec) / 1000);
			break;
		case CRASH:
			// handle crash
			need_power_reset = 1;
#if DEBUG_MODE
			JLINKARM_Halt();
			U32 reg = JLINKARM_ReadReg(PC_REG);
			fprintf(fp, "Current Crash PC: 0x%x\n", reg);
#endif
			return 0 - CRASH;
			break;
		case TIMEOUT:
			// handle timeout
			need_power_reset = 1;
			return 0 - TIMEOUT;
			break;
		default:
			AFL_ERROR_OUT("Unexpected exit reason, may check unknown bkpt.");
		}
	}
	return 0 - TIMEOUT;
}