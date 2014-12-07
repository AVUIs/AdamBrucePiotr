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
// parameters
#include "ParameterBag.h"
// OSC
#include "OSCWrapper.h"
#include "UIController.h"
#include "singleton.h"
#include "neRender.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace ABP;

struct TouchPoint {
	TouchPoint() {}
	TouchPoint(const vec2 &initialPt, const Color &color) : mColor(color), mTimeOfDeath(-1.0)
	{
		mLine.push_back(initialPt);
	}

	void addPoint(const vec2 &pt) { mLine.push_back(pt); }

	void draw() const
	{
		if (mTimeOfDeath > 0) // are we dying? then fade out
			gl::color(ColorA(mColor, (mTimeOfDeath - getElapsedSeconds()) / 2.0f));
		else
			gl::color(mColor);

		gl::draw(mLine);
	}

	void startDying() { mTimeOfDeath = getElapsedSeconds() + 2.0f; } // two seconds til dead

	bool isDead() const { return getElapsedSeconds() > mTimeOfDeath; }

	PolyLine<vec2>	mLine;
	Color			mColor;
	float			mTimeOfDeath;
};

class ABPApp : public AppNative {
public:
	void						prepareSettings(Settings *settings);
	void						setup();
	void						update();
	void						draw();
	// mouse events
	void						mouseMove(MouseEvent event);
	void						mouseDown(MouseEvent event);
	void						mouseDrag(MouseEvent event);
	void						mouseUp(MouseEvent event);

	// touch events
	void						touchesBegan(TouchEvent event);
	void						touchesMoved(TouchEvent event);
	void						touchesEnded(TouchEvent event);
	void						setRepetition(const int &aRepeat, const bool &pressed) { mRepetition = aRepeat; }
	void						setShape(const int &aShape, const bool &pressed) { mShape = aShape; }
	void						lockZ(const bool &pressed) { mLockZ = pressed; }
	void						lockRotation(const bool &pressed) { mLockRotation = pressed; }
	void						lockSize(const bool &pressed) { mLockSize = pressed; }
	void						lockMotionVector(const bool &pressed) { mLockMotionVector = pressed; }
	MinimalUI::UIControllerRef	mParams;
	MinimalUI::UIElementRef		sliderXY, sliderRed, sliderGreen, sliderBlue, sliderAlpha;
	bool						mLockFR;
	void						lockFR(const bool &pressed) { mLockFR = pressed; };
private:
	// parameters
	ParameterBagRef				mParameterBag;
	// osc
	OSCRef						mOSC;
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

	CameraPersp					mCamera;
	// touch events
	map<uint32_t, TouchPoint>	mActivePoints;
	list<TouchPoint>			mDyingPoints;
	int							mMouseIndex;
	bool						isMouseDown;
	// recording
	bool						isRecording;
	int							timer;
	int							presentIndex;
	void						record(const bool &pressed);
	neRenderer					neRender;
};
