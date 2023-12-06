#include "systolic_2.h"

const unsigned int max_size = MAX_SIZE;
const unsigned int B_ROW = A_COL;
const unsigned int C_ROW = A_ROW;
const unsigned int C_COL = B_COL;


void sys_matmult (hls::stream<axis_data> &in_A, hls::stream<axis_data> &out_C) {

	#pragma HLS INTERFACE ap_ctrl_none port = return
	#pragma HLS INTERFACE axis register both port = in_A
    #pragma HLS INTERFACE axis register both port = out_C


	// Matrices to store inputs and outputs

	dtype input_A[A_ROW][A_COL];
	#pragma HLS ARRAY_PARTITION dim=1 type=complete variable=input_A	//solution_2 solution_3

	dtype input_B[B_ROW][B_COL];
	#pragma HLS ARRAY_PARTITION dim=2 type=complete variable=input_B	//solution_2 solution_3

	dtype output_C[C_ROW][C_COL];
	#pragma HLS ARRAY_PARTITION dim=0 type=complete variable=output_C	//solution_2 solution_3


	 int row, col, index;
	 axis_data local_stream;

	 //read data for matrix A

	     loop_A1: for (row =0; row < A_ROW; row++){
	         loop_A2: for (col =0; col< A_COL; col++){
//		#pragma HLS PIPELINE //solution_2
	    #pragma HLS LOOP_TRIPCOUNT min = C_ROW * B_ROW max = max_size * max_size //solution_3

	             local_stream = in_A.read();
	             input_A[row][col] = local_stream.data.data;
	         }
	     }

	 //read data for matrix B

		loop_B1: for (row =0; row < B_ROW; row++){
			loop_B2: for (col =0; col< B_COL; col++){
//		#pragma HLS PIPELINE	//solution_2
	    #pragma HLS LOOP_TRIPCOUNT min = B_ROW * C_COL max = max_size * max_size	//solution_3

				local_stream = in_A.read();
				input_B[row][col] = local_stream.data.data;
			}
		}

	//Systolic Matrix Multiplication

		systolic_1: for (index =0; index < A_COL; index++){
	   #pragma HLS LOOP_TRIPCOUNT min = B_ROW max = max_size	//solution_3
			systolic_2: for (row =0; row < A_ROW; row++){
//		#pragma HLS PIPELINE	//solution_2
	   #pragma HLS UNROLL	//solution_3
				systolic_3: for (col =0; col < B_COL; col++){
	   #pragma HLS UNROLL	//solution_3
								dtype last = (index == 0)? 0 : output_C[row][col];
								dtype aval = (row < A_ROW && index < A_COL) ? input_A[row][index] : 0;
								dtype bval = (index < B_ROW && col < B_COL) ? input_B[index][col] : 0;
								dtype res=  mac(last, aval, bval);
								output_C[row][col] = res;
				}
			}
		}

	// Stream output from the local output matrix

		loop_C1: for(row = 0; row < C_ROW; row ++){
			loop_C2: for (col =0; col < C_COL; col++){
//		#pragma HLS PIPELINE	//solution_2
	    #pragma HLS LOOP_TRIPCOUNT min =C_ROW * C_COL max = max_size * max_size	//solution_3
				local_stream.data.data = output_C[row][col];

				// Last signal for proper framework
				if ((row == C_ROW - 1) && (col == C_COL - 1))
					local_stream.last = 1;
				else
					local_stream.last = 0;
				out_C.write(local_stream);
			}
		}

}
