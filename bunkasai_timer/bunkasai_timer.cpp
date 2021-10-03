#include<iostream>
#include<opencv2/opencv.hpp>
#include<time.h>

#ifdef _DEBUG
#pragma comment(lib, "opencv_world453d.lib")
#else
#pragma comment(lib, "opencv_world453.lib")
#endif

using namespace std;
using namespace cv;

#define FRAME_WIDTH 1920
#define FRAME_HEIGHT 1200
#define CAN_CANT_X 200
#define CAN_CANT_Y 300

Mat people(int people_val);
Mat ninzu;
Mat time_output(Mat img);
Mat enter(Mat img);
void PinP_point(const cv::Mat& srcImg, const cv::Mat& smallImg, const cv::Point2f p0, const cv::Point2f p1);
bool out_flag=true;
struct tm tm;
Mat cant = imread("cant_enter.jpg"), can = imread("can_enter.jpg");

int main() {
	int people_val;
	ninzu=imread("ninzu.png");
	namedWindow("show", WINDOW_AUTOSIZE|WINDOW_FREERATIO);
	Mat img(Size(FRAME_WIDTH,FRAME_HEIGHT), CV_8UC3,Scalar(0,0,0));//最終的に表示する画像データ
	img=people(10);
	img=time_output(img);
	img=enter(img);
	imshow("show",img );
	while (waitKey(10) != 113) {
	}
	return 0;
}

Mat people(int people_val) {
	Mat img = imread("ninzu.jpg");
	putText(img, to_string(people_val), Point(FRAME_WIDTH/4, FRAME_HEIGHT/1.25),FONT_HERSHEY_TRIPLEX,30, Scalar(0,0 , 255),50);
	return img;
}

Mat time_output(Mat img) {
	time_t t = time(NULL);
	localtime_s(&tm,&t);
	cout << tm.tm_hour << ":" << tm.tm_min << endl;
	string hour= to_string(tm.tm_hour),min= to_string(tm.tm_min);
	if (tm.tm_hour <= 10)hour.insert(0, "0");
	if (tm.tm_min <= 10)min.insert(0, "0");
	String time_st = hour + ":" + min;
	putText(img, time_st, Point(1300, 150), FONT_HERSHEY_TRIPLEX, 6, Scalar(0, 0, 0), 8);
	return img;
}

Mat enter(Mat img) {
	if (out_flag == true) {//入室可能なら
		PinP_point(img,can,Point2f(CAN_CANT_X,CAN_CANT_Y),Point2f(CAN_CANT_X+400,CAN_CANT_Y+800));
	}
	else if (out_flag==false) {//入室不能なら
		PinP_point(img, cant, Point2f(CAN_CANT_X, CAN_CANT_Y), Point2f(CAN_CANT_X + 400, CAN_CANT_Y + 800));
	}
	return img;
}

void PinP_point(const cv::Mat& srcImg, const cv::Mat& smallImg, const cv::Point2f p0, const cv::Point2f p1)
{
	//背景画像の作成
	cv::Mat dstImg;
	srcImg.copyTo(dstImg);

	//３組の対応点を作成
	vector<cv::Point2f> src, dst;
	src.push_back(cv::Point2f(0, 0));
	src.push_back(cv::Point2f(smallImg.cols, 0));
	src.push_back(cv::Point2f(smallImg.cols, smallImg.rows));

	dst.push_back(p0);
	dst.push_back(cv::Point2f(p1.x, p0.y));
	dst.push_back(p1);

	//前景画像の変形行列
	cv::Mat mat = cv::getAffineTransform(src, dst);

	//アフィン変換の実行
	cv::warpAffine(smallImg, dstImg, mat, dstImg.size(), INTER_LINEAR, cv::BORDER_TRANSPARENT);
}

