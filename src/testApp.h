#pragma once
#include "ofMain.h"
#include "ofxOpenCv.h"

#define ROI_WIDTH       200
#define ROI_HEIGHT      200
#define ROI_CENTER      100
#define RESOLUTION_X    1920
#define RESOLUTION_Y    1080

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
    
        ofxCvColorImage			prevImg;
        ofxCvGrayscaleImage     grayPrevImg;
    
        ofxCvGrayscaleImage		grayResultImg;
        ofxCvGrayscaleImage		grayResultImgNoRoi;
    
        ofxCvContourFinder      contourFinder;
    
        ofxCvBlob               theAnt;
    
    
        int                     vidWidth; 
        int                     vidHeight;
        int                     nmbContours;
    
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
    
        int                     xCenter;
        int                     yCenter;
        int                     scale;
        vector<ofxCvBlob>       points;
        vector<ofPoint>         path;
    
        bool                    haveToSelect;
        int                     remaining;
        int                     startIndex;
        float                     threshold;
        int                     brightness;
        int                     contrast;
    
};