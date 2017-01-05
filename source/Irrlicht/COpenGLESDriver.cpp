
#include "COpenGLESDriver.h"
#include "CIrrDeviceWin32.h"



#include "os.h"

namespace irr
{
namespace video
{

IVideoDriver* createOpenGLESDriver(const SIrrlichtCreationParameters& params,
	io::IFileSystem* io, CIrrDeviceWin32* device)
{
	COpenGLESDriver* ogl =  new COpenGLESDriver(params, io, device);
	if (!ogl->initDriver(device))
	{
		ogl->drop();
		ogl = 0;
	}
	return ogl;
}


COpenGLESDriver::COpenGLESDriver( const SIrrlichtCreationParameters& params, io::IFileSystem* io, CIrrDeviceWin32* device )
	: CNullDriver(io, params.WindowSize), 
	Params(params), 
	DeviceType(EIDT_WIN32),
	Window(static_cast<HWND>(params.WindowId))
{
#ifdef _DEBUG
	setDebugName("COpenGLDriver");
#endif
}

bool COpenGLESDriver::initDriver( CIrrDeviceWin32* device )
{
	HDc=GetDC(Window);

	return true;
}

bool COpenGLESDriver::beginScene( bool backBuffer/*=true*/, bool zBuffer/*=true*/, SColor color/*=SColor(255,0,0,0)*/, const SExposedVideoData& videoData/*=SExposedVideoData()*/, core::rect<s32>* sourceRect/*=0*/ )
{
	CNullDriver::beginScene(backBuffer, zBuffer, color, videoData, sourceRect);

	glClearColor(0.00f, 0.70f, 0.67f, 1.0f);

	return true;
}

bool COpenGLESDriver::endScene()
{
	CNullDriver::endScene();

	glFlush();

	if (DeviceType == EIDT_WIN32)
		return SwapBuffers(HDc) == TRUE;

	return false;
}

}
}

