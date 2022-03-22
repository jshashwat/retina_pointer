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
#pragma HLS stream variable=mul_in depth=16
#pragma HLS stream variable=mul_out depth=16

vmul:
	pkt p;
	int num, seq_num, res;
	int cnt = 0;
	
#ifdef IS_SW_EMU
	std::cout << "#### krnl_mult begin ####" << std::endl;
#endif
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
#ifdef IS_SW_EMU
		std::cout << "#### krnl_mul processed " << cnt << " items ####" << std::endl;
#endif
	}
#ifdef IS_SW_EMU
	std::cout << "#### krnl_mul end ####" << std::endl;
#endif

}
}/*end extern*/
