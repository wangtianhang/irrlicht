
#include "COpenGLESDriver.h"

#include "CNullDriver.h"

//#include "CIrrDeviceWin32.h"
#include "COpenGLESTexture.h"
#include "COpenGLESMaterialRenderer.h"
#include "COpenGLESShaderMaterialRenderer.h"
#include "COpenGLESGLSLMaterialRenderer.h"
#include "COpenGLESNormalMapRender.h"
#include "COpenGLESParallaxMapRenderer.h"

#include "os.h"

namespace irr
{
namespace video
{

/*!*********************************************************************************************************************
\param		deviceContext               The device context used by the application
\param[out]	eglDisplay				    EGLDisplay created from deviceContext
\return	Whether the function succeeded or not.
\brief	Creates an EGLDisplay from a native device context, and initializes it.
***********************************************************************************************************************/
bool createEGLDisplay(HDC deviceContext, EGLDisplay& eglDisplay)
{
	//	Get an EGL display.
	//	EGL uses the concept of a "display" which in most environments corresponds to a single physical screen. After creating a native
	//	display for a given windowing system, EGL can use this handle to get a corresponding EGLDisplay handle to it for use in rendering.
	//	Should this fail, EGL is usually able to provide access to a default display.

	eglDisplay = eglGetDisplay(deviceContext);
	if (eglDisplay == EGL_NO_DISPLAY)
	{
		eglDisplay = eglGetDisplay((EGLNativeDisplayType) EGL_DEFAULT_DISPLAY);
	}

	// If a display still couldn't be obtained, return an error.
	if (eglDisplay == EGL_NO_DISPLAY)
	{
		MessageBox(0, "Failed to get an EGLDisplay", "Error", MB_OK | MB_ICONEXCLAMATION);
		return false;
	}

	//	Initialize EGL.
	//	EGL has to be initialized with the display obtained in the previous step. All EGL functions other than eglGetDisplay
	//	and eglGetError need an initialized EGLDisplay.
	//	If an application is not interested in the EGL version number it can just pass NULL for the second and third parameters, but they
	//	are queried here for illustration purposes.

	EGLint eglMajorVersion, eglMinorVersion;
	if (!eglInitialize(eglDisplay, &eglMajorVersion, &eglMinorVersion))
	{
		MessageBox(0, "Failed to initialize the EGLDisplay", "Error", MB_OK | MB_ICONEXCLAMATION);
		return false;
	}
	return true;
}

/*!*********************************************************************************************************************
\param		eglDisplay                  The EGLDisplay used by the application
\param[out]	eglConfig                   The EGLConfig chosen by the function
\return	Whether the function succeeded or not.
\brief	Chooses an appropriate EGLConfig and return it.
***********************************************************************************************************************/
bool chooseEGLConfig(EGLDisplay eglDisplay, EGLConfig& eglConfig)
{

	//	Specify the required configuration attributes.
	//	An EGL "configuration" describes the capabilities an application requires and the type of surfaces that can be used for drawing.
	//	Each implementation exposes a number of different configurations, and an application needs to describe to EGL what capabilities it
	//	requires so that an appropriate one can be chosen. The first step in doing this is to create an attribute list, which is an array
	//	of key/value pairs which describe particular capabilities requested. In this application nothing special is required so we can query
	//	the minimum of needing it to render to a window, and being OpenGL ES 2.0 capable.
	const EGLint configurationAttributes[] =
	{
		EGL_SURFACE_TYPE,		EGL_WINDOW_BIT,
		EGL_RENDERABLE_TYPE,	EGL_OPENGL_ES2_BIT,
		EGL_NONE
	};


	//	Find a suitable EGLConfig
	//	eglChooseConfig is provided by EGL to provide an easy way to select an appropriate configuration. It takes in the capabilities
	//	specified in the attribute list, and returns a list of available configurations that match or exceed the capabilities requested.
	//	Details of all the possible attributes and how they are selected for by this function are available in the EGL reference pages here:
	//	http://www.khronos.org/registry/egl/sdk/docs/man/xhtml/eglChooseConfig.html
	//	It is also possible to simply get the entire list of configurations and use a custom algorithm to choose a suitable one, as many
	//	advanced applications choose to do. For this application however, taking the first EGLConfig that the function returns suits
	//	its needs perfectly, so we limit it to returning a single EGLConfig.

	EGLint configsReturned;
	if (!eglChooseConfig(eglDisplay, configurationAttributes, &eglConfig, 1, &configsReturned) || (configsReturned != 1))
	{
		MessageBox(0, "eglChooseConfig() failed.", "Error", MB_OK | MB_ICONEXCLAMATION);
		return false;
	}
	return true;
}

/*!*********************************************************************************************************************
\param			nativeWindow                Handle to the window
\param			functionLastCalled          Function which triggered the error
\return	True if no EGL error was detected
\brief	Tests for an EGL error and prints it in a message box.
***********************************************************************************************************************/
bool testEGLError(HWND nativeWindow, const char* functionLastCalled)
{
	/*	eglGetError returns the last error that occurred using EGL, not necessarily the status of the last called function. The user has to
		check after every single EGL call or at least once every frame. Usually this would be for debugging only, but for this example
		it is enabled always.
	*/
	EGLint lastError = eglGetError();
	if (lastError != EGL_SUCCESS)
	{
		TCHAR stringBuffer[256];
		printf(stringBuffer, "%s failed (%x).\n", functionLastCalled, lastError);
		MessageBox(nativeWindow, stringBuffer, "Error", MB_OK | MB_ICONEXCLAMATION);
		return false;
	}

	return true;
}

/*!*********************************************************************************************************************
\param		nativeWindow                A native window that's been created
\param		eglDisplay                  The EGLDisplay used by the application
\param		eglConfig                   An EGLConfig chosen by the application
\param[out]	eglSurface					The EGLSurface created from the native window.
\return	Whether the function succeeds or not.
\brief	Creates an EGLSurface from a native window
***********************************************************************************************************************/
bool createEGLSurface(HWND nativeWindow, EGLDisplay eglDisplay, EGLConfig eglConfig, EGLSurface& eglSurface)
{

	//	Create an EGLSurface for rendering.
	//	Using a native window created earlier and a suitable eglConfig, a surface is created that can be used to render OpenGL ES calls to.
	//	There are three main surface types in EGL, which can all be used in the same way once created but work slightly differently:
	//	 - Window Surfaces  - These are created from a native window and are drawn to the screen.
	//	 - Pixmap Surfaces  - These are created from a native windowing system as well, but are offscreen and are not displayed to the user.
	//	 - PBuffer Surfaces - These are created directly within EGL, and like Pixmap Surfaces are offscreen and thus not displayed.
	//	The offscreen surfaces are useful for non-rendering contexts and in certain other scenarios, but for most applications the main
	//	surface used will be a window surface as performed below.

	eglSurface = eglCreateWindowSurface(eglDisplay, eglConfig, nativeWindow, NULL);
	if (eglSurface == EGL_NO_SURFACE)
	{
		eglGetError(); // Clear error
		eglSurface = eglCreateWindowSurface(eglDisplay, eglConfig, NULL, NULL);
	}

	// Check for any EGL Errors
	if (!testEGLError(nativeWindow, "eglCreateWindowSurface")) {	return false; }
	return true;
}

/*!*********************************************************************************************************************
\param			eglDisplay                  The EGLDisplay used by the application
\param			eglConfig                   An EGLConfig chosen by the application
\param			eglSurface					The EGLSurface created from the native window.
\param[out] 	eglContext                  The EGLContext created by this function
\param			nativeWindow                A native window, used to display error messages
\return	Whether the function succeeds or not.
\brief	Sets up the EGLContext, creating it and then installing it to the current thread.
***********************************************************************************************************************/
bool setupEGLContext(EGLDisplay eglDisplay, EGLConfig eglConfig, EGLSurface eglSurface, EGLContext& eglContext, HWND nativeWindow)
{
	//	Make OpenGL ES the current API.
	//	EGL needs a way to know that any subsequent EGL calls are going to be affecting OpenGL ES,
	//	rather than any other API (such as OpenVG).

	eglBindAPI(EGL_OPENGL_ES_API);
	if (!testEGLError(nativeWindow, "eglBindAPI")) {	return false; }

	//	Create a context.
	//	EGL has to create what is known as a context for OpenGL ES. The concept of a context is OpenGL ES's way of encapsulating any
	//	resources and state. What appear to be "global" functions in OpenGL actually only operate on the current context. A context
	//	is required for any operations in OpenGL ES.
	//	Similar to an EGLConfig, a context takes in a list of attributes specifying some of its capabilities. However in most cases this
	//	is limited to just requiring the version of the OpenGL ES context required - In this case, OpenGL ES 2.0.
	EGLint contextAttributes[] =
	{
		EGL_CONTEXT_CLIENT_VERSION, 2,
		EGL_NONE
	};

	// Create the context with the context attributes supplied
	eglContext = eglCreateContext(eglDisplay, eglConfig, NULL, contextAttributes);
	if (!testEGLError(nativeWindow, "eglCreateContext")) {	return false;	}

	//	Bind the context to the current thread.
	//	Due to the way OpenGL uses global functions, contexts need to be made current so that any function call can operate on the correct
	//	context. Specifically, make current will bind the context to the thread it's called from, and unbind it from any others. To use
	//	multiple contexts at the same time, users should use multiple threads and synchronise between them.

	eglMakeCurrent(eglDisplay, eglSurface, eglSurface, eglContext);
	if (!testEGLError(nativeWindow, "eglMakeCurrent")) {	return false; }
	return true;
}

/*!*********************************************************************************************************************
\param			eglDisplay  The EGLDisplay used by the application
\brief	Releases all resources allocated by EGL
***********************************************************************************************************************/
void releaseEGLState(EGLDisplay eglDisplay)
{
	// To release the resources in the context, first the context has to be released from its binding with the current thread.
	eglMakeCurrent(eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);

	// Terminate the display, and any resources associated with it (including the EGLContext)
	eglTerminate(eglDisplay);
}

/*!*********************************************************************************************************************
\param			nativeWindow                The native window to release
\param			deviceContext               The native display to release
\brief	Releases all resources allocated by the windowing system
***********************************************************************************************************************/
void releaseWindowAndDisplay(HWND nativeWindow, HDC deviceContext)
{
	// Release the device context.
	if (deviceContext) {	ReleaseDC(nativeWindow, deviceContext);	}

	// Destroy the window
	//if (nativeWindow) {	DestroyWindow(nativeWindow); }
}

COpenGLESDriver::COpenGLESDriver( const SIrrlichtCreationParameters& params, io::IFileSystem* io, CIrrDeviceWin32* device )
	: CNullDriver(io, params.WindowSize), COpenGLESExtensionHandler(),
	m_Params(params), 
	m_HDc(0),
	m_Window(static_cast<HWND>(params.WindowId)),
	m_DeviceType(EIDT_WIN32),
	m_eglDisplay(NULL),
	m_eglSurface(NULL)
{
#ifdef _DEBUG
	setDebugName("COpenGLDriver");
#endif
}

bool COpenGLESDriver::initDriver( CIrrDeviceWin32* device )
{
	m_HDc=GetDC(m_Window);

	//HDC	deviceContext = m_HDc;

	// EGL variables
	m_eglDisplay = NULL;
	EGLConfig			eglConfig = NULL;
	m_eglSurface = NULL;
	EGLContext			eglContext = NULL;

	// Create and Initialize an EGLDisplay from the native display
	if (!createEGLDisplay(m_HDc, m_eglDisplay))
	{
		return false;
	}

	// Choose an EGLConfig for the application, used when setting up the rendering surface and EGLContext
	if (!chooseEGLConfig(m_eglDisplay, eglConfig)) 
	{
		return false;
	}

	// Create an EGLSurface for rendering from the native window
	if (!createEGLSurface(m_Window, m_eglDisplay, eglConfig, m_eglSurface)) 
	{ 
		return false; 
	}

	// Setup the EGL Context from the other EGL constructs created so far, so that the application is ready to submit OpenGL ES commands
	if (!setupEGLContext(m_eglDisplay, eglConfig, m_eglSurface, eglContext, m_Window)) 
	{
		return false;
	}

	genericDriverInit();

	return true;
}


COpenGLESDriver::~COpenGLESDriver()
{
	deleteMaterialRenders();

	// I get a blue screen on my laptop, when I do not delete the
	// textures manually before releasing the dc. Oh how I love this.
	deleteAllTextures();
	removeAllOcclusionQueries();
	removeAllHardwareBuffers();

	// Release the EGL State
	releaseEGLState(m_eglDisplay);
	// Release the windowing system resources
	releaseWindowAndDisplay(m_Window, m_HDc);
}

void COpenGLESDriver::createMaterialRenderers()
{
// 	// create OpenGL material renderers
// 
// 	addAndDropMaterialRenderer(new COpenGLMaterialRenderer_SOLID(this));
// 	addAndDropMaterialRenderer(new COpenGLMaterialRenderer_SOLID_2_LAYER(this));
// 
// 	// add the same renderer for all lightmap types
// 	COpenGLMaterialRenderer_LIGHTMAP* lmr = new COpenGLMaterialRenderer_LIGHTMAP(this);
// 	addMaterialRenderer(lmr); // for EMT_LIGHTMAP:
// 	addMaterialRenderer(lmr); // for EMT_LIGHTMAP_ADD:
// 	addMaterialRenderer(lmr); // for EMT_LIGHTMAP_M2:
// 	addMaterialRenderer(lmr); // for EMT_LIGHTMAP_M4:
// 	addMaterialRenderer(lmr); // for EMT_LIGHTMAP_LIGHTING:
// 	addMaterialRenderer(lmr); // for EMT_LIGHTMAP_LIGHTING_M2:
// 	addMaterialRenderer(lmr); // for EMT_LIGHTMAP_LIGHTING_M4:
// 	lmr->drop();
// 
// 	// add remaining material renderer
// 	addAndDropMaterialRenderer(new COpenGLMaterialRenderer_DETAIL_MAP(this));
// 	addAndDropMaterialRenderer(new COpenGLMaterialRenderer_SPHERE_MAP(this));
// 	addAndDropMaterialRenderer(new COpenGLMaterialRenderer_REFLECTION_2_LAYER(this));
// 	addAndDropMaterialRenderer(new COpenGLMaterialRenderer_TRANSPARENT_ADD_COLOR(this));
// 	addAndDropMaterialRenderer(new COpenGLMaterialRenderer_TRANSPARENT_ALPHA_CHANNEL(this));
// 	addAndDropMaterialRenderer(new COpenGLMaterialRenderer_TRANSPARENT_ALPHA_CHANNEL_REF(this));
// 	addAndDropMaterialRenderer(new COpenGLMaterialRenderer_TRANSPARENT_VERTEX_ALPHA(this));
// 	addAndDropMaterialRenderer(new COpenGLMaterialRenderer_TRANSPARENT_REFLECTION_2_LAYER(this));
// 
// 	// add normal map renderers
// 	s32 tmp = 0;
// 	video::IMaterialRenderer* renderer = 0;
// 	renderer = new COpenGLNormalMapRenderer(this, tmp, MaterialRenderers[EMT_SOLID].Renderer);
// 	renderer->drop();
// 	renderer = new COpenGLNormalMapRenderer(this, tmp, MaterialRenderers[EMT_TRANSPARENT_ADD_COLOR].Renderer);
// 	renderer->drop();
// 	renderer = new COpenGLNormalMapRenderer(this, tmp, MaterialRenderers[EMT_TRANSPARENT_VERTEX_ALPHA].Renderer);
// 	renderer->drop();
// 
// 	// add parallax map renderers
// 	renderer = new COpenGLParallaxMapRenderer(this, tmp, MaterialRenderers[EMT_SOLID].Renderer);
// 	renderer->drop();
// 	renderer = new COpenGLParallaxMapRenderer(this, tmp, MaterialRenderers[EMT_TRANSPARENT_ADD_COLOR].Renderer);
// 	renderer->drop();
// 	renderer = new COpenGLParallaxMapRenderer(this, tmp, MaterialRenderers[EMT_TRANSPARENT_VERTEX_ALPHA].Renderer);
// 	renderer->drop();
// 
// 	// add basic 1 texture blending
// 	addAndDropMaterialRenderer(new COpenGLMaterialRenderer_ONETEXTURE_BLEND(this));
}

bool COpenGLESDriver::genericDriverInit()
{
	initExtensions(m_Params.Stencilbuffer);

	createMaterialRenderers();

	return true;
}

bool COpenGLESDriver::beginScene( bool backBuffer/*=true*/, bool zBuffer/*=true*/, SColor color/*=SColor(255,0,0,0)*/, const SExposedVideoData& videoData/*=SExposedVideoData()*/, core::rect<s32>* sourceRect/*=0*/ )
{
	CNullDriver::beginScene(backBuffer, zBuffer, color, videoData, sourceRect);

	glClearColor(0.00f, 0.70f, 0.67f, 1.0f);

	glClear(GL_COLOR_BUFFER_BIT);

	return true;
}

bool COpenGLESDriver::endScene()
{
	CNullDriver::endScene();

	glFlush();

	eglSwapBuffers(m_eglDisplay, m_eglSurface);

	return true;
}

bool COpenGLESDriver::setActiveTexture(u32 stage, const video::ITexture* texture)
{
	if (stage >= m_MaxSupportedTextures)
		return false;

	return false;
}

IVideoDriver* createOpenGLESDriver(const SIrrlichtCreationParameters& params,
	io::IFileSystem* io, CIrrDeviceWin32* device)
{
	COpenGLESDriver* ogl =  new COpenGLESDriver(params, io, device);
	if (!ogl->initDriver(device))
	{
		ogl->drop();
		ogl = 0;
	}
	return ogl;
}



}
}

