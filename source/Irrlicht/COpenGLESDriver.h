#ifndef __C_VIDEO_OPEN_GLES_H_INCLUDED__
#define __C_VIDEO_OPEN_GLES_H_INCLUDED__

//#include <Windows.h>

#include "IrrCompileConfig.h"

#include "SIrrCreationParameters.h"


#include "CNullDriver.h"
#include "IMaterialRendererServices.h"

#define GL_GLEXT_PROTOTYPES
#include "opengles/EGL/egl.h"
#include "opengles/GLES2/gl2.h"

//#include "SIrrCreationParameters.h"

#ifdef _MSC_VER
#pragma comment(lib, "opengles/x86_32/libEGL.lib")
#pragma comment(lib, "opengles/x86_32/libGLESv2.lib")
#endif

namespace irr
{
	class CIrrDeviceWin32;
}

namespace irr
{

namespace video
{

	class COpenGLESDriver : public CNullDriver
	{
	public:
		COpenGLESDriver(const SIrrlichtCreationParameters& params, io::IFileSystem* io, CIrrDeviceWin32* device);
		bool initDriver(CIrrDeviceWin32* device);

		//! destructor
		virtual ~COpenGLESDriver();

		//! clears the zbuffer
		virtual bool beginScene(bool backBuffer=true, bool zBuffer=true,
			SColor color=SColor(255,0,0,0),
			const SExposedVideoData& videoData=SExposedVideoData(),
			core::rect<s32>* sourceRect=0);

		//! presents the rendered scene on the screen, returns false if failed
		virtual bool endScene();

	private:
		bool genericDriverInit();

		void createMaterialRenderers();

		SIrrlichtCreationParameters m_Params;
		HDC m_HDc; // Private GDI Device Context
		HWND m_Window;
		E_DEVICE_TYPE m_DeviceType;

		EGLDisplay			m_eglDisplay;
		EGLSurface			m_eglSurface;
	};

}

}

#endif