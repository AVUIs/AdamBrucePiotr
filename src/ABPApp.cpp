
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
		g_Width = 1280;
		g_Height = 720;
		x = g_Width / 2;
		y = g_Height / 2;
		mFbo = gl::Fbo::create(g_Width, g_Height, format.depthTexture());
	}

	updateWindowTitle();

	mSendOSC = false;

	mMouseIndex = 0;
	isMouseDown = false;
	mZoom = 0.3f;
	mXYVector = vec2(1.0);
	mRepetitions = 1;
	mBranchesRepetitions = 0;
	mShape = 0;
	mZPosition = -1.0f;
	mRotation = 2.5f;
	mSize = 0.9f;
	mMotionVector = 0.2f;
	mBend = 0.1f;
	mLockZ = false;
	mLockRepetitions = false;
	mLockRotation = false;
	mLockSize = false;
	mLockMotionVector = false;
	mLockBend = true;
	mGlobalMode = true;
	mColorFactor = 0.06;
	mR = 0.5f;
	mG = 0.0f;
	mB = 0.8f;
	mA = 0.0f;
	// init one brick
	addBrick();
	// gl setup
	gl::enableDepthRead();
	gl::enableDepthWrite();
	gl::enableAlphaBlending();

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
	bBegin = 64;
	bEnd = 552;
	bBreakBegin = 278;
	bBreakEnd = 320;
}

void ABPApp::newRendering() {
	gl::clear();
}

void ABPApp::cleanup()
{
	mVDSettings->save();
	mVDSession->save();
	quit();
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

void ABPApp::updateWindowTitle()
{
	getWindow()->setTitle(toString(mVDSettings->iBeat) + " " + toString(floor(getElapsedFrames())) + " " + toString(mRepetitions) + " " + toString(mBranchesRepetitions) + " " + toString(floor(getAverageFps())));
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
		addBrick();
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
		mRepetitions--;
		break;
	case KeyEvent::KEY_q:
		mGlobalMode = !mGlobalMode;
		break;
	case KeyEvent::KEY_s:
		mShape++;
		if (mShape > 3) mShape = 0;
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
void ABPApp::addBrick()
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
void ABPApp::update()
{
	mVDSession->setFloatUniformValueByIndex(mVDSettings->IFPS, getAverageFps());
	mVDSession->update();

	mZPosition = mLockZ ? sin(getElapsedFrames() / 100.0f) : mZPosition;
	mRotation = mLockRotation ? sin(getElapsedFrames() / 100.0f)*4.0f : mRotation;
	mSize = mLockSize ? sin(getElapsedFrames() / 100.0f) + 0.7f : mSize;
	mMotionVector = mLockMotionVector ? sin(getElapsedFrames() / 50.0f) : mMotionVector;
	mRotationMatrix *= rotate(0.06f, normalize(vec3(0.16666f, 0.333333f, 0.666666f)));
	mRepetitions = mLockRepetitions ? (sin(getElapsedFrames() / 100.0f) + 2) * 20 : mRepetitions;
	mBend = mLockBend ? sin(getElapsedFrames() / 100.0f) * 4.0f : mBend;
	if (mVDSettings->iBeat < bBegin)
	{
		mRepetitions = (mVDSettings->iBeat / 8) + 1;
	}
	else
	{
		if (mVDSettings->iBeat % 8 == 0)
		{
			if (bricks.size() < 20) {
				addBrick();
			}
			if (mVDSettings->iBeat > 92 && mVDSettings->iBeat < 150)
			{
				mGlobalMode = true;
				mR = 1.0f;
				mB = 0.0f;
			}
			// change to cube
			if (mVDSettings->iBeat > bBreakBegin && mVDSettings->iBeat < bBreakEnd)
			{
				if (mVDSettings->iBeat < bBreakEnd - 8) {
					mShape = 3;
					mLockBend = false;
				}
				else {
					mShape = 1;
				}
			}

		}
		else
		{
			mR = 0.5f;
			mB = 0.8f;
		}
		if (mVDSettings->iBeat > 492 && mVDSettings->iBeat < bEnd && mVDSettings->iBeat % 2 == 0)
		{
			mRotation += 0.2;
			if (mRotation > 6.35) mRotation = 0;
		}
		if (mVDSettings->iBeat > 370)
		{
			mShape = 0;
		}
		// end
		if (mVDSettings->iBeat > bEnd) {
			mR = mG = mB = 0.8f;
			mShape = 3;
		}
	}
	updateBricks();

	updateWindowTitle();
}
void ABPApp::updateBricks()
{
	float new_x;
	float new_y;
	float bendFactor;
	float volumeFactor;
	volumeFactor = 1.0f;// CHECK mVDAnimation->controlValues[14];
#ifdef _DEBUG
#else
	// avoid fps drop
	if (mVDSettings->iBeat > 158) {
		mBranchesRepetitions = mVDSession->getMaxVolume() / 10;
	}
	if (mVDSettings->iBeat > 370) {
		mZoom = mVDSession->getMaxVolume() / 300;
	}
#endif  // _DEBUG

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
		bendFactor = 0.1f;
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
				gl::drawStrokedCircle(vec2(0, 0), bricks[i].size * mSize);
			}
			// branch begin
			gl::pushMatrices();
			for (int k = 0; k < mBranchesRepetitions; k++)
			{

				gl::color(r - 0.1f, g - 0.1f, b - 0.1f, a - 0.1f);
				new_x = sin(rotation*0.017) * distance * 2;
				new_y = cos(rotation*0.017) * distance * 2;

				x = new_x + bricks[i].vx;
				y = new_y + bricks[i].vy;
				bendFactor += mBend / 10.0f;
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
					gl::drawStrokedCircle(vec2(0, 0), bricks[i].size * mSize);
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
				setWindowSize(mVDSettings->mRenderWidth, mVDSettings->mRenderHeight + 1);
				setWindowPos(ivec2(mVDSettings->mRenderX, mVDSettings->mRenderY));
			}
			else {
				setWindowSize(1280, 720);
				setWindowPos(ivec2(mVDSettings->mRenderX, mVDSettings->mRenderY));
			}
			timeline().apply(&mA, 0.0f, 1.0f, 3.0f, EaseInCubic());
		}
	}
	if (mWaveDelay) {
		if (getElapsedFrames() > mVDSession->getWavePlaybackDelay()) {
			mWaveDelay = false;
			fs::path waveFile = getAssetPath("") / mVDSettings->mAssetsPath / mVDSession->getWaveFileName();
			mVDSession->loadAudioFile(waveFile.string());
		}
	}
	if (mFadeOutDelay) {
		if (getElapsedFrames() > mVDSession->getEndFrame()) {
			mFadeOutDelay = false;
			timeline().apply(&mA, 1.0f, 0.0f, 10.0f, EaseInCubic());
		}
	}
	gl::clear(Color::black());
	gl::color(Color::white());

	gl::setMatricesWindow(toPixels(getWindowSize()));


	gl::draw(mFbo->getColorTexture(), Rectf(0, 0, g_Width, g_Height));

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
