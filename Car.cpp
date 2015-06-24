#include "Car.hpp"

inline int Car::area()
{
        return r.area();
}

inline double Car::velocity()
{
        return v;
}

inline Point2f Car::pos()
{
        return 0.5*(r.tl()+r.br());
}

Car& Car::operator=(const Car& c)
{
        r = c.r;
        c.hist.copyTo(hist);
        v = c.v;
        ts = c.ts;
}

void Car::plot(Mat& f, char c)
{
        Scalar color;
        
        if (c=='b')
                color=Scalar(255,0,0);
        else
                color=Scalar(0,255,0);
                
        rectangle(f, r, color);
        
        string s = string("v=")+to_string(velocity());
        putText(f, s, pos(), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0,255,0));
}

void Car::calcHistogram(Mat& h, Mat& im)
{
/*        const int sizes[] = {256};
        const int channels[] = {0,1,2};
        float range[] = {0,256};
        const float *ranges[] = {range,range,range};

        calcHist(&im, 1, channels, Mat(), h, 2, sizes, ranges);
        normalize(h, h, 100, NORM_L2);*/
        
        Mat im2;
        cvtColor(im, im2, CV_BGR2GRAY);
        
        // Quantize the hue to 30 levels
        // and the saturation to 32 levels
        int hbins = 256;
        int histSize[] = {hbins};
        
        // hue varies from 0 to 179, see cvtColor
        float hranges[] = { 0, 256 };
        
        // saturation varies from 0 (black-gray-white) to
        // 255 (pure spectrum color)
        const float* ranges[] = { hranges};
        
        // we compute the histogram from the 0-th and 1-st channels
        int channels[] = {0};

        calcHist( &im2, 1, channels, Mat(), h, 1, histSize, ranges, true, false );
        normalize(h, h, 100, NORM_L2);
        
}

bool Car::onScene(Mat& f)
{
        //Crop image to rect area
        Mat im = f(r);
        
        //Calculate new histogram
        Mat nhist;
        calcHistogram(nhist, im);
        
        //Compare them (correlation)
        double corr = compareHist(hist, nhist, CV_COMP_CORREL);                      
        
        //DEBUG
        /*Mat p = f.clone();
        plot(p, 0);
        imshow("OnScene Debug", p);
        cout << "corr = " << corr << endl;

        //cout << nhist << endl;
        imshow("last car", im);
        waitKey();*/
                              
        return corr > 0.6;                
}

bool Car::match(Car& c)
{
        //Distance between points
        double d = norm(pos()-c.pos());
        
        //Area comparisson
        double a = abs(double(area())/ c.area());
        a = abs(a-1);
        
        //Hist correlation
        double hCorr = compareHist(hist, c.hist, CV_COMP_CORREL);
        
        //DEBUG
        cout << "d="<< d << "   a=" << a << "     corr="<< hCorr << endl;
        
        //Check all conditions to confirm a match
        if (d < 20 && a < 0.5 && hCorr > 0.1)
        {
                //Update velocity
                v = d/(ts-c.ts);
                
                return true;
        }
        else
                return false;
}
