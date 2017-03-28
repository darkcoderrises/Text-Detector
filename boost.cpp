#include "er.h"
#include <vector>
#include <set>
#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <iterator>
#include <algorithm>
#include <fstream>
#include <opencv2/ml/ml.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace std;
using namespace cv;

int line_count(String file_name) {
	ifstream afile(file_name);
	return count(istreambuf_iterator<char>(afile), istreambuf_iterator<char>(), '\n');
}

Mat read_mat_from_file(string filename, int rows,int cols) {
	float m;
    Mat out = Mat::zeros(rows, cols, CV_32F);//Matrix to store values

    ifstream fileStream(filename);
    int cnt = 0;//index starts from 0
    while (fileStream >> m)
    {
    	int temprow = cnt / cols;
    	int tempcol = cnt % cols;
    	out.at<float>(temprow, tempcol) = m;
    	cnt++;
    }
    return out;
}

bool train = true;

int main(int argc, char *argv[]) {
	String train_file_x = "train_x", train_file_y = "train_y", result_file_x = "Result/result_x", result_file_y = "Result/result_y";
	Mat train_x, train_y, result_x, result_y;

	train_x = read_mat_from_file(train_file_x, line_count(train_file_x), 22*22);
	train_y = read_mat_from_file(train_file_y, line_count(train_file_y), 1);
	result_x = read_mat_from_file(result_file_x, line_count(result_file_x), 22*22);
	result_y = read_mat_from_file(result_file_y, line_count(result_file_y), 1);

	CvBoostParams param(CvBoost::REAL,20,0,100,false,0);
	CvBoost boost;
	
	if (train) {
		boost.train(train_x, CV_ROW_SAMPLE, train_y, Mat(), Mat(), Mat(), Mat(), param, false);
		boost.save("./data1.xml", "boost");
		cout << "Training complete\n";
	} 
	boost.load("./data1.xml");
	
	int total = result_y.size[0], correct = 0;
	for (int i=0; i<total; i++) {
		float result = boost.predict(result_x.row(i));
		correct += (result_y.at<float>(i,0) == result);
	}
	cout << ((float) correct) / ((float) total) << endl;
}
