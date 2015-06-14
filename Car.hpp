#ifndef CAR
#define CAR

#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

class Car
{
        private:
                Rect r;
                Mat im;
                double v;
                double lu;                 //Last update time
                
        public:
                Car(Rect re, Mat i, double t) : r(re), im(i(re)), v(0), lu(t) {};
                Car& operator=(const Car& c);
                
                int area();
                Point2f pos();
                double velocity();
                
                void plot(Mat& f);

                bool onScene(Mat& f);
                bool match(Car& c);     //Assume function is being called from the 'newest' car.
                
};

#endif
