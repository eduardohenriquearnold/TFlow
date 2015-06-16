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
                SparseMat hist;
                double v;
                double ts;                 //TimeStamp: Last update time 
                
                void calcHistogram(SparseMat& h, Mat& im);               
                
        public:
                Car(Rect re, Mat i, double t) : r(re), v(0), ts(t){Mat im = i(re); calcHistogram(hist, im);};
                
                Car& operator=(const Car& c);
                
                int area();
                Point2f pos();
                double velocity();
                
                void plot(Mat& f, char);

                bool onScene(Mat& f);
                bool match(Car& c);     //Assume function is being called from the 'newest' car.
                
};

#endif
