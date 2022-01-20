#include <ap_int.h>
#include <hls_stream.h>
#include <ap_axi_sdata.h>

#define DATA_WIDTH 8

#define FRAME_W 120//Change the width according to the png file selected
#define FRAME_H 90 //Change the height according to the png file selected

typedef ap_axiu<DATA_WIDTH,1,1,1> data_t; // Defines the signals of AXIS
typedef hls::stream<data_t> stream_t;

void sobel_filter(stream_t& inStream, stream_t& outStream); // Top-level function
