//
// Created by harshil on 19/8/16.
//

#include "er.h"
#include <vector>
#include <set>
#include <stdlib.h>
#include <time.h>
#include <ctime>
#include <iostream>
#include <fstream>
#include <stack>
#include <opencv2/ml/ml.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace std;
using namespace cv;

// #define error(args...) { vector<string> _v = split(#args, ','); err(_v.begin(), args); }

// vector<string> split(const string& s, char c) {
// 	vector<string> v;
// 	stringstream ss(s);
// 	string x;
// 	while (getline(ss, x, c))
// 		v.emplace_back(x);
// 	return move(v);
// }

// void err(vector<string>::iterator it) { cerr << '\n';}
// template<typename T, typename... Args>
// void err(vector<string>::iterator it, T a, Args... args) {
// 	cerr << it -> substr((*it)[0] == ' ', it -> length()) << " = " << a << ' ';
// 	err(++it, args...);
// }


int histValue=22*22, direction[][2] = {{-1,-1}, {-1,0}, {-1,1}, {0,1}, {1,1}, {1,0}, {1,-1}, {0,-1}}, **gt_arr;
// ofstream result_x("Result/result_x", ofstream::out|ofstream::app), result_y("Result/result_y", ofstream::out|ofstream::app), 
// boundary("Result/boundary", ofstream::out|ofstream::app);
CvBoost boost;

int countIm = 0;
String globalName;

Mat gt_image_mat;
vector<Rect> chars;

Rect dfs(int i, int j, bool** visited, int** image_bool, Rect &rect) {
	stack<tuple<int, int>> s;
	s.push(make_tuple(i, j));

	while (!s.empty()) {
		tuple<int, int> t = s.top(); s.pop();
		int x=get<0>(t), y=get<1>(t);
		visited[x][y] = true;

		int x1 = rect.x, y1 = rect.y, x2 = x1 + rect.width, y2 = y1 + rect.height;
		x1 = min(x1, y); x2 = max(x2, y); y1 = min(y1, x); y2 = max(y2, x);
		
		rect.x = x1; rect.y = y1; rect.width = x2-x1; rect.height = y2-y1;

		int direction[][2] = {{-1,0}, {1,0}, {0,1}, {0,-1}};

		for (int i=0; i<4; i++) {
			int X = x + direction[i][0], Y = y + direction[i][1];
			if (X<0 || X>=gt_image_mat.rows || Y<0 || Y>=gt_image_mat.cols || visited[X][Y] || abs(image_bool[X][Y] - image_bool[x][y]) > 5 ) continue;
			s.push(make_tuple(X,Y));
		}
	}
}

void readGroundTruth(String image_path) {
	gt_image_mat = imread(image_path, CV_LOAD_IMAGE_GRAYSCALE);

	bool **visited;
	int **image_bool;
	
	gt_arr = new int*[gt_image_mat.rows];
	visited = new bool*[gt_image_mat.rows];
	image_bool = new int*[gt_image_mat.rows];

	for (int i=0; i<gt_image_mat.rows; i++) {
		gt_arr[i] = new int[gt_image_mat.cols];
		visited[i] = new bool[gt_image_mat.cols];
		image_bool[i] = new int[gt_image_mat.cols];
	}

	for (int i=0; i<gt_image_mat.rows; i++) {
		uchar *ptr = gt_image_mat.ptr<uchar>(i);
		for (int j=0; j<gt_image_mat.cols; j++) {
			image_bool[i][j] = (int) ptr[j];

			gt_arr[i][j] = ((int) ptr[j]) < 255;
			if (i==0 && j == 0) continue;
			else if (i==0) gt_arr[i][j] += gt_arr[i][j-1];
			else if (j==0) gt_arr[i][j] += gt_arr[i-1][j];
			else gt_arr[i][j] += gt_arr[i][j-1] + gt_arr[i-1][j] - gt_arr[i-1][j-1];
		}
	}

	for (int i=0; i<gt_image_mat.rows; i++) {
		for (int j=0; j<gt_image_mat.cols; j++) {
			if (!(visited[i][j] == 0 && image_bool[i][j] < 255)) 
				continue;
		
			Rect rect(j,i,0,0);
			dfs(i, j, visited, image_bool, rect);
			chars.push_back(rect);
		}
	}
}	

double getIntersection(Point p1, Point p2) {
	Rect rect(p1.x, p1.y, p2.x-p1.x, p2.y-p1.y), selectedRect;
	int intersection = -1; 

	for (Rect charRect: chars) {
		Rect intersectionRect = charRect & rect;
		int area = intersectionRect.area();

		if (area > intersection) {
			intersection = area;
			selectedRect = charRect;
		}
	}

	return ((double) intersection) / ((double) rect.width*rect.height + selectedRect.width*selectedRect.height - intersection);
}

bool checkGroundTruth(Point p1, Point p2) {
	return getIntersection(p1, p2) > 0.7;
}

double* calcHistogram(Point p1, Point p2, Mat image) {
	Mat im1 = image(Rect(max(p1.x,0), max(p1.y,0), p2.x-p1.x, p2.y-p1.y)), resized;
	resize(im1, resized, cvSize(24,24));
        
    countIm++;
    char filename[128];
/*    if (getIntersection(p1, p2) > 0.7) {
        sprintf(filename, "images/CLASS1/%s_%02d.jpg", globalName.c_str(), countIm);
    	imwrite(filename, im1);
    }
    else if (getIntersection(p1, p2) <= 0.3) {
        sprintf(filename, "images/CLASS2/%s_%02d.jpg", globalName.c_str(), countIm);
    	imwrite(filename, im1);
    }
    else {
        sprintf(filename, "images/CLASS3/%s_%02d.jpg", globalName.c_str(), countIm);
    	imwrite(filename, im1);
    }*/

	double* histogram = new double[histValue];
	double mean = 0, var = 0;

	int number_of_items = 0;
	for (int i=1; i<=22; i++) {
		for (int j=1; j<=22; j++) {
			double sum=0;
			for (int direc=0; direc<8; direc++) {
				int x = i+direction[direc][1], y = j+direction[direc][0];
				sum += (double) resized.at<uchar>(y,x);
			}
			sum /= 8;
			unsigned char code = 0;
			for (int direc=0; direc<8; direc++) {
				int x = i+direction[direc][1], y = j+direction[direc][0];
				code |= (((double) resized.at<uchar>(y,x)) > sum) << (7-direc);
			}

			histogram[(i-1)*22+j-1] = code;
			mean += code / histValue;
		}
	}

	return histogram;
}

void runOnImage(String name) {
    globalName = name;
	String image_name = "database/" + name + ".jpg";
	String gt_image = "database/gt_" + name + ".png";

	cerr << image_name << " " << gt_image << endl;
	vector<int> levels;
	for (int i=1; i<256; i+=2) 
		levels.push_back(i);
	ER er(levels), er_inv(levels);

	Mat image = imread(image_name, CV_LOAD_IMAGE_GRAYSCALE);
	Mat im1 = imread(image_name, CV_LOAD_IMAGE_COLOR);
        Mat imTemp = imread(image_name, CV_LOAD_IMAGE_COLOR);

	im1.convertTo(im1, CV_8U);
	image.convertTo(image, CV_8U);
	vector<int> points[2];
	readGroundTruth(gt_image);

	// for (Rect c : chars) {
	// 	rectangle(gt_image_mat, c,CV_RGB(0,255,0),1);
	// }
	// imshow("gt", gt_image_mat);

	cerr << image.rows << " * " << image.cols << endl;
	for (int i=0;i<image.rows;i++) {
		uchar* ptr = image.ptr<uchar>(i);
		for (int j=0; j<image.cols;j++) {
			points[0].push_back((int)(ptr[j]));
			points[1].push_back(255-(int)(ptr[j]));
		}
	}

	clock_t begin = clock();
	vector<Region *> result_i = er_inv.find(points[1], image.cols, image.rows);
	vector<Region *> result = er.find(points[0], image.cols, image.rows);
	vector<Region *> suppressed_inv = er.non_maximum_suppression(result_i);
	vector<Region *> suppressed = er.non_maximum_suppression(result);
	for (int i=0; i<suppressed_inv.size();i++){
		suppressed.push_back(suppressed_inv[i]);
	}

	clock_t end = clock();
	double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;

	int precision = 0;

	cerr << elapsed_secs << " Printing suppressed\n";
	for (int i=0;i<suppressed.size();i++) {
		Point p1,p2;
		p1.x = suppressed[i]->min_x_;
		p1.y = suppressed[i]->min_y_;
		p2.x = suppressed[i]->max_x_;
		p2.y = suppressed[i]->max_y_;

		if (!((p2.x-p1.x)>=3 && (p2.y-p1.y)>=3)) continue;

		double* hist = calcHistogram(p1, p2, image);

//		boundary << p1.x << " " << p1.y << " " << p2.x << " " << p2.y << endl;

		Mat out = Mat::zeros(1, histValue, CV_32F);
		for (int t=0; t<(histValue); t++) {
//			result_x << hist[t] << " ";
			out.at<float>(0, t) = (float) hist[t]; 
		}
//		result_x << endl;

		bool label = false;
		if ((boost.predict(out) == 1 && getIntersection(p1, p2) > 0) || (boost.predict(out) == -1 && getIntersection(p1, p2)==0))
		 	label = true;
		precision += (int) label;


		//boost.predict(out) == 1 checkGroundTruth(p1, p2) getIntersection(p1, p2)
		if (boost.predict(out)==1) { 
			rectangle(im1, p1, p2, CV_RGB(0,255,0),1);
			rectangle(imTemp, p1, p2, CV_RGB(0,255,0),1);
//			result_y << 1 << endl;
		}
		else {
			rectangle(imTemp, p1, p2, CV_RGB(255,0,0),1);
//			result_y << -1 << endl;
		}
	}

	//cout << (float) precision / suppressed.size() << " " << precision << " " << suppressed.size() << endl;
        imshow("imTemp", imTemp);
	imshow("im1", im1);
	waitKey(0);
}

int main(int argc, char *argv[]) {
	boost.load("./data1.xml");
	runOnImage(argv[1]);
//	result_y.close();
//	result_x.close();
//	boundary.close();
	return 0;
}
