/*
The MIT License (MIT)

Copyright (c) 2014, Adam - Bruce Lane - Piotr - All rights reserved.
This code is intended for use with the Cinder C++ library: http://libcinder.org

This file is part of AdamBrucePiotr.

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
#include "cinder/gl/Shader.h"
#include "cinder/gl/Texture.h"
#include "cinder/gl/Batch.h"
#include "cinder/gl/VboMesh.h"
#include "cinder/ObjLoader.h"
#include "cinder/ImageIo.h"
#include "cinder/Utilities.h"
// parameters
#include "ParameterBag.h"
// OSC
#include "OSCWrapper.h"
#include "UIController.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace ABP;

// brick
struct brick {
	int shape;
	float size;
	float r;
	float g;
	float b;
	float a;
	float motionVector;
	float rotation;
	float repetition;
};
const float DRAW_SCALE = 200;
const pair<float, float> CAMERA_Y_RANGE(32, 80);


class ABPApp : public AppNative {
public:
	void						prepareSettings(Settings *settings);
	void						setup();
	void						resize();
	void						update();
	void						draw();
	void						updateWindowTitle();
	// mouse events
	void						mouseMove(MouseEvent event);
	void						mouseDown(MouseEvent event);
	void						mouseDrag(MouseEvent event);
	void						mouseUp(MouseEvent event);

	void						setShape(const int &aShape, const bool &pressed) { mShape = aShape; }
	void						setRepetitions(const int &aRepetition, const bool &pressed);
	void						lockZ(const bool &pressed) { mLockZ = pressed; }
	void						lockRotation(const bool &pressed) { mLockRotation = pressed; }
	void						lockSize(const bool &pressed) { mLockSize = pressed; }
	void						lockMotionVector(const bool &pressed) { mLockMotionVector = pressed; }
	MinimalUI::UIControllerRef	mParams;
	MinimalUI::UIElementRef		sliderXY, sliderRed, sliderGreen, sliderBlue, sliderAlpha;
	bool						mLockFR;
	void						lockFR(const bool &pressed) { mLockFR = pressed; };
	void						sendOSC(const bool &pressed) { mSendOSC = pressed; };
	void						addBrick(const bool &pressed);
	CameraPersp					mCam;
	gl::BatchRef				mBatch;
	gl::TextureRef				mTexture;
	gl::GlslProgRef				mGlsl;
	gl::VboRef					mInstanceDataVbo;
	void						createPositions();
private:
	// parameters
	ParameterBagRef				mParameterBag;
	// osc
	OSCRef						mOSC;
	bool						mSendOSC;
	float						mR, mG, mB, mA;
	float						mZoom;
	vec2						mXYSize;
	int							mRepetition;
	int							mShape;
	float						mZPosition;
	float						mRotation;
	bool						mLockZ;
	bool						mLockRotation;
	bool						mLockSize;
	bool						mLockMotionVector;
	float						mSize;
	float						mMotionVector;

	int							mMouseIndex;
	bool						isMouseDown;
	// recording
	bool						isRecording;
	int							timer;
	int							presentIndex;
	void						record(const bool &pressed);
	// neRenderer
	float						x, y;
	float						r, g, b, a;
	float						scale;
	ColorA						kolor;
	int							repetitions;
	int							shape;
	float						rotation;
	float						motion;
	float						steps;
	float						size;
	float						distance;
	gl::FboRef					myFbo;
	void						newRendering();
	void						updateBricks(int timer);
	vector <brick>				bricks;
	bool						newRecording;

};
