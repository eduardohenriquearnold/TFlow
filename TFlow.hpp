#ifndef TFLOW
#define TFLOW
#include <iostream>
#include <fstream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

class TFlow
{
        private:
                VideoCapture vc;
                Mat ROITransform;
                Size ROISize;
                
                void orderPointsClockwise(vector<Point2f>& pts);
                Mat getTransMatrix(vector<Point2f>& src, Size s);
                Mat getROI(Mat f);
                
        public:
                TFlow();
                void genConfig();
                void loadConfig();
                void play();
};

#endif
