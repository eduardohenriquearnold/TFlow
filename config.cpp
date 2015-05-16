#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/background_segm.hpp>
#include <iostream>

using namespace cv;
using namespace std;

void WCallBack(int event, int x, int y, int flags, void* param)
{
        vector<Point2f>* v = (vector<Point2f>*) param;
        
        if  (event == EVENT_LBUTTONDOWN)
                v->push_back(Point2f(x,y));
}

void genConfig()
{
        string url;
        vector<Point2f> pts;
        
        cout << "TFlow Configuration Process" << endl;
        cout << "Type the stream url or the video file path." << endl;
        getline(cin, url);
        
        cout << "An image of the video will appear, please select the four points of quadrilateral delimiting detection zone." << endl;
        cin.get();
        
        VideoCapture vc(url);
        Mat frame;
        for (int i; i<10; i++)
                vc >> frame;


        namedWindow("Video", 1);
        setMouseCallback("Video", WCallBack, &pts);
        
        while(pts.size()<4)
        {
                //Draw already selected points
                for (int i=0; i<pts.size(); i++)
                        circle(frame, pts[i], 3, Scalar(255,0,0), -1);
                
                imshow("Video", frame);
                waitKey(10);
        }
        
        //Save config
        FileStorage fs("data.xml", FileStorage::WRITE);
        fs << "url" << url;
        fs << "pts" << pts;
        fs.release();
        
        //Finish
        cout << "Configuration file created successfully." << endl;
        exit(0);
        
}

int main()
{
        genConfig();
}
