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

	mMouseIndex = 0;
	timer = 0;
	isMouseDown = false;
	isRecording = false;
	mZoom = 0.3f;
	mXYSize = vec2(1.0);
	mRepetition = 1;
	mShape = 1;
	mZPosition = 0.0f;
	mRotation = 0.0f;
	mSize = 1.0f;
	mMotionVector = 1.0f;
	mLockZ = false;
	mLockRotation = false;
	mLockSize = false;
	mLockMotionVector = false;
	mR = mG = mB = mA = 0.2f;
	singleton::Instance()->presentIndex = 0;
	// init one brick
	/*singleton::brick newBrick;
	newBrick.r = 0.7f;
	newBrick.g = 0.0f;
	newBrick.b = 0.0f;
	newBrick.a = 0.7f;
	newBrick.shape = mShape;
	newBrick.size = 0.7f;
	newBrick.motionVector = 0.7f;
	newBrick.rotation = 0.7f;
	newBrick.repetition = 2.0f;

	singleton::Instance()->bricks.push_back(newBrick); */
	for (int i = 0; i<100; i++) {
		singleton::brick newBrick;
		newBrick.r = 1;
		newBrick.g = 1;
		newBrick.b = 1;
		newBrick.a = .5;
		newBrick.shape = 1;
		newBrick.size = i / 10;
		newBrick.rotation = 50;
		newBrick.motionVector = 1;
		newBrick.repetition = 1;
		singleton::Instance()->bricks.push_back(newBrick);
	}
	timer = 0;
//	gl::enableDepthRead();
//	gl::enableDepthWrite();

//	mCamera = CameraPersp(getWindowWidth(), getWindowHeight(), 60.0f, 1.0f, 1000.0f);
//	mCamera.lookAt(vec3(-2, 2, 2), vec3(0.0));

	mParams = MinimalUI::UIController::create();

	// 2D Sliders
	sliderXY = mParams->addSlider2D("XY", &mXYSize, "{ \"minX\":-2.0, \"maxX\":2.0, \"minY\":-2.0, \"maxY\":2.0 }");
	// Slider
	mParams->addSlider("Zoom", &mZoom);

	sliderRed = mParams->addSlider("R", &mR, "{ \"handleVisible\":false, \"nameColor\":\"0xEEFF0000\" }");
	sliderGreen = mParams->addSlider("G", &mG, "{ \"handleVisible\":false, \"nameColor\":\"0xEE00FF00\" }");
	sliderBlue = mParams->addSlider("B", &mB, "{  \"handleVisible\":false, \"nameColor\":\"0xEE0000FF\" }");
	sliderAlpha = mParams->addSlider("A", &mA, "{ \"handleVisible\":false, \"nameColor\":\"0x0xFFFFFFFF\" }");

	// Separator
	mParams->addSeparator();

	// Label
	mParams->addLabel("Repetitions", "{ \"clear\":false }");

	// Button Group
	mParams->addButton("1", std::bind(&ABPApp::setRepetition, this, 1, std::placeholders::_1), "{ \"clear\":false, \"stateless\":false, \"group\":\"count\", \"exclusive\":true, \"pressed\":true }");
	mParams->addButton("2", std::bind(&ABPApp::setRepetition, this, 2, std::placeholders::_1), "{ \"clear\":false, \"stateless\":false, \"group\":\"count\", \"exclusive\":true }");
	mParams->addButton("3", std::bind(&ABPApp::setRepetition, this, 3, std::placeholders::_1), "{ \"clear\":false, \"stateless\":false, \"group\":\"count\", \"exclusive\":true }");
	mParams->addButton("4", std::bind(&ABPApp::setRepetition, this, 4, std::placeholders::_1), "{ \"stateless\":false, \"group\":\"count\", \"exclusive\":true }");
	mParams->addButton("Record", std::bind(&ABPApp::record, this, std::placeholders::_1), "{ \"stateless\":false }");

	// Label
	mParams->addLabel("Shape", "{ \"clear\":false }");

	// Button Group
	mParams->addButton("1", std::bind(&ABPApp::setShape, this, 1, std::placeholders::_1), "{ \"clear\":false, \"stateless\":false, \"group\":\"count\", \"exclusive\":true, \"pressed\":true }");
	mParams->addButton("2", std::bind(&ABPApp::setShape, this, 2, std::placeholders::_1), "{ \"clear\":false, \"stateless\":false, \"group\":\"count\", \"exclusive\":true }");
	mParams->addButton("3", std::bind(&ABPApp::setShape, this, 3, std::placeholders::_1), "{ \"clear\":false, \"stateless\":false, \"group\":\"count\", \"exclusive\":true }");
	mParams->addButton("4", std::bind(&ABPApp::setShape, this, 4, std::placeholders::_1), "{ \"stateless\":false, \"group\":\"count\", \"exclusive\":true }");
	mParams->addButton("Record", std::bind(&ABPApp::record, this, std::placeholders::_1), "{ \"stateless\":false }");
	// Toggle Slider
	mParams->addToggleSlider("Z Position", &mZPosition, "A", std::bind(&ABPApp::lockZ, this, std::placeholders::_1), "{ \"width\":156, \"clear\":false, \"min\": -1, \"max\": 1 }", "{ \"stateless\":false }");
	mParams->addToggleSlider("Rotation", &mRotation, "A", std::bind(&ABPApp::lockRotation, this, std::placeholders::_1), "{ \"width\":156, \"clear\":false, \"min\": 0, \"max\": 6.28 }", "{ \"stateless\":false }");
	mParams->addToggleSlider("Size", &mSize, "A", std::bind(&ABPApp::lockSize, this, std::placeholders::_1), "{ \"width\":156, \"clear\":false, \"min\": 0, \"max\": 6 }", "{ \"stateless\":false }");
	mParams->addToggleSlider("MotionVector", &mMotionVector, "A", std::bind(&ABPApp::lockMotionVector, this, std::placeholders::_1), "{ \"width\":156, \"clear\":false, \"min\": 0, \"max\": 6 }", "{ \"stateless\":false }");
	gl::enableAlphaBlending();
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
	/*sliderRed->setBackgroundColor(ColorA(mR, 0, 0));
	sliderGreen->setBackgroundColor(ColorA(0, mG, 0));
	sliderBlue->setBackgroundColor(ColorA(0, 0, mB));
	sliderAlpha->setBackgroundColor(ColorA(mR, mG, mB, mA));
	if (isRecording)
	{
		singleton::brick newBrick;
		newBrick.r = mR;
		newBrick.g = mG;
		newBrick.b = mB;
		newBrick.a = mA;
		newBrick.rotation = mRotation;
		newBrick.shape = mShape;
		newBrick.size = mSize;
		newBrick.motionVector = mMotionVector;
		newBrick.repetition = mRepetition;

		singleton::Instance()->bricks.push_back(newBrick);
		mOSC->sendOSCMessage("brick", singleton::Instance()->bricks.size() - 1, newBrick.r, newBrick.g, newBrick.b, newBrick.a, newBrick.rotation);
		neRender.update();
		timer++;
		if (timer > singleton::Instance()->bricks.size() - 1) timer = 0;
	}*/
	neRender.update(timer);
	timer++;
	if (timer>99) {
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
	//gl::clear();

	neRender.draw();
	
//	gl::setMatrices(mCamera);

	//	gl::pushModelView();
	//	gl::scale(vec3(1.0) * mZoom);
	//	gl::rotate(mRotation);
	/*

	for (int i = 0; i < singleton::Instance()->bricks.size(); i++)
	{
		gl::color(ColorA(singleton::Instance()->bricks[i].r, singleton::Instance()->bricks[i].g, singleton::Instance()->bricks[i].b, singleton::Instance()->bricks[i].a));
		gl::pushModelView();
		gl::translate(i * 1.5f, 0.0f, mZPosition);
		gl::rotate(singleton::Instance()->bricks[i].rotation);
		gl::drawCube(vec3(0.0), vec3(mXYSize, 1.0f));
		gl::popModelView();
	}
	gl::popModelView();
	//! touch events only make sense on the UI
	for (map<uint32_t, TouchPoint>::const_iterator activeIt = mActivePoints.begin(); activeIt != mActivePoints.end(); ++activeIt) {
		activeIt->second.draw();
	}*/

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
		gl::drawStrokedCircle(touchIt->getPos(), 20.0f);
	mParams->draw();
	
}

// This line tells Cinder to actually create the application
CINDER_APP_NATIVE(ABPApp, RendererGl)
