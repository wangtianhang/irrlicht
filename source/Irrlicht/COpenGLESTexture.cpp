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
			ColorFormat(ECF_A8R8G8B8),
			Driver(driver),
			Image(0),
			MipImage(0),
			TextureName(0),
			InternalFormat(GL_RGBA),
			PixelFormat(GL_RGBA),
			PixelType(GL_UNSIGNED_BYTE),
			MipLevelStored(0),
			MipmapLegacyMode(false),
			IsRenderTarget(false),
			AutomaticMipmapUpdate(false),
			ReadOnlyLock(false),
			KeepImage(true)
			//m_InternalFormat(GL_RGBA),
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
			ColorFormat(ECF_A8R8G8B8),
			Driver(driver),
			Image(0),
			MipImage(0),
			TextureName(0),
			InternalFormat(GL_RGBA),
			PixelFormat(GL_RGBA),
			PixelType(GL_UNSIGNED_BYTE),
			MipLevelStored(0),
			MipmapLegacyMode(false),
			IsRenderTarget(false),
			AutomaticMipmapUpdate(false),
			ReadOnlyLock(false),
			KeepImage(true)
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
					if (Driver->Version > 101)
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
			GLenum oldInternalFormat = InternalFormat;
			GLint filtering;
			InternalFormat = getOpenGLFormatAndParametersFromColorFormat(ColorFormat, filtering, PixelFormat, PixelType);
			if(!newTexture)
			{
				InternalFormat = oldInternalFormat;
			}

			Driver->setActiveTexture(0, this);
			if (Driver->testGLError())
				os::Printer::log("Could not bind Texture", ELL_ERROR);

			// mipmap handling for main texture
			if (!level && newTexture)
			{
				{
					// Either generate manually due to missing capability
					// or use predefined mipmap data
					AutomaticMipmapUpdate=false;
					regenerateMipMapLevels(mipmapData);
				}
				if (HasMipMaps) // might have changed in regenerateMipMapLevels
				{
					// enable bilinear mipmap filter
					glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST );
					glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				}
				else
				{
					// enable bilinear filter without mipmaps
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				}
			}

			// now get image data and upload to GPU
			void* source = image->lock();
			if (newTexture)
				glTexImage2D(GL_TEXTURE_2D, level, InternalFormat, image->getDimension().Width,
				image->getDimension().Height, 0, PixelFormat, PixelType, source);
			else
				glTexSubImage2D(GL_TEXTURE_2D, level, 0, 0, image->getDimension().Width,
				image->getDimension().Height, PixelFormat, PixelType, source);
			image->unlock();

			if (!MipmapLegacyMode && AutomaticMipmapUpdate)
			{
				glEnable(GL_TEXTURE_2D);
				glGenerateMipmap(GL_TEXTURE_2D);
			}

			if (Driver->testGLError())
				os::Printer::log("Could not glTexImage2D", ELL_ERROR);
		}

		void COpenGLESTexture::regenerateMipMapLevels(void* mipmapData/*=0*/)
		{
			if (AutomaticMipmapUpdate || !HasMipMaps || !Image)
				return;
			if ((Image->getDimension().Width==1) && (Image->getDimension().Height==1))
				return;

			// Manually create mipmaps or use prepared version
			u32 width=Image->getDimension().Width;
			u32 height=Image->getDimension().Height;
			u32 i=0;
			u8* target = static_cast<u8*>(mipmapData);
			do
			{
				if (width>1)
					width>>=1;
				if (height>1)
					height>>=1;
				++i;
				if (!target)
					target = new u8[width*height*Image->getBytesPerPixel()];
				// create scaled version if no mipdata available
				if (!mipmapData)
					Image->copyToScaling(target, width, height, Image->getColorFormat());
				glTexImage2D(GL_TEXTURE_2D, i, InternalFormat, width, height,
					0, PixelFormat, PixelType, target);
				// get next prepared mipmap data if available
				if (mipmapData)
				{
					mipmapData = static_cast<u8*>(mipmapData)+width*height*Image->getBytesPerPixel();
					target = static_cast<u8*>(mipmapData);
				}
			}
			while (width!=1 || height!=1);
			// cleanup
			if (!mipmapData)
				delete [] target;
		}

		void* COpenGLESTexture::lock(E_TEXTURE_LOCK_MODE mode/*=ETLM_READ_WRITE*/, u32 mipmapLevel/*=0*/)
		{
			// store info about which image is locked
			IImage* image = (mipmapLevel==0)?Image:MipImage;
			ReadOnlyLock |= (mode==ETLM_READ_ONLY);
			MipLevelStored = mipmapLevel;
			if (!ReadOnlyLock && mipmapLevel)
			{
				AutomaticMipmapUpdate=false;
			}

// 			// if data not available or might have changed on GPU download it
// 			if (!image || IsRenderTarget)
// 			{
// 				// prepare the data storage if necessary
// 				if (!image)
// 				{
// 					if (mipmapLevel)
// 					{
// 						u32 i=0;
// 						u32 width = TextureSize.Width;
// 						u32 height = TextureSize.Height;
// 						do
// 						{
// 							if (width>1)
// 								width>>=1;
// 							if (height>1)
// 								height>>=1;
// 							++i;
// 						}
// 						while (i != mipmapLevel);
// 						MipImage = image = Driver->createImage(ECF_A8R8G8B8, core::dimension2du(width,height));
// 					}
// 					else
// 						Image = image = Driver->createImage(ECF_A8R8G8B8, ImageSize);
// 					ColorFormat = ECF_A8R8G8B8;
// 				}
// 				if (!image)
// 					return 0;
// 
// 				if (mode != ETLM_WRITE_ONLY)
// 				{
// 					u8* pixels = static_cast<u8*>(image->lock());
// 					if (!pixels)
// 						return 0;
// 
// 					// we need to keep the correct texture bound later on
// 					GLint tmpTexture;
// 					glGetIntegerv(GL_TEXTURE_BINDING_2D, &tmpTexture);
// 					glBindTexture(GL_TEXTURE_2D, TextureName);
// 
// 					// we need to flip textures vertical
// 					// however, it seems that this does not hold for mipmap
// 					// textures, for unknown reasons.
// 
// 					// download GPU data as ARGB8 to pixels;
// 					glGetTexImage(GL_TEXTURE_2D, mipmapLevel, GL_BGRA_EXT, GL_UNSIGNED_BYTE, pixels);
// 
// 					if (!mipmapLevel)
// 					{
// 						{
// 							// opengl images are horizontally flipped, so we have to fix that here.
// 							const s32 pitch=image->getPitch();
// 							u8* p2 = pixels + (image->getDimension().Height - 1) * pitch;
// 							u8* tmpBuffer = new u8[pitch];
// 							for (u32 i=0; i < image->getDimension().Height; i += 2)
// 							{
// 								memcpy(tmpBuffer, pixels, pitch);
// 								memcpy(pixels, p2, pitch);
// 								memcpy(p2, tmpBuffer, pitch);
// 								pixels += pitch;
// 								p2 -= pitch;
// 							}
// 							delete [] tmpBuffer;
// 						}
// 					}
// 					image->unlock();
// 
// 					//reset old bound texture
// 					glBindTexture(GL_TEXTURE_2D, tmpTexture);
// 				}
// 			}
			return image->lock();
		}

		void COpenGLESTexture::unlock()
		{
			// test if miplevel or main texture was locked
			IImage* image = MipImage?MipImage:Image;
			if (!image)
				return;
			// unlock image to see changes
			image->unlock();
			// copy texture data to GPU
			if (!ReadOnlyLock)
				uploadTexture(false, 0, MipLevelStored);
			ReadOnlyLock = false;
			// cleanup local image
			if (MipImage)
			{
				MipImage->drop();
				MipImage=0;
			}
			else if (!KeepImage)
			{
				Image->drop();
				Image=0;
			}
			// update information
			if (Image)
				ColorFormat=Image->getColorFormat();
			else
				ColorFormat=ECF_A8R8G8B8;
		}



		const core::dimension2d<u32>& COpenGLESTexture::getOriginalSize() const
		{
			return ImageSize;
		}

		const core::dimension2d<u32>& COpenGLESTexture::getSize() const
		{
			return TextureSize;
		}

		irr::video::E_DRIVER_TYPE COpenGLESTexture::getDriverType() const
		{
			return EDT_OPENGL_ES;
		}

		irr::video::ECOLOR_FORMAT COpenGLESTexture::getColorFormat() const
		{
			return ColorFormat;
		}

		irr::u32 COpenGLESTexture::getPitch() const
		{
			if (Image)
				return Image->getPitch();
			else
				return 0;
		}

		GLuint COpenGLESTexture::getOpenGLTextureName() const
		{
			return TextureName;
		}

		bool COpenGLESTexture::hasMipMaps() const
		{
			return HasMipMaps;
		}

		bool COpenGLESTexture::isRenderTarget() const
		{
			return IsRenderTarget;
		}

		void COpenGLESTexture::setIsRenderTarget( bool isTarget )
		{
			IsRenderTarget = isTarget;
		}

		bool COpenGLESTexture::isFrameBufferObject() const
		{
			return false;
		}

		void COpenGLESTexture::bindRTT()
		{

		}

		void COpenGLESTexture::unbindRTT()
		{
			Driver->setActiveTexture(0, this);

			// Copy Our ViewPort To The Texture
			glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, getSize().Width, getSize().Height);
		}



	}
}
