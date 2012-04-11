#pragma once
#include "ofMain.h"
#include "ofxOpenCv.h"
#include "opencv/cv.h"
#include "opencv/cvaux.h"
#include "opencv/highgui.h"


#define ROI_WIDTH       200
#define ROI_HEIGHT      200
#define ROI_CENTER      100
#define RESOLUTION_X    1920
#define RESOLUTION_Y    1080
#define MAX_STEP        50

class testApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
        void nextVideo();
        void previousVideo();
		void drawScene(int iCameraDraw);
	
		void keyPressed  (int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
    
        void resetVideo(int x, int y);
        void writeVideo();
        void loadVideo(int index);
        void resetROI(ofRectangle ROI_);

	
		//viewports
		ofRectangle				viewMain;
    
        // ofxOpenCv
    
        ofVideoPlayer           *vidPlayer;
        ofxCvColorImage			colorImg;
        ofxCvGrayscaleImage     grayColorImg;
    
        ofxCvColorImage         openCVImage;
        ofxCvColorImage         background;
        ofxCvGrayscaleImage     foreground;
    
        ofxCvColorImage			prevImg;
        ofxCvGrayscaleImage     grayPrevImg;
    
        ofxCvGrayscaleImage		grayResultImg;
        ofxCvGrayscaleImage		grayResultImgNoRoi;
    
        ofxCvContourFinder      contourFinder;
    
        ofxCvBlob               theAnt;
    
        CvBGStatModel*          bgModel;
        CvGaussBGStatModelParams* params;
    
        int                     vidWidth; 
        int                     vidHeight;
        int                     nmbContours;
    
        int                     neg_x_offset;
        int                     neg_y_offset;
    
        // For output
        string                  results;
        string                  summary;
        string                  trajectory;
        string                  name;
        vector<ofFile>          files;
        int                     videoIndex;
    
        ofRectangle             selection;
        ofRectangle             ROI;
    
        float                   videoDuration;
        float                   videoPosition;
        bool                    play;
    
        int                     xCenter;
        int                     yCenter;
        int                     scale;
        vector<ofxCvBlob>       points;
        vector<ofVec4f>         path;
        vector<ofVec4f>::iterator selectedPoint;
        bool                    bSelected;
    
        bool                    haveToSelect;
        int                     remaining;
        int                     startIndex;
        float                   threshold;
        int                     brightness;
        int                     contrast;
    
};