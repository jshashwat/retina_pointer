#include "data.h"

typedef ap_axiu<32,0,0,0> pkt;

extern "C" {

void krnl_stream_arb(data *in, int *out, hls::stream<pkt> &r_add_in,
					  hls::stream<pkt> &r_add_out/*, hls::stream<pkt> &r_add_in1,
					  hls::stream<pkt> &r_add_out1*/) 
{

#pragma HLS INTERFACE m_axi port = in offset = slave bundle = gmem
#pragma HLS INTERFACE m_axi port = out offset = slave bundle = gmem
#pragma HLS INTERFACE axis port = r_add_in
#pragma HLS INTERFACE axis port = r_add_out
// #pragma HLS INTERFACE axis port = r_add_in1
// #pragma HLS INTERFACE axis port = r_add_out1
// #pragma HLS INTERFACE axis port = r_sub_in
// #pragma HLS INTERFACE axis port = r_sub_out
// #pragma HLS INTERFACE axis port = r_mul_in
// #pragma HLS INTERFACE axis port = r_mul_out
#pragma HLS INTERFACE s_axilite port = in bundle = control
#pragma HLS INTERFACE s_axilite port = out bundle = control
#pragma HLS INTERFACE s_axilite port = return bundle = control
#pragma HLS stream variable=r_add_in depth=16
#pragma HLS stream variable=r_add_out depth=16
// #pragma HLS stream variable=r_add_in1 depth=16
// #pragma HLS stream variable=r_add_out1 depth=16
// #pragma HLS stream variable=r_sub_in depth=16
// #pragma HLS stream variable=r_sub_out depth=16
// #pragma HLS stream variable=r_mul_in depth=16
// #pragma HLS stream variable=r_mul_out depth=16
#pragma HLS DATA_PACK variable=in struct_level

varb:
	int stage_cnt = 0;
	int op, next_op, op_cnt;
	int res, seq_num;
	pkt add, mul, sub;
	pkt seq, d, end;
	
	//op = in->op_id[stage_cnt];
	//next_op = in->op_id[stage_cnt + 1];
	/* stream data to the first pipeline stage*/
#ifdef IS_SW_EMU
	cout << "STAGE 0: " << op << " begin" << endl;
#endif
	
	add.data = 0;
	r_add_in.write(add);
	add.data = in->num[0];
	r_add_in.write(add);

#ifdef IS_SW_EMU
	cout << "Instance 2 " << op << " begin" << endl;
#endif
	for(stage_cnt = 0; stage_cnt<1000; ++stage_cnt);

	// add.data = 1;
	// r_add_in1.write(add);
	// add.data = in->num[1];
	// r_add_in1.write(add);

#ifdef IS_SW_EMU
	cout << "Read from Instance 1 " << op << " begin" << endl;
#endif

	end = r_add_out.read();
	seq_num = end.data;
	end = r_add_out.read();
	res = end.data;
	out[seq_num] = res;

#ifdef IS_SW_EMU
	cout << "Result Read from Instance 1 " << res << " begin" << endl;
#endif
	

	// end = r_add_out1.read();
	// seq_num = end.data;
	// end = r_add_out1.read();
	// res = end.data;
	// out[seq_num] = res;

	return;

// 	for (int i = 0; i < N_INPUT; ++i) {
// 		switch(op) {
// 			case OP_ADD:
// 			/* write to add stream
// 			 * 1. write sequence num --> vec index
// 			 * 2. write input number*/
// 				add.data = i;
// 				r_add_in.write(add);
// 				add.data = in->num[i];
// 				r_add_in.write(add);
// 				break;
			
// 			case OP_SUB:
// 				sub.data = i;
// 				r_sub_in.write(sub);
// 				sub.data = in->num[i];
// 				r_sub_in.write(sub);
// 				break;

// 			case OP_MUL:
// 				mul.data = i;
// 				r_mul_in.write(mul);
// 				mul.data = in->num[i];
// 				r_mul_in.write(mul);

// 			default:
// 				/* never be here*/
// 	#ifdef IS_SW_EMU
// 				cout << "INVALID KERNEL" << endl;
// 	#endif
// 				break;
// 		}	
// 	}
// 	++stage_cnt;
// #ifdef IS_SW_EMU
// 	cout << "STAGE 0: " << op << " end" << endl;
// #endif

// 	/* stream data to next pipline stages*/
// next_stage:
// 	op_cnt = 0;
// #ifdef IS_SW_EMU
// 	cout << "STAGE " << stage_cnt << ": " << op << " begin" << endl;
// #endif
// 	while (op_cnt != N_INPUT) {
// 		switch(op) {
// 			case OP_ADD:
// 				/* 1. retrieve data from add stream
// 				 * 2. prepare and enque data to next stage*/
// #ifdef IS_SW_EMU
// 				cout << " reading from add_out" << endl;
// #endif
// 				add = r_add_out.read();
// 				seq_num = add.data;
// 				add = r_add_out.read();
// 				res = add.data;
// 				seq.data = seq_num;
// 				d.data = res;
// #ifdef IS_SW_EMU
// 				cout << " data read from add_out, streaming to sub_in" << endl;
// #endif
// 				if (next_op == OP_SUB) {
// 					r_sub_in.write(seq);
// 					r_sub_in.write(d);
// 					++op_cnt;
// 					break;
// 				}
// 				if (next_op == OP_MUL) {
// 					r_mul_in.write(seq);
// 					r_mul_in.write(d);
// 				}
// 				++op_cnt;
// 				break;

// 			case OP_SUB:
// 				sub = r_sub_out.read();
// 				seq_num = sub.data;
// 				sub = r_sub_out.read();
// 				res = sub.data;
// 				seq.data = seq_num;
// 				d.data = res;
// 				if (next_op == OP_ADD) {
// 					r_add_in.write(seq);
// 					r_add_in.write(d);
// 					++op_cnt;
// 					break;
// 				}
// 				if (next_op == OP_MUL) {
// 					r_mul_in.write(seq);
// 					r_mul_in.write(d);
// 				}
// 				++op_cnt;
// 				break;

// 			case OP_MUL:
// 				mul = r_mul_out.read();
// 				seq_num = mul.data;
// 				mul = r_mul_out.read();
// 				res = mul.data;
// 				seq.data = seq_num;
// 				d.data = res;
// 				if (next_op == OP_ADD) {
// 					r_add_in.write(seq);
// 					r_add_in.write(d);
// 					++op_cnt;
// 					break;
// 				}
// 				if (next_op == OP_SUB) {
// 					r_sub_in.write(seq);
// 					r_sub_in.write(d);
// 				}
// 				++op_cnt;
// 				break;
// 			default:
// 				/* never be here*/
// 	#ifdef IS_SW_EMU
// 				cout << "INVALID KERNEL" << endl;
// 	#endif
// 				/* insert an assert here*/
// 				break;
// 		}
// 	}
// #ifdef IS_SW_EMU
// 	cout << "STAGE " << stage_cnt << ": " << op << "end" << endl;
// #endif
// 	++stage_cnt;

// /* written with assumption that are atleast 2 active pipeline stages
//  * the code needs to be refactored*/
// 	if (stage_cnt == N_PIPELINE_STAGE) {
// 		/* prepare the output*/
// 		op_cnt = 0;
// 		while (op_cnt != N_INPUT) {
// 			switch(next_op) {
// 				case OP_ADD:
// 					end = r_add_out.read();
// 					seq_num = end.data;
// 					end = r_add_out.read();
// 					res = end.data;
// 					out[seq_num] = res;
// 					++op_cnt;
// 					break;

// 				case OP_SUB:
// 					end = r_sub_out.read();
// 					seq_num = end.data;
// 					end = r_sub_out.read();
// 					res = end.data;
// 					out[seq_num] = res;
// 					++op_cnt;
// 					break;

// 				case OP_MUL:
// 					end = r_mul_out.read();
// 					seq_num = end.data;
// 					end = r_mul_out.read();
// 					res = end.data;
// 					out[seq_num] = res;
// 					++op_cnt;
// 					break;
// 				default:
// 					// never be here
// 		#ifdef IS_SW_EMU
// 					cout << "INVALID KERNEL" << endl;
// 		#endif
// 					// insert an assert here
// 					break;
// 			} // end switch
// 		}//end while
// 		return;
// 	} //end if
// 	/* continue with next pipeline stages*/
// 	op = next_op;
// 	next_op = in->op_id[stage_cnt];
// 	goto next_stage;

}
} /* end extern*/























