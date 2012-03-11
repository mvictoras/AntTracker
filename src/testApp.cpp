#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	
	ofSetVerticalSync(true);
	ofBackground(0, 0, 0);
	ofEnableSmoothing();
	glEnable(GL_DEPTH_TEST);
	
    haveToSelect = false;

    ofDirectory dir("./videos/");
    dir.allowExt("avi");
    dir.sort();
    //populate the directory object
    dir.listDir();
    files = dir.getFiles();
    
    startIndex = 0;
    for( int i = 0; i < files.size(); i ++ ) {
        string fileName = files[i].getBaseName();
        fileName = fileName.substr(0, 5);
        
        string logFile = "experiments/" + fileName + "-summary.txt";
        
        cout << "Checking: " << logFile << endl;
        if( !ofFile(logFile).exists() ) {
            startIndex = i;
            break;
        }
    }
    
    threshold = 30;
    
    remaining = files.size() - startIndex;
    
    loadVideo(startIndex);
    
    scale = 1;
    brightness = 1;
    contrast = 1;
    
    colorImg.allocate(RESOLUTION_X,RESOLUTION_Y);
    prevImg.allocate(RESOLUTION_X,RESOLUTION_Y);
    
    grayColorImg.allocate(RESOLUTION_X,RESOLUTION_Y);
    grayPrevImg.allocate(RESOLUTION_X,RESOLUTION_Y);
    grayResultImg.allocate(RESOLUTION_X,RESOLUTION_Y);
    grayResultImgNoRoi.allocate(RESOLUTION_X,RESOLUTION_Y);
    
    resetVideo(960, 540);
}


//--------------------------------------------------------------
void testApp::update() 
{
    vidPlayer->idleMovie();
    bool bNewFrame = vidPlayer->isFrameNew();
    
    if ( bNewFrame )
    {
        videoPosition = vidPlayer->getPosition();
        
        prevImg = colorImg;
        colorImg.setFromPixels(vidPlayer->getPixels(), vidWidth, vidHeight);
        
        grayColorImg = colorImg;
        grayPrevImg = prevImg;
        
        //threshold = 30;
        grayResultImg.absDiff(grayPrevImg, grayColorImg);
        //grayResultImg.brightnessContrast(brightness, contrast);
        //grayResultImg.contrastStretch();
        grayResultImg.threshold(threshold);
        grayResultImg.blurGaussian();
        grayResultImg.dilate();
        grayResultImg.erode();
        grayResultImg.erode();
        grayResultImgNoRoi = grayResultImg;
        //grayResultImg.dilate();
                    
        contourFinder.findContours(grayResultImg, 5, 200, 20, true, true);
        
        nmbContours = contourFinder.nBlobs;
        if( nmbContours == 0 ) {
            threshold = (threshold < 50) ? threshold + 0.1 : threshold;
        }
        if( nmbContours == 1 ) {
            
            ofxCvBlob theBlob = contourFinder.blobs.at(0);
            threshold = (threshold > 15) ? threshold - 0.01 : threshold;
            if( theBlob.centroid.x < ROI_CENTER && ROI.x - ROI_CENTER + theBlob.centroid.x >= 0) ROI.x -= (ROI_CENTER - theBlob.centroid.x);
            else if (ROI.x + theBlob.centroid.x - ROI_CENTER <= RESOLUTION_X) {
                ROI.x += (theBlob.centroid.x - ROI_CENTER);
            }
            
            if( theBlob.centroid.y < ROI_CENTER && ROI.y - ROI_CENTER + theBlob.centroid.y) ROI.y -= (ROI_CENTER - theBlob.centroid.y);
            else if ( ROI.y + theBlob.centroid.y - ROI_CENTER <= RESOLUTION_Y) {
                ROI.y += (theBlob.centroid.y - ROI_CENTER);
            }
            
            
            if( path.size() > 3 ) {
                ofPoint prevPoint = path.at( path.size() - 1 );
                if( (prevPoint.x > ROI.x - 50 + ROI_CENTER && prevPoint.x < ROI.x + 50 + ROI_CENTER) &&
                    (prevPoint.y > ROI.y - 50 + ROI_CENTER && prevPoint.y < ROI.y + 50 + ROI_CENTER) ) {
                    //cout << results << endl;
                        results += ofToString(path.size()) + "\t20\t255\t" + ofToString(ROI.x + ROI_CENTER) + "\t" + ofToString(RESOLUTION_Y - (ROI.y + ROI_CENTER)) + "\t" + ofToString(ROI.x + ROI_CENTER) + "\t" + ofToString(RESOLUTION_Y - (ROI.y + ROI_CENTER)) + "\n";
                        summary += name + "-" + ofToString(path.size()) + ".jpeg\t1\t171.000\t24.429\t0.0\t255\n";
                        trajectory += ofToString(path.size()) + "\t" + ofToString(ROI.x + ROI_CENTER) + "\t" + ofToString(RESOLUTION_Y - (ROI.y + ROI_CENTER)) + "\n";
                        
                        path.push_back( ofPoint(ROI.x + ROI_CENTER, ROI.y + ROI_CENTER));
                }
                else { // Reverse the operation
                    if( theBlob.centroid.x < ROI_CENTER ) ROI.x += (ROI_CENTER - theBlob.centroid.x);
                    else ROI.x -= (theBlob.centroid.x - ROI_CENTER);
                    
                    if( theBlob.centroid.y < ROI_CENTER ) ROI.y += (ROI_CENTER - theBlob.centroid.y);
                    else ROI.y -= (theBlob.centroid.y - ROI_CENTER);
                }
            }
            else {
            
                results += ofToString(path.size()) + "\t20\t255\t" + ofToString(ROI.x + ROI_CENTER) + "\t" + ofToString(RESOLUTION_Y - (ROI.y + ROI_CENTER)) + "\t" + ofToString(ROI.x + ROI_CENTER) + "\t" + ofToString(RESOLUTION_Y - (ROI.y + ROI_CENTER)) + "\n";
                summary += name + "-" + ofToString(path.size()) + ".jpeg\t1\t171.000\t24.429\t0.0\t255\n";
                trajectory += ofToString(path.size()) + "\t" + ofToString(ROI.x + ROI_CENTER) + "\t" + ofToString(RESOLUTION_Y - (ROI.y + ROI_CENTER)) + "\n";
                
                path.push_back( ofPoint(ROI.x + ROI_CENTER, ROI.y + ROI_CENTER));
            }
            
            resetROI(ROI);
        }
        else if( nmbContours > 2 ) {
            threshold = (threshold > 15) ? threshold - 0.1 : threshold;
            points = contourFinder.blobs;
            bool bSuccess = false;
            
            if( path.size() > 3 ) {
                ofPoint prevPoint = path.at( path.size() - 1 );
                bool bSuccess = false;

                int i = 0;
                while( i < contourFinder.blobs.size() && !bSuccess ) {
                    
                    ofxCvBlob theBlob = contourFinder.blobs.at(i);
                    
                    if( theBlob.centroid.x < ROI_CENTER ) ROI.x -= (ROI_CENTER - theBlob.centroid.x);
                    else ROI.x += (theBlob.centroid.x - ROI_CENTER);
                    
                    if( theBlob.centroid.y < ROI_CENTER ) ROI.y -= (ROI_CENTER - theBlob.centroid.y);
                    else ROI.y += (theBlob.centroid.y - ROI_CENTER);
                    
                    if( (prevPoint.x > ROI.x - 15 + ROI_CENTER && prevPoint.x < ROI.x + 15 + ROI_CENTER) &&
                       (prevPoint.y > ROI.y - 15 + ROI_CENTER && prevPoint.y < ROI.y + 15 + ROI_CENTER) ) {
                        results += ofToString(path.size()) + "\t20\t255\t" + ofToString(ROI.x + ROI_CENTER) + "\t" + ofToString(RESOLUTION_Y - (ROI.y + ROI_CENTER)) + "\t" + ofToString(ROI.x + ROI_CENTER) + "\t" + ofToString(RESOLUTION_Y - (ROI.y + ROI_CENTER)) + "\n";
                        summary += name + "-" + ofToString(path.size()) + ".jpeg\t1\t171.000\t24.429\t0.0\t255\n";
                        trajectory += ofToString(path.size()) + "\t" + ofToString(ROI.x + ROI_CENTER) + "\t" + ofToString(RESOLUTION_Y - (ROI.y + ROI_CENTER)) + "\n";
                        
                        path.push_back( ofPoint(ROI.x + ROI_CENTER, ROI.y + ROI_CENTER));
                        bSuccess = true;
                    }
                    else { // Reverse the operation
                        if( theBlob.centroid.x < ROI_CENTER ) ROI.x += (ROI_CENTER - theBlob.centroid.x);
                        else ROI.x -= (theBlob.centroid.x - ROI_CENTER);
                        
                        if( theBlob.centroid.y < ROI_CENTER ) ROI.y += (ROI_CENTER - theBlob.centroid.y);
                        else ROI.y -= (theBlob.centroid.y - ROI_CENTER);
                    }
                    i++;
                } 
            }
            
            if( !bSuccess ) {
                vidPlayer->stop();
                haveToSelect = true;
            }
        }
        
        
        
        if( vidPlayer->getCurrentFrame() == vidPlayer->getTotalNumFrames() ) {
            writeVideo();
        }
    }
}


//--------------------------------------------------------------
void testApp::draw(){
	
    ofClear(0, 0, 0);
    //resultImg.draw(0,0, 720, 900);
    ofDrawBitmapString("AntTracker", 10, 10);
    ofDrawBitmapString("Threshold: " + ofToString(threshold), 10, 25);
    ofDrawBitmapString("Resolution: " + ofToString(vidWidth) + "x" + ofToString(vidHeight), 150, 10);
    ofDrawBitmapString("Number of contours: " + ofToString(nmbContours), 350, 10);
    ofDrawBitmapString("File: " + name, 600, 10);
    ofDrawBitmapString(ofToString(videoPosition * 100) + " of " + ofToString(videoDuration) + "sec", 800, 10);
    
    // Commands
    ofDrawBitmapString("Commands",                   RESOLUTION_X / scale - 200, 25);
    ofDrawBitmapString("p        - Pause",           RESOLUTION_X / scale - 200, 35);
    ofDrawBitmapString("r        - Resume",          RESOLUTION_X / scale - 200, 45);
    ofDrawBitmapString("f        - Save file",       RESOLUTION_X / scale - 200, 55);
    ofDrawBitmapString("s        - Skip frame",      RESOLUTION_X / scale - 200, 65);
    ofDrawBitmapString(",        - Prev video",      RESOLUTION_X / scale - 200, 75);
    ofDrawBitmapString(".        - Next Video",      RESOLUTION_X / scale - 200, 85);
    ofDrawBitmapString("1        - Res: 1920x1080",  RESOLUTION_X / scale - 200, 95);
    ofDrawBitmapString("2        - Res: 960x540",    RESOLUTION_X / scale - 200, 105);
    ofDrawBitmapString("SpaceBar - Reset ",          RESOLUTION_X / scale - 200, 115);
    ofDrawBitmapString("Click    - Select Blob",     RESOLUTION_X / scale - 200, 125);
    
    ofDrawBitmapString("Remaining: " + ofToString(remaining),     RESOLUTION_X / scale - 200, RESOLUTION_Y / 2 - 30);

    ofSetColor(255, 50, 50);
    
    ofPushMatrix();
    {
    
        ofTranslate(ROI.x / scale, ROI.y / scale);    
        ofScale( 1.0f / (float) scale, 1.0f / (float) scale, 1.0f);
        
        if( haveToSelect ) {
            for( int i =0; i < points.size(); i++ ) {

                points.at(i).draw();
            }
        }
    }
    ofPopMatrix();
    
    ofSetColor(160, 32, 240); // Purple
    ofBeginShape();
    
    for(int i = 0; i < path.size(); i++)
    {
        ofVertex(path[i].x /scale, path[i].y / scale);
    }
    ofEndShape();
    
    ofSetColor(255, 255, 255);
    ofNoFill();
    ofRect(selection);
    //ofRect(ROI.x / 2, ROI.y / 2, ROI.width / 2, ROI.height / 2);
    ofRect(ROI.x / scale, ROI.y / scale, ROI.width / scale, ROI.height / scale);
    
    //grayResultImg.resetROI();
    //grayResultImg.draw(RESOLUTION_X /scale, 0, RESOLUTION_X /scale, RESOLUTION_Y / scale);
    //resetROI(ROI);
    colorImg.draw(0, 0, RESOLUTION_X /scale, RESOLUTION_Y / scale);
    
    ofSetColor(255, 255, 255);
}

void testApp::resetVideo(int x, int y) {
    results = " \tArea\tMean\tX\tY\tXM\tYM\n";
    summary = " \tArea\tMean\tX\tY\tXM\tYM\n";
    trajectory = "Frame\tX\tY\n";
    xCenter = x;
    yCenter = y;
    
    ROI.x = xCenter - ROI_WIDTH / 2;
    ROI.y = yCenter - ROI_HEIGHT / 2;
    ROI.width = ROI_WIDTH;
    ROI.height = ROI_HEIGHT;
    
    resetROI(ROI);
}


void testApp::resetROI(ofRectangle ROI_) {
    colorImg.resetROI();
    prevImg.resetROI();
    grayColorImg.resetROI();
    grayPrevImg.resetROI();
    grayResultImg.resetROI();
    
    colorImg.setROI(ROI_);
    prevImg.setROI(ROI_);
    grayColorImg.setROI(ROI_);
    grayPrevImg.setROI(ROI_);
    grayResultImg.setROI(ROI_);
    
}

void testApp::loadVideo(int index) {
    videoIndex = index;
    vidPlayer = new ofVideoPlayer();
    vidPlayer->loadMovie(files[videoIndex].getAbsolutePath());
    vidPlayer->setLoopState(OF_LOOP_NONE);
    vidPlayer->play();
    
    videoDuration = vidPlayer->getDuration();
    videoPosition = vidPlayer->getPosition();
    
    vidWidth = vidPlayer->getWidth();
    vidHeight = vidPlayer->getHeight();
    
    name = files[videoIndex].getBaseName();
    name = name.substr(0, 5);
    
}

void testApp::writeVideo() {
    ofBuffer bResults;
    ofBuffer bSummary;
    ofBuffer bTrajectory;
    
    bResults.set(results.c_str(), results.size());
    bSummary.set(summary.c_str(), summary.size());
    bTrajectory.set(trajectory.c_str(), trajectory.size());
    
    ofBufferToFile("experiments/" + name + "-particles.txt", bResults);
    ofBufferToFile("experiments/" + name + "-summary.txt", bSummary);
    ofBufferToFile("ants/"        + name + "-ant.txt", bTrajectory);
    nextVideo();
}

//--------------------------------------------------------------
void testApp::previousVideo()
{    
    if ( videoIndex > 0 ) {
        cout << videoIndex << endl;
        videoIndex--;
        
        vidPlayer->stop();
        vidPlayer->closeMovie();
        delete vidPlayer;
        
        loadVideo(videoIndex);        
        path.clear();
        threshold = 30;
        remaining++;
        resetVideo(xCenter, yCenter);
    }
}

void testApp::nextVideo()
{   
    if ( videoIndex < files.size() ) {
        cout << videoIndex << endl;
        videoIndex++;
    
        vidPlayer->stop();
        vidPlayer->closeMovie();
        delete vidPlayer;
        
        loadVideo(videoIndex);
        
        path.clear();
        threshold = 30;
        remaining--;
        resetVideo(xCenter, yCenter);  
    }
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
    //nextVideo();
    
    if( key == '1' ) {
        scale = 1;
        ofSetWindowShape(RESOLUTION_X, RESOLUTION_Y);
        
    }
    else if( key == '2' ) {
        scale = 2;
        ofSetWindowShape(RESOLUTION_X / scale, RESOLUTION_Y / scale);
        
    }
    else if( key == ' ' ) {
        
        threshold = 30;
        resetVideo(xCenter, yCenter);
        path.clear();
        
        if( haveToSelect ) {
            vidPlayer->play();
            haveToSelect = false; 
        }
    }
    else if( key == '.' ) {
        nextVideo();
    }
    else if( key == ',' ) {
        previousVideo();
    }
    else if( key == 'p' ) {
        vidPlayer->stop();
    }
    else if( key == 'r' ) {
        vidPlayer->play();
    }
    else if( key == 's' ) {
        if( haveToSelect == true ) {
            vidPlayer->play();
            haveToSelect = false;
        }
    }
    else if( key =='2') {
        vidPlayer->setSpeed(8);
    }
    else if( key == OF_KEY_RIGHT ) {
        if( !vidPlayer->isPlaying() ) {
            vidPlayer->nextFrame();
        }
    }
    else if( key == OF_KEY_LEFT ) {
        if( !vidPlayer->isPlaying() ) {
            vidPlayer->previousFrame();
        }
    }
    else if( key == 'f' ) {
        writeVideo();
    }
    else if( key == 'w' ) {
        brightness--;
        
    }
    else if( key == 'e' ) {
        brightness++;
    }
    else if( key == 's' ) {
        contrast--;
    }
    else if( key == 'd' ) {
        contrast++;
    }
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
    x *= scale;
    y *= scale;
    if( haveToSelect == false ) {
        resetVideo(x, y);
    }
    else {
        for( int i = 0; i < points.size(); i++ ) {
            if( abs( ROI.x + points.at(i).centroid.x - x) < 5 && abs( ROI.y + points.at(i).centroid.y - y) < 5 ) {
                haveToSelect = false;
                
                if( points.at(i).centroid.x < ROI_CENTER ) ROI.x -= (ROI_CENTER - points.at(i).centroid.x);
                else ROI.x += (points.at(i).centroid.x - ROI_CENTER);
                
                if( points.at(i).centroid.y < ROI_CENTER ) ROI.y -= (ROI_CENTER - points.at(i).centroid.y);
                else ROI.y += (points.at(i).centroid.y - ROI_CENTER);
                
                results += ofToString(path.size()) + "\t20\t255\t" + ofToString(ROI.x + ROI_CENTER) + "\t" + ofToString(RESOLUTION_Y - (ROI.y + ROI_CENTER)) + "\t" + ofToString(ROI.x + ROI_CENTER) + "\t" + ofToString(RESOLUTION_Y - (ROI.y + ROI_CENTER)) + "\n";
                summary += name + "-" + ofToString(path.size()) + ".jpeg\t1\t171.000\t24.429\t0.0\t255\n";
                trajectory += ofToString(path.size()) + "\t" + ofToString(ROI.x + ROI_CENTER) + "\t" + ofToString(RESOLUTION_Y - (ROI.y + ROI_CENTER)) + "\n";
                
                path.push_back( ofPoint(ROI.x + ROI_CENTER, ROI.y + ROI_CENTER) );
                
                
                resetROI(ROI);
                
                vidPlayer->play();
            }
        }
    }
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){
	
}

