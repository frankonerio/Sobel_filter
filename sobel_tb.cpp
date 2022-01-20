#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <hls_opencv.h>

#include "sobel_filter.h"

#define N_SIZE_MASK		3

using namespace std;

int main()
{
	hls::stream<data_t> stream_in;
	hls::stream<data_t> stream_out;

	cv::Mat img_src = cv::imread("/home/user/task_2/images/rects.png", CV_LOAD_IMAGE_GRAYSCALE);
	cv::Mat img_golden_x = cv::Mat(img_src.rows, img_src.cols, CV_16S);
	cv::Mat img_golden_y = cv::Mat(img_src.rows, img_src.cols, CV_16S);
	cv::Mat img_golden = cv::Mat(img_src.rows, img_src.cols, CV_8U);
	cv::Mat img_fpga = cv::Mat(img_src.rows, img_src.cols, CV_8U);

	cv::Sobel(img_src, img_golden_x, CV_16S, 1, 0, 3);
	cv::Sobel(img_src, img_golden_y, CV_16S, 0, 1, 3);

	for(int y=0; y < img_src.rows; y++)
	{
		for(int x=0; x < img_src.cols; x++)
		{
			data_t tmp;
			tmp.strb = 0;
			tmp.keep = 15;
			tmp.user = 0;
			tmp.id = 0;
			tmp.dest = 0;
			tmp.data = ap_uint<8>( img_src.at<uint8_t>(y, x) );
			tmp.last = ( (y==img_src.rows-N_SIZE_MASK/2) && (x==img_src.cols-N_SIZE_MASK/2) ) ? 1 : 0;
			stream_in.write( tmp );

			int pixel = abs(int(img_golden_x.at<int16_t>(y,x))) + abs(int(img_golden_y.at<int16_t>(y,x)));
			if( (x==0) || (y==0) || (x==img_src.cols-N_SIZE_MASK/2) || (y==img_src.rows-N_SIZE_MASK/2) )
				pixel = 0;
			else if( pixel > 255 )
				pixel = 255;
			img_golden.at<uchar>(y,x) = pixel;
		}
	}

	// Add your filter core here
	sobel_filter( stream_in, stream_out );


	int error_cnt = 0;
	for(int y=0; y < img_src.rows; y++)
	{
		for(int x=0; x < img_src.cols; x++)
		{
			ap_uint<8> data = stream_out.read().data;
			img_fpga.at<uchar>(y, x) = data;
			int diff = int(img_golden.at<uchar>(y, x)) - data;
			if( diff != 0 )
			{
				if( !((x==0) || (y==0) || (x==img_src.cols-N_SIZE_MASK/2) || (y==img_src.rows-N_SIZE_MASK/2)) )
				{
					error_cnt++;
					//cout << x << "," << y << ": " << int(img_golden.at<uchar>(y, x)) << " " << data << " " << abs(img_golden_x.at<int16_t>(y,x)) << " " << abs(img_golden_y.at<int16_t>(y,x)) << endl;
					//cout << x << "," << y << ": " << diff << endl;
				}
			}
		}
	}

	cv::imwrite("/home/user/task_2/images/output/golden_x.png", img_golden_x);
	cv::imwrite("/home/user/task_2/images/output/golden_y.png", img_golden_y);
	cv::imwrite("/home/user/task_2/images/output/golden.png", img_golden);
	cv::imwrite("/home/user/task_2/images/output/fpga.png", img_fpga);

	cout << "Error count: " << error_cnt << endl;

	return 0;
}
