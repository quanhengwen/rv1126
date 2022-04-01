#include "display.h"
#include <iostream>

extern float tempData;
using namespace std;
time_t timer0, timer1;
uint8_t is_displaying = 0;
uint8_t* image_tmp_frame1;
uint8_t* image_tmp_frame2;

//PseudocolorYuv_t psdcolor_yuv_mapping_table[] = yuv_8bit_pseudocolor_table;
//PseudocolorRgb_t psdcolor_rgb_mapping_table[] = rgb_8bit_pseudocolor_table;

//init the display parameters
void display_init(StreamFrameInfo_t* stream_frame_info)
{
	//is_displaying = 1;
	time_t timer0 = clock();
	time_t timer1 = timer0;

	int pixel_size = stream_frame_info->image_info.width * stream_frame_info->image_info.height;
	if (image_tmp_frame1 == NULL)
	{
		image_tmp_frame1 = (uint8_t*)malloc(pixel_size * 3);
	}
	if (image_tmp_frame2 == NULL)
	{
		image_tmp_frame2 = (uint8_t*)malloc(pixel_size * 3);
	}
}

//recyle the display parameters
void display_release(void)
{
	//is_displaying = 0;

	if (image_tmp_frame1 != NULL)
	{
		free(image_tmp_frame1);
		image_tmp_frame1 = NULL;
	}

	if (image_tmp_frame2 != NULL)
	{
		free(image_tmp_frame2);
		image_tmp_frame2 = NULL;
	}
}

//enhance the image frame by the frameinfo
int enhance_image_frame(uint16_t* src_frame, FrameInfo_t* frameinfo, uint16_t* dst_frame)
{
	int ret, lower_limit, upper_limit = 0;
	int temp_enhace_en = 1;
	float lower_temp = 10;
	float upper_temp = 35.5;

	lower_limit = (lower_temp + 273.15) * 16;
	upper_limit = (upper_temp + 273.15) * 16;

	ImgEnhanceParam_t img_enhance_param = { { 0.01,0.01,0.001,0.0001,150,0,0.25,\
					  {temp_enhace_en,lower_limit,upper_limit} }, { 0,6,2,0,10,60,120 } };
	ImageRes_t image_res = { frameinfo->width,frameinfo->height };
	int pix_num = frameinfo->width * frameinfo->height;

	if (frameinfo->img_enhance_status == IMG_ENHANCE_ON)
	{
		y14_image_enhance((uint16_t*)src_frame, image_res, img_enhance_param, (uint16_t*)dst_frame);
	}
	else
	{
		memcpy(dst_frame, src_frame, pix_num * 2);
	}
	return 0;
}

//color the image frame
void color_image_frame(uint8_t* src_frame, FrameInfo_t* frameinfo, uint8_t* dst_frame)
{
	int pix_num = frameinfo->width * frameinfo->height;
	uint8_t* tmp_frame = (uint8_t*)malloc(pix_num * 3);
	free(tmp_frame);
}

//convert the image process  image_tmp_frame2 is the default output frame
void display_image_process(uint8_t* image_frame, int pix_num, FrameInfo_t* frameinfo)
{
	ImageRes_t image_res = { frameinfo->width,frameinfo->height };
	if (frameinfo->input_format == INPUT_FMT_Y14 || frameinfo->input_format == INPUT_FMT_Y16)
	{
		if (frameinfo->input_format == INPUT_FMT_Y16)
		{
			y16_to_y14((uint16_t*)image_frame, pix_num, (uint16_t*)image_frame);
		}

		enhance_image_frame((uint16_t*)image_frame, frameinfo, (uint16_t*)image_tmp_frame1);

		switch (frameinfo->output_format)
		{
			case OUTPUT_FMT_Y14:
			{
				frameinfo->byte_size = pix_num * 2;
				memcpy(image_tmp_frame2, image_tmp_frame1, frameinfo->byte_size);
				break;
			}
			case OUTPUT_FMT_YUV444:
			{
				frameinfo->byte_size = pix_num * 3;
				y14_to_yuv444((uint16_t *)image_tmp_frame1, pix_num, (uint8_t*)image_tmp_frame2);
				break;
			}
			case OUTPUT_FMT_YUV422:
			{
				frameinfo->byte_size = pix_num * 2;
				if (frameinfo->pseudo_color_status == PSEUDO_COLOR_ON)
				{
					// use IRPROC_COLOR_MODE_3 mode
					y14_map_to_yuyv_pseudocolor((uint16_t*)image_tmp_frame1, pix_num, \
												IRPROC_COLOR_MODE_3, (uint8_t*)image_tmp_frame2);
				}
				else
				{
					y14_to_yuv444((uint16_t*)image_tmp_frame1, pix_num, (uint8_t*)image_tmp_frame2);
					memcpy(image_tmp_frame1, image_tmp_frame2, frameinfo->byte_size);
					yuv444_to_yuv422((uint8_t*)image_tmp_frame1, pix_num, (uint8_t*)image_tmp_frame2);
				}
				break;
			}
			case OUTPUT_FMT_RGB888:
			{
				frameinfo->byte_size = pix_num * 3;
				if (frameinfo->pseudo_color_status == PSEUDO_COLOR_ON)
				{
					y14_map_to_yuyv_pseudocolor((uint16_t*)image_tmp_frame1, pix_num, \
												IRPROC_COLOR_MODE_3, image_tmp_frame2);
					memcpy(image_tmp_frame1, image_tmp_frame2, frameinfo->byte_size);
					yuv422_to_rgb(image_tmp_frame1, pix_num, image_tmp_frame2);
				}
				else
				{
					y14_to_rgb((uint16_t*)image_tmp_frame1, pix_num, image_tmp_frame2);
				}
				break;
			}
			case OUTPUT_FMT_BGR888:
			default:
			{
				frameinfo->byte_size = pix_num * 3;
				if (frameinfo->pseudo_color_status == PSEUDO_COLOR_ON)
				{
					y14_map_to_yuyv_pseudocolor((uint16_t*)image_tmp_frame1, pix_num, \
												IRPROC_COLOR_MODE_4, image_tmp_frame2);
					yuv422_to_rgb(image_tmp_frame2, pix_num, image_tmp_frame1);
					rgb_to_bgr(image_tmp_frame1, pix_num, image_tmp_frame2);
				}
				else
				{
					y14_to_rgb((uint16_t*)image_tmp_frame1, pix_num, image_tmp_frame2);
					rgb_to_bgr(image_tmp_frame2, pix_num, image_tmp_frame2);
				}
				break;
			}
		}
	}
	else if (frameinfo->input_format == INPUT_FMT_YUV422)
	{
		switch (frameinfo->output_format)
		{
			case OUTPUT_FMT_Y14:
			{
				frameinfo->byte_size = 0;
				printf("convert error!\n");
				break;
			}
			case OUTPUT_FMT_YUV444:
			{
				frameinfo->byte_size = 0;
				printf("convert error!\n");
				break;
			}
			case OUTPUT_FMT_YUV422:
			{
				frameinfo->byte_size = pix_num * 2;
				memcpy(image_tmp_frame2, image_frame, pix_num * 2);
				break;
			}
			case OUTPUT_FMT_RGB888:
			{
				frameinfo->byte_size = pix_num * 3;
				yuv422_to_rgb((uint8_t*)image_frame, pix_num, image_tmp_frame2);
				break;
			}
			case OUTPUT_FMT_BGR888:
			default:
			{
				frameinfo->byte_size = pix_num * 3;
				yuv422_to_rgb((uint8_t*)image_frame, pix_num, image_tmp_frame1);
				rgb_to_bgr(image_tmp_frame1, pix_num, image_tmp_frame2);
				break;
			}
		}
	}
}

irproc_src_fmt_t format_converter(OutputFormat_t output_format)
{
	switch (output_format)
	{
	case OUTPUT_FMT_Y14:
		return IRPROC_SRC_FMT_Y14;
		break;
	case OUTPUT_FMT_YUV422:
		return IRPROC_SRC_FMT_YUV422;
		break;
	case OUTPUT_FMT_YUV444:
		return IRPROC_SRC_FMT_YUV444;
		break;
	case OUTPUT_FMT_RGB888:
		return IRPROC_SRC_FMT_RGB888;
		break;
	case OUTPUT_FMT_BGR888:
		return IRPROC_SRC_FMT_BGR888;
		break;
	default:
		return IRPROC_SRC_FMT_Y14;
		break;
	}
}

//rotate the frame data according to rotate_side
void rotate_demo(FrameInfo_t* frame_info, uint8_t* frame, RotateSide_t rotate_side)
{
	ImageRes_t image_res = { frame_info->width,frame_info->height };
	irproc_src_fmt_t tmp_fmt = format_converter(frame_info->output_format);

	switch (rotate_side)
	{
	case NO_ROTATE:
		break;
	case LEFT_90D:
		rotate_left_90(frame, image_res, tmp_fmt, image_tmp_frame1);
		memcpy(frame, image_tmp_frame1, frame_info->byte_size);
		break;
	case RIGHT_90D:
		rotate_right_90(frame, image_res, tmp_fmt, image_tmp_frame1);
		memcpy(frame, image_tmp_frame1, frame_info->byte_size);
		break;
	case ROTATE_180D:
		rotate_180(frame, image_res, tmp_fmt, image_tmp_frame1);
		memcpy(frame, image_tmp_frame1, frame_info->byte_size);
		break;
	default:
		break;
	}
}

//mirror/flip the frame data according to mirror_flip_status
void mirror_flip_demo(FrameInfo_t* frame_info, uint8_t* frame, MirrorFlipStatus_t mirror_flip_status)
{
	ImageRes_t image_res = { frame_info->width,frame_info->height };
	irproc_src_fmt_t tmp_fmt = format_converter(frame_info->output_format);

	switch (mirror_flip_status)
	{
	case STATUS_NO_MIRROR_FLIP:
		break;
	case STATUS_ONLY_MIRROR:
		mirror(frame, image_res, tmp_fmt, image_tmp_frame1);
		memcpy(frame, image_tmp_frame1, frame_info->byte_size);
		break;
	case STATUS_ONLY_FLIP:
		flip(frame, image_res, tmp_fmt, image_tmp_frame1);
		memcpy(frame, image_tmp_frame1, frame_info->byte_size);
		break;
	case STATUS_MIRROR_FLIP:
		mirror(frame, image_res, tmp_fmt, image_tmp_frame1);
		flip(image_tmp_frame1, image_res, tmp_fmt, frame);
		break;
	default:
		break;
	}
}

//display the frame by opencv 
void display_one_frame(StreamFrameInfo_t* stream_frame_info)
{
	if (stream_frame_info == NULL)
	{
		return;
	}

	char key_press = 0;
	int rst = 0;
	timer0 = timer1;
	timer1 = clock();
	float frame = CLOCKS_PER_SEC / (double)(timer1 - timer0);
	char frameText[10] = { " " };
	sprintf(frameText, "%.2f", frame);

	char tempText[30] = { " " };
	sprintf(tempText, "P(128,96)temp: %.2f", tempData);

	int pix_num = stream_frame_info->image_info.width * stream_frame_info->image_info.height;
	int width = stream_frame_info->image_info.width;
	int height = stream_frame_info->image_info.height;
#ifndef OPENCV_ENABLE
	printf("raw data=%d\n", ((uint16_t*)stream_frame_info->image_frame)[1000]);
#endif

	display_image_process(stream_frame_info->image_frame, pix_num, &stream_frame_info->image_info);
	if ((stream_frame_info->image_info.rotate_side == LEFT_90D)|| \
		(stream_frame_info->image_info.rotate_side == RIGHT_90D))
	{
		width = stream_frame_info->image_info.height;
		height = stream_frame_info->image_info.width;
	}

	mirror_flip_demo(&stream_frame_info->image_info, image_tmp_frame2, \
					stream_frame_info->image_info.mirror_flip_status);
	rotate_demo(&stream_frame_info->image_info, image_tmp_frame2, \
				stream_frame_info->image_info.rotate_side);

#ifdef OPENCV_ENABLE
	cv::Mat image = cv::Mat(height, width, CV_8UC3, image_tmp_frame2);
	cv::circle(image, cv::Point(128,96), 4, cv::Scalar(0,0,255), 2);
	cv::resize(image, image, cv::Size(), 1.5, 1.5);
	putText(image, frameText, cv::Point(11, 11), cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar::all(0), 1, 8);
	putText(image, frameText, cv::Point(10, 10), cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar::all(255), 1, 8);
	putText(image, tempText, cv::Point(11, 26), cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar::all(0), 1, 8);
	putText(image, tempText, cv::Point(10, 25), cv::FONT_HERSHEY_PLAIN, 1, cv::Scalar::all(255), 1, 8);
	cv::imshow("Test", image);
	cvWaitKey(5);
#endif
}

//display thread function
void* display_function(void* threadarg)
{
	StreamFrameInfo_t* stream_frame_info;
	stream_frame_info = (StreamFrameInfo_t*)threadarg;
	if (stream_frame_info == NULL)
	{
		return NULL;
	}

	display_init(stream_frame_info);

	int i = 0;
	int timer = 0;
	while (is_streaming || (i <= stream_time * fps))
	{
#if defined(_WIN32)
		WaitForSingleObject(image_sem, INFINITE);	//waitting for image singnal 
#elif defined(linux) || defined(unix)
		sem_wait(&image_sem);
#endif
		display_one_frame(stream_frame_info);
#if defined(_WIN32)
		ReleaseSemaphore(image_done_sem, 1, NULL);
#elif defined(linux) || defined(unix)
		sem_post(&image_done_sem);
#endif
		i++;
	}
	display_release();
#ifdef OPENCV_ENABLE
	cv::destroyAllWindows();
#endif
	printf("display thread exit!!\n");
	return NULL;
}
