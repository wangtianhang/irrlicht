#include "IrrCompileConfig.h"

#include "irrTypes.h"

#include "COpenGLESTexture.h"

namespace irr
{
	namespace video
	{
		COpenGLESTexture::COpenGLESTexture( IImage* origImage, const io::path& name, void* mipmapData/*=0*/, COpenGLESDriver* driver/*=0*/ )
			: ITexture(name), m_Driver(driver)
		{
#ifdef _DEBUG
			setDebugName("COpenGLTexture");
#endif
			//m_HasMipMaps = m_Driver->getTextureCreationFlag(ETCF_CREATE_MIP_MAPS);
			getImageValues(origImage);

			//glGenTextures(1, &TextureName);
		}

		COpenGLESTexture::~COpenGLESTexture()
		{

		}

		void COpenGLESTexture::getImageValues( IImage* image )
		{

		}

	}
}
