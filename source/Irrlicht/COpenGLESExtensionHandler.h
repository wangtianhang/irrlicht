#ifndef _COpenGLESExtensionHandler_
#define _COpenGLESExtensionHandler_

#include "IrrCompileConfig.h"

#include "EDriverFeatures.h"
#include "irrTypes.h"
#include "os.h"

#define GL_GLEXT_PROTOTYPES
#include "opengles/EGL/egl.h"
#include "opengles/GLES2/gl2.h"

#ifdef _MSC_VER
#pragma comment(lib, "opengles/x86_32/libEGL.lib")
#pragma comment(lib, "opengles/x86_32/libGLESv2.lib")
#endif

namespace irr
{
	namespace video
	{

	class COpenGLESExtensionHandler
	{
	public:
		// constructor
		COpenGLESExtensionHandler();

		// deferred initialization
		void initExtensions(bool stencilBuffer);

		//! OpenGL version as Integer: 100*Major+Minor, i.e. 2.1 becomes 201
		u16 m_Version;

		//! Maximal texture dimension
		u32 m_MaxTextureSize;

		//! Maxmimum texture layers supported by the fixed pipeline
		u8 m_MaxSupportedTextures;
	};

	}
}

#endif