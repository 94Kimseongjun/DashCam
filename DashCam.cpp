/*
Example code for displaying gstreamer video from the CSI port of the Nvidia Jetson in OpenCV.
Created by Peter Moran on 7/29/17.
https://gist.github.com/peter-moran/742998d893cd013edf6d0c86cc86ff7f
inseock, seongjun
*/

#include <opencv2/opencv.hpp>
#include <iostream>
#include <ctime>
#include <list>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
using namespace cv;
using namespace std;

std::string get_tegra_pipeline(int width, int height, int fps) {
	return "nvarguscamerasrc ! video/x-raw(memory:NVMM), width=(int)" + std::to_string(width) + ", height=(int)" +
		std::to_string(height) + ", format=(string)NV12, framerate=(fraction)" + std::to_string(fps) +
		"/1 ! nvvidconv flip-method=0 ! video/x-raw, format=(string)BGRx ! videoconvert ! video/x-raw, format=(string)BGR ! appsink";
}

int main() {
	// Options
	int WIDTH = 640;
	int HEIGHT = 480;
	int FPS = 10;

	// Define the gstream pipeline
	std::string pipeline = get_tegra_pipeline(WIDTH, HEIGHT, FPS);
	std::cout << "Using pipeline: \n\t" << pipeline << "\n";
	
	// Create OpenCV capture object, ensure it works.
	cv::VideoCapture cap(pipeline, cv::CAP_GSTREAMER);
	if (!cap.isOpened()) {
		std::cout << "Connection failed";
		return -1;
	}
	
	// View video
	cv::Mat frame;
	
	int inkey = 1;
	
	while (inkey) //ESC�� ������ ������ �ݺ������� ���� ���� (inkey ���� 0�� �Ǳ� ������ �ݺ�)
	{
		FILE *fp;
		list<string> avi;
		char buf[255] = "\0";
		
		fp = popen("ls -tr save_avi/", "r");
		
		if (fp == NULL)
		{
			perror("error");
			exit(0);
		}
		
		while (fgets(buf, 255, fp) != NULL)
		{
			avi.push_back(buf);
		}
		
		if (avi.size()<3) //���� 3�� �����϶� 
		{
		}
		else
		{
			char buf2[255] = "\0";
			strcpy(buf, avi.front().c_str());
			int i = 0;
			char buff_null[255] = "\0";
			
			chdir("/home/user/0712black/save_avi/"); //������ ������ ���丮�� ����
			
			while (buf[i + 1] != '\0') //���ڿ��� ������ NULL�� ����
			{
				buff_null[i] = buf[i];
				i++;
			}
			
			if (remove(buff_null) == 0) //���� ������ ���� ����
			{
				cout << "ok" << endl;
			}
			else {
				cout << "error" << endl;
			}
			
			chdir("/home/user/0712black/"); //������ ���丮�� ����
		
		}

		time_t curr_time;
		struct tm *curr_tm;
		curr_time = time(NULL);
		curr_tm = localtime(&curr_time);
		int num;
		num = (curr_tm->tm_min + 1) % 60;
		
		//avi���ϸ��� ���۽ð����� ����
		VideoWriter video("save_avi/" +
			to_string(curr_tm->tm_year + 1900) + "_" +
			to_string(curr_tm->tm_mon + 1) + "_" +
			to_string(curr_tm->tm_mday) + "_" +
			to_string(curr_tm->tm_hour) + ":" +
			to_string(curr_tm->tm_min) + ":" +
			to_string(curr_tm->tm_sec) + ".avi", CV_FOURCC('M', 'J', 'P', 'G'), FPS, Size(WIDTH, HEIGHT), true);

		while (1) {
			curr_time = time(NULL);
			curr_tm = localtime(&curr_time);
			cap >> frame;  // Get a new frame from camera
			cv::putText(frame,
				to_string(curr_tm->tm_year + 1900) + "." +
				to_string(curr_tm->tm_mon + 1) + "." +
				to_string(curr_tm->tm_mday) + "." +
				to_string(curr_tm->tm_hour) + ":" +
				to_string(curr_tm->tm_min) + ":" +
				to_string(curr_tm->tm_sec),
				Point(30, 450), 1, 1.2, Scalar::all(255));
			video.write(frame);
			imshow("Display window", frame);
			
			int wait_key;
			
			wait_key = cv::waitKey(1); //needed to show frame

			if (wait_key == 27) //ESCŰ�� ������ ����
			{
				inkey = 0;
				break;
			}
			if (curr_tm->tm_min == num) //���۽ð����� ������ �ð��� ����ð��� �������� break;
				break;
		}
	}
	cout << "exit!!" << endl;
}