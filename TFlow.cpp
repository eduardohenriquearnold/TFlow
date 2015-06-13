#include "TFlow.hpp"

//Helper function to genConfig
void WCallBack(int event, int x, int y, int flags, void* param)
{
        static int pointID = -1;
        vector<Point2f>& v = *(vector<Point2f>*) param;
        
        //On Double click, select point to move
        if  (event == EVENT_LBUTTONDBLCLK)
        {
                Point2f t(x, y);
                
                //Find closest point to t
                int c=0;
                int d=norm(t-v[0]);
                for (int i=1; i< v.size(); i++)
                {
                        int dt = norm(t-v[i]);
                        if (dt < d)
                        {
                                c = i;
                                d = dt;
                        }
                }
                
                pointID = c;       
        }
        
        //If a point was selected move the point with the movement of the mouse
        if  (event == EVENT_MOUSEMOVE && pointID != -1)
        {
                v[pointID].x = x;
                v[pointID].y = y;
        }
        
        //On click with left, point is fixed
        if (event == EVENT_LBUTTONDOWN)
                pointID= -1;
}

void TFlow::genConfig(string cFile)
{
        cout << "TFlow Configuration Process" << endl;
        cout << "Type the stream url or the video file path." << endl;
        string url;
        getline(cin, url);
        
        int w,h;
        cout << "Type the size of the Region of Interest. width, height." << endl;
        cin >> w >> h;
        ROISize.width = w;
        ROISize.height = h;       
        
        cout << "An image of the video will appear, please drag the points to adjust the Region of Interest (ROI). When done, press Q to quit. To begin, press ENTER." << endl;
        cin.get();

        //Open Stream and get 10th frame
        vc.open(url);
        Mat frame;
        for (int i=0; i<10; i++)
                vc >> frame;
                 
        //Create points in ROI
        vector<Point2f> roi(4, Point2f(frame.size()));
        roi[0] *= 1./4;
        roi[1] *= 1./4;
        roi[2] *= 3./4;
        roi[3] *= 3./4;
        roi[1].x = roi[2].x;
        roi[2].x = roi[0].x; 
        orderPointsClockwise(roi);
        
        //Create window and set mouse event
        namedWindow("Video", 1);
        setMouseCallback("Video", WCallBack, &roi);          
                
        //Let user customize points
        Mat dr, preview;
        char c = 'c';        
        while(c != 'q')
        {
                frame.copyTo(dr);                
                ROITransform = getTransMatrix(roi, ROISize);
                
                //Show preview of ROI
                preview = getROI(dr);
                
                //Draw already selected points
                for (int i=0; i<roi.size(); i++)
                {
                        circle(dr, roi[i], 3, Scalar(255,0,0), -1);
                        
                        if (i+1 < roi.size())
                                line(dr, roi[i], roi[i+1], Scalar(0,255,0));
                        else
                                line(dr, roi[i], roi[0], Scalar(0,255,0));
                }
                
                imshow("Video", dr);
                imshow("Preview ROI", preview);
                c = waitKey(10);
        }
        
        //Save config
        FileStorage fs(cFile, FileStorage::WRITE);
        fs << "url" << url;
        fs << "ROISize" << ROISize;
        fs << "ROITransform" << ROITransform;
        fs.release();
        
        //Finish
        cout << "Configuration file created successfully." << endl;
        exit(0);
}

void TFlow::loadConfig(string cFile)
{
        //Check for configuration file
        ifstream confFile(cFile);        
        if (!confFile.good())
        {
                cout << "Configuration File not found. Configure the system before using." << endl;  
                return;
        }
                                
        //Load configuration file and parameters
        FileStorage fs(cFile, FileStorage::READ);
        string url;
        fs["url"] >> url;
        fs["ROISize"] >> ROISize;
        fs["ROITransform"] >> ROITransform;
        fs.release();
        vc.open(url);
}

//Given an array of points, sort them clockwise
void TFlow::orderPointsClockwise(vector<Point2f>& pts)
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

//Given a frame, a set of src points and a frame size, gives the perspective transformation Matrix
Mat TFlow::getTransMatrix(vector<Point2f>& src1, Size s)
{
        //Generate copy of src vector to avoid loosing references (Wcallback)
        vector<Point2f> src(src1);
        
        //Generate set of dst points
        vector<Point2f> dst(4, Point2f(s));
        dst[0].x=0; dst[0].y=0;
        dst[1].x=0;
        dst[2].y=0;
        
        //Order set of points
        orderPointsClockwise(src);
        orderPointsClockwise(dst);
        
        //Get Perspective Matrix
        Mat transform= getPerspectiveTransform(src, dst);
        return transform;        
}

Mat TFlow::getROI(Mat f)
{
        //Perform transformation
        Mat r;
        warpPerspective(f, r, ROITransform, Size(ROISize.width,ROISize.height));
        return r;
}

void TFlow::blobs(Mat f, Mat& ROI)
{
        Mat m;
        f.convertTo(m, CV_8UC1);
                        
        //Get contours
        vector<contour> contours;
        findContours(m, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
        
        //Remove contours with area less than 1% of ROI area (false positives)
        auto filter = [&](contour r) { return contourArea(r)<0.01*ROISize.area(); };
        vector<contour>::iterator end = remove_if(contours.begin(), contours.end(), filter);
        contours.erase(end, contours.end());        
               
        //Get corresponding rectangles and draw them
        for (int i=0; i< contours.size(); i++)
        {
                Rect r = boundingRect(contours[i]);
                rectangle(ROI, r, Scalar(255,0,0));
        }
        
        //imshow("ROI", ROI);
        
}

void TFlow::play()
{
        BackgroundSubtractorMOG2 bs;        
        bs.set("backgroundRatio", 0.8); 
        bs.set("nShadowDetection", 0);       
                
        Mat f, roi, fg;
        
        char c = 'c';        
        while(c != 'e')
        {
                vc >> f;
                roi = getROI(f);
                bs(roi, fg, 0.01);
                
                blobs(fg, roi);
                
                imshow("Video", f);
                imshow("ROI", roi);
                imshow("Foreground", fg);
                c = waitKey(100);
        }
}
