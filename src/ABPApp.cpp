#include "ABPApp.h"

void ABPApp::prepareSettings(Settings *settings)
{
	// parameters
	mParameterBag = ParameterBag::create();
	// utils
	mBatchass = Batchass::create(mParameterBag);
	// if AutoLayout, try to position the window on the 2nd screen
	if (mParameterBag->mAutoLayout)
	{
		mBatchass->getWindowsResolution();
	}
#ifdef _DEBUG
	// debug mode
	settings->setWindowSize(mParameterBag->mRenderWidth, mParameterBag->mRenderHeight);
	settings->setWindowPos(ivec2(mParameterBag->mRenderX, mParameterBag->mRenderY));
#else
	settings->setWindowSize(mParameterBag->mRenderWidth, mParameterBag->mRenderHeight);
	settings->setWindowPos(ivec2(mParameterBag->mRenderX, mParameterBag->mRenderY));
#endif  // _DEBUG
	settings->setBorderless();
	settings->setResizable(false); // resize allowed for a receiver, but runtime error on the resize in the shaders drawing
	// set a high frame rate 1000 to disable limitation
	settings->setFrameRate(60.0f);
	g_Width = mParameterBag->mRenderWidth;
	g_Height = mParameterBag->mRenderHeight;

}
void ABPApp::setup()
{
	mBatchass->setup();

	// instanciate the OSC class
	mOSC = OSC::create(mParameterBag, mBatchass->mShaders, mBatchass->mTextures);

	updateWindowTitle();

	mSendOSC = false;
	// neRenderer
	x = mParameterBag->mRenderWidth / 2;
	y = mParameterBag->mRenderHeight / 2;
	gl::Fbo::Format format;
	mFbo = gl::Fbo::create(mParameterBag->mFboWidth, mParameterBag->mFboHeight, format.depthTexture());
	mUseCam = false;
	mMouseIndex = 0;
	isMouseDown = false;
	isRecording = false;
	mZoom = 0.3f;
	mXYVector = vec2(1.0);
	mRepetitions = 1;
	mShape = 0;
	mZPosition = 0.0f;
	mRotation = 0.0f;
	mSize = 1.0f;
	mMotionVector = 0.2f;
	mBend = 0.1f;
	mLockZ = false;
	mLockRepetitions = false;
	mLockRotation = false;
	mLockSize = false;
	mLockMotionVector = false;
	mLockBend = false;
	mColorFactor = 0.01;
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

	gl::enableDepthWrite();
	gl::enableDepthRead();

	mParams = MinimalUI::UIController::create("{ \"x\":0, \"y\":0, \"depth\":100, \"width\":220, \"height\":700, \"fboNumSamples\":0, \"defaultBackgroundColor\":\"0x9912110c\", \"defaultStrokeColor\":\"0xFF44422f\",\"defaultNameColor\":\"0xFF44422f\", \"activeStrokeColor\":\"0xFF737446\", \"panelColor\":\"0x441e1e1e\" }");

	// 2D Sliders
	sliderXY = mParams->addSlider2D("XY", &mXYVector, "{ \"minX\":-2.0, \"maxX\":2.0, \"minY\":-2.0, \"maxY\":2.0 }");
	// Slider
	mParams->addSlider("Zoom", &mZoom);

	sliderRed = mParams->addSlider("R", &mR, "{ \"nameColor\":\"0xEEFF0000\" }");
	sliderGreen = mParams->addSlider("G", &mG, "{ \"nameColor\":\"0xEE00FF00\" }");
	sliderBlue = mParams->addSlider("B", &mB, "{  \"nameColor\":\"0xEE0000FF\" }");
	sliderAlpha = mParams->addSlider("A", &mA, "{ \"nameColor\":\"0xFFFFFFFF\" }");
	mParams->addSlider("Color\nFactor", &mColorFactor, "{ \"min\": -0.1, \"max\": 0.1 }");

	// Separator
	mParams->addSeparator();

	// Label
	mParams->addLabel("Shape", "{ \"clear\":false }");

	// Button Group
	mParams->addButton("Sphere", std::bind(&ABPApp::setShape, this, 0, std::placeholders::_1), "{ \"clear\":false, \"group\":\"shape\", \"exclusive\":true }");
	mParams->addButton("Cube", std::bind(&ABPApp::setShape, this, 1, std::placeholders::_1), "{ \"clear\":false, \"group\":\"shape\", \"exclusive\":true }");
	mParams->addButton("Circle", std::bind(&ABPApp::setShape, this, 2, std::placeholders::_1), "{ \"group\":\"shape\", \"exclusive\":true }");
	//mParams->addButton("Triangle", std::bind(&ABPApp::setShape, this, 3, std::placeholders::_1), "{ \"clear\":false, \"group\":\"shape\", \"exclusive\":true }");
	// Repetitions
	mParams->addToggleSlider("Repetitions", &mRepetitions, "A", std::bind(&ABPApp::lockRepetitions, this, std::placeholders::_1), "{ \"width\":156, \"clear\":false, \"min\": 1, \"max\": 20 }", "{ \"stateless\":false }");

	//mParams->addButton("Record", std::bind(&ABPApp::record, this, std::placeholders::_1), "{ \"clear\":false, \"stateless\":false }");
	//mParams->addButton("Send OSC", std::bind(&ABPApp::sendOSC, this, std::placeholders::_1), "{ \"clear\":false, \"stateless\":false, \"pressed\":false }");
	mParams->addButton("Add brick", std::bind(&ABPApp::addBrick, this, std::placeholders::_1), "{ \"stateless\":false, \"pressed\":false }");
	// Toggle Slider
	mParams->addToggleSlider("Z Position", &mZPosition, "A", std::bind(&ABPApp::lockZ, this, std::placeholders::_1), "{ \"width\":156, \"clear\":false, \"min\": -1, \"max\": 1 }", "{ \"stateless\":false }");
	mParams->addToggleSlider("Rotation", &mRotation, "A", std::bind(&ABPApp::lockRotation, this, std::placeholders::_1), "{ \"width\":156, \"clear\":false, \"min\": 0, \"max\": 6.28 }", "{ \"stateless\":false }");
	mParams->addToggleSlider("Size", &mSize, "A", std::bind(&ABPApp::lockSize, this, std::placeholders::_1), "{ \"width\":156, \"clear\":false, \"min\": 0.7, \"max\": 6.0 }", "{ \"stateless\":false }");
	mParams->addToggleSlider("MotionVector", &mMotionVector, "A", std::bind(&ABPApp::lockMotionVector, this, std::placeholders::_1), "{ \"width\":156, \"clear\":false, \"min\": 0.0, \"max\": 1.0 }", "{ \"stateless\":false }");
	mParams->addToggleSlider("Bend", &mBend, "A", std::bind(&ABPApp::lockBend, this, std::placeholders::_1), "{ \"width\":156, \"clear\":false, \"min\": -20.0, \"max\": 20.0 }", "{ \"stateless\":false }");
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

}
void ABPApp::resize()
{
	// now tell our Camera that the window aspect ratio has changed
	mCam.setPerspective(60, getWindowAspectRatio(), 1, 1000);

	// and in turn, let OpenGL know we have a new camera
	gl::setMatrices(mCam);
}

void ABPApp::newRendering() {
	gl::clear();
}

void ABPApp::mouseDown(MouseEvent event)
{
	isMouseDown = true;
}
void ABPApp::mouseMove(MouseEvent event)
{
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
	getWindow()->setTitle("(" + toString(floor(getAverageFps())) + " fps) ABP");
}
void ABPApp::record(const bool &pressed)
{
	mRotation++;
	isRecording = !isRecording;
}
void ABPApp::keyDown(KeyEvent event)
{
	switch (event.getCode())
	{
	case ci::app::KeyEvent::KEY_u:
		mParameterBag->mShowUI = !mParameterBag->mShowUI;
		break;
	case ci::app::KeyEvent::KEY_a:
		//mUseCam = !mUseCam;
		break;
	case ci::app::KeyEvent::KEY_c:
		if (mParameterBag->mCursorVisible)
		{
			hideCursor();
		}
		else
		{
			showCursor();
		}
		mParameterBag->mCursorVisible = !mParameterBag->mCursorVisible;
		break;
	default:
		break;
	}
}
void ABPApp::update()
{
	mParameterBag->iChannelTime[0] = getElapsedSeconds();
	mParameterBag->iChannelTime[1] = getElapsedSeconds() - 1;
	mParameterBag->iChannelTime[2] = getElapsedSeconds() - 2;
	mParameterBag->iChannelTime[3] = getElapsedSeconds() - 3;
	//
	if (mParameterBag->mUseTimeWithTempo)
	{
		mParameterBag->iGlobalTime = mParameterBag->iTempoTime*mParameterBag->iTimeFactor;
	}
	else
	{
		mParameterBag->iGlobalTime = getElapsedSeconds();
	}
	mOSC->update();
	//! update textures
	mBatchass->mTextures->update();
	//! update shaders (must be after the textures update)
	mBatchass->mShaders->update();

	updateWindowTitle();
	mZPosition = mLockZ ? sin(getElapsedFrames() / 100.0f) : mZPosition;
	mRotation = mLockRotation ? sin(getElapsedFrames() / 100.0f)*4.0f : mRotation;
	mSize = mLockSize ? sin(getElapsedFrames() / 100.0f)+0.7f : mSize;
	mMotionVector = mLockMotionVector ? sin(getElapsedFrames() / 50.0f) : mMotionVector;
	mRotationMatrix *= rotate(0.06f, normalize(vec3(0.16666f, 0.333333f, 0.666666f)));
	mRepetitions = mLockRepetitions ? (sin(getElapsedFrames() / 100.0f)+1) * 20 : mRepetitions;
	mBend = mLockBend ? sin(getElapsedFrames() / 100.0f) * 10.0f : mBend;

	updateBricks();

	mParams->update();
	updateWindowTitle();
	// move the camera up and down on Y
	mCam.lookAt(vec3(0, CAMERA_Y_RANGE.first + abs(sin(getElapsedSeconds() / 4)) * (CAMERA_Y_RANGE.second - CAMERA_Y_RANGE.first), 0), vec3(0));
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
		r = bricks[i].r;
		g = bricks[i].g;
		b = bricks[i].b;
		a = bricks[i].a;
		rotation = bricks[i].rotation++;
		distance = mMotionVector * mSize;
		shape = bricks[i].shape;
		bendFactor = 0.0f;
		//save state to restart translation from center point
		gl::pushMatrices();
		for (int j = 0; j < mRepetitions; j++)
		{
			r -= mColorFactor / (mParameterBag->maxVolume + 0.1);
			g -= mColorFactor / (mParameterBag->maxVolume + 0.1);
			b -= mColorFactor / (mParameterBag->maxVolume + 0.1);
			a -= mColorFactor / (mParameterBag->maxVolume + 0.1);
			gl::color(r, g, b, a);
			new_x = sin(rotation*0.01745329251994329576923690768489) * distance;
			new_y = cos(rotation*0.01745329251994329576923690768489) * distance;

			x = new_x + bricks[i].vx;
			y = new_y + bricks[i].vy;
			bendFactor += mBend/10.0f;
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
		}
		gl::popMatrices();

	}
	gl::color(Color::white());
}
void ABPApp::draw()
{
	gl::clear();
	gl::color(Color::white());
	if (mUseCam)
	{
		gl::setMatrices(mCam);
	}
	else
	{
		gl::setMatricesWindow(toPixels(getWindowSize()));
	}

	gl::draw(mFbo->getColorTexture(), Rectf(0, 0, mParameterBag->mRenderWidth, mParameterBag->mRenderHeight));
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
	if (mParameterBag->mShowUI) mParams->draw();
}
void ABPApp::shutdown()
{
	// save params
	mParameterBag->save();
	// close spout

}
auto options = RendererGl::Options().version(3, 3); // instancing functions are technically only in GL 3.3

CINDER_APP_NATIVE(ABPApp, RendererGl(options))
