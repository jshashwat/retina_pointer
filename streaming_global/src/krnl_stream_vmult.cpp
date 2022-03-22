#include "data.h"

#define CONST 3
typedef ap_axiu<32,0,0,0> pkt;

extern "C" {
void krnl_stream_vmult(hls::stream<pkt> &mul_in, 
		hls::stream<pkt> &mul_out, int size) 
{
#pragma HLS INTERFACE axis port = mul_in
#pragma HLS INTERFACE axis port = mul_out
#pragma HLS INTERFACE s_axilite port = size bundle = control
#pragma HLS INTERFACE s_axilite port = return bundle = control

vmul:
	pkt p;
	int num, seq_num, res;
	int cnt = 0;
	
	while(cnt != size) {
		p = mul_in.read();
		seq_num = p.data;
		p = mul_in.read();
		num = p.data;
		res = num * CONST;
		p.data = seq_num;
		mul_out.write(p);
		p.data = res;
		mul_out.write(p);
		++cnt;
	}

}
}/*end extern*/
