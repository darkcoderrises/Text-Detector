#include <iostream>
#include <fstream>
#include <string>
#include <tuple>
#include <iomanip>
#include <stack>
#include <utility>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

typedef std::tuple<int, int, int, int> boundary;

using namespace std;
using namespace cv;

#define error(args...) { vector<string> _v = split(#args, ','); err(_v.begin(), args); }

vector<string> split(const string& s, char c) {
	vector<string> v;
	stringstream ss(s);
	string x;
	while (getline(ss, x, c))
		v.emplace_back(x);
	return move(v);
}

void err(vector<string>::iterator it) { cerr << '\n';}
template<typename T, typename... Args>
void err(vector<string>::iterator it, T a, Args... args) {
	cerr << it -> substr((*it)[0] == ' ', it -> length()) << " = " << a << ' ';
	err(++it, args...);
}

int histValue=22*22;
int directions_[][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}, {-1, 1}, {-1, -1}, {1, -1}, {1, 1}};
int direction[][2] = {{-1,-1}, {-1,0}, {-1,1}, {0,1}, {1,1}, {1,0}, {1,-1}, {0,-1}};
int width, height, maxX, maxY, minX, minY; 
bool *image, *visited;
vector<boundary> textArea;
ofstream train_x("train_x", ofstream::out|ofstream::app), train_y("train_y", ofstream::out|ofstream::app);

int get_x(int pixel) {
    if (width>0) return pixel%width;
    return -1;
}

int get_y(int pixel) {
    if (width>0) return pixel/width;
    return -1;
}

int get_pixel(int x, int y) {
    return x+y*width;
}

bool check(int x, int y) {
	if (x>=width || y>=height || x<0 || y<0) return false;
	return true;
}

void dfs(int x, int y) {
	stack<pair<int, int>> recursive_stack;
	recursive_stack.push(make_pair(x, y));

	while (!recursive_stack.empty()) {
		pair<int, int> top = recursive_stack.top(); x = top.first; y = top.second;
		recursive_stack.pop();

		visited[get_pixel(x,y)] = true;
		maxX = max(x, maxX); minX = min(x, minX); maxY = max(y, maxY); minY = min(y, minY);
		for (int i=0; i<8; i++) {
			int X = x+directions_[i][0], Y = y+directions_[i][1];
			if (check(X,Y) && !visited[get_pixel(X,Y)] && image[get_pixel(X,Y)]==true) {
				recursive_stack.push(make_pair(X, Y));
			}
		}
	}
}

bool checkOverlap(boundary text, boundary nonText) {
	if (get<0>(text) > get<1>(nonText) || get<0>(nonText) > get<1>(text)) 
		return false;
	if (get<2>(text) > get<3>(nonText) || get<2>(nonText) > get<3>(text))
		return false;
	return true;
}

bool checkAllOverlap(boundary nonText) {
	if (get<1>(nonText)-get<0>(nonText) <= 3) return true;
	if (get<3>(nonText)-get<2>(nonText) <= 3) return true;
	
	for (int i=0; i<textArea.size(); i++) 
		if (checkOverlap(textArea[i], nonText))
			return true;
	return false;
}

boundary makeBoundary() {
	int x1=rand()%width, x2=rand()%width, y1=rand()%height, y2=rand()%height;
	return boundary(min(x1, x2), max(x1, x2), min(y1, y2), max(y1, y2));
}


double* calcHistogram(Point p1, Point p2, Mat image) {
	Mat im1 = image(Rect(max(p1.x,0), max(p1.y,0), p2.x-p1.x, p2.y-p1.y)), resized;
	resize(im1, resized, cvSize(24,24));
        
    char filename[128];
    // if (getIntersection(p1, p2) > 0.7) {
    //     sprintf(filename, "images/i1/%s_%04d.jpg", globalName.c_str(), countIm);
    // 	imwrite(filename, im1);
    // }
    // else if (getIntersection(p1, p2) <= 0) {
    //     sprintf(filename, "images/i2/%s_%04d.jpg", globalName.c_str(), countIm);
    // 	imwrite(filename, im1);
    // }

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

double* calcHistogram(boundary b, Mat image) {
	Point p1(get<0>(b), get<2>(b)), p2(get<1>(b), get<3>(b));
	// cout << get<0>(b) << " " << get<2>(b) << "     " << get<1>(b) << " " << get<3>(b) << endl;
	return calcHistogram(p1, p2, image);
}

void printMat(double* hist, bool truth) {
	for (int t=0; t<(histValue); t++) {
    	train_x << hist[t] << " ";
    }
    train_x << endl;

    if (truth) train_y << 1 << endl;
    else train_y << -1 << endl;
}

void trainFile(string fileName, string gtName) {
	Mat gt_image_mat = imread(gtName, CV_LOAD_IMAGE_GRAYSCALE);
	Mat image_mat = imread(fileName, CV_LOAD_IMAGE_GRAYSCALE);
	Mat to_show = imread(fileName, CV_LOAD_IMAGE_COLOR);

	width = gt_image_mat.cols;
	height = gt_image_mat.rows;
	image = new bool[width*height];
	visited = new bool[width*height];

	for (int i=0; i<height; i++) {
		uchar *ptr = gt_image_mat.ptr<uchar>(i);
		for (int j=0; j<width; j++) {
			image[get_pixel(j,i)] = ((int) ptr[j]) < 255;
			visited[get_pixel(j,i)] = false;
		}
	}

	for (int i=0; i<width; i++) {
		for (int j=0; j<height; j++) {
			if (visited[get_pixel(i,j)]==false && image[get_pixel(i,j)]==true) {
				maxX = 0; maxY = 0; minX = max(height, width); minY = minX;
				dfs(i,j);
				if ((maxX-minX)<2 || (maxY-minY)<2) continue;
				textArea.push_back(boundary(minX, maxX, minY, maxY));
			}
		}
	}

	for (int i=0; i<textArea.size(); i++) {
		printMat(calcHistogram(textArea[i], image_mat), true);
		rectangle(to_show, Point(get<0>(textArea[i]), get<2>(textArea[i])), Point(get<1>(textArea[i]), get<3>(textArea[i])), CV_RGB(0,255,0),1);
		while (true) {
			boundary b = makeBoundary();
			if (checkAllOverlap(b)==false) {
				rectangle(to_show, Point(get<0>(b), get<2>(b)), Point(get<1>(b), get<3>(b)), CV_RGB(0,0,255),1);
				printMat(calcHistogram(b, image_mat), false);
				break;
			}
		}
	}

	// imshow("im1", to_show);
	// waitKey(0);
}

int main(int argc, char *argv[]) {
	string fileName = "database/img_"+ (string)argv[1] +".jpg";
	string gtName = "database/gt_img_"+ (string)argv[1] +".png";

	trainFile(fileName, gtName);
	return 0;
}