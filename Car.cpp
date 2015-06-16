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
        c.im.copyTo(im);
        v = c.v;
        ts = c.ts;
}

void Car::plot(Mat& f, int c)
{
        Scalar color;
        
        if (c==0)
                color=Scalar(255,0,0);
        else
                color=Scalar(0,255,0);
                
        rectangle(f, r, color);
        
        string s = string("v=")+to_string(velocity());
        putText(f, s, pos(), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0,255,0));
}

bool Car::onScene(Mat& f, double time)
{
        //If timestamp is not current, force velocity to be zero (cannot be moving), otherwise it wouldnt match
        if (ts<time)
                v = 0;
        
        //Crop image to car area
        Mat im2 = f(r);

        //Calculate absolute difference between new frame and the one with the car
        Mat abd;
        absdiff(im, im2, abd);
        double cmp = norm(abd, NORM_L1)/area();  
                      
        return cmp < 30;
}

bool Car::match(Car& c)
{
        //Distance between points
        double d = norm(pos()-c.pos());
        
        //Area comparisson
        double a = abs(double(area())/ c.area());
        a = abs(a-1);
        
        //Im comparisson
        Mat c2, abd;
        resize(c.im, c2, im.size());
        absdiff(im, c2, abd);
        double cmp = norm(abd, NORM_L1)/area();
        
        //DEBUG
        //cout << "d="<< d << "   a=" << a << "     cmp="<< cmp << endl;
        
        //Check all conditions to confirm a match
        if (d < 20 && a < 0.5 && cmp < 200)
        {
                //Update velocity
                v = d/(ts-c.ts);
                
                return true;
        }
        else
                return false;
}
