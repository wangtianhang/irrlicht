
#include "COpenGLESDriver.h"
#include "CIrrDeviceWin32.h"

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
	: CNullDriver(io, params.WindowSize)
{
#ifdef _DEBUG
	setDebugName("COpenGLDriver");
#endif
}

bool COpenGLESDriver::initDriver( CIrrDeviceWin32* device )
{
	return false;
}

}
}

