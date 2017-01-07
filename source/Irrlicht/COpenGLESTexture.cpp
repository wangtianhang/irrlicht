#include "IrrCompileConfig.h"

#include "irrTypes.h"

#include "COpenGLESTexture.h"
#include "COpenGLESDriver.h"
#include "os.h"
#include "CColorConverter.h"

#include "irrString.h"

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
			m_HasMipMaps = m_Driver->getTextureCreationFlag(ETCF_CREATE_MIP_MAPS);
			getImageValues(origImage);

			//glGenTextures(1, &TextureName);
		}

		COpenGLESTexture::~COpenGLESTexture()
		{

		}

		void COpenGLESTexture::getImageValues( IImage* image )
		{
			if (!image)
			{
				os::Printer::log("No image for OpenGL texture.", ELL_ERROR);
				return;
			}

			m_ImageSize = image->getDimension();

			if ( !m_ImageSize.Width || !m_ImageSize.Height)
			{
				os::Printer::log("Invalid size of image for OpenGL Texture.", ELL_ERROR);
				return;
			}

			const f32 ratio = (f32)m_ImageSize.Width/(f32)m_ImageSize.Height;
			if ((m_ImageSize.Width>m_Driver->m_MaxTextureSize) && (ratio >= 1.0f))
			{
				m_ImageSize.Width = m_Driver->m_MaxTextureSize;
				m_ImageSize.Height = (u32)(m_Driver->m_MaxTextureSize/ratio);
			}
			else if (m_ImageSize.Height>m_Driver->m_MaxTextureSize)
			{
				m_ImageSize.Height = m_Driver->m_MaxTextureSize;
				m_ImageSize.Width = (u32)(m_Driver->m_MaxTextureSize*ratio);
			}
			m_TextureSize=m_ImageSize.getOptimalSize(!m_Driver->queryFeature(EVDF_TEXTURE_NPOT));

			m_ColorFormat = getBestColorFormat(image->getColorFormat());
		}

		ECOLOR_FORMAT COpenGLESTexture::getBestColorFormat(ECOLOR_FORMAT format)
		{
			ECOLOR_FORMAT destFormat = ECF_A8R8G8B8;
			switch (format)
			{
			case ECF_A1R5G5B5:
				if (!m_Driver->getTextureCreationFlag(ETCF_ALWAYS_32_BIT))
					destFormat = ECF_A1R5G5B5;
				break;
			case ECF_R5G6B5:
				if (!m_Driver->getTextureCreationFlag(ETCF_ALWAYS_32_BIT))
					destFormat = ECF_A1R5G5B5;
				break;
			case ECF_A8R8G8B8:
				if (m_Driver->getTextureCreationFlag(ETCF_ALWAYS_16_BIT) ||
					m_Driver->getTextureCreationFlag(ETCF_OPTIMIZED_FOR_SPEED))
					destFormat = ECF_A1R5G5B5;
				break;
			case ECF_R8G8B8:
				if (m_Driver->getTextureCreationFlag(ETCF_ALWAYS_16_BIT) ||
					m_Driver->getTextureCreationFlag(ETCF_OPTIMIZED_FOR_SPEED))
					destFormat = ECF_A1R5G5B5;
			default:
				break;
			}
			if (m_Driver->getTextureCreationFlag(ETCF_NO_ALPHA_CHANNEL))
			{
				switch (destFormat)
				{
				case ECF_A1R5G5B5:
					destFormat = ECF_R5G6B5;
					break;
				case ECF_A8R8G8B8:
					destFormat = ECF_R8G8B8;
					break;
				default:
					break;
				}
			}
			return destFormat;
		}

	}
}
