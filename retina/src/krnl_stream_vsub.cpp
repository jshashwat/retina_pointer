#include "data.h"

#define CONST 1
typedef ap_axiu<32,0,0,0> pkt;

extern "C" {
void krnl_stream_vsub(hls::stream<pkt> &sub_in, 
		hls::stream<pkt> &sub_out, int size) 
{
#pragma HLS INTERFACE axis port = sub_in
#pragma HLS INTERFACE axis port = sub_out
#pragma HLS INTERFACE s_axilite port = size bundle = control
#pragma HLS INTERFACE s_axilite port = return bundle = control
#pragma HLS stream variable=sub_in depth=16
#pragma HLS stream variable=sub_out depth=16

vsub:
	pkt p;
	int num, seq_num, res;
	int cnt = 0;

#ifdef IS_SW_EMU
	std::cout << "**** krnl_sub begin ****" << std::endl;
#endif
	while(cnt != size) {
		p = sub_in.read();
		seq_num = p.data;
		p = sub_in.read();
		num = p.data;
		res = num - CONST;
		p.data = seq_num;
		sub_out.write(p);
		p.data = res;
		sub_out.write(p);
		++cnt;
#ifdef IS_SW_EMU
		std::cout << "**** krnl_sub processed " << cnt << " items ****" << std::endl;
#endif
	}
#ifdef IS_SW_EMU
	std::cout << "**** krnl_sub end ****" << std::endl;
#endif
}
} /* end extern*/
