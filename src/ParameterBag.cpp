#include "ParameterBag.h"

using namespace ci;
using namespace std;
using namespace ABP;

ParameterBag::ParameterBag()
{
	// reset no matter what, so we don't miss anything
	reset();
}

ParameterBagRef ParameterBag::create()
{
	return shared_ptr<ParameterBag>(new ParameterBag());
}


void ParameterBag::reset()
{
	// OSC
	mOSCDestinationHost = "127.0.0.1";
	mOSCDestinationPort = 7000;
	mOSCReceiverPort = 9000;
	OSCMsg = "OSC listening on port 9000";

}
