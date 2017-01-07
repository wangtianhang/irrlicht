#include "COpenGLESExtensionHandler.h"

namespace irr
{
	namespace video
	{
		COpenGLESExtensionHandler::COpenGLESExtensionHandler():
			m_MaxTextureSize(1)
		{

		}

		void COpenGLESExtensionHandler::initExtensions(bool stencilBuffer)
		{
			GLint num=0;
			glGetIntegerv(GL_MAX_TEXTURE_SIZE, &num);
			m_MaxTextureSize=static_cast<u32>(num);
		}

	}
}

