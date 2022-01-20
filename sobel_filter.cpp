#include "sobel_filter.h"
#include <cmath>
#include <stdio.h>
#include <stdlib.h>

void sobel_filter (stream_t& inStream, stream_t& outStream){
#pragma HLS INTERFACE axis register both port=inStream
#pragma HLS INTERFACE axis register both port=outStream
#pragma HLS INTERFACE ap_ctrl_none port = return

	data_t input[FRAME_H][FRAME_W];

		for (int i=0; i < FRAME_H; i++){
			for (int j=0; j<FRAME_W; j++){
#pragma HLS LOOP_FLATTEN
				input[i][j]=inStream.read();
			}
		}
		//Determine the edges
		calc:
		ap_int<8> xfilter [3][3] = {{1,2,1},{0,0,0},{-1,-2,-1}};
		ap_int<8> yfilter [3][3] = {{1,0,-1},{2,0,-2},{1,0,-1}};
		ap_int<8> ximage[FRAME_H][FRAME_W];
		ap_int<8> yimage[FRAME_H][FRAME_W];
		ap_int<16> oimage[FRAME_H][FRAME_W];
		for (int i=0; i<FRAME_H; i++){
			for (int j=0; j<FRAME_W; j++){
						oimage[i][j]=0;
						ximage[i][j]=0;
						ximage[i][j]=0;
					}
		}
		for (int i=0; i<FRAME_H-2; i++){
			for (int j=0; j<FRAME_W-2; j++){
#pragma HLS LOOP_FLATTEN
				//Horizontal Edge Detection
				ximage[i+1][j+1]=input[i][j].data*xfilter[0][0]+input[i][j+1].data*xfilter[0][1]+input[i][j+2].data*xfilter[0][2];
				ximage[i+1][j+1]+=input[i+1][j].data*xfilter[1][0]+input[i+1][j+1].data*xfilter[1][1]+input[i+1][j+2].data*xfilter[1][2];
				ximage[i+1][j+1]+=input[i+2][j].data*xfilter[2][0]+input[i+2][j+1].data*xfilter[2][1]+input[i+2][j+2].data*xfilter[2][2];

				//Vertical Edge Detection
				yimage[i+1][j+1]=input[i][j].data*yfilter[0][0]+input[i][j+1].data*yfilter[0][1]+input[i][j+2].data*yfilter[0][2];
				yimage[i+1][j+1]+=input[i+1][j].data*yfilter[1][0]+input[i+1][j+1].data*yfilter[1][1]+input[i+1][j+2].data*yfilter[1][2];
				yimage[i+1][j+1]+input[i+2][j].data*yfilter[2][0]+input[i+2][j+1].data*yfilter[2][1]+input[i+2][j+2].data*yfilter[2][2];
				//Merging the two images
				oimage[i+1][j+1]=(abs(ximage[i+1][j+1])+abs(yimage[i+1][j+1]));
					if (oimage[i+1][j+1]>255)
						oimage[i+1][j+1] = 255;
			}
		}
		data_t output[FRAME_H][FRAME_W];
		//Write the output stream

		for (int i = 0; i<FRAME_H; i++){
			write :	for (int j = 0; j<FRAME_W; j++){
				output[i][j].strb=-1;
				output[i][j].keep=-1;
				output[i][j].user=0;
				output[i][j].id=0;
				output[i][j].dest=0;
				output[i][j].data=oimage[i][j];
				output[i][j].last= (i==FRAME_H-1 && j==FRAME_W-1)? 1:0;
				outStream.write(output[i][j]);
			}
		}
}
