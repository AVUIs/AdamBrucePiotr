/*
Copyright (c) 2010-2015, Paul Houx - All rights reserved.
This code is intended for use with the Cinder C++ library: http://libcinder.org

This file is part of Cinder-Warping.

Cinder-Warping is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Cinder-Warping is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Cinder-Warping.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <string>
#include <vector>

#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/ImageIo.h"
#include "cinder/Rand.h"

#include "Warp.h"
// Settings
#include "VDSettings.h"
// Session
#include "VDSession.h"
// Utils
#include "VDUtils.h"
// Audio
//#include "VDAudio.h"
// Animation
#include "VDAnimation.h"
// shaders
//#include "VDShaders.h"
// Message router
#include "VDRouter.h"
// fbo
#include "VDFbo.h"
// Textures
#include "VDTexture.h"

using namespace ci;
using namespace ci::app;
using namespace ph::warping;
using namespace std;
using namespace VideoDromm;


#define IM_ARRAYSIZE(_ARR)			((int)(sizeof(_ARR)/sizeof(*_ARR)))
// brick
struct brick {
	int shape;
	float size;
	float r;
	float g;
	float b;
	float a;
	float vx;
	float vy;
	float motionVector;
	float rotation;
};
const pair<float, float> CAMERA_Y_RANGE(32.0f, 80.0f);

class BatchassSpidermoonApp : public App {

public:
	static void					prepare(Settings *settings);

	void						setup() override;
	void						cleanup() override;
	void						update() override;
	void						draw() override;

	void						resize() override;

	void						mouseMove(MouseEvent event) override;
	void						mouseDown(MouseEvent event) override;
	void						mouseDrag(MouseEvent event) override;
	void						mouseUp(MouseEvent event) override;

	void						keyDown(KeyEvent event) override;
	void						keyUp(KeyEvent event) override;

	void						updateWindowTitle();
private:
	// Settings
	VDSettingsRef				mVDSettings;
	// Session
	VDSessionRef				mVDSession;
	// Utils
	VDUtilsRef					mVDUtils;
	// Audio
	//VDAudioRef					mVDAudio;
	// Animation
	VDAnimationRef				mVDAnimation;
	// Shaders
	//VDShadersRef				mVDShaders;
	// Fbos
	vector<VDFboRef>			mVDFbos;
	// Message router
	VDRouterRef					mVDRouter;
	// Textures
	VDTextureList				mTexs;
	fs::path					mTexturesFilepath;
	// shaders
	gl::GlslProgRef				aShader;

	fs::path					mSettings;

	WarpList					mWarps;
	bool						mFadeInDelay;
	bool						mFadeOutDelay;
	gl::TextureRef				mImage;

	// fbo
	void						renderSceneToFbo();
	gl::FboRef					mRenderFbo;
	// abp
	void						setShape(const int &aShape, const bool &pressed) { mShape = aShape; }
	void						addBrick(const bool &pressed);
	CameraPersp					mCam;
	float						mR, mG, mB, mA;
	float						mZoom;
	float						mBend;
	vec2						mXYVector;
	float						mRepetitions;
	int							mShape;
	float						mZPosition;
	float						mRotation;
	bool						mLockZ;
	bool						mLockRepetitions;
	bool						mLockRotation;
	bool						mLockSize;
	bool						mLockMotionVector;
	bool						mLockBend;
	bool						mGlobalMode;

	float						mSize;
	float						mMotionVector;
	// neRenderer
	float						x, y;
	float						r, g, b, a;
	float						scale;
	ColorA						kolor;
	int							shape;
	float						rotation;
	float						motion;
	float						steps;
	float						distance;
	//mat4						mRotationMatrix;
	float						mColorFactor;

	void						newRendering();
	vector <brick>				bricks;
	bool						alreadyCreated;
};
