
#include <iostream>

#include <GLES/gl.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>

#include "bcm_host.h"

#include <SDL2/SDL.h>

#include <fstream>

#define LOG(LOGERROR) cout
#define _RPI_ 1

using namespace std;

int v;

void exitfunc()
{
  SDL_Quit();
  bcm_host_deinit();
}

ofstream file;

#ifdef _RPI_
    #include <bcm_host.h>
#endif

int SmallFunc();

namespace Renderer
{
    



	SDL_Surface* sdlScreen;

	EGLDisplay display;
	EGLSurface surface;
	EGLContext context;
	EGLConfig config;

#ifdef _RPI_
	static EGL_DISPMANX_WINDOW_T nativewindow;
#else
    NativeWindowType nativewindow;
#endif

	unsigned int display_width = 1366;
	unsigned int display_height = 768;

	unsigned int getScreenWidth() { return display_width; }
	unsigned int getScreenHeight() { return display_height; }
	
	SDL_Window* window = NULL;

	bool createSurface() //unsigned int display_width, unsigned int display_height)
	{
                bcm_host_init();
                atexit(exitfunc);

		LOG(LogInfo) << "Starting SDL...";
		file.flush();

		

		
     


		LOG(LogInfo) << "Creating surface...";

file.flush();

#ifdef _RPI_
		DISPMANX_ELEMENT_HANDLE_T dispman_element;
		DISPMANX_DISPLAY_HANDLE_T dispman_display;
		DISPMANX_UPDATE_HANDLE_T dispman_update;
		VC_RECT_T dst_rect;
		VC_RECT_T src_rect;
#endif



		display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
		if(display == EGL_NO_DISPLAY)
		{
			LOG(LogError) << "Error getting display!"; file.flush();
			return false;
		}

		bool result = eglInitialize(display, NULL, NULL);
		if(result == EGL_FALSE)
		{
			LOG(LogError) << "Error initializing display!"; file.flush();
			return false;
		}

		result = eglBindAPI(EGL_OPENGL_ES_API);
		if(result == EGL_FALSE)
		{
			LOG(LogError) << "Error binding API!"; file.flush();
			return false;
		}


		static const EGLint config_attributes[] =
		{
			EGL_RED_SIZE, 8,
			EGL_GREEN_SIZE, 8,
			EGL_BLUE_SIZE, 8,
			EGL_ALPHA_SIZE, 8,
			EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
			EGL_NONE
		};

		GLint numConfigs;
		result = eglChooseConfig(display, config_attributes, &config, 1, &numConfigs);

		if(result == EGL_FALSE)
		{
			LOG(LogError) << "Error choosing config!";
			return false;
		}


		context = eglCreateContext(display, config, EGL_NO_CONTEXT, NULL);
		if(context == EGL_NO_CONTEXT)
		{
			LOG(LogError) << "Error getting context!\n	" << eglGetError(); 
			return false;
		}

#ifdef _RPI_
        //get hardware info for screen/desktop from BCM interface
		if(!display_width || !display_height)
		{
			bool success = graphics_get_display_size(0, &display_width, &display_height); //0 = LCD

			if(success < 0)
			{
				LOG(LogError) << "Error getting display size!";
file.flush();
				return false;
			}
		}
#else
     
#endif

		LOG(LogInfo) << "Resolution: " << display_width << "x" << display_height << "..."; 

#ifdef _RPI_
		dst_rect.x = 0; dst_rect.y = 0;
		dst_rect.width = display_width; dst_rect.height = display_height;

		src_rect.x = 0; src_rect.y = 0;
		src_rect.width = display_width << 16; src_rect.height = display_height << 16;

		dispman_display = vc_dispmanx_display_open(0); //0 = LCD
		dispman_update = vc_dispmanx_update_start(0);

		dispman_element = vc_dispmanx_element_add(dispman_update, dispman_display, 0 /*layer*/, &dst_rect, 0 /*src*/, &src_rect, DISPMANX_PROTECTION_NONE, 0 /*alpha*/, 0 /*clamp*/, DISPMANX_NO_ROTATE /*transform*/);

		nativewindow.element = dispman_element;
        nativewindow.width = display_width; nativewindow.height = display_height;
        vc_dispmanx_update_submit_sync(dispman_update);
#endif

		surface = eglCreateWindowSurface(display, config, &nativewindow, NULL);
		if(surface == EGL_NO_SURFACE)
		{
			LOG(LogError) << "Error creating window surface!"; 
			return false;
		}

		result = eglMakeCurrent(display, surface, surface, context);
		if(result == EGL_FALSE)
		{
			LOG(LogError) << "Error with eglMakeCurrent!"; 
			return false;
		}


		LOG(LogInfo) << "Created surface successfully!"; 


		return true;
	}

	void swapBuffers()
	{
		glClearColor(1,0,0,1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		//eglSwapBuffers(display, surface);
		
		
		  glPushMatrix();
     
        static const GLfloat squareVertices[] = {
        .5f, -.5f, 0,            // Top left
        -.5f, -.5f, 0,            // Bottom left
        -.5f, .5f, 0,            // Bottom right
        .5f, .5f, 0,             // Top right
    };
    
    glColor4f(0,1,0,1);
   

    //glTranslatef(rX,rY,0);
    //glRotatef(rot,0,0,1);
  //  glRotatef(rotY, 0, 1.0f, 0.0f);
   // glTranslatef(-rX,-rY,0);
    glScalef(100, 100, 1.0f);
   // glTranslatef(0, 0, shiftZ2);




    glDisable(GL_TEXTURE_2D);
	glVertexPointer(3, GL_FLOAT, 0, squareVertices);
    glEnableClientState(GL_VERTEX_ARRAY);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    glPopMatrix();
		
		
			eglSwapBuffers(display, surface);
	}

	void destroySurface()
	{
		eglSwapBuffers(display, surface);
		eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
		eglDestroySurface(display, surface);
		eglDestroyContext(display, context);
		eglTerminate(display);

		display = EGL_NO_DISPLAY;
		surface = EGL_NO_SURFACE;
		context = EGL_NO_CONTEXT;

		SDL_FreeSurface(sdlScreen);
		sdlScreen = NULL;
		SDL_Quit();
	}

	bool init(int w, int h)
	{
		if(w)
			display_width = w;
		if(h)
			display_height = h;

		bool createdSurface = createSurface();

		if(!createdSurface)
			return false;

		glViewport(0, 0, display_width, display_height);
		glMatrixMode(GL_PROJECTION);
		glOrthof(0, display_width, display_height, 0, -1.0, 1.0);
		glMatrixMode(GL_MODELVIEW);
		glClearColor(1.0f, 0.0f, 1.0f, 1.0f);

		//onInit();

		return true;
	}

	void deinit()
	{
		//onDeinit();
		destroySurface();
	}
};


int main(int argc,char* args[]) 
{
        file.open("log.txt", fstream::out | fstream::app);

file << "Log start\n";
file.close();
file.open("log.txt", fstream::out | fstream::app);



	if (!Renderer::createSurface()){return 0;}
	
	if (!Renderer::init(1024,768)){return 0;}
	Renderer::swapBuffers();
	
	//SmallFunc();

file.close();
	
	//SDL_Rect r = {0,0,320,240};
 // SDL_FillRect(Renderer::sdlScreen,&r, SDL_MapRGB(Renderer::sdlScreen->format, 200,200,0) );

  //SDL_Flip( Renderer::sdlScreen );
  
  
	
	SDL_Delay( 2000 );
	
	//system("PAUSE");
	
}





int SmallFunc()
{
	
				int SCREEN_WIDTH = Renderer::display_width;
			int SCREEN_HEIGHT = Renderer::display_height;
	
	int frames = 0;
	SDL_Window* window = NULL;
	SDL_Surface* screenSurface = NULL;
	if (SDL_Init(SDL_INIT_VIDEO)<0) {
		printf("Could not initialize SDL. SDL_Error: %s\n",SDL_GetError());
	} else {
		
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);

		
		
		window = SDL_CreateWindow("SDL Test",SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,SCREEN_WIDTH,SCREEN_HEIGHT,SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
		if (window==NULL) {
			printf("Cannot create window. SDL_Error: %s\n",SDL_GetError());
		} else {
			SDL_Rect arect;
			screenSurface=SDL_GetWindowSurface(window);
			SDL_FillRect(screenSurface,NULL,SDL_MapRGB(screenSurface->format,0x00,0x00,0x44));
			arect.x=64; arect.y=64; arect.w=SCREEN_WIDTH-128; arect.h=SCREEN_HEIGHT-128;
			
			
			Uint32 holdTime = SDL_GetTicks();
		
			while (SDL_GetTicks() - holdTime < 5000)
			{
			   SDL_FillRect(screenSurface,&arect,SDL_MapRGB(screenSurface->format,0x00,0x77,0x77));
				frames++;
			 SDL_UpdateWindowSurface(window);
			}
			//SDL_Delay(3000);
		}
	}
	
	std::cout << "displays:" << SDL_GetNumVideoDisplays();
	
	
	SDL_DestroyWindow(window);
	SDL_Quit();

	std::cout << "frames:" << frames << std::endl;
	std::cout << "fps:" << frames/5 << std::endl;
	
	
	


	return 0;
}

