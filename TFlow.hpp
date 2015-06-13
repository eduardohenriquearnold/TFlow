#ifndef TFLOW
#define TFLOW

#include <iostream>
#include <fstream>
#include <algorithm>
#include <opencv2/opencv.hpp>

#define contour vector<Point>

using namespace std;
using namespace cv;

class TFlow
{
        private:
                VideoCapture vc;
                Mat ROITransform;
                Size ROISize;
                
                void orderPointsClockwise(vector<Point2f>& pts);
                Mat getTransMatrix(vector<Point2f>& src1, Size s);
                Mat getROI(Mat f);
                void blobs(Mat f, Mat& ROI);
                
        public:
                TFlow(){};
                void genConfig(string cFile);
                void loadConfig(string cFile);
                void play();
};

#endif
