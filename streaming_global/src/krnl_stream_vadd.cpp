#include "data.h"

#define CONST 2
typedef ap_axiu<32,0,0,0> pkt;

extern "C" {
void krnl_stream_vadd(hls::stream<pkt> &add_in, 
		hls::stream<pkt> &add_out, int size) 
{
#pragma HLS INTERFACE axis port = add_in
#pragma HLS INTERFACE axis port = add_out
#pragma HLS INTERFACE s_axilite port = size bundle = control
#pragma HLS INTERFACE s_axilite port = return bundle = control

vadd:
	pkt p;
	int num, seq_num, res;
	int cnt = 0;
	
	while(cnt != size) {
		p = add_in.read();
		seq_num = p.data;
		p = add_in.read();
		num = p.data;
		res = num + CONST;
		p.data = seq_num;
		add_out.write(p);
		p.data = res;
		add_out.write(p);
		++cnt;
	}
}
} /* end extern*/




















