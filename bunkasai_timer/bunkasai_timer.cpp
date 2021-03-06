#include<iostream>
#include<opencv2/opencv.hpp>
#include<time.h>
#define CVUI_IMPLEMENTATION
#include "cvui.h"

#ifdef _DEBUG
#pragma comment(lib, "opencv_world453d.lib")
#else
#pragma comment(lib, "opencv_world453.lib")
#endif

using namespace std;
using namespace cv;

#define FRAME_WIDTH 1920	//画像のサイズ
#define FRAME_HEIGHT 1200
//#define FRAME_HEIGHT 1080
#define CAN_CANT_X 0		//入室可不可の座標
#define CAN_CANT_Y 100
#define DEFAULT_MAX_NINN 25

Mat people(int people_val, Mat img);
Mat ninzu;
Mat time_output(Mat img);
Mat enter(Mat img);
Mat PinP_tr(const cv::Mat& srcImg, const cv::Mat& smallImg, const int tx, const int ty);
Mat Overview(Mat img);
int button_input();
bool out_flag = false, time_flag;
int button_flag,ninn_num=0;//0が何も押されていない,1が増やす,2が減らす:現在の人数
int old_sec;
struct tm tm;
Mat cant = imread("cant_enter.jpg"), can = imread("can_enter.jpg");
Mat ninn = imread("ninn.jpg");
Mat genzai = imread("ninzu.jpg");
Mat control(Size(640, 480), CV_8UC3, Scalar(0, 0, 0));
int MAX_NINN = 25;

int main() {
	int people_val;
	ninzu=imread("ninzu.png");
	namedWindow("show", WINDOW_AUTOSIZE|WINDOW_FREERATIO);
	namedWindow("control", WINDOW_AUTOSIZE | WINDOW_FREERATIO);
	cvui::init("control");
	while (1) {
		Mat img(Size(FRAME_WIDTH, FRAME_HEIGHT), CV_8UC3, Scalar(255, 255, 255));//最終的に表示する画像データ
		if(button_input()==0)return 0;
		img = Overview(img);
		img = people(10, img);
		img = time_output(img);
		img = enter(img);
		imshow("show", img);
		imshow("control",control);
		if(waitKey(1)==113)break;
	}
EXIT:;
}

Mat Overview(Mat img) {
	img=PinP_tr(img, genzai, FRAME_WIDTH/4, 0);//現在の人数
	img=PinP_tr(img, ninn, FRAME_WIDTH-FRAME_WIDTH / 7, FRAME_HEIGHT-FRAME_HEIGHT / 4);//人
	return img;
}

Mat people(int people_val,Mat img) {//人数を出力
	if (ninn_num < MAX_NINN) {
		putText(img, to_string(ninn_num), Point(FRAME_WIDTH / 4, FRAME_HEIGHT / 1.5), FONT_HERSHEY_TRIPLEX, 20, Scalar(255, 0, 0), 50);
		out_flag = true;
	}
	else if (ninn_num >= MAX_NINN) {
		putText(img, to_string(ninn_num), Point(FRAME_WIDTH / 4, FRAME_HEIGHT / 1.5), FONT_HERSHEY_TRIPLEX, 20, Scalar(0, 0, 255), 50);
		out_flag = false;
	}
	putText(img, "/"+to_string(MAX_NINN), Point(FRAME_WIDTH / 1.7, FRAME_HEIGHT / 1.3), FONT_HERSHEY_TRIPLEX, 10, Scalar(0, 0, 0), 30);
	return img;
}

Mat time_output(Mat img) {//時間を出力
	String time_st;
	time_t t = time(NULL);
	localtime_s(&tm,&t);
	int sec = tm.tm_sec;
	if (sec != old_sec) {
		if (time_flag == true) {
			time_flag = false;
			cout << "be false" << endl;
		}
		else if (time_flag == false) {
			time_flag = true;
			cout << "be true" << endl;
		}
	}

	old_sec = sec;
	cout << tm.tm_hour << ":" << tm.tm_min << ":"<<sec<<endl;
	string hour= to_string(tm.tm_hour),min= to_string(tm.tm_min);

	if (tm.tm_hour < 10)hour.insert(0, "0");
	if (tm.tm_min < 10)min.insert(0, "0");
	if (time_flag ==true) {
		time_st = hour + ":" + min;
		putText(img, time_st, Point(FRAME_WIDTH * 0.67, FRAME_HEIGHT * 0.125), FONT_HERSHEY_TRIPLEX, 6, Scalar(0, 0, 0), 8);
		cout << ":"<<endl;
	}
	else if (time_flag ==false) {
		putText(img, hour, Point(FRAME_WIDTH * 0.67, FRAME_HEIGHT * 0.125), FONT_HERSHEY_TRIPLEX, 6, Scalar(0, 0, 0), 8);
		putText(img, min, Point(FRAME_WIDTH * 0.67+306, FRAME_HEIGHT * 0.125), FONT_HERSHEY_TRIPLEX, 6, Scalar(0, 0, 0), 8);
		cout << ";" << endl;
	}
	return img;
}

Mat enter(Mat img) {//入室可不可を出力
	if (out_flag == true) {//入室可能なら
		img=PinP_tr(img,can,CAN_CANT_X,CAN_CANT_Y);
		cvui::printf(control, 20, 150, 1.4,0x46ffff,"can enter");
	}
	else if (out_flag==false) {//入室不能なら
		img=PinP_tr(img, cant, CAN_CANT_X,CAN_CANT_Y);
		cvui::printf(control, 20, 150, 1.4,0xff0000,"cant enter");
	}
	return img;
}

Mat PinP_tr(const cv::Mat& srcImg, const cv::Mat& smallImg, const int tx, const int ty)
{
	//背景画像の作成
	cv::Mat dstImg;
	srcImg.copyTo(dstImg);

	//前景画像の変形行列
	cv::Mat mat = (cv::Mat_<double>(2, 3) << 1.0, 0.0, tx, 0.0, 1.0, ty);

	//アフィン変換の実行
	cv::warpAffine(smallImg, dstImg, mat, dstImg.size(), INTER_LINEAR, cv::BORDER_TRANSPARENT);
	return dstImg;
}

int button_input() {
	control = Mat(Size(320,240), CV_8UC3, Scalar(49, 52, 49));
	cvui::printf(control, 10, 220, "This program was made by masadaruma");
	cvui::printf(control, 20, 10, "current count");
	cvui::counter(control, 20, 30, &ninn_num);
	cvui::printf(control, 20, 60, "Max count");
	cvui::counter(control, 20, 80, &MAX_NINN);
	if (cvui::button(control, 250, 190, "exit"))return 0;
	if (cvui::button(control, 150, 30, "Reset current count"))ninn_num=0;
	if (cvui::button(control, 150, 80, "Reset max count"))MAX_NINN=DEFAULT_MAX_NINN;
	if (ninn_num < 0)ninn_num = 0;
	if (MAX_NINN < 0)MAX_NINN = 0;
	cout << "ninn_num : " << ninn_num << endl;
	cout << "MAX_NINN" << MAX_NINN << endl;
	cvui::update();
	return 1;
}



