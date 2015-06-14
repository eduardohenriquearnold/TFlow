#ifndef CAR
#define CAR

#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;

class Car
{
        private:
                Point2f pos;
                Mat im;
                double v;
                double lu;                 //Last update time
                
        public:
                Car(Point2f p, Mat i, double t) : pos(p), im(i), v(0), lu(t) {};                
                Car& operator=(Car& c);
                
                int area();
                double velocity();
                void plot(Mat& f);
                
                bool update(Car& c);
                
};

#endif
