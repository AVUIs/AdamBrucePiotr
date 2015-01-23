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

}
void ABPApp::setup()
{
	mBatchass->setup();

	// instanciate the spout class
	mSpout = SpoutWrapper::create(mParameterBag, mBatchass->mTextures);
	// instanciate the OSC class
	mOSC = OSC::create(mParameterBag, mBatchass->mShaders, mBatchass->mTextures);

	updateWindowTitle();

	mSendOSC = false;
	// neRenderer
	x = mParameterBag->mRenderWidth / 2;
	y = mParameterBag->mRenderHeight / 2;
	gl::Fbo::Format format;
	mFbo = gl::Fbo::create(mParameterBag->mFboWidth, mParameterBag->mFboHeight, format.depthTexture());
	mUseCam = true;
	mMouseIndex = 0;
	timer = 0;
	isMouseDown = false;
	isRecording = false;
	mZoom = 0.3f;
	mXYSize = vec2(1.0);
	mRepetition = repetitions = 10;
	mShape = 0;
	mZPosition = 0.0f;
	mRotation = 0.0f;
	mSize = 1.0f;
	mMotionVector = 0.2f;
	mLockZ = false;
	mLockRotation = false;
	mLockSize = false;
	mLockMotionVector = false;
	mR = 0.8f;
	mG = 0.2f;
	mB = 0.0f;
	mA = 1.0f;
	presentIndex = 0;
	// init one brick
	addBrick(true);
	timer = 0;
	// gl setup
	gl::enableDepthRead();
	gl::enableDepthWrite();
	gl::enableAlphaBlending();

	mCam.lookAt(vec3(0, CAMERA_Y_RANGE.first, 0), vec3(0));

	mGlsl = gl::GlslProg::create(loadAsset("shader.vert"), loadAsset("shader.frag"));

	createPositions();

	gl::enableDepthWrite();
	gl::enableDepthRead();

	mParams = MinimalUI::UIController::create("{ \"x\":0, \"y\":0, \"depth\":100, \"width\":260, \"height\":600, \"fboNumSamples\":0, \"panelColor\":\"0x44402828\" }");

	// 2D Sliders
	sliderXY = mParams->addSlider2D("XY", &mXYSize, "{ \"minX\":-2.0, \"maxX\":2.0, \"minY\":-2.0, \"maxY\":2.0 }");
	// Slider
	mParams->addSlider("Zoom", &mZoom);

	sliderRed = mParams->addSlider("R", &mR, "{ \"nameColor\":\"0xEEFF0000\" }");
	sliderGreen = mParams->addSlider("G", &mG, "{ \"nameColor\":\"0xEE00FF00\" }");
	sliderBlue = mParams->addSlider("B", &mB, "{  \"nameColor\":\"0xEE0000FF\" }");
	sliderAlpha = mParams->addSlider("A", &mA, "{ \"nameColor\":\"0xFFFFFFFF\" }");

	// Separator
	mParams->addSeparator();

	// Label
	mParams->addLabel("Shape", "{ \"clear\":false }");

	// Button Group
	mParams->addButton("Cube", std::bind(&ABPApp::setShape, this, 0, std::placeholders::_1), "{ \"clear\":false, \"group\":\"shape\", \"exclusive\":true }");
	mParams->addButton("Rect", std::bind(&ABPApp::setShape, this, 1, std::placeholders::_1), "{ \"clear\":false, \"group\":\"shape\", \"exclusive\":true }");
	mParams->addButton("Circle", std::bind(&ABPApp::setShape, this, 2, std::placeholders::_1), "{ \"clear\":false, \"group\":\"shape\", \"exclusive\":true }");
	mParams->addButton("Triangle", std::bind(&ABPApp::setShape, this, 3, std::placeholders::_1), "{ \"group\":\"shape\", \"exclusive\":true }");
	// Repetitions
	mParams->addButton("+1", std::bind(&ABPApp::setRepetitions, this, 1, std::placeholders::_1), "{ \"clear\":false, \"group\":\"repeat\", \"exclusive\":true }");
	mParams->addButton("+10", std::bind(&ABPApp::setRepetitions, this, 10, std::placeholders::_1), "{ \"clear\":false, \"group\":\"repeat\", \"exclusive\":true }");
	mParams->addButton("+100", std::bind(&ABPApp::setRepetitions, this, 100, std::placeholders::_1), "{ \"clear\":false, \"group\":\"repeat\", \"exclusive\":true }");
	mParams->addButton("-1", std::bind(&ABPApp::setRepetitions, this, -1, std::placeholders::_1), "{ \"clear\":false, \"group\":\"repeat\", \"exclusive\":true }");
	mParams->addButton("-10", std::bind(&ABPApp::setRepetitions, this, -10, std::placeholders::_1), "{ \"clear\":false, \"group\":\"repeat\", \"exclusive\":true }");
	mParams->addButton("-100", std::bind(&ABPApp::setRepetitions, this, -100, std::placeholders::_1), "{ \"group\":\"repeat\", \"exclusive\":true }");

	mParams->addButton("Record", std::bind(&ABPApp::record, this, std::placeholders::_1), "{ \"clear\":false, \"stateless\":false }");
	mParams->addButton("Send OSC", std::bind(&ABPApp::sendOSC, this, std::placeholders::_1), "{ \"clear\":false, \"stateless\":false, \"pressed\":false }");
	mParams->addButton("Add brick", std::bind(&ABPApp::addBrick, this, std::placeholders::_1), "{ \"stateless\":false, \"pressed\":false }");
	// Toggle Slider
	mParams->addToggleSlider("Z Position", &mZPosition, "A", std::bind(&ABPApp::lockZ, this, std::placeholders::_1), "{ \"width\":156, \"clear\":false, \"min\": -1, \"max\": 1 }", "{ \"stateless\":false }");
	mParams->addToggleSlider("Rotation", &mRotation, "A", std::bind(&ABPApp::lockRotation, this, std::placeholders::_1), "{ \"width\":156, \"clear\":false, \"min\": 0, \"max\": 6.28 }", "{ \"stateless\":false }");
	mParams->addToggleSlider("Size", &mSize, "A", std::bind(&ABPApp::lockSize, this, std::placeholders::_1), "{ \"width\":156, \"clear\":false, \"min\": 0, \"max\": 6 }", "{ \"stateless\":false }");
	mParams->addToggleSlider("MotionVector", &mMotionVector, "A", std::bind(&ABPApp::lockMotionVector, this, std::placeholders::_1), "{ \"width\":156, \"clear\":false, \"min\": 0, \"max\": 6 }", "{ \"stateless\":false }");
	// -------- SPOUT -------------
	// Set up the texture we will use to send out
	// We grab the screen so it has to be the same size
	bSenderInitialized = false;
	spoutSenderTexture = gl::Texture::create(g_Width, g_Height);
	strcpy_s(SenderName, "ABP Spout Sender"); // we have to set a sender name first
	// Initialize a sender
	bSenderInitialized = spoutsender.CreateSender(SenderName, g_Width, g_Height);

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

void ABPApp::setRepetitions(const int &aRepetition, const bool &pressed)
{
	mRepetition += aRepetition;
	if (mRepetition < 1) mRepetition = 1;
	createPositions();
}

void ABPApp::createPositions()
{
	gl::VboMeshRef mesh = gl::VboMesh::create(geom::Sphere().subdivisions(4));

	// create an array of initial per-instance positions laid out in a 2D grid
	std::vector<vec3> positions;
	for (size_t potX = 0; potX < mRepetition; ++potX) {
		for (size_t potY = 0; potY < mRepetition; ++potY) {
			float instanceX = potX / (float)mRepetition - 0.5f;
			float instanceY = potY / (float)mRepetition - 0.5f;
			positions.push_back(vec3(instanceX * vec3(DRAW_SCALE, 0, 0) + instanceY * vec3(0, 0, DRAW_SCALE)));
		}
	}

	// create the VBO which will contain per-instance (rather than per-vertex) data
	mInstanceDataVbo = gl::Vbo::create(GL_ARRAY_BUFFER, positions.size() * sizeof(vec3), positions.data(), GL_DYNAMIC_DRAW);
	// we need a geom::BufferLayout to describe this data as mapping to the CUSTOM_0 semantic, and the 1 (rather than 0) as the last param indicates per-instance (rather than per-vertex)
	geom::BufferLayout instanceDataLayout;
	instanceDataLayout.append(geom::Attrib::CUSTOM_0, 3, 0, 0, 1 /* per instance */);

	// now add it to the VboMesh we already made of the Teapot
	mesh->appendVbo(instanceDataLayout, mInstanceDataVbo);

	// and finally, build our batch, mapping our CUSTOM_0 attribute to the "vInstancePosition" GLSL vertex attribute
	mBatch = gl::Batch::create(mesh, mGlsl, { { geom::Attrib::CUSTOM_0, "vInstancePosition" } });
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
		mUseCam = !mUseCam;
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
	mParameterBag->iChannelTime[3] = getElapsedSeconds() - 2;
	mParameterBag->iChannelTime[4] = getElapsedSeconds() - 3;
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

	mSpout->update();
	updateWindowTitle();
	mZPosition = mLockZ ? sin(getElapsedFrames() / 100.0f) : mZPosition;
	mRotation = mLockRotation ? sin(getElapsedFrames() / 100.0f)*4.0f : mRotation;
	mRotationMatrix *= rotate(0.06f, normalize(vec3(0.16666f, 0.333333f, 0.666666f)));

	updateBricks(timer);
	timer++;
	if (timer > bricks.size() - 1) {
		timer = 0;
	}
	mParams->update();
	updateWindowTitle();
	// move the camera up and down on Y
	mCam.lookAt(vec3(0, CAMERA_Y_RANGE.first + abs(sin(getElapsedSeconds() / 4)) * (CAMERA_Y_RANGE.second - CAMERA_Y_RANGE.first), 0), vec3(0));

	// update our instance positions; map our instance data VBO, write new positions, unmap
	vec3 *positions = (vec3*)mInstanceDataVbo->mapWriteOnly(true);
	for (size_t potX = 0; potX < mRepetition; ++potX) {
		for (size_t potY = 0; potY < mRepetition; ++potY) {
			float instanceX = potX / (float)mRepetition - 0.5f;
			float instanceY = potY / (float)mRepetition - 0.5f;
			// just some nonsense math to move the teapots in a wave
			vec3 newPos(instanceX * vec3(DRAW_SCALE, 0, 0) +
				instanceY * vec3(0, 0, DRAW_SCALE) +
				vec3(0, 30, 0) * sinf(getElapsedSeconds() * 3 + instanceX * 3 + instanceY * 3));
			*positions++ = newPos;
		}
	}
	mInstanceDataVbo->unmap();
}
void ABPApp::addBrick(const bool &pressed)
{
	for (int i = 0; i < mRepetition; i++) {
		brick newBrick;
		newBrick.r = mR + (i*mMotionVector);
		newBrick.g = mG + (i*mMotionVector);
		newBrick.b = mB + (i*mMotionVector);
		newBrick.a = mA;
		newBrick.x = mParameterBag->mRenderWidth / 2;
		newBrick.y = mParameterBag->mRenderHeight / 2;
		newBrick.shape = mShape;
		newBrick.size = mSize;
		newBrick.rotation = mRotation + (i*mMotionVector);
		newBrick.motionVector = mMotionVector;
		newBrick.repetition = mRepetition;
		bricks.push_back(newBrick);
	}
}
void ABPApp::updateBricks(int timer)
{
	if (newRecording == true) {
		newRendering();
	}
	r = bricks[timer].r;
	g = bricks[timer].g;
	b = bricks[timer].b;
	a = bricks[timer].a;
	rotation = bricks[timer].rotation++;
	repetitions = bricks[timer].repetition;
	distance = bricks[timer].motionVector * mSize;
	shape = bricks[timer].shape;
	gl::ScopedFramebuffer fbScp(mFbo);

	// clear out the FBO with red
	gl::clear(Color(0.3f, 0.0f, 0.0f));
	// setup the viewport to match the dimensions of the FBO
	gl::ScopedViewport scpVp(ivec2(0), mFbo->getSize());

	CameraPersp cam(mFbo->getWidth(), mFbo->getHeight(), 60.0f);
	cam.setPerspective(60, mFbo->getAspectRatio(), 1, 1000);
	cam.lookAt(vec3(2.8f, 1.8f, -2.8f), vec3(0));
	gl::setMatrices(cam);

	gl::ScopedGlslProg shaderScp(gl::getStockShader(gl::ShaderDef().color()));
	//gl::drawColorCube(vec3(0), vec3(2.2f));
	gl::color(Color::white());
	float new_x;
	float new_y;

	gl::scale(vec3(1.0) * mZoom);
	//gl::rotate(mRotation);
	// set the modelview matrix to reflect our current rotation
	//gl::setModelMatrix(mRotationMatrix);


	for (int j = 0; j < repetitions; j++)
	{
		//gl::color(ColorA(bricks[timer].r, bricks[timer].g, bricks[timer].b, bricks[timer].a));
		//gl::color(r, g, b, a);
		new_x = sin(rotation*0.01745329251994329576923690768489) * distance;
		new_y = cos(rotation*0.01745329251994329576923690768489) * distance;

		x = new_x + bricks[timer].x;
		y = new_y + bricks[timer].y;

		gl::translate(x, y);
		//gl::translate(i * 1.5f, 0.0f, mZPosition);
		//gl::rotate(rotation);
		//gl::pushModelView();

		//gl::translate(bricks[i].x + mXYSize.x, bricks[i].y + mXYSize.y, mZPosition);
		//gl::rotate(bricks[i].rotation);
		if (shape == 0) {
			//gl::drawCube(vec3(0.0), vec3(bricks[timer].size * mSize));
			gl::ScopedGlslProg shaderScp(gl::getStockShader(gl::ShaderDef().color()));

			gl::drawColorCube(vec3(0), vec3(2.2f));

		}
		if (shape == 1) {
			gl::drawSolidRect(Rectf(0, 0, bricks[timer].size * mSize, bricks[timer].size * mSize));
		}
		if (shape == 2) {
			gl::drawSolidCircle(vec2(0, 0), bricks[timer].size * mSize);
		}
		if (shape == 3) {
			vec2 dupa[3] = { vec2(0, bricks[timer].size * mSize), vec2(-bricks[timer].size * mSize, -bricks[timer].size * mSize), vec2(-bricks[timer].size * mSize, -bricks[timer].size * mSize) };
			gl::drawSolidTriangle(dupa);
		}
		//gl::popModelView();
	}

	gl::color(Color::white());

}
void ABPApp::draw()
{
	gl::clear();
	gl::color(Color::white());

	// draw textures
	//mSpout->draw();
	//mBatchass->mTextures->draw();
	// -------- SPOUT SENDER-------------
	/*if (bSenderInitialized) {

		// Grab the screen (current read buffer) into the local spout texture
		spoutSenderTexture->bind();
		mBatch->drawInstanced(mRepetition * mRepetition);
		glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, g_Width, g_Height);
		spoutSenderTexture->unbind();
		spoutsender.SendTexture(spoutSenderTexture->getId(), spoutSenderTexture->getTarget(), g_Width, g_Height);
		}
		gl::draw(spoutSenderTexture);*/
	if (mUseCam)
	{
		gl::setMatrices(mCam);
	}
	else
	{
		gl::setMatricesWindow(toPixels(getWindowSize()) * 2);
	}
	//gl::drawCube(vec3(0.0), vec3(bricks[0].size * mSize, bricks[0].size * mSize, 1.0f));
	//mBatch->drawInstanced(mRepetition * mRepetition);

	gl::draw(mFbo->getColorTexture(), Rectf(0, 0, mParameterBag->mRenderWidth, mParameterBag->mRenderHeight));
	//mFbo->bindTexture();

	// draw a cube textured with the FBO
	//{
	//	gl::ScopedGlslProg shaderScp(gl::getStockShader(gl::ShaderDef().texture()));
	//	//gl::drawSolidRect(Rectf(0, 0, mParameterBag->mRenderWidth, mParameterBag->mRenderHeight));
	//	gl::drawCube(vec3(0), vec3(22.2f));
	//}
	if (mParameterBag->mShowUI) mParams->draw();
	// show the FBO color texture in the upper left corner
	gl::setMatricesWindow(toPixels(getWindowSize()));
	gl::draw(mFbo->getColorTexture(), Rectf(0, 0, 128, 128));
	// and draw the depth texture adjacent
	gl::draw(mFbo->getDepthTexture(), Rectf(128, 0, 256, 128));

}
void ABPApp::shutdown()
{
	// save params
	mParameterBag->save();
	// close ui and save settings
	mSpout->shutdown();
}
#if defined( CINDER_MSW ) && ! defined( CINDER_GL_ANGLE )
auto options = RendererGl::Options().version(3, 3); // instancing functions are technically only in GL 3.3
#else
auto options = RendererGl::Options(); // implemented as extensions in Mac OS 10.7+
#endif
CINDER_APP_NATIVE(ABPApp, RendererGl(options))
