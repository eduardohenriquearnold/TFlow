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
        lu = c.lu;
}

void Car::plot(Mat& f)
{
        rectangle(f, r, Scalar(255,0,0));
        
        string s = string("v=")+to_string(velocity());
        putText(f, s, pos(), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0,255,0));
}

bool Car::onScene(Mat& f)
{
        //Crop image to car area
        Mat im2 = f(r);

        //Calculate absolute difference between new frame and the one with the car
        Mat abd;
        absdiff(im, im2, abd);
        double cmp = norm(abd, NORM_L1)/area();  
                      
        return cmp < 100;
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
        cout << "d="<< d << "   a=" << a << "     cmp="<< cmp << endl;
        
        //Check all conditions to confirm a match
        if (d < 20 && a < 0.4 && cmp < 200)
        {
                //Update velocity
                v = d/(lu-c.lu);
                
                return true;
        }
        else
                return false;
}
