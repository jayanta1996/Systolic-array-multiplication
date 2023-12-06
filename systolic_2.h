#include <stdio.h>
#include <hls_stream.h>
#include "ap_axi_sdata.h"
#include <ap_int.h>

// Define the size of the input matrices

#define A_ROW 4
#define A_COL 4
#define B_COL 4

#define MAX_SIZE 16         // maximum array size for the trip count

//  Define the data type of the matrix inputs and outputs

typedef double  dtype ;

//  Define the axis data structure

struct t_dat {
    dtype data;
    };

typedef hls::axis<t_dat,0,0,0> axis_data ;

dtype mac(dtype last, dtype aval, dtype bval)
 {
     dtype result = last + aval * bval ;
     return result;
 }

void sys_matmult(hls::stream<axis_data> &in_A, hls::stream<axis_data> &out_C);


