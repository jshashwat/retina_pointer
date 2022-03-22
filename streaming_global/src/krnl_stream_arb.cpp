#include "data.h"

typedef ap_axiu<32,0,0,0> pkt;

extern "C" {

void krnl_stream_arb(data *in, int *out, hls::stream<pkt> &r_add_in,
					  hls::stream<pkt> &r_add_out, hls::stream<pkt> &r_sub_in,
					  hls::stream<pkt> &r_sub_out, hls::stream<pkt> &r_mul_in,
					  hls::stream<pkt> &r_mul_out ) 
{

#pragma HLS INTERFACE m_axi port = in offset = slave bundle = gmem
#pragma HLS INTERFACE m_axi port = out offset = slave bundle = gmem
#pragma HLS INTERFACE axis port = r_add_in
#pragma HLS INTERFACE axis port = r_add_out
#pragma HLS INTERFACE axis port = r_sub_in
#pragma HLS INTERFACE axis port = r_sub_out
#pragma HLS INTERFACE axis port = r_mul_in
#pragma HLS INTERFACE axis port = r_mul_out
#pragma HLS INTERFACE s_axilite port = in bundle = control
#pragma HLS INTERFACE s_axilite port = out bundle = control
#pragma HLS INTERFACE s_axilite port = return bundle = control

varb:
	int stage_cnt = 0;
	int op, next_op, op_cnt;
	int res, seq_num;
	pkt add, mul, sub;
	pkt seq, d, end;
	
	op = in->op_id[stage_cnt];
	next_op = in->op_id[stage_cnt + 1];
	/* stream data to the first pipeline stage*/
	cout << "STAGE 1: " << op << "begin" << endl;
	for (int i = 0; i < in->num.size(); ++i) {
		switch(op) {
			case OP_ADD:
			/* write to add stream
			 * 1. write sequence num --> vec index
			 * 2. write input number*/
				add.data = i;
				r_add_in.write(add);
				add.data = in->num[i];
				r_add_in.write(add);
				break;
			
			case OP_SUB:
				sub.data = i;
				r_sub_in.write(sub);
				sub.data = in->num[i];
				r_sub_in.write(sub);
				break;

			case OP_MUL:
				mul.data = i;
				r_mul_in.write(mul);
				mul.data = in->num[i];
				r_mul_in.write(mul);

			default:
				/* never be here*/
				cout << "INVALID KERNEL" << endl;
				break;
		}	
	}
	++stage_cnt;
	cout << "STAGE 1: " << op << "end" << endl;

	/* stream data to next pipline stages*/
next_stage:
	op_cnt = 0;
	cout << "STAGE " << stage_cnt << ": " << op << "begin" << endl;
	while (op_cnt != in->num.size()) {
		switch(op) {
			case OP_ADD:
				/* 1. retrieve data from add stream
				 * 2. prepare and enque data to next stage*/
				add = r_add_out.read();
				seq_num = add.data;
				add = r_add_out.read();
				res = add.data;
				seq.data = seq_num;
				d.data = res;
				if (next_op == OP_SUB) {
					r_sub_in.write(seq);
					r_sub_in.write(d);
					++op_cnt;
					break;
				}
				if (next_op == OP_MUL) {
					r_mul_in.write(seq);
					r_mul_in.write(d);
				}
				++op_cnt;
				break;

			case OP_SUB:
				sub = r_sub_out.read();
				seq_num = sub.data;
				sub = r_sub_out.read();
				res = sub.data;
				seq.data = seq_num;
				d.data = res;
				if (next_op == OP_ADD) {
					r_add_in.write(seq);
					r_add_in.write(d);
					++op_cnt;
					break;
				}
				if (next_op == OP_MUL) {
					r_mul_in.write(seq);
					r_mul_in.write(d);
				}
				++op_cnt;
				break;

			case OP_MUL:
				mul = r_mul_out.read();
				seq_num = mul.data;
				mul = r_mul_out.read();
				res = mul.data;
				seq.data = seq_num;
				d.data = res;
				if (next_op == OP_ADD) {
					r_add_in.write(seq);
					r_add_in.write(d);
					++op_cnt;
					break;
				}
				if (next_op == OP_SUB) {
					r_sub_in.write(seq);
					r_sub_in.write(d);
				}
				++op_cnt;
				break;
			default:
				/* never be here*/
				cout << "INVALID KERNEL" << endl;
		}
	}
	cout << "STAGE " << stage_cnt << ": " << op << "end" << endl;
	++stage_cnt;
	
	if (stage_cnt == in->op_id.size()) {
		/* prepare the output*/
		op_cnt = 0;
		while (op_cnt != in->num.size()) {
			switch(op) {
				case OP_ADD:
					end = r_add_out.read();
					seq_num = end.data;
					end = r_add_out.read();
					res = end.data;
					out[seq_num] = res;
					++op_cnt;
					break;

				case OP_SUB:
					end = r_sub_out.read();
					seq_num = end.data;
					end = r_sub_out.read();
					res = end.data;
					out[seq_num] = res;
					++op_cnt;
					break;

				case OP_MUL:
					end = r_mul_out.read();
					seq_num = end.data;
					end = r_mul_out.read();
					res = end.data;
					out[seq_num] = res;
					++op_cnt;
					break;
			}
		}
		return;
	}
	/* continue with next pipeline stages*/
	op = next_op;
	next_op = in->op_id[stage_cnt];
	goto next_stage;

}
} /* end extern*/























