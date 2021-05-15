#include <opencv2\opencv.hpp>
#include <iostream>

using namespace std;
using namespace cv;

int main() {
	VideoCapture capture1, capture2;
	capture1.open("pipoball.avi");
	capture2.open("GLOBE.avi");
	CV_Assert(capture1.isOpened() | capture2.isOpened()); //����ó��

	double frame_rate = capture1.get(CAP_PROP_FPS);
	int delay = 1000 / frame_rate;
	Mat frame1, frame2;

	while (1) {
		if (!capture1.read(frame1)) break; //Ź���� ������ ������ ��
		if (!capture2.read(frame2)) { //������ ������ Ź���� ������ ���� �� ���� �ݺ�
			capture2.open("GLOBE.avi");
			capture2.read(frame2);
		}

		Point2i pt, sum(0, 0);
		int count = 0;
		Mat dst(frame1.size(), CV_8UC3, Scalar(0, 0, 0));

		for (pt.y = 0; pt.y < frame1.rows; pt.y++) {
			for (pt.x = 0; pt.x < frame1.cols; pt.x++) {
				Vec3b val = frame1.at<Vec3b>(pt.y, pt.x);
				float lum = 0.11*(float)val[0] + 0.59*(float)val[1] + 0.3*(float)val[2];

				if (val[2]>0.5*lum&&val[2]>1.5*val[0] && val[2]>1.5 + val[1] && lum>50.) { //��Ȳ��
					count++; //��Ȳ���� ��Ÿ���� �ȼ� �� ����
					sum += pt; //�� ���ϱ�
				}
			}
		}

		Point2i center(sum.x / count, sum.y / count); //���� ����

		int radius = sqrt((float)count / 3.14); //�� ������ ���ϱ�

		Point2f pt1[4] = { Point2f(0,0), Point2f(frame2.cols,0), 
			Point2f(0, frame2.rows), Point2f(frame2.cols, frame2.rows) }; //������
		Point2f pt2[4] = { Point2f(center.x - radius, center.y - radius),
			Point2f(center.x + radius, center.y - radius),Point2f(center.x - radius, center.y + radius)
			,Point2f(center.x + radius, center.y + radius) }; //Ź������ ���δ� ���簢�� ��ǥ


		Mat aff_map = getAffineTransform(pt1, pt2); //������ ���
		warpAffine(frame2, dst, aff_map, frame1.size(), INTER_LINEAR); //������ ��ȯ

		imshow("frame1", frame1);
		imshow("frame2", frame2);
		imshow("dst", dst);

		if (waitKey(delay) >= 0) break;
	}
}