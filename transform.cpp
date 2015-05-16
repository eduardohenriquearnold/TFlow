#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/background_segm.hpp>
#include <iostream>

using namespace cv;

//Given an array of points, sort them clockwise
void orderPointsClockwise(vector<Point2f>& pts)
{
        int n = pts.size();
        
        //Get centroid
        Point2f mean(0,0);
        for (int i=0; i<n; i++)
                mean += pts[i];
        mean *= double(1)/n;
        
        //Compare angles of points        
        auto comp = [&] (Point2f a, Point2f b)
        {
                Point2f ao = a - mean;
                Point2f bo = b - mean;
                double angA = atan2(ao.y, ao.x);
                double angB = atan2(bo.y, bo.x);
                
                return angA < angB;
        };
        
        sort(pts.begin(), pts.end(), comp);
}

//Given a frame, a set of src points and a frame size, gives the perspective transformed frame
Mat getROI(Mat f, vector<Point2f>& src, Point2f size)
{
        //Generate set of dst points
        vector<Point2f> dst(4, size);
        dst[0].x=0; dst[0].y=0;
        dst[1].x=0;
        dst[2].y=0;
        
        //Order set of points
        orderPointsClockwise(src);
        orderPointsClockwise(dst);
        
        //Get Perspective Matrix
        Mat transform= getPerspectiveTransform(src, dst);
        
        //Perform transformation
        Mat r;
        warpPerspective(f, r, transform, Size(size.x,size.y));
        return r;
}

int main()
{
        //Load configuration file and parameters
        FileStorage fs("data.xml", FileStorage::READ);
        string url;
        vector<Point2f> pts;
        fs["url"] >> url;
        fs["pts"] >> pts;

        //Capture
        VideoCapture vc(url);
                
        //Current frame and bgnd
        Mat cur, trans;
                     
        char c = 'a';     
        while(c != 'e')
        {
                //Get new Frame
                vc >> cur;
                //cvtColor(cur, cur, CV_RGB2GRAY);               
                trans = getROI(cur, pts, Point2f(200,600));
                                                                           
                //Show result
                imshow("Current frame", cur);
                imshow("Transform", trans);
                c = waitKey(100);
        }
}
