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
			: ITexture(name), 
			m_Driver(driver),
			m_KeepImage(true),
			m_InternalFormat(GL_RGBA),
			m_PixelFormat(GL_RGBA),
			m_PixelType(GL_UNSIGNED_BYTE)
		{
#ifdef _DEBUG
			setDebugName("COpenGLTexture");
#endif
			m_HasMipMaps = m_Driver->getTextureCreationFlag(ETCF_CREATE_MIP_MAPS);
			getImageValues(origImage);

			glGenTextures(1, &m_TextureName);

			if (m_ImageSize==m_TextureSize)
			{
				m_Image = m_Driver->createImage(m_ColorFormat, m_ImageSize);
				origImage->copyTo(m_Image);
			}
			else
			{
				m_Image = m_Driver->createImage(m_ColorFormat, m_TextureSize);
				// scale texture
				origImage->copyToScaling(m_Image);
			}
			uploadTexture(true, mipmapData);
			if (!m_KeepImage)
			{
				m_Image->drop();
				m_Image=0;
			}
		}

		COpenGLESTexture::COpenGLESTexture(const io::path& name, COpenGLDriver* driver)
			: ITexture(name), 
			m_Driver(driver),
			m_KeepImage(true),
			m_InternalFormat(GL_RGBA),
			m_PixelFormat(GL_RGBA),
			m_PixelType(GL_UNSIGNED_BYTE)
		{
#ifdef _DEBUG
			setDebugName("COpenGLTexture");
#endif
		}

		COpenGLESTexture::~COpenGLESTexture()
		{
			if (m_TextureName)
				glDeleteTextures(1, &m_TextureName);
			if (m_Image)
				m_Image->drop();
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

		GLint COpenGLESTexture::getOpenGLFormatAndParametersFromColorFormat(ECOLOR_FORMAT format, GLint& filtering, GLenum& colorformat, GLenum& type)
		{
			// default
			filtering = GL_LINEAR;
			colorformat = GL_RGBA;
			type = GL_UNSIGNED_BYTE;
			GLenum internalformat = GL_RGBA;

			switch(format)
			{
// 			case ECF_A1R5G5B5:
// 				colorformat=GL_BGRA_EXT;
// 				type=GL_UNSIGNED_SHORT_1_5_5_5_REV;
// 				internalformat =  GL_RGBA;
// 				break;
// 			case ECF_R5G6B5:
// 				colorformat=GL_RGB;
// 				type=GL_UNSIGNED_SHORT_5_6_5;
// 				internalformat =  GL_RGB;
// 				break;
// 			case ECF_R8G8B8:
// 				colorformat=GL_BGR;
// 				type=GL_UNSIGNED_BYTE;
// 				internalformat =  GL_RGB;
// 				break;
			case ECF_A8R8G8B8:
				{
					colorformat=GL_RGBA;
					if (m_Driver->m_Version > 101)
						type=GL_UNSIGNED_INT;
					internalformat =  GL_RGBA;
					break;
				}

				// Floating Point texture formats. Thanks to Patryk "Nadro" Nadrowski.
// 			case ECF_R16F:
// 				{
// 					m_ColorFormat = ECF_A8R8G8B8;
// 					internalformat =  GL_RGB8;
// 				}
// 				break;
// 			case ECF_G16R16F:
// 				{
// 					m_ColorFormat = ECF_A8R8G8B8;
// 					internalformat =  GL_RGB8;
// 				}
// 				break;
// 			case ECF_A16B16G16R16F:
// 				{
// 					m_ColorFormat = ECF_A8R8G8B8;
// 					internalformat =  GL_RGBA8;
// 				}
// 				break;
// 			case ECF_R32F:
// 				{
// 					m_ColorFormat = ECF_A8R8G8B8;
// 					internalformat =  GL_RGB8;
// 				}
// 				break;
// 			case ECF_G32R32F:
// 				{
// 					m_ColorFormat = ECF_A8R8G8B8;
// 					internalformat =  GL_RGB8;
// 				}
// 				break;
// 			case ECF_A32B32G32R32F:
// 				{
// 					m_ColorFormat = ECF_A8R8G8B8;
// 					internalformat =  GL_RGBA8;
// 				}
// 				break;
			default:
				{
					os::Printer::log("Unsupported texture format", ELL_ERROR);
					internalformat =  GL_RGBA;
 				}
			}

			return internalformat;
		}

		void COpenGLESTexture::uploadTexture(bool newTexture, void* mipmapData, u32 level)
		{
			// check which image needs to be uploaded
			IImage* image = level?m_MipImage:m_Image;
			if (!image)
			{
				os::Printer::log("No image for OpenGL texture to upload", ELL_ERROR);
				return;
			}

			// get correct opengl color data values
			GLenum oldInternalFormat = m_InternalFormat;
			GLint filtering;
			m_InternalFormat = getOpenGLFormatAndParametersFromColorFormat(m_ColorFormat, filtering, m_PixelFormat, m_PixelType);
		}

	}
}
