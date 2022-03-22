#include <iostream>
#include <vector>
#include <array>
#include <unistd.h>
#include <cstring>
#include <string>
#include <algorithm>
#include "ap_int.h"
#include "ap_axi_sdata.h"
#include "hls_stream.h"

using namespace std;
#define OP_ADD 0x11
#define OP_SUB 0x12
#define OP_MUL 0x13
#define OP_INV 0xFF
#define N_INPUT 2
#define N_PIPELINE_STAGE 3

typedef struct data
{
	array<int, 3> op_id;
	array<int, 2> num;

}data;
