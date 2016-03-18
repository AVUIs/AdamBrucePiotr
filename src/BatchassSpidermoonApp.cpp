#include "BatchassSpidermoonApp.h"


void BatchassSpidermoonApp::prepare(Settings *settings)
{
	settings->setWindowSize(40, 10);
}

void BatchassSpidermoonApp::setup()
{
	mFadeInDelay = mFadeOutDelay = true;
	// Settings
	mVDSettings = VDSettings::create();
	mVDSettings->mLiveCode = false;
	mVDSettings->mRenderThumbs = false;
	mVDSession = VDSession::create(mVDSettings);
	// Utils
	mVDUtils = VDUtils::create(mVDSettings);
	mVDUtils->getWindowsResolution();
	// Audio
	mVDAudio = VDAudio::create(mVDSettings);
	// Animation
	mVDAnimation = VDAnimation::create(mVDSettings, mVDSession);
	// Message router
	mVDRouter = VDRouter::create(mVDSettings, mVDAnimation, mVDSession);
	// Shaders
	mVDShaders = VDShaders::create(mVDSettings);
	// mix fbo at index 0
	mVDFbos.push_back(VDFbo::create(mVDSettings, "mix", mVDSettings->mFboWidth, mVDSettings->mFboHeight));

	gl::enableDepthWrite();
	gl::enableDepthRead();
	//gl::disableBlending();
	gl::enableAlphaBlending();

	// warping
	updateWindowTitle();

	// initialize warps
	mSettings = getAssetPath("") / mVDSettings->mAssetsPath / "warps.xml";
	if (fs::exists(mSettings)) {
		// load warp settings from file if one exists
		mWarps = Warp::readSettings(loadFile(mSettings));
	}
	else {
		// otherwise create a warp from scratch
		mWarps.push_back(WarpPerspectiveBilinear::create());
		mWarps.push_back(WarpPerspectiveBilinear::create());
	}

	mSrcArea = Area(0, 22, 398, 420);//Area(x1, y1, x2, y2);

	// render fbo
	gl::Fbo::Format fboFormat;
	//format.setSamples( 4 ); // uncomment this to enable 4x antialiasing
	mRenderFbo = gl::Fbo::create(mVDSettings->mFboWidth, mVDSettings->mFboHeight, fboFormat.colorTexture());
	// bpm
	setFrameRate(mVDSession->getTargetFps());
	// abp
	// neRenderer
	x = mVDSettings->mFboWidth / 2;
	y = mVDSettings->mFboHeight / 2;

	mZoom = 0.3f;
	mXYVector = vec2(1.0);
	mRepetitions = 1;
	mShape = 0;
	mZPosition = 0.0f;
	mRotation = 2.5f;
	mSize = 1.0f;
	mMotionVector = 0.2f;
	mBend = 0.1f;
	mLockZ = false;
	mLockRepetitions = false;
	mLockRotation = false;
	mLockSize = false;
	mLockMotionVector = false;
	mLockBend = false;
	mGlobalMode = false;
	mColorFactor = 0.06;
	mR = 0.5f;
	mG = 0.0f;
	mB = 0.8f;
	mA = 1.0f;
	// init one brick
	addBrick(true);
	alreadyCreated = false;
	mCam.lookAt(vec3(0.0f, CAMERA_Y_RANGE.first, 0.0f), vec3(0.0f, 0.0f, 0.0f));

}
void BatchassSpidermoonApp::addBrick(const bool &pressed)
{
	brick newBrick;
	newBrick.r = mR;
	newBrick.g = mG;
	newBrick.b = mB;
	newBrick.a = mA;
	newBrick.vx = mXYVector.x;
	newBrick.vy = mXYVector.y;
	newBrick.shape = mShape;
	newBrick.size = mSize;
	newBrick.rotation = mRotation;
	newBrick.motionVector = mMotionVector;
	bricks.push_back(newBrick);
	mXYVector.x = Rand::randFloat(-2.0, 2.0);
	mXYVector.y = Rand::randFloat(-2.0, 2.0);
}

void BatchassSpidermoonApp::cleanup()
{
	// save warp settings
	// Warp::writeSettings(mWarps, writeFile(mSettings)); TODO put back
	mVDSettings->save();
	mVDSession->save();
}
void BatchassSpidermoonApp::update()
{
	
	mVDAnimation->update();
	mVDRouter->update();
	updateWindowTitle();
	// abp
	mZPosition = mLockZ ? sin(getElapsedFrames() / 100.0f) : mZPosition;
	mRotation = mLockRotation ? sin(getElapsedFrames() / 100.0f)*4.0f : mRotation;
	mSize = mLockSize ? sin(getElapsedFrames() / 100.0f) + 0.7f : mSize;
	mMotionVector = mLockMotionVector ? sin(getElapsedFrames() / 50.0f) : mMotionVector;
	//mRotationMatrix *= rotate(0.06f, normalize(vec3(0.16666f, 0.333333f, 0.666666f)));
	mRepetitions = mLockRepetitions ? (sin(getElapsedFrames() / 100.0f) + 1) * 20 : mRepetitions;
	mBend = mLockBend ? sin(getElapsedFrames() / 100.0f) * 10.0f : mBend;
	if (mVDSettings->iBeat < 64)
	{
		mRepetitions = (mVDSettings->iBeat / 8) + 1;
	}
	else
	{

		if (mVDSettings->iBeat % 8 == 0)
		{
			if (bricks.size() < 20 && !alreadyCreated)
			{
				addBrick(false);
				alreadyCreated = true;
			}

		}
		else
		{
			alreadyCreated = false;
		}
		if (mVDSettings->iBeat % 8 == 0)
		{
			if (mVDSettings->iBeat > 92)
			{
				mGlobalMode = true;
				mR = 1.0f;
				mB = 0.0f;
			}
			if (mVDSettings->iBeat > 280 && mVDSettings->iBeat < 292)
			{
				mShape = 1;
			}
			if (mVDSettings->iBeat > 316)
			{
				mRotation += 0.2;
				if (mRotation > 6.35) mRotation = 0;
			}
		}
		else
		{
			mR = 0.6f;
			mB = 0.9f;
			if (mVDSettings->iBeat > 510 && mVDSettings->iBeat % 2 == 0)
			{
				mRotation += 0.5 + mVDSettings->controlValues[12]; //was 0.2
				if (mRotation > 6.35) mRotation = 0;
			}
		}
	}
	// move the camera up and down on Y
	mCam.lookAt(vec3(0.0f, CAMERA_Y_RANGE.first + abs(sin(getElapsedSeconds() / 4)) * (CAMERA_Y_RANGE.second - CAMERA_Y_RANGE.first), 0.0f), vec3(0.0f, 0.0f, 0.0f));

}

// Render the scene into the FBO
void BatchassSpidermoonApp::renderSceneToFbo()
{
	float new_x;
	float new_y;
	float bendFactor;
	float volumeFactor;
	// this will restore the old framebuffer binding when we leave this function
	// on non-OpenGL ES platforms, you can just call mFbo->unbindFramebuffer() at the end of the function
	// but this will restore the "screen" FBO on OpenGL ES, and does the right thing on both platforms
	gl::ScopedFramebuffer fbScp(mRenderFbo);
	gl::clear(Color::black());
	// setup the viewport to match the dimensions of the FBO
	gl::ScopedViewport scpVp(ivec2(0), mRenderFbo->getSize());
	//gl::color(Color::white());
	mBend = mVDSettings->controlValues[11]*10;

	volumeFactor = 1.0f;// CHECK mVDSettings->controlValues[14];

	CameraPersp cam(640, 480, 60.0f); // hardcoded
	cam.setPerspective(60, 0.75, 1, 1000);//mTextures->getFbo(mVDSettings->mABPFboIndex).getAspectRatio()
	cam.lookAt(vec3(2.8f, 1.8f, -2.8f), vec3(0.0f, 0.0f, 0.0f));
	gl::setMatrices(cam);
	gl::scale(vec3(1.0f, 1.0f, 1.0f) * mZoom);
	gl::rotate(mRotation);
	for (int i = 0; i < bricks.size(); i++)
	{
		if (mGlobalMode)
		{
			r = mR;
			g = mG;
			b = mB;
			a = mA;
			shape = mShape;
		}
		else
		{
			r = bricks[i].r;
			g = bricks[i].g;
			b = bricks[i].b;
			a = bricks[i].a;
			shape = bricks[i].shape;
		}
		rotation = bricks[i].rotation++;
		distance = mMotionVector * mSize;
		bendFactor = 0.0f;
		//save state to restart translation from center point
		gl::pushMatrices();
		for (int j = 0; j < mRepetitions; j++)
		{
			r -= mColorFactor / volumeFactor;
			g -= mColorFactor / volumeFactor;
			b -= mColorFactor / volumeFactor;
			a -= mColorFactor / volumeFactor;
			gl::color(r, g, b, a);
			//gl::color(1.0, 0.0, 0.0, 1.0);
			new_x = sin(rotation*0.01745329251994329576923690768489) * distance;
			new_y = cos(rotation*0.01745329251994329576923690768489) * distance;

			x = new_x + bricks[i].vx;
			y = new_y + bricks[i].vy;
			bendFactor += mBend / 10.0f;
			gl::translate(x, y, mZPosition + bendFactor);

			if (shape == 0)
			{
				gl::drawSphere(vec3(0.0f, 0.0f, 0.0f), bricks[i].size * mSize, 16);
			}
			if (shape == 1)
			{
				gl::drawCube(vec3(0.0f, 0.0f, 0.0f), vec3(bricks[i].size * mSize, bricks[i].size * mSize, bricks[i].size * mSize));
			}
			if (shape == 2)
			{
				gl::drawSolidCircle(vec2(0.0f, 0.0f), bricks[i].size * mSize);
			}
			if (shape == 3)
			{
				vec2 dupa[3] = { vec2(0, bricks[i].size * mSize), vec2(-bricks[i].size * mSize, -bricks[i].size * mSize), vec2(-bricks[i].size * mSize, -bricks[i].size * mSize) };
				gl::drawSolidTriangle(dupa);
			}
		}
		gl::popMatrices();

	}
	//gl::color(Color::white());
}
void BatchassSpidermoonApp::draw()
{
renderSceneToFbo();	
if (mFadeInDelay) {
		if (getElapsedFrames() > mVDSession->getFadeInDelay()) {
			mFadeInDelay = false;
			setWindowSize(mVDSettings->mRenderWidth, mVDSettings->mRenderHeight);
			setWindowPos(ivec2(mVDSettings->mRenderX, mVDSettings->mRenderY));
			timeline().apply(&mVDSettings->iAlpha, 0.0f, 1.0f, 2.0f, EaseInCubic());
		}
	}
	if (mFadeOutDelay) {
		if (getElapsedFrames() > mVDSession->getEndFrame()) {
			mFadeOutDelay = false;
			timeline().apply(&mVDSettings->iAlpha, 1.0f, 0.0f, 2.0f, EaseInCubic());
		}
	}

	gl::clear(Color::black());
	gl::setMatricesWindow(toPixels(getWindowSize()));

	for (auto &warp : mWarps) {
		warp->draw(mRenderFbo->getColorTexture());
	}
	//end
	/*renderSceneToFbo();

	// draw using the mix shader
	mVDFbos[mVDSettings->mMixFboIndex]->getFboRef()->bindFramebuffer();

	// clear the FBO
	gl::clear();
	gl::setMatricesWindow(mVDSettings->mFboWidth, mVDSettings->mFboHeight);

	aShader = mVDShaders->getMixShader();
	aShader->bind();
	aShader->uniform("iGlobalTime", mVDSettings->iGlobalTime);
	aShader->uniform("iResolution", vec3(mVDSettings->mFboWidth, mVDSettings->mFboHeight, 1.0));
	aShader->uniform("iChannelResolution", mVDSettings->iChannelResolution, 4);
	aShader->uniform("iMouse", vec4(mVDSettings->mRenderPosXY.x, mVDSettings->mRenderPosXY.y, mVDSettings->iMouse.z, mVDSettings->iMouse.z));//iMouse =  Vec3i( event.getX(), mRenderHeight - event.getY(), 1 );
	aShader->uniform("iChannel0", 0);
	aShader->uniform("iChannel1", 1);
	aShader->uniform("iAudio0", 0);
	aShader->uniform("iFreq0", mVDSettings->iFreqs[0]);
	aShader->uniform("iFreq1", mVDSettings->iFreqs[1]);
	aShader->uniform("iFreq2", mVDSettings->iFreqs[2]);
	aShader->uniform("iFreq3", mVDSettings->iFreqs[3]);
	aShader->uniform("iChannelTime", mVDSettings->iChannelTime, 4);
	aShader->uniform("iColor", vec3(mVDSettings->controlValues[1], mVDSettings->controlValues[2], mVDSettings->controlValues[3]));// mVDSettings->iColor);
	aShader->uniform("iBackgroundColor", vec3(mVDSettings->controlValues[5], mVDSettings->controlValues[6], mVDSettings->controlValues[7]));// mVDSettings->iBackgroundColor);
	aShader->uniform("iSteps", (int)mVDSettings->controlValues[20]);
	aShader->uniform("iRatio", 20.0f);// mVDSettings->controlValues[11]);//check if needed: +1;
	aShader->uniform("width", 1);
	aShader->uniform("height", 1);
	aShader->uniform("iRenderXY", mVDSettings->mRenderXY);
	aShader->uniform("iZoom", mVDSettings->controlValues[22]);
	aShader->uniform("iAlpha", mVDSettings->controlValues[4] * mVDSettings->iAlpha);
	aShader->uniform("iBlendmode", mVDSettings->iBlendMode);
	aShader->uniform("iChromatic", mVDSettings->controlValues[10]);
	aShader->uniform("iRotationSpeed", mVDSettings->controlValues[19]);
	aShader->uniform("iCrossfade", mVDSettings->controlValues[18]);
	aShader->uniform("iPixelate", mVDSettings->controlValues[15]);
	aShader->uniform("iExposure", mVDSettings->controlValues[14]);
	aShader->uniform("iDeltaTime", mVDAnimation->iDeltaTime);
	aShader->uniform("iFade", (int)mVDSettings->iFade);
	aShader->uniform("iToggle", (int)mVDSettings->controlValues[46]);
	aShader->uniform("iLight", (int)mVDSettings->iLight);
	aShader->uniform("iLightAuto", (int)mVDSettings->iLightAuto);
	aShader->uniform("iGreyScale", (int)mVDSettings->iGreyScale);
	aShader->uniform("iTransition", mVDSettings->iTransition);
	aShader->uniform("iAnim", mVDSettings->iAnim.value());
	aShader->uniform("iRepeat", (int)mVDSettings->iRepeat);
	aShader->uniform("iVignette", 1);// (int)mVDSettings->controlValues[47]);
	aShader->uniform("iInvert", (int)mVDSettings->controlValues[48]);
	aShader->uniform("iDebug", (int)mVDSettings->iDebug);
	aShader->uniform("iShowFps", (int)mVDSettings->iShowFps);
	aShader->uniform("iFps", mVDSettings->iFps);
	aShader->uniform("iTempoTime", mVDAnimation->iTempoTime);
	aShader->uniform("iGlitch", (int)mVDSettings->controlValues[45]);
	aShader->uniform("iTrixels", mVDSettings->controlValues[16]);
	aShader->uniform("iGridSize", mVDSettings->controlValues[17]);
	aShader->uniform("iBeat", mVDSettings->iBeat);
	aShader->uniform("iSeed", mVDSettings->iSeed);
	aShader->uniform("iRedMultiplier", mVDSettings->iRedMultiplier);
	aShader->uniform("iGreenMultiplier", mVDSettings->iGreenMultiplier);
	aShader->uniform("iBlueMultiplier", mVDSettings->iBlueMultiplier);
	aShader->uniform("iFlipH", mVDFbos[mVDSettings->mMixFboIndex]->isFlipH());
	aShader->uniform("iFlipV", mVDFbos[mVDSettings->mMixFboIndex]->isFlipV());
	aShader->uniform("iParam1", mVDSettings->iParam1);
	aShader->uniform("iParam2", mVDSettings->iParam2);
	aShader->uniform("iXorY", mVDSettings->iXorY);
	aShader->uniform("iBadTv", mVDSettings->iBadTv);

	mRenderFbo->getColorTexture()->bind(0);
	mRenderFbo->getColorTexture()->bind(1);
	gl::drawSolidRect(Rectf(0, 0, mVDSettings->mFboWidth, mVDSettings->mFboHeight));
	// stop drawing into the FBO
	mVDFbos[mVDSettings->mMixFboIndex]->getFboRef()->unbindFramebuffer();
	mRenderFbo->getColorTexture()->unbind();
	mRenderFbo->getColorTexture()->unbind();

	if (mFadeInDelay) {
		if (getElapsedFrames() > mVDSession->getFadeInDelay()) {
			mFadeInDelay = false;
			setWindowSize(mVDSettings->mRenderWidth, mVDSettings->mRenderHeight);
			setWindowPos(ivec2(mVDSettings->mRenderX, mVDSettings->mRenderY));
			timeline().apply(&mVDSettings->iAlpha, 0.0f, 1.0f, 2.0f, EaseInCubic());
		}
	}
	if (mFadeOutDelay) {
		if (getElapsedFrames() > mVDSession->getEndFrame()) {
			mFadeOutDelay = false;
			timeline().apply(&mVDSettings->iAlpha, 1.0f, 0.0f, 2.0f, EaseInCubic());
		}
	}
	gl::clear(Color::black());
	gl::setMatricesWindow(toPixels(getWindowSize()));

	for (auto &warp : mWarps) {		
		warp->draw(mVDFbos[mVDSettings->mMixFboIndex]->getTexture());
	}*/

}
void BatchassSpidermoonApp::resize()
{
	// tell the warps our window has been resized, so they properly scale up or down
	Warp::handleResize(mWarps);
}

void BatchassSpidermoonApp::mouseMove(MouseEvent event)
{
	// pass this mouse event to the warp editor first
	if (!Warp::handleMouseMove(mWarps, event)) {
		// let your application perform its mouseMove handling here
		mVDSettings->controlValues[10] = event.getX() / mVDSettings->mRenderWidth;

	}
}

void BatchassSpidermoonApp::mouseDown(MouseEvent event)
{
	// pass this mouse event to the warp editor first
	if (!Warp::handleMouseDown(mWarps, event)) {
		// let your application perform its mouseDown handling here
		mVDSettings->controlValues[45] = 1.0f;
	}
}
void BatchassSpidermoonApp::mouseDrag(MouseEvent event)
{
	// pass this mouse event to the warp editor first
	if (!Warp::handleMouseDrag(mWarps, event)) {
		// let your application perform its mouseDrag handling here
	}
}

void BatchassSpidermoonApp::mouseUp(MouseEvent event)
{
	// pass this mouse event to the warp editor first
	if (!Warp::handleMouseUp(mWarps, event)) {
		// let your application perform its mouseUp handling here
		mVDSettings->controlValues[45] = 0.0f;
	}
}
void BatchassSpidermoonApp::keyDown(KeyEvent event)
{
	fs::path moviePath;
	string fileName;

	// pass this key event to the warp editor first
	if (!Warp::handleKeyDown(mWarps, event)) {
		// warp editor did not handle the key, so handle it here
		if (!mVDAnimation->handleKeyDown(event)) {
			// Animation did not handle the key, so handle it here
			switch (event.getCode()) {

			case KeyEvent::KEY_ESCAPE:
				// quit the application
				quit();
				break;
			case KeyEvent::KEY_w:
				// toggle warp edit mode
				Warp::enableEditMode(!Warp::isEditModeEnabled());
				break;
			case KeyEvent::KEY_r:
				mR += 0.2;
				if (mR > 0.9) mR = 0.0;
				break;
			case KeyEvent::KEY_g:
				mG += 0.2;
				if (mG > 0.9) mG = 0.0;
				break;
			case KeyEvent::KEY_c:
				if (mVDSettings->mCursorVisible)
				{
					hideCursor();
				}
				else
				{
					showCursor();
				}
				mVDSettings->mCursorVisible = !mVDSettings->mCursorVisible;
				break;
			case KeyEvent::KEY_a:
				fileName = "warps" + toString(getElapsedFrames()) + ".xml";
				mSettings = getAssetPath("") / mVDSettings->mAssetsPath / fileName;
				Warp::writeSettings(mWarps, writeFile(mSettings));
				mSettings = getAssetPath("") / mVDSettings->mAssetsPath / "warps.xml";
				break;
			}

		}
	}
}

void BatchassSpidermoonApp::keyUp(KeyEvent event)
{
	// pass this key event to the warp editor first
	if (!Warp::handleKeyUp(mWarps, event)) {
		if (!mVDAnimation->handleKeyUp(event)) {
			// Animation did not handle the key, so handle it here
		}
	}
}

void BatchassSpidermoonApp::updateWindowTitle()
{
	getWindow()->setTitle(to_string(getElapsedFrames()) + " " + to_string((int)getAverageFps()) + " fps Spidermoon");
}

CINDER_APP(BatchassSpidermoonApp, RendererGl, &BatchassSpidermoonApp::prepare)


