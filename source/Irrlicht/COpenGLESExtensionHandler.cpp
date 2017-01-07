#include "IrrCompileConfig.h"

#include "COpenGLESExtensionHandler.h"

#include "irrString.h"
#include "SMaterial.h" // for MATERIAL_MAX_TEXTURES
#include "fast_atof.h"

namespace irr
{
	namespace video
	{
		COpenGLESExtensionHandler::COpenGLESExtensionHandler():
			m_MaxTextureSize(1),
			m_MaxSupportedTextures(0)
		{

		}

		void COpenGLESExtensionHandler::initExtensions(bool stencilBuffer)
		{
			const f32 ogl_ver = core::fast_atof(reinterpret_cast<const c8*>(glGetString(GL_VERSION)));
			m_Version = static_cast<u16>(core::floor32(ogl_ver)*100+core::round32(core::fract(ogl_ver)*10.0f));
			if ( m_Version >= 102)
				os::Printer::log("OpenGL driver version is 1.2 or better.", ELL_INFORMATION);
			else
				os::Printer::log("OpenGL driver version is not 1.2 or better.", ELL_WARNING);

			GLint num=0;
			glGetIntegerv(GL_MAX_TEXTURE_SIZE, &num);
			m_MaxTextureSize=static_cast<u32>(num);

			num=0;
			glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &num);
			m_MaxSupportedTextures=core::max_(m_MaxSupportedTextures,static_cast<u8>(num));

		}

		bool COpenGLESExtensionHandler::queryFeature(E_VIDEO_DRIVER_FEATURE feature) const
		{
			return true;
		}

	}
}

