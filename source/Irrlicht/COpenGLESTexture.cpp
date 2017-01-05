#include "COpenGLESTexture.h"

namespace irr
{
	namespace video
	{
		COpenGLESTexture::COpenGLESTexture( IImage* origImage, const io::path& name, void* mipmapData/*=0*/, COpenGLESDriver* driver/*=0*/ )
			: ITexture(name)
		{
#ifdef _DEBUG
			setDebugName("COpenGLTexture");
#endif
			getImageValues(origImage);

			glGenTextures(1, &TextureName);
		}

		COpenGLESTexture::~COpenGLESTexture()
		{

		}

		void COpenGLESTexture::getImageValues( IImage* image )
		{

		}

	}
}
