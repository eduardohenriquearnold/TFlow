#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/background_segm.hpp>
#include <iostream>

using namespace cv;

//TAKE A LOOK http://experienceopencv.blogspot.com.br/2011/01/background-subtraction-and-models.html

int main()
{
        //Video source
        //VideoCapture vc("http://rtmp.0be3-2.upx.net.br:1935/aovivo/0be3_1173.stream/playlist.m3u8");
        VideoCapture vc("./test.mp4");
        
        BackgroundSubtractorMOG2 bs;
        //bs.set("backgroundRatio", 1.4);
        //bs.set("nmixtures", 10);
        //bs.set("fVarInit", 5);
        bs.setInt("nmixtures" , 3);
bs.setDouble("fVarInit" , 80.0);
bs.setDouble("fTau" , 0.2);
bs.setDouble("fVarMin" , 200.0);
bs.setDouble("fVarMax" , 80.0);
bs.setBool("detectShadows",false);
        
        //Current frame and bgnd
        Mat cur, fgnd, bgnd;
        
             
        char c = 'a';     
        while(c != 'e')
        {
                //Get new Frame
                vc >> cur;
                //cvtColor(cur, cur, CV_RGB2GRAY);
                
                //Update background frame
                bs(cur, fgnd);
                bs.getBackgroundImage(bgnd);
                
                if (fgnd.rows==0 || bgnd.rows==0)
                        continue;
                                                                           
                //Show result
                imshow("Current frame", cur);
                imshow("Foreground", fgnd);
                imshow("Background", bgnd);
                c = waitKey(100);
        }
}
