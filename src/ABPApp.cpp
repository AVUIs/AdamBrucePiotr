#include "ABPApp.h"

void ABPApp::prepareSettings(Settings *settings)
{
	settings->setWindowSize(1024, 600);
	settings->setWindowPos(30, 50);

	// set a high frame rate to disable limitation
	settings->setFrameRate(1000.0f);
}
void ABPApp::setup()
{
	// parameters
	mParameterBag = ParameterBag::create();
	// instanciate the OSC class
	mOSC = OSC::create(mParameterBag);
	mSendOSC = false;
	// neRenderer
	x = 512;
	y = 300;
	gl::Fbo::Format format;
	myFbo = gl::Fbo::create(1024, 600, format.depthTexture());


	mMouseIndex = 0;
	timer = 0;
	isMouseDown = false;
	isRecording = false;
	mZoom = 0.3f;
	mXYSize = vec2(1.0);
	mRepetition = 10;
	mShape = 0;
	mZPosition = 0.0f;
	mRotation = 0.0f;
	mSize = 1.0f;
	mMotionVector = 0.02f;
	mLockZ = false;
	mLockRotation = false;
	mLockSize = false;
	mLockMotionVector = false;
	mR = mG = mB = mA = 0.2f;
	presentIndex = 0;
	// init one brick
	addBrick(true);
	timer = 0;
	// gl setup
	gl::enableDepthRead();
	gl::enableDepthWrite();
	gl::enableAlphaBlending();

	mCamera = CameraPersp(getWindowWidth(), getWindowHeight(), 60.0f, 1.0f, 1000.0f);
	mCamera.lookAt(vec3(-2, 2, 2), vec3(0.0));

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
	mParams->addButton("0", std::bind(&ABPApp::setShape, this, 0, std::placeholders::_1), "{ \"clear\":false, \"stateless\":false, \"group\":\"shape\", \"exclusive\":true, \"pressed\":true }");
	mParams->addButton("1", std::bind(&ABPApp::setShape, this, 1, std::placeholders::_1), "{ \"clear\":false, \"stateless\":false, \"group\":\"shape\", \"exclusive\":true }");
	mParams->addButton("2", std::bind(&ABPApp::setShape, this, 2, std::placeholders::_1), "{ \"clear\":false, \"stateless\":false, \"group\":\"shape\", \"exclusive\":true }");
	mParams->addButton("3", std::bind(&ABPApp::setShape, this, 3, std::placeholders::_1), "{  \"stateless\":false, \"group\":\"shape\", \"exclusive\":true }");
	// Repetitions
	mParams->addSlider("Repetitions", &mRepetition, "{ \"min\": 1, \"max\": 600 }");
	mParams->addButton("Record", std::bind(&ABPApp::record, this, std::placeholders::_1), "{ \"clear\":false, \"stateless\":false }");
	mParams->addButton("Send OSC", std::bind(&ABPApp::sendOSC, this, std::placeholders::_1), "{ \"clear\":false, \"stateless\":false, \"pressed\":false }");
	mParams->addButton("Add brick", std::bind(&ABPApp::addBrick, this, std::placeholders::_1), "{ \"stateless\":false, \"pressed\":false }");
	// Toggle Slider
	mParams->addToggleSlider("Z Position", &mZPosition, "A", std::bind(&ABPApp::lockZ, this, std::placeholders::_1), "{ \"width\":156, \"clear\":false, \"min\": -1, \"max\": 1 }", "{ \"stateless\":false }");
	mParams->addToggleSlider("Rotation", &mRotation, "A", std::bind(&ABPApp::lockRotation, this, std::placeholders::_1), "{ \"width\":156, \"clear\":false, \"min\": 0, \"max\": 6.28 }", "{ \"stateless\":false }");
	mParams->addToggleSlider("Size", &mSize, "A", std::bind(&ABPApp::lockSize, this, std::placeholders::_1), "{ \"width\":156, \"clear\":false, \"min\": 0, \"max\": 6 }", "{ \"stateless\":false }");
	mParams->addToggleSlider("MotionVector", &mMotionVector, "A", std::bind(&ABPApp::lockMotionVector, this, std::placeholders::_1), "{ \"width\":156, \"clear\":false, \"min\": 0, \"max\": 6 }", "{ \"stateless\":false }");
}
void ABPApp::addBrick(const bool &pressed)
{
	for (int i = 0; i < mRepetition; i++) {
		brick newBrick;
		newBrick.r = mR + (i*mMotionVector);
		newBrick.g = mG + (i*mMotionVector);
		newBrick.b = mB + (i*mMotionVector);
		newBrick.a = mA;
		newBrick.shape = mShape;
		newBrick.size = mSize;
		newBrick.rotation = mRotation + (i*mMotionVector);
		newBrick.motionVector = mMotionVector;
		newBrick.repetition = mRepetition;
		bricks.push_back(newBrick);
	}
}
void ABPApp::newRendering() {
	gl::clear();
}

void ABPApp::updateBricks(int timer) {
	if (newRecording == true) {
		newRendering();
	}
	r = bricks[timer].r;
	g = bricks[timer].g;
	b = bricks[timer].b;
	a = bricks[timer].a;
	rotation = bricks[timer].rotation;
	size = bricks[timer].size;
	repetitions = bricks[timer].repetition;
	shape = bricks[timer].shape;
	distance = bricks[timer].motionVector * size;

	gl::ScopedFramebuffer fbScp(myFbo);
	// clear out the FBO with red
	gl::clear(Color(1.0f, 0.5f, 0.25f));

	// setup the viewport to match the dimensions of the FBO
	gl::ScopedViewport scpVp(ivec2(0), myFbo->getSize());

	float new_x;
	float new_y;

	gl::color(0, 0, 0, 0.0039215686274509803921568627451);
	gl::drawSolidRect(Rectf(0, 0, 1024, 600));
	for (int i = 0; i<repetitions; i++) {

		new_x = sin(rotation*0.01745329251994329576923690768489) * distance;
		new_y = cos(rotation*0.01745329251994329576923690768489) * distance;
		gl::color(r, g, b, a);
		x = new_x + x;
		y = new_y + y;


		if (x >= 1024) {
			x -= 1024;
		};

		if (y >= 600) {
			y -= 600;
		}

		if (x<0) {
			x += 1024;
		}

		if (y<0) {
			y += 600;
		}

		//gl::pushModelView();
		gl::translate(x, y);
		//gl::translate(i * 1.5f, 0.0f, mZPosition);
		//gl::rotate(rotation);
		if (shape == 0) {
			gl::drawCube(vec3(0.0), vec3(mSize, mSize, 1.0f));
		}
		if (shape == 1) {
			gl::drawSolidRect(Rectf(0, 0, size * 10, size * 10));
		}
		if (shape == 2) {
			gl::drawSolidCircle(vec2(0, 0), size * 10);
		}
		if (shape == 3) {
			vec2 dupa[3] = { vec2(0, size * 10), vec2(-size * 10, -size * 10), vec2(-size * 10, -size * 10) };
			gl::drawSolidTriangle(dupa);
		}
		//gl::popModelView();

	}
}
void ABPApp::mouseDown(MouseEvent event)
{
	isMouseDown = true;
	mActivePoints.insert(make_pair(mMouseIndex, TouchPoint(event.getPos(), Color(mR, mG, mB))));
}
void ABPApp::mouseMove(MouseEvent event)
{

}

void ABPApp::mouseDrag(MouseEvent event)
{
	isMouseDown = true;
	mActivePoints[mMouseIndex].addPoint(event.getPos());
}

void ABPApp::mouseUp(MouseEvent event)
{
	isMouseDown = false;
	mActivePoints[mMouseIndex].startDying();
	mDyingPoints.push_back(mActivePoints[mMouseIndex]);
	mActivePoints.erase(mMouseIndex);
}
void ABPApp::touchesBegan(TouchEvent event)
{
	console() << "Began: " << event << std::endl;
	for (vector<TouchEvent::Touch>::const_iterator touchIt = event.getTouches().begin(); touchIt != event.getTouches().end(); ++touchIt) {
		mActivePoints.insert(make_pair(touchIt->getId(), TouchPoint(touchIt->getPos(), Color(mR, mG, mB))));
	}
}

void ABPApp::touchesMoved(TouchEvent event)
{
	console() << "Moved: " << event << std::endl;
	for (vector<TouchEvent::Touch>::const_iterator touchIt = event.getTouches().begin(); touchIt != event.getTouches().end(); ++touchIt)
		mActivePoints[touchIt->getId()].addPoint(touchIt->getPos());
}

void ABPApp::touchesEnded(TouchEvent event)
{
	console() << "Ended: " << event << std::endl;
	for (vector<TouchEvent::Touch>::const_iterator touchIt = event.getTouches().begin(); touchIt != event.getTouches().end(); ++touchIt) {
		mActivePoints[touchIt->getId()].startDying();
		mDyingPoints.push_back(mActivePoints[touchIt->getId()]);
		mActivePoints.erase(touchIt->getId());
	}
}

void ABPApp::update()
{
	mZPosition = mLockZ ? sin(getElapsedFrames() / 100.0f) : mZPosition;
	mRotation = mLockRotation ? sin(getElapsedFrames() / 100.0f)*4.0f : mRotation;
	/*sliderRed->setBackgroundColor(ColorA(mR, 0, 0, 1.0));
	sliderGreen->setBackgroundColor(ColorA(0, mG, 0));
	sliderBlue->setBackgroundColor(ColorA(0, 0, mB));
	sliderAlpha->setBackgroundColor(ColorA(mR, mG, mB, mA));
	*/
	updateBricks(timer);
	timer++;
	if (timer>bricks.size()-1) {
		timer = 0;
	}
	mParams->update();
}
void ABPApp::record(const bool &pressed)
{
	mRotation++;
	isRecording = !isRecording;
}

void ABPApp::draw()
{
	gl::clear();
	gl::color(Color(1, 1, 1));

	gl::draw(myFbo->getColorTexture());

	gl::setMatrices(mCamera);

	gl::scale(vec3(1.0) * mZoom);
	gl::rotate(mRotation);
	
	for (int i = 0; i < bricks.size(); i++)
	{
		gl::color(ColorA(bricks[i].r, bricks[i].g, bricks[i].b, bricks[i].a));
		gl::pushModelView();
		gl::translate(i * 1.5f, 0.0f, mZPosition);
		gl::rotate(bricks[i].rotation);
		gl::drawCube(vec3(0.0), vec3(mXYSize, 1.0f));
		gl::popModelView();
	}
	/*
	//! touch events on the UI
	for (map<uint32_t, TouchPoint>::const_iterator activeIt = mActivePoints.begin(); activeIt != mActivePoints.end(); ++activeIt) {
		activeIt->second.draw();
	}
	for (list<TouchPoint>::iterator dyingIt = mDyingPoints.begin(); dyingIt != mDyingPoints.end();) {
		dyingIt->draw();
		if (dyingIt->isDead())
			dyingIt = mDyingPoints.erase(dyingIt);
		else
			++dyingIt;
	}

	//! draw yellow circles at the active touch points
	gl::color(Color(1, 1, 0));
	for (vector<TouchEvent::Touch>::const_iterator touchIt = getActiveTouches().begin(); touchIt != getActiveTouches().end(); ++touchIt)
		gl::drawStrokedCircle(touchIt->getPos(), 20.0f);*/
	mParams->draw();
}

// This line tells Cinder to actually create the application
CINDER_APP_NATIVE(ABPApp, RendererGl)
