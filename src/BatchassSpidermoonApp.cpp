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
	// Animation
	mVDAnimation = VDAnimation::create(mVDSettings, mVDSession);
	// Message router
	mVDRouter = VDRouter::create(mVDSettings, mVDAnimation, mVDSession);
	// mix fbo at index 0
	mVDFbos.push_back(VDFbo::create());

	gl::enableDepthWrite();
	gl::enableDepthRead();
	gl::enableAlphaBlending();

	// Textures 
	mTexturesFilepath = getAssetPath("") / mVDSettings->mAssetsPath / "textures.xml";
	if (fs::exists(mTexturesFilepath)) {
		// load textures from file if one exists
		mTexs = VDTexture::readSettings(loadFile(mTexturesFilepath));
	}
	else {
		// otherwise create a texture from scratch
		mTexs.push_back(TextureAudio::create());
	}
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
	}

	// render fbo
	gl::Fbo::Format fboFormat;
	//format.setSamples( 4 ); // uncomment this to enable 4x antialiasing
	//mRenderFbo = gl::Fbo::create(mVDSettings->mFboWidth, mVDSettings->mFboHeight, fboFormat.colorTexture());
	mRenderFbo = gl::Fbo::create(1024, 768, fboFormat.colorTexture());
	// bpm
	setFrameRate(mVDSession->getTargetFps());
	// abp
	// neRenderer
	x = 512;
	y = 384;

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
	mouseX = 2.0f;
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
	// save textures
	VDTexture::writeSettings(mTexs, writeFile(mTexturesFilepath));
}
void BatchassSpidermoonApp::update()
{
	// get audio spectrum
	mTexs[0]->getTexture();

	mVDAnimation->update();
	mVDRouter->update();
	updateWindowTitle();
	// abp
	mZPosition = sin(getElapsedFrames() / 100.0f);
	mRotation = sin(getElapsedFrames() / 100.0f)*4.0f ;
	mSize = sin(getElapsedFrames() / 100.0f) + 0.7f;
	mMotionVector = sin(getElapsedFrames() / 50.0f);
	//mRotationMatrix *= rotate(0.06f, normalize(vec3(0.16666f, 0.333333f, 0.666666f)));
	//mRepetitions = mLockRepetitions ? (sin(getElapsedFrames() / 100.0f) + 1) * 20 : mRepetitions;
	mBend =  sin(getElapsedFrames() / 100.0f) * 10.0f ;
	// move the camera up and down on Y
	//mCam.lookAt(vec3(0.0f, CAMERA_Y_RANGE.first + abs(sin(getElapsedSeconds() / 4)) * (CAMERA_Y_RANGE.second - CAMERA_Y_RANGE.first), 0.0f), vec3(mouseX, 0.0f, 0.0f));
	mCam.lookAt(vec3(mouseX, mouseY, 0.0f), vec3(mouseX, 0.0f, 0.0f));

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
	mBend = mVDSettings->controlValues[11] * 10;

	volumeFactor = 1.0f;// CHECK mVDSettings->controlValues[14];
	/*
		CameraPersp cam(mTextures->getFbo(mParameterBag->mABPFboIndex).getWidth(), mTextures->getFbo(mParameterBag->mABPFboIndex).getHeight(), 60.0f);
	cam.setPerspective(60, mTextures->getFbo(mParameterBag->mABPFboIndex).getAspectRatio(), 1, 1000);
	cam.lookAt(Vec3f(2.8f, 1.8f, -2.8f), Vec3f(0.0f, 0.0f, 0.0f));

	*/
	CameraPersp cam(1024, 768, 60.0f); // hardcoded
	cam.setPerspective(60, 0.75, 1, 1000);//mTextures->getFbo(mVDSettings->mABPFboIndex).getAspectRatio()
	//cam.lookAt(vec3(2.8f, 1.8f, -2.8f), vec3(mouseX, 1.0f, 0.0f));
	cam.lookAt(vec3(0.0f, 0.0f, -1.0f), vec3(0.0f, 0.0f, 0.0f));
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
			new_x = sin(rotation*0.01745329251994329576923690768489) * distance;
			new_y = cos(rotation*0.01745329251994329576923690768489) * distance;

			x = new_x + bricks[i].vx;
			y = new_y + bricks[i].vy;
			bendFactor += mBend / 20.0f;
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
	gl::color(Color::white());
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
	gl::draw(mRenderFbo->getColorTexture());
	/*for (auto &warp : mWarps) {
		warp->draw();
		}*/
	//end
	/*renderSceneToFbo();



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
		mVDSettings->controlValues[45] = 1.0f;
		mouseX = event.getX() / 640.0f;
		mouseY = event.getY() / 640.0f;

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
			switch (event.getCode()) {

			case KeyEvent::KEY_ESCAPE:
				// quit the application
				quit();
				break;			
			case KeyEvent::KEY_a:
				addBrick(true);
				break;
			case KeyEvent::KEY_q:
				mShape = !mShape;
				break;
			case KeyEvent::KEY_r:
				mRotation += 0.2;
				if (mRotation > 6.35) mRotation = 0;
				break;
			case KeyEvent::KEY_b:
				mBend = mBend + 1.2f;
				if (mBend > 10.0f) mBend = 0.0f;
				break;
			case KeyEvent::KEY_d:
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

			case KeyEvent::KEY_p:
				mRepetitions++;
				break;
			case KeyEvent::KEY_m:
				mGlobalMode = !mGlobalMode;
				break;
			default:
				break;
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


