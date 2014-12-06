/*
The MIT License (MIT)

 Copyright (c) 2014, Nathan Selikoff - Bruce Lane - All rights reserved.
 This code is intended for use with the Cinder C++ library: http://libcinder.org

 This file is part of Cinder-MinimalUI.

 Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "cinder/app/AppNative.h"
#include "cinder/app/RendererGl.h"
#include "cinder/Camera.h"
// parameters
#include "ParameterBag.h"
// OSC
#include "OSCWrapper.h"
#include "UIController.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace ABP;


class ABPApp : public AppNative {
public:
	void setup();
	void update();
	void draw();
    void buttonCallback( const bool &pressed );
    void setCount( const int &aCount, const bool &pressed ) { mCount = aCount; }
    void lockZ( const bool &pressed ) { mLockZ = pressed; }
    MinimalUI::UIControllerRef	mParams;
	MinimalUI::UIElementRef		sliderXY, sliderRed, sliderGreen, sliderBlue, sliderAlpha;
	bool mLockFR;
	void								lockFR(const bool &pressed) { mLockFR = pressed; };
private:
	// parameters
	ParameterBagRef				mParameterBag;
	// osc
	OSCRef						mOSC;
	float mR, mG, mB, mA;
    float mZoom;
    vec2 mXYSize;
    int mCount;
    float mZPosition;
    bool mLockZ;
    
    CameraPersp mCamera;
};

void ABPApp::setup()
{
	// parameters
	mParameterBag = ParameterBag::create();
	
	
	mZoom = 1.0f;
    mXYSize = vec2(1.0);
    mCount = 1;
    mZPosition = 0.0f;
    mLockZ = false;
	mR = mG = mB = mA = 1.0f;
    
    gl::enableDepthRead();
    gl::enableDepthWrite();
    
    mCamera = CameraPersp( getWindowWidth(), getWindowHeight(), 60.0f, 1.0f, 1000.0f );
    mCamera.lookAt(vec3(-2, 2, 2), vec3(0.0));
    
    mParams = MinimalUI::UIController::create();
    
    // 2D Sliders
	sliderXY = mParams->addSlider2D("XY", &mXYSize, "{ \"minX\":-2.0, \"maxX\":2.0, \"minY\":-2.0, \"maxY\":2.0 }");
    // Slider
    mParams->addSlider( "Zoom", &mZoom );
    
	sliderRed = mParams->addToggleSlider("R", &mR, "a", std::bind(&ABPApp::lockFR, this, std::placeholders::_1), "{ \"width\":156, \"clear\":false, \"handleVisible\":false, \"vertical\":true, \"nameColor\":\"0xEEFF0000\" }", "{ \"width\":9, \"stateless\":false, \"group\":\"fr\", \"exclusive\":true }");
	//sliderRed = mParams->addSlider("R", &mR, "{ \"handleVisible\":false, \"nameColor\":\"0xEEFF0000\" }");
	sliderGreen = mParams->addSlider("G", &mG, "{ \"handleVisible\":false, \"nameColor\":\"0xEE00FF00\" }");
	sliderBlue = mParams->addSlider("B", &mB, "{  \"handleVisible\":false, \"nameColor\":\"0xEE0000FF\" }");
	sliderAlpha = mParams->addSlider("A", &mA, "{ \"handleVisible\":false, \"nameColor\":\"0x0xFFFFFFFF\" }");

    // Simple Button
    mParams->addButton( "Stateless!", std::bind( &ABPApp::buttonCallback, this, std::placeholders::_1 ), "{ \"width\":96, \"clear\":false }" );
    mParams->addButton( "Stateful!", std::bind( &ABPApp::buttonCallback, this, std::placeholders::_1 ), "{ \"width\":96, \"stateless\":false }" );
    
    // Separator
    mParams->addSeparator();
    
    // Label
    mParams->addLabel( "Count", "{ \"clear\":false }" );
    
    // Button Group
    mParams->addButton( "1", std::bind( &ABPApp::setCount, this, 1, std::placeholders::_1 ), "{ \"clear\":false, \"stateless\":false, \"group\":\"count\", \"exclusive\":true, \"pressed\":true }" );
    mParams->addButton( "2", std::bind( &ABPApp::setCount, this, 2, std::placeholders::_1 ), "{ \"clear\":false, \"stateless\":false, \"group\":\"count\", \"exclusive\":true }" );
    mParams->addButton( "3", std::bind( &ABPApp::setCount, this, 3, std::placeholders::_1 ), "{ \"stateless\":false, \"group\":\"count\", \"exclusive\":true }" );
    
    // Toggle Slider
    mParams->addToggleSlider( "Z Position", &mZPosition, "A", std::bind(&ABPApp::lockZ, this, std::placeholders::_1 ), "{ \"width\":156, \"clear\":false, \"min\": -1, \"max\": 1 }", "{ \"stateless\":false }" );
}

void ABPApp::update()
{
    mZPosition = mLockZ ? sin( getElapsedFrames() / 100.0f ) : mZPosition;
	/*sliderRed->setBackgroundColor(ColorA(mR, 0, 0));
	sliderGreen->setBackgroundColor(ColorA(0, mG, 0));
	sliderBlue->setBackgroundColor(ColorA(0, 0, mB));
	sliderAlpha->setBackgroundColor(ColorA(mR, mG, mB, mA));*/
    mParams->update();
}

void ABPApp::draw()
{
    gl::clear();
    
    gl::setMatrices( mCamera );
    
    gl::pushModelView();
    gl::scale( vec3(1.0) * mZoom );
	gl::color(ColorA(mR, mG, mB, mA));
    for ( int i = 0; i < mCount; i++ )
    {
        gl::pushModelView();
        gl::translate( i * 1.5f, 0.0f, mZPosition );
		gl::drawCube(vec3(0.0), vec3( mXYSize, 1.0f ));
        gl::popModelView();
    }
    gl::popModelView();
    
    mParams->draw();

}

void ABPApp::buttonCallback( const bool &pressed )
{
    console() << "Hello! Button state: " << pressed << endl;
}

// This line tells Cinder to actually create the application
CINDER_APP_NATIVE( ABPApp, RendererGl )
