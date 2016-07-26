#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

// Settings
#include "VDSettings.h"
// Session
#include "VDSession.h"
// Animation
#include "VDAnimation.h"
// Utils
#include "VDUtils.h"
// Textures
#include "VDTexture.h"
// Message router
#include "VDRouter.h"

#if defined( CINDER_MSW )
// spout
#include "Spout.h"
#endif

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace VideoDromm;

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
const float DRAW_SCALE = 200;
const pair<float, float> CAMERA_Y_RANGE(32.0f, 80.0f);



class ABPApp : public App {

public:
	static void					prepare(Settings *settings);
	void						setup() override;
	void						update() override;
	void						draw() override;
	void						fileDrop(FileDropEvent event) override;
	void						cleanup() override;
	void						resize();
	void						shutdown();
	void						updateWindowTitle();
	// mouse events
	void						mouseDown(MouseEvent event) override;
	void						mouseDrag(MouseEvent event) override;
	void						mouseUp(MouseEvent event);
	void						keyDown(KeyEvent event);

	void						setShape(const int &aShape, const bool &pressed) { mShape = aShape; }
	void						lockZ(const bool &pressed) { mLockZ = pressed; }
	void						lockRepetitions(const bool &pressed) { mLockRepetitions = pressed; }
	void						lockRotation(const bool &pressed) { mLockRotation = pressed; }
	void						lockMotionVector(const bool &pressed) { mLockMotionVector = pressed; }
	void						lockSize(const bool &pressed) { mLockSize = pressed; }
	void						lockBend(const bool &pressed) { mLockBend = pressed; }

	bool						mLockFR;
	void						lockFR(const bool &pressed) { mLockFR = pressed; };
	void						sendOSC(const bool &pressed) { mSendOSC = pressed; };
	void						addBrick(const bool &pressed);
	void						setGlobalMode(const bool &pressed);
	void						reset(const bool &pressed);
	CameraPersp					mCam; 
private:
	// Settings
	VDSettingsRef				mVDSettings;
	// Session
	VDSessionRef				mVDSession;
	// Log
	VDLogRef					mVDLog;
	// Utils
	VDUtilsRef					mVDUtils;
	// Animation
	VDAnimationRef				mVDAnimation;
	// Message router
	VDRouterRef					mVDRouter;

	VDTextureList				mTexs;
	fs::path					mTexturesFilepath;

	// ABP
	string						mLogMsg;
	bool						newLogMsg;
	static const int			MAX = 16;
	bool						mUseCam;
	bool						mSendOSC;
	float						mR, mG, mB, mA;
	float						mZoom;
	float						mBend;
	vec2						mXYVector;
	int							mRepetitions;
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

	int							mMouseIndex;
	bool						isMouseDown;
	// recording
	bool						isRecording;
	int							presentIndex;
	void						record(const bool &pressed);
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
	gl::FboRef					mFbo;
	mat4						mRotationMatrix;
	float						mColorFactor;

	void						newRendering();
	void						updateBricks();
	vector <brick>				bricks;
	bool						newRecording;
	// -------- SPOUT -------------
#if defined( CINDER_MSW )
	SpoutSender					spoutsender;                    // Create a Spout sender object
	bool						bSenderInitialized;             // true if a sender initializes OK
	bool						bMemoryMode;                    // tells us if texture share compatible
	unsigned int				g_Width, g_Height;              // size of the texture being sent out
	char						SenderName[256];                // sender name 
	gl::TextureRef				spoutSenderTexture;             // Local Cinder texture used for sharing
#endif	
	// window management
	bool						mFadeInDelay;
	bool						mFadeOutDelay;

};

