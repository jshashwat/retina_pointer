#include "ap_axi_sdata.h"
#include "ap_int.h"
#include "hls_stream.h"

#define DWIDTH 32
#define CONST 1

typedef ap_axiu<DWIDTH, 0, 0, 0> pkt;

extern "C" {
void krnl_stream_vsub(hls::stream<pkt> &s_in, // Internal Stream input
					  hls::stream<pkt> &s_out,// Output Stream to mult
                       int size               // Size in integer
                       ) {
#pragma HLS INTERFACE axis port = s_in
#pragma HLS INTERFACE axis port = s_out
#pragma HLS INTERFACE s_axilite port = size bundle = control
#pragma HLS INTERFACE s_axilite port = return bundle = control
#pragma HLS stream variable=s_in depth=4 
#pragma HLS stream variable=s_out depth=4 

vsub:
  for (int i = 0; i < size; i++) {
#pragma HLS PIPELINE II = 1
	pkt v2, v3;
	int res;

    v2 = s_in.read();
    res = v2.data - CONST;
	v3.data = res;
	s_out.write(v3);
  }
}


}
