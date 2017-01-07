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
			Driver(driver),
			KeepImage(true),
			//m_InternalFormat(GL_RGBA),
			PixelFormat(GL_RGBA),
			PixelType(GL_UNSIGNED_BYTE)
		{
#ifdef _DEBUG
			setDebugName("COpenGLTexture");
#endif
			HasMipMaps = Driver->getTextureCreationFlag(ETCF_CREATE_MIP_MAPS);
			getImageValues(origImage);

			glGenTextures(1, &TextureName);

			if (ImageSize==TextureSize)
			{
				Image = Driver->createImage(ColorFormat, ImageSize);
				origImage->copyTo(Image);
			}
			else
			{
				Image = Driver->createImage(ColorFormat, TextureSize);
				// scale texture
				origImage->copyToScaling(Image);
			}
			uploadTexture(true, mipmapData);
			if (!KeepImage)
			{
				Image->drop();
				Image=0;
			}
		}

		COpenGLESTexture::COpenGLESTexture(const io::path& name, COpenGLESDriver* driver)
			: ITexture(name), 
			Driver(driver),
			KeepImage(true),
			//m_InternalFormat(GL_RGBA),
			PixelFormat(GL_RGBA),
			PixelType(GL_UNSIGNED_BYTE)
		{
#ifdef _DEBUG
			setDebugName("COpenGLTexture");
#endif
		}

		COpenGLESTexture::~COpenGLESTexture()
		{
			if (TextureName)
				glDeleteTextures(1, &TextureName);
			if (Image)
				Image->drop();
		}

		GLuint COpenGLESTexture::getOpenGLTextureName() const
		{
			return TextureName;
		}

		ECOLOR_FORMAT COpenGLESTexture::getBestColorFormat(ECOLOR_FORMAT format)
		{
			ECOLOR_FORMAT destFormat = ECF_A8R8G8B8;
			switch (format)
			{
			case ECF_A1R5G5B5:
				if (!Driver->getTextureCreationFlag(ETCF_ALWAYS_32_BIT))
					destFormat = ECF_A1R5G5B5;
				break;
			case ECF_R5G6B5:
				if (!Driver->getTextureCreationFlag(ETCF_ALWAYS_32_BIT))
					destFormat = ECF_A1R5G5B5;
				break;
			case ECF_A8R8G8B8:
				if (Driver->getTextureCreationFlag(ETCF_ALWAYS_16_BIT) ||
					Driver->getTextureCreationFlag(ETCF_OPTIMIZED_FOR_SPEED))
					destFormat = ECF_A1R5G5B5;
				break;
			case ECF_R8G8B8:
				if (Driver->getTextureCreationFlag(ETCF_ALWAYS_16_BIT) ||
					Driver->getTextureCreationFlag(ETCF_OPTIMIZED_FOR_SPEED))
					destFormat = ECF_A1R5G5B5;
			default:
				break;
			}
			if (Driver->getTextureCreationFlag(ETCF_NO_ALPHA_CHANNEL))
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

		void COpenGLESTexture::getOpenGLFormatAndParametersFromColorFormat(ECOLOR_FORMAT format, GLint& filtering, GLenum& colorformat, GLenum& type)
		{
			// default
			filtering = GL_LINEAR;
			colorformat = GL_RGBA;
			type = GL_UNSIGNED_BYTE;
			//GLenum internalformat = GL_RGBA;

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
					if (Driver->Version > 101)
						type=GL_UNSIGNED_INT;
					//internalformat =  GL_RGBA;
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
					//internalformat =  GL_RGBA;
 				}
			}

			//return internalformat;
		}

		void COpenGLESTexture::getImageValues( IImage* image )
		{
			if (!image)
			{
				os::Printer::log("No image for OpenGL texture.", ELL_ERROR);
				return;
			}

			ImageSize = image->getDimension();

			if ( !ImageSize.Width || !ImageSize.Height)
			{
				os::Printer::log("Invalid size of image for OpenGL Texture.", ELL_ERROR);
				return;
			}

			const f32 ratio = (f32)ImageSize.Width/(f32)ImageSize.Height;
			if ((ImageSize.Width>Driver->MaxTextureSize) && (ratio >= 1.0f))
			{
				ImageSize.Width = Driver->MaxTextureSize;
				ImageSize.Height = (u32)(Driver->MaxTextureSize/ratio);
			}
			else if (ImageSize.Height>Driver->MaxTextureSize)
			{
				ImageSize.Height = Driver->MaxTextureSize;
				ImageSize.Width = (u32)(Driver->MaxTextureSize*ratio);
			}
			TextureSize=ImageSize.getOptimalSize(!Driver->queryFeature(EVDF_TEXTURE_NPOT));

			ColorFormat = getBestColorFormat(image->getColorFormat());
		}

		void COpenGLESTexture::uploadTexture(bool newTexture, void* mipmapData, u32 level)
		{
			// check which image needs to be uploaded
			IImage* image = level?MipImage:Image;
			if (!image)
			{
				os::Printer::log("No image for OpenGL texture to upload", ELL_ERROR);
				return;
			}

			// get correct opengl color data values
			//GLenum oldInternalFormat = m_InternalFormat;
			GLint filtering;
			getOpenGLFormatAndParametersFromColorFormat(ColorFormat, filtering, PixelFormat, PixelType);

			
		}

	}
}
