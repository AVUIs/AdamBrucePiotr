#include <string>
#include <vector>

#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/ImageIo.h"
#include "cinder/Rand.h"

#include "Warp.h"
// Settings
#include "VDSettings.h"
// Session
#include "VDSession.h"
// Utils
#include "VDUtils.h"
// Animation
#include "VDAnimation.h"
// Message router
#include "VDRouter.h"

using namespace ci;
using namespace ci::app;
using namespace ph::warping;
using namespace std;
using namespace VideoDromm;


#define IM_ARRAYSIZE(_ARR)			((int)(sizeof(_ARR)/sizeof(*_ARR)))
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
const pair<float, float> CAMERA_Y_RANGE(32.0f, 80.0f);

class BatchassSpidermoonApp : public App {

public:
	static void					prepare(Settings *settings);

	void						setup() override;
	void						cleanup() override;
	void						update() override;
	void						draw() override;

	void						resize() override;

	void						mouseMove(MouseEvent event) override;
	void						mouseDown(MouseEvent event) override;
	void						mouseDrag(MouseEvent event) override;
	void						mouseUp(MouseEvent event) override;

	void						keyDown(KeyEvent event) override;
	void						keyUp(KeyEvent event) override;

	void						updateWindowTitle();
private:
	// Settings
	VDSettingsRef				mVDSettings;
	// Session
	VDSessionRef				mVDSession;
	// Utils
	VDUtilsRef					mVDUtils;
	// Animation
	VDAnimationRef				mVDAnimation;
	// Message router
	VDRouterRef					mVDRouter;
	// shaders
	gl::GlslProgRef				aShader;

	fs::path					mSettings;

	WarpList					mWarps;
	bool						mFadeInDelay;
	bool						mFadeOutDelay;
	gl::TextureRef				mImage;

	// fbo
	void						renderSceneToFbo();
	gl::FboRef					mRenderFbo;
	// abp
	void						setShape(const int &aShape, const bool &pressed) { mShape = aShape; }
	void						addBrick(const bool &pressed);
	CameraPersp					mCam;
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
	//mat4						mRotationMatrix;
	float						mColorFactor;

	void						newRendering();
	vector <brick>				bricks;
	bool						alreadyCreated;
	float						mouseX, mouseY;
	
};
