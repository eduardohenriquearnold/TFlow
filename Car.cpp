#include "Car.hpp"

inline int Car::area()
{
        return im.rows*im.cols;
}

inline double Car::velocity()
{
        return v;
}

Car& Car::operator=(Car& c)
{
        pos = c.pos;
        c.im.copyTo(im);
        v = c.v;
        lu = c.lu;
}

void Car::plot(Mat& f)
{
        Point2f s = 0.5*Point2f(im.size());
        rectangle(f, pos-s, pos+s, Scalar(255,0,0));
}

bool Car::update(Car& c)
{
        //Distance between points
        double d = norm(pos-c.pos);
        
        //Area comparisson
        double a = abs(area() - c.area());
        
        //Im comparisson
        Mat c2, abd;
        resize(c.im, c2, im.size());
        absdiff(im, c2, abd);
        double cmp = norm(abd, NORM_L1);
        
        //Check all conditions to confirm a match
        if (d < 10 && a < 50 && cmp < 100)
        {
                //Accept as a match between cars
                double oldtime = lu;
                operator=(c);
                v = d/(lu-oldtime);
                
                return true;
        }
        else
                return false;
}
