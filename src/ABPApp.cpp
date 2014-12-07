#include "ABPApp.h"


void ABPApp::setup()
{
	// parameters
	mParameterBag = ParameterBag::create();
	// instanciate the OSC class
	mOSC = OSC::create(mParameterBag);

	mMouseIndex = 0;
	isMouseDown = false;
	mZoom = 1.0f;
    mXYSize = vec2(1.0);
    mCount = 1;
    mZPosition = 0.0f;
    mLockZ = false;
	mR = mG = mB = mA = 1.0f;
    
    gl::enableDepthRead();
    gl::enableDepthWrite();
    
    mCamera = CameraPersp( getWindowWidth(), getWindowHeight(), 60.0f, 1.0f, 1000.0f );
    mCamera.lookAt(vec3(-2, 2, 2), vec3(0.0));
    
    mParams = MinimalUI::UIController::create();
    
    // 2D Sliders
	sliderXY = mParams->addSlider2D("XY", &mXYSize, "{ \"minX\":-2.0, \"maxX\":2.0, \"minY\":-2.0, \"maxY\":2.0 }");
    // Slider
    mParams->addSlider( "Zoom", &mZoom );
    
	sliderRed = mParams->addToggleSlider("R", &mR, "a", std::bind(&ABPApp::lockFR, this, std::placeholders::_1), "{ \"width\":156, \"clear\":false, \"handleVisible\":false, \"vertical\":true, \"nameColor\":\"0xEEFF0000\" }", "{ \"width\":9, \"stateless\":false, \"group\":\"fr\", \"exclusive\":true }");
	//sliderRed = mParams->addSlider("R", &mR, "{ \"handleVisible\":false, \"nameColor\":\"0xEEFF0000\" }");
	sliderGreen = mParams->addSlider("G", &mG, "{ \"handleVisible\":false, \"nameColor\":\"0xEE00FF00\" }");
	sliderBlue = mParams->addSlider("B", &mB, "{  \"handleVisible\":false, \"nameColor\":\"0xEE0000FF\" }");
	sliderAlpha = mParams->addSlider("A", &mA, "{ \"handleVisible\":false, \"nameColor\":\"0x0xFFFFFFFF\" }");

    // Simple Button
    mParams->addButton( "Stateless!", std::bind( &ABPApp::buttonCallback, this, std::placeholders::_1 ), "{ \"width\":96, \"clear\":false }" );
    mParams->addButton( "Stateful!", std::bind( &ABPApp::buttonCallback, this, std::placeholders::_1 ), "{ \"width\":96, \"stateless\":false }" );
    
    // Separator
    mParams->addSeparator();
    
    // Label
    mParams->addLabel( "Count", "{ \"clear\":false }" );
    
    // Button Group
    mParams->addButton( "1", std::bind( &ABPApp::setCount, this, 1, std::placeholders::_1 ), "{ \"clear\":false, \"stateless\":false, \"group\":\"count\", \"exclusive\":true, \"pressed\":true }" );
    mParams->addButton( "2", std::bind( &ABPApp::setCount, this, 2, std::placeholders::_1 ), "{ \"clear\":false, \"stateless\":false, \"group\":\"count\", \"exclusive\":true }" );
    mParams->addButton( "3", std::bind( &ABPApp::setCount, this, 3, std::placeholders::_1 ), "{ \"stateless\":false, \"group\":\"count\", \"exclusive\":true }" );
    
    // Toggle Slider
    mParams->addToggleSlider( "Z Position", &mZPosition, "A", std::bind(&ABPApp::lockZ, this, std::placeholders::_1 ), "{ \"width\":156, \"clear\":false, \"min\": -1, \"max\": 1 }", "{ \"stateless\":false }" );
}

void ABPApp::mouseDown(MouseEvent event)
{
	isMouseDown = true;
	mActivePoints.insert(make_pair(0, TouchPoint(event.getPos(), Color(mR, mG, mB))));
}
void ABPApp::mouseMove(MouseEvent event)
{
	
}

void ABPApp::mouseDrag(MouseEvent event)
{
	isMouseDown = true;
	mActivePoints[0].addPoint(event.getPos());
}

void ABPApp::mouseUp(MouseEvent event)
{
	isMouseDown = false;
	mActivePoints[0].startDying();
	mDyingPoints.push_back(mActivePoints[0]);
	mActivePoints.erase(0);
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
    mZPosition = mLockZ ? sin( getElapsedFrames() / 100.0f ) : mZPosition;
	/*sliderRed->setBackgroundColor(ColorA(mR, 0, 0));
	sliderGreen->setBackgroundColor(ColorA(0, mG, 0));
	sliderBlue->setBackgroundColor(ColorA(0, 0, mB));
	sliderAlpha->setBackgroundColor(ColorA(mR, mG, mB, mA));*/
    mParams->update();
}

void ABPApp::draw()
{
    gl::clear();
    
    gl::setMatrices( mCamera );
    
    gl::pushModelView();
    gl::scale( vec3(1.0) * mZoom );
	gl::color(ColorA(mR, mG, mB, mA));
    for ( int i = 0; i < mCount; i++ )
    {
        gl::pushModelView();
        gl::translate( i * 1.5f, 0.0f, mZPosition );
		gl::drawCube(vec3(0.0), vec3( mXYSize, 1.0f ));
        gl::popModelView();
    }
    gl::popModelView();
	//! touch events only make sense on the UI
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
		gl::drawStrokedCircle(touchIt->getPos(), 20.0f);
	gl::disableAlphaBlending();
    mParams->draw();

}

void ABPApp::buttonCallback( const bool &pressed )
{
    console() << "Hello! Button state: " << pressed << endl;
}

// This line tells Cinder to actually create the application
CINDER_APP_NATIVE( ABPApp, RendererGl )
