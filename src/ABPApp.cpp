
#include "ABPApp.h"
void ABPApp::prepare(Settings *settings)
{
	settings->setWindowSize(40, 10);
}
void ABPApp::setup()
{
	mWaveDelay = mFadeInDelay = mFadeOutDelay = true;

	// Settings
	mVDSettings = VDSettings::create();
	// Session
	mVDSession = VDSession::create(mVDSettings);
	mVDSession->getWindowsResolution();
	gl::Fbo::Format format;
	if (mVDSettings->mStandalone) {
		g_Width = mVDSettings->mRenderWidth;
		g_Height = mVDSettings->mRenderHeight;
		x = mVDSettings->mRenderWidth / 2;
		y = mVDSettings->mRenderHeight / 2;
		mFbo = gl::Fbo::create(mVDSettings->mRenderWidth, mVDSettings->mRenderHeight, format.depthTexture());
	}
	else {
		g_Width = 1024;
		g_Height = 768;
		x = 1024 / 2;
		y = 768 / 2;
		mFbo = gl::Fbo::create(1024, 768, format.depthTexture());
	}

	updateWindowTitle();

	mSendOSC = false;

	mUseCam = false;
	mMouseIndex = 0;
	isMouseDown = false;
	isRecording = false;
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
	mGlobalMode = true;
	mColorFactor = 0.06;
	mR = 0.5f;
	mG = 0.0f;
	mB = 0.8f;
	mA = 1.0f;
	presentIndex = 0;
	// init one brick
	addBrick(true);
	// gl setup
	gl::enableDepthRead();
	gl::enableDepthWrite();
	gl::enableAlphaBlending();

	mCam.lookAt(vec3(0, CAMERA_Y_RANGE.first, 0), vec3(0));

#if defined( CINDER_MSW )
	// -------- SPOUT -------------
	// Set up the texture we will use to send out
	// We grab the screen so it has to be the same size
	bSenderInitialized = false;
	spoutSenderTexture = gl::Texture::create(g_Width, g_Height);
	strcpy_s(SenderName, "ABP Spout Sender"); // we have to set a sender name first
	// Initialize a sender
	bSenderInitialized = spoutsender.CreateSender(SenderName, g_Width, g_Height);
#endif

	//mVDSettings->iBeat = -1;
}

void ABPApp::newRendering() {
	gl::clear();
}
void ABPApp::fileDrop(FileDropEvent event)
{

}
void ABPApp::update()
{
	mVDSession->setFloatUniformValueByIndex(mVDSettings->IFPS, getAverageFps());
	mVDSession->update();

	mZPosition = mLockZ ? sin(getElapsedFrames() / 100.0f) : mZPosition;
	mRotation = mLockRotation ? sin(getElapsedFrames() / 100.0f)*4.0f : mRotation;
	mSize = mLockSize ? sin(getElapsedFrames() / 100.0f) + 0.7f : mSize;
	mMotionVector = mLockMotionVector ? sin(getElapsedFrames() / 50.0f) : mMotionVector;
	mRotationMatrix *= rotate(0.06f, normalize(vec3(0.16666f, 0.333333f, 0.666666f)));
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
			if (bricks.size() < 20) addBrick(false);
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
			mR = 0.5f;
			mB = 0.8f;
			if (mVDSettings->iBeat > 510 && mVDSettings->iBeat % 2 == 0)
			{
				mRotation += 0.2;
				if (mRotation > 6.35) mRotation = 0;
			}
		}
	}
	updateBricks();

	//mParams->update();
	updateWindowTitle();
	// move the camera up and down on Y
	mCam.lookAt(vec3(0, CAMERA_Y_RANGE.first + abs(sin(getElapsedSeconds() / 4)) * (CAMERA_Y_RANGE.second - CAMERA_Y_RANGE.first), 0), vec3(0));

}
void ABPApp::cleanup()
{
	mVDSettings->save();
	mVDSession->save();
	quit();
}
void ABPApp::resize()
{
	// now tell our Camera that the window aspect ratio has changed
	mCam.setPerspective(60, getWindowAspectRatio(), 1, 1000);

	// and in turn, let OpenGL know we have a new camera
	gl::setMatrices(mCam);
}
void ABPApp::mouseDown(MouseEvent event)
{
	isMouseDown = true;
}
void ABPApp::mouseDrag(MouseEvent event)
{
	isMouseDown = true;
}
void ABPApp::mouseUp(MouseEvent event)
{
	isMouseDown = false;
}
void ABPApp::record(const bool &pressed)
{
	mRotation++;
	isRecording = !isRecording;
}
void ABPApp::updateWindowTitle()
{
	getWindow()->setTitle("(" + toString(floor(getAverageFps())) + " fps) ABP");
}
void ABPApp::keyDown(KeyEvent event)
{
	switch (event.getCode())
	{
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
	case KeyEvent::KEY_o:
		mRotation += 0.2;
		if (mRotation > 6.35) mRotation = 0;
		break;
	case KeyEvent::KEY_b:
		mBend = mBend + 1.2f;
		if (mBend > 10.0f) mBend = 0.0f;
		break;
	case ci::app::KeyEvent::KEY_r:
		mR += 0.2;
		if (mR > 0.9) mR = 0.0;
		break;
	case ci::app::KeyEvent::KEY_g:
		mG += 0.2;
		if (mG > 0.9) mG = 0.0;
		break;
	case ci::app::KeyEvent::KEY_l:
		mLockBend = !mLockBend;
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

void ABPApp::setGlobalMode(const bool &pressed)
{
	mGlobalMode = !mGlobalMode;
}
void ABPApp::reset(const bool &pressed)
{
	bricks.clear();
}
void ABPApp::addBrick(const bool &pressed)
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
void ABPApp::updateBricks()
{
	float new_x;
	float new_y;
	float bendFactor;
	float volumeFactor;
	volumeFactor = 1.0f;// CHECK mVDAnimation->controlValues[14];
	/*if (mVDSettings->maxVolume > 0.7)
	{
	float between08and1 = mParameterBag->maxVolume - 0.7;
	volumeFactor = lmap<float>(between08and1, 0.0, 0.3, 0.1, 0.8);
	}
	else
	{
	volumeFactor = 0.01;
	}*/
	if (newRecording == true) {
		newRendering();
	}
	gl::ScopedFramebuffer fbScp(mFbo);

	// clear out the FBO with red
	gl::clear(Color::black());
	// setup the viewport to match the dimensions of the FBO
	gl::ScopedViewport scpVp(ivec2(0), mFbo->getSize());

	CameraPersp cam(mFbo->getWidth(), mFbo->getHeight(), 60.0f);
	cam.setPerspective(60, mFbo->getAspectRatio(), 1, 1000);
	cam.lookAt(vec3(2.8f, 1.8f, -2.8f), vec3(0));
	gl::setMatrices(cam);
	gl::color(Color::white());
	gl::scale(vec3(1.0) * mZoom);
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
			bendFactor += mBend / 10.0f;
			gl::translate(x, y, mZPosition + bendFactor);

			if (shape == 0)
			{
				gl::drawSphere(vec3(0.0), bricks[i].size * mSize, 16);

			}
			if (shape == 1)
			{
				gl::drawCube(vec3(0.0), vec3(bricks[i].size * mSize));
			}
			if (shape == 2)
			{
				gl::drawSolidCircle(vec2(0, 0), bricks[i].size * mSize);
			}
			if (shape == 3)
			{
				vec2 dupa[3] = { vec2(0, bricks[i].size * mSize), vec2(-bricks[i].size * mSize, -bricks[i].size * mSize), vec2(-bricks[i].size * mSize, -bricks[i].size * mSize) };
				gl::drawSolidTriangle(dupa);
			}
			// branch begin
			gl::pushMatrices();
			for (int k = 0; k < mRepetitions; k++)
			{

				gl::color(r - 0.1f, g - 0.1f, b - 0.1f, a - 0.1f);
				new_x = sin(rotation*0.017) * distance * 2;
				new_y = cos(rotation*0.017) * distance * 2;

				x = new_x + bricks[i].vx;
				y = new_y + bricks[i].vy;
				bendFactor -= mBend / 10.0f;
				gl::translate(x, y, mZPosition + bendFactor);

				if (shape == 0)
				{
					gl::drawSphere(vec3(0.0), bricks[i].size * mSize / 2, 16);
				}
				if (shape == 1)
				{
					gl::drawCube(vec3(0.0), vec3(bricks[i].size * mSize / 2));
				}
				if (shape == 2)
				{
					gl::drawSolidCircle(vec2(0, 0), bricks[i].size * mSize / 2);
				}
				if (shape == 3)
				{
					vec2 dupa[3] = { vec2(0, bricks[i].size * mSize / 2), vec2(-bricks[i].size * mSize, -bricks[i].size * mSize), vec2(-bricks[i].size * mSize, -bricks[i].size * mSize) };
					gl::drawSolidTriangle(dupa);
				}
			}
			gl::popMatrices();
			// branch end
		}
		gl::popMatrices();




	}
	gl::color(Color::white());
}
void ABPApp::draw()
{
	if (mFadeInDelay) {
		if (getElapsedFrames() > mVDSession->getFadeInDelay()) {
			mFadeInDelay = false;
			if (mVDSettings->mStandalone) {
				setWindowSize(mVDSettings->mRenderWidth, mVDSettings->mRenderHeight);
				setWindowPos(ivec2(mVDSettings->mRenderX, mVDSettings->mRenderY));
			}
			else {
				setWindowSize(1024, 768);
				setWindowPos(ivec2(0, 0));

			}
			timeline().apply(&mVDSettings->iAlpha, 0.0f, 1.0f, 2.0f, EaseInCubic());
		}
	}
	if (mWaveDelay) {
		if (getElapsedFrames() > mVDSession->getWavePlaybackDelay()) {
			mWaveDelay = false;
			fs::path waveFile = getAssetPath("") / mVDSettings->mAssetsPath / mVDSession->getWaveFileName();
			mVDSession->loadAudioFile(waveFile.string());
		}
	}
	/*if (mFadeOutDelay) {
		if (getElapsedFrames() > mVDSession->getEndFrame()) {
			mFadeOutDelay = false;
			timeline().apply(&mVDSettings->iAlpha, 1.0f, 0.0f, 2.0f, EaseInCubic());
		}
	}*/
	gl::clear(Color::black());
	gl::color(Color::white());
	if (mUseCam)
	{
		gl::setMatrices(mCam);
	}
	else
	{
		gl::setMatricesWindow(toPixels(getWindowSize()));
	}

	gl::draw(mFbo->getColorTexture(), Rectf(0, 0, 1024, 768));
	// draw textures
	//mBatchass->mTextures->draw();
	//mBatch->drawInstanced(mRepetitions * mRepetitions);
#if defined( CINDER_MSW )
	// -------- SPOUT SENDER-------------
	if (bSenderInitialized) {

		// Grab the screen (current read buffer) into the local spout texture
		spoutSenderTexture->bind();
		glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, g_Width, g_Height);
		spoutSenderTexture->unbind();
		spoutsender.SendTexture(spoutSenderTexture->getId(), spoutSenderTexture->getTarget(), g_Width, g_Height);
	}
	gl::draw(spoutSenderTexture);
#endif	
}


CINDER_APP(ABPApp, RendererGl, &ABPApp::prepare)
