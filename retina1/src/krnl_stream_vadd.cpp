#include "data.h"

#define CONST 2
typedef ap_axiu<32,0,0,0> pkt;

extern "C" {
void krnl_stream_vadd(data *in, hls::stream<pkt> &add_in, 
		hls::stream<pkt> &add_out, int size) 
{
#pragma HLS INTERFACE m_axi port = in offset = slave bundle = gmem
#pragma HLS INTERFACE axis port = add_in
#pragma HLS INTERFACE axis port = add_out
#pragma HLS INTERFACE s_axilite port = in bundle = control
#pragma HLS INTERFACE s_axilite port = size bundle = control
#pragma HLS INTERFACE s_axilite port = return bundle = control
#pragma HLS stream variable=add_in depth=16 
#pragma HLS stream variable=add_out depth=16
#pragma HLS DATA_PACK variable=in struct_level

vadd:
	pkt p;
	int num, seq_num, res;
	int cnt = 0;
	
#ifdef IS_SW_EMU
	std::cout << "$$$$ krnl_add begin $$$$" << size << std::endl;
#endif
	while(cnt != size) { 
		p = add_in.read();
		seq_num = p.data;
		p = add_in.read();
		num = in->num[seq_num];
		res = num + CONST;
		p.data = seq_num;
#ifdef IS_SW_EMU
	std::cout << "\n\n$$$$ krnl_add before write $$$$\n" << size << std::endl;
#endif
		add_out.write(p);
		p.data = res;
		add_out.write(p);
		++cnt;
#ifdef IS_SW_EMU
		std::cout << "$$$$ krnl_add processed " << res << " items $$$$$" << std::endl;
#endif
	}
#ifdef IS_SW_EMU
	std::cout << "$$$$ krnl_add end $$$$" << std::endl;
#endif
}
} /* end extern*/




















