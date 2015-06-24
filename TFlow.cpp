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

void TFlow::getCarsFG(Mat fg, Mat ROI, double time)
{       
        //Convert matrix to 8U with 1C
        Mat fg2;
        fg.convertTo(fg2, CV_8UC1);
                        
        //Get contours
        vector<contour> contours;
        findContours(fg2, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
        
        //Remove contours with area less than 1% of ROI area (false positives)
        auto filter = [&](contour r) { return contourArea(r)<0.01*ROISize.area(); };
        vector<contour>::iterator end = remove_if(contours.begin(), contours.end(), filter);
        contours.erase(end, contours.end());        
               
        //Get corresponding rectangles and create Cars object for each one
        for (contour& c : contours)
        {
                Rect r = boundingRect(c);
                fgDetected.push_back(Car(r, ROI, time));                
        }                
}

void TFlow::updateCars(Mat ROI, bool DEBUG)
{       
        //For each car in the FG, try to match to a previous, existing car. If it matches, swap cars and delete the oldest one.
        list<Car>::iterator itFG, itC;
        
        for (itFG = fgDetected.begin(); itFG != fgDetected.end(); )
        {
                
                bool foundMatch = false;                
                for (itC = cars.begin(); itC != cars.end(); itC++)                        
                {
                
                        //DEBUG
                        if (DEBUG)
                        {
                        Mat t;
                        ROI.copyTo(t);
                        itC->plot(t,'b');
                        itFG->plot(t,'g');
                        imshow("comparing", t);
                        waitKey();
                        }
                        
                        //If there's a match, swap Cars and delete the oldest one
                        if (itFG->match(*itC))
                        {
                                cout << "match found" << endl;
                                iter_swap(itC, itFG);       
                                itFG = fgDetected.erase(itFG);             
                                foundMatch = true;
                                break;                    
                        }                                                                       
                        
                }
                
                if (!foundMatch)
                        itFG++;                
                        
        }
        
        //Remove cars that are not in the scene anymore
        auto onScene = [&](Car c) {return !c.onScene(ROI);};
        itC = remove_if(cars.begin(), cars.end(), onScene);
        cars.erase(itC, cars.end());

        //New FG Cars that did not match old ones are included (should improve with some tests)
        cars.insert(cars.end(), fgDetected.begin(), fgDetected.end());
        
}

void TFlow::play()
{
        BackgroundSubtractorMOG2 bs;        
        bs.set("backgroundRatio", 0.8); 
        bs.set("nShadowDetection", 0);       
                
        Mat f, roi, fg;
        double t;
        
        bool debug = false;
        
        //Record video object
        VideoWriter makeVideo;
        
        char c = 'c';        
        while(c != 'e')
        {
                //Get frame and time
                vc >> f;
                f.convertTo(f, CV_8UC3);
                t = vc.get(CV_CAP_PROP_POS_MSEC);
                                
                //Get ROI and do BackgroundSubtraction to obtain Foreground MASK
                roi = getROI(f);
                bs(roi, fg, 0.01);


                //Detect Foreground cars
                fgDetected.clear();
                getCarsFG(fg, roi, t);
                updateCars(roi, debug);               
                                
                //Debug
                for (Car& c : cars)
                        c.plot(roi,'b');
                        
                cout << t << endl;                              
                imshow("Video", f);
                imshow("ROI", roi);
                imshow("Foreground", fg);
                
                //Record video
                cvtColor(fg, fg, CV_GRAY2BGR);
                Mat record;
                vconcat(roi, fg, record); 
                
                if (!makeVideo.isOpened())
                        makeVideo.open("./res.avi", CV_FOURCC('M', 'J', 'P', 'G'), vc.get(CV_CAP_PROP_FPS), record.size(), true);
                        
                makeVideo << record;

                        
                c = waitKey(100);
        }
        
        makeVideo.release();
        
}


