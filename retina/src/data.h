#include <iostream>
//#include <vector>
//#include <array>
#include <unistd.h>
#include <cstring>
#include <string>
#include <algorithm>
#include "ap_int.h"
#include "ap_axi_sdata.h"
#include "hls_stream.h"

using namespace std;
//#define IS_SW_EMU
#define OP_ADD 0x11
#define OP_SUB 0x12
#define OP_MUL 0x13
#define OP_INV 0xFF
#define N_INPUT 2
#define N_PIPELINE_STAGE 3

typedef struct data
{
	int op_id[N_PIPELINE_STAGE];
	int num[N_INPUT];
	int padding[3];

} __attribute__((packed, aligned(4))) data;
