/*
* Copyright (c) 2012, Nicolas My
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of the <organization> nor the
*       names of its contributors may be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY Nicolas My ''AS IS'' AND ANY
* EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL Nicolas My BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include "Renderer.h"
#include <SDL.h>
#undef main
#include <SDL_ttf.h>
#include <iostream>
#include <GL/glew.h>
#include "Shader.h"
#include "FrameBuffer.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Sprite.h"
#include "Texture.h"
#include "FastMath.h"
#include "Camera.h"
#include "ClosedSpaceGenerator.h"
#include "OutputConsole.h"
#include "TextureGenerator.h"

#include "UIWidget.h"

//#define OVR
#define OVR_OS_WIN32
/*
#ifdef WIN32
#define OVR_OS_WIN32
#elif defined(__APPLE__)
#define OVR_OS_MAC
#else
#define OVR_OS_LINUX
#include <X11/Xlib.h>
#include <GL/glx.h>
#endif
*/
#include <windows.h>
#include <OVR_CAPI.h>
#include <OVR_CAPI_GL.h>

unsigned char g_texdata[]= { 255, 255, 255, 255, 255, 255, 255, 255,
							255, 255, 255, 255, 255, 255, 255, 255};


unsigned char  g_checker_texdata[]= { 255, 255, 255, 255, 0, 0, 0, 0,
							0, 0, 0, 0, 255, 255, 255, 255};

int thread_func(void *data);
SDL_Surface* CreateSurface(Uint32 flags,int width,int height,const SDL_Surface* display);


#include "Renderable.h"
//Renderable * g_renderable;
std::vector<Renderable*> g_renderableList = std::vector<Renderable*>();
Shader * g_shader_debug;
float * g_vertexBuffer = NULL;

static unsigned int fbo, fb_tex, fb_depth;
static int fb_width, fb_height;
static int fb_tex_width, fb_tex_height;

static ovrHmd hmd;
static ovrSizei eyeres[2];
static ovrEyeRenderDesc eye_rdesc[2];
static ovrGLTexture fb_ovr_tex[2];
static union ovrGLConfig glcfg;
static unsigned int distort_caps;
static unsigned int hmd_caps;

void Renderer::init(unsigned int screenWidth, unsigned int screenHeight)
{
	/* libovr must be initialized before we create the OpenGL context */
    ovr_Initialize();

	this->screenHeight = screenHeight;
	this->screenWidth = screenWidth;

	for (int i = 0;i < 10;i++) {
		effectList[i].duration = 0;
	}

	if ( SDL_Init(SDL_INIT_AUDIO|SDL_INIT_VIDEO) < 0 ) {
		fprintf(stderr, "Impossible d'initialiser SDL: %s\n", SDL_GetError());
		exit(1);
	}

    atexit(SDL_Quit);

     SDL_Init(SDL_INIT_VIDEO);

    // Version d'OpenGL
      
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
      
      
    // Double Buffer
      
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);


	displayWindow = SDL_CreateWindow("Test SDL 2.0", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, this->screenWidth, this->screenHeight, SDL_WINDOW_OPENGL  /*| SDL_WINDOW_FULLSCREEN*/);
    
    // Création du contexte OpenGL
  
    contexteOpenGL = SDL_GL_CreateContext(displayWindow);
  
    if(contexteOpenGL == 0)
    {
        std::cout << SDL_GetError() << std::endl; //// >> AFFICHE : " the specified window isn't an OpenGL window"
        SDL_DestroyWindow(displayWindow);
        SDL_Quit();
  
        exit(-1); //// >> PLANTE ICI : return -1 ..
    }
      

	if (TTF_Init() < 0) {
		puts("ERROR : unable to initialize font library");
		exit(1);
	}

	this->font = TTF_OpenFont( "digital display tfb.ttf",28);
	if (this->font == NULL)
	{
		puts("ERROR : unable to load font");
		exit(1);
	}


#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    unsigned int rmask = 0xff000000;
    unsigned int gmask = 0x00ff0000;
    unsigned int bmask = 0x0000ff00;
    unsigned int amask = 0x000000ff;
#else
    unsigned int rmask = 0x000000ff;
    unsigned int gmask = 0x0000ff00;
    unsigned int bmask = 0x00ff0000;
    unsigned int amask = 0xff000000;
#endif
	this->textSurface = SDL_CreateRGBSurface( 0, this->screenWidth, this->screenHeight, 32, rmask, gmask, bmask, amask);
	bExit = false;

	shaderTexturing = new Shader();
	shaderTexturing->load_fragment("fragment_texturing.gl");
	shaderTexturing->load_vertex("vertex.gl");

	Texture * texture = new Texture(2,2,(unsigned char*)g_texdata);
	this->spriteDrawing = new Sprite(texture,100.f,100.f,0,0,1,1);
	Texture * textSurfaceTexture = new Texture(this->screenWidth,this->screenHeight,(unsigned char*)this->textSurface->pixels);
	this->spriteTextSurface = new Sprite(textSurfaceTexture,(float)this->screenWidth,(float)this->screenHeight,0,1,1,0);
	this->fbDrawing = NULL;


	//Texture * checker_texture = new Texture(2,2,(unsigned char*)g_checker_texdata);


	//unsigned int triangleCount = 0;

	T_SPACE_OBJECT * spaceObjectArray = NULL;
	unsigned int spaceCount = 0;
	ClosedSpaceGenerator::generateSpace(100.f,10.f,&spaceObjectArray,&spaceCount,1024,256);
	printf("Generated space : space count = %d\n",spaceCount);

	g_shader_debug = new Shader();
	g_shader_debug->load_fragment("fragment_debug.gl");
	g_shader_debug->load_vertex("vertex.gl");

	
	for (int i = 0;i < spaceCount;i++) {
		Renderable * renderable = Renderable::createRenderable(g_shader_debug, spaceObjectArray[i].texture, spaceObjectArray[i].triangleArray, spaceObjectArray[i].triangleCount);
		renderable->setSecondaryTexture(spaceObjectArray[i].lightMapTexture);
		g_renderableList.push_back(renderable);
	}
	
	camera = new Camera();
	camera->SetClipping(0.1f,200.f);
	camera->SetPosition(glm::vec3(14.7f,1.9f,57.5f));
	//camera->SetLookAt(glm::vec3(35.3f,7.0f,46.6f));
	for (int i = 0;i < 25;i++) {
		camera->ChangePitch(1.f);
		camera->Update();
	}
	
	for (int i = 0;i < 20;i++) {
		camera->ChangeHeading(1.f);
		camera->Update();
	}
	//camera->ChangeHeading(90.f);
	

	OutputConsole::setRenderer(this);

 	if(!(hmd = ovrHmd_Create(0))) {
    		fprintf(stderr, "failed to open Oculus HMD, falling back to virtual debug HMD\n");
    		if(!(hmd = ovrHmd_CreateDebug(ovrHmd_DK2))) {
    			fprintf(stderr, "failed to create virtual debug HMD\n");
    			exit(0);
    		}
   }

/* resize our window to match the HMD resolution */
   SDL_SetWindowSize(displayWindow, hmd->Resolution.w, hmd->Resolution.h);
   SDL_SetWindowPosition(displayWindow, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
   int win_width = hmd->Resolution.w;
   int win_height = hmd->Resolution.h;
    
   /* enable position and rotation tracking */
   ovrHmd_ConfigureTracking(hmd, ovrTrackingCap_Orientation | ovrTrackingCap_MagYawCorrection | ovrTrackingCap_Position, 0);
   /* retrieve the optimal render target resolution for each eye */
   eyeres[0] = ovrHmd_GetFovTextureSize(hmd, ovrEye_Left, hmd->DefaultEyeFov[0], 1.0);
   eyeres[1] = ovrHmd_GetFovTextureSize(hmd, ovrEye_Right, hmd->DefaultEyeFov[1], 1.0);
    
   /* and create a single render target texture to encompass both eyes */
   //fb_width = eyeres[0].w + eyeres[1].w;
   //fb_height = eyeres[0].h > eyeres[1].h ? eyeres[0].h : eyeres[1].h;
   //update_rtarg(fb_width, fb_height);
    
   /* fill in the ovrGLTexture structures that describe our render target texture */
   for(int i=0; i<2; i++) {
    		fb_ovr_tex[i].OGL.Header.API = ovrRenderAPI_OpenGL;
    		fb_ovr_tex[i].OGL.Header.TextureSize.w = screenWidth;
    		fb_ovr_tex[i].OGL.Header.TextureSize.h = screenHeight;
    		/* this next field is the only one that differs between the two eyes */
			fb_ovr_tex[i].OGL.Header.RenderViewport.Pos.x = i == 0 ? 0 : screenWidth / 2.0;
    		fb_ovr_tex[i].OGL.Header.RenderViewport.Pos.y = 0;
    		fb_ovr_tex[i].OGL.Header.RenderViewport.Size.w = screenWidth / 2.0;
    		fb_ovr_tex[i].OGL.Header.RenderViewport.Size.h = screenHeight;
    		fb_ovr_tex[i].OGL.TexId = fb_tex;	/* both eyes will use the same texture id */
    	}
    
   /* fill in the ovrGLConfig structure needed by the SDK to draw our stereo pair
	* to the actual HMD display (SDK-distortion mode)
	*/
   
   memset(&glcfg, 0, sizeof glcfg);
   glcfg.OGL.Header.API = ovrRenderAPI_OpenGL;
   glcfg.OGL.Header.BackBufferSize = hmd->Resolution;
   glcfg.OGL.Header.Multisample = 1;
   /* 
   #ifdef WIN32
   157 	glcfg.OGL.Window = GetActiveWindow();
   158 	glcfg.OGL.DC = wglGetCurrentDC();
   159 #else
   160 	glcfg.OGL.Disp = glXGetCurrentDisplay();
   161 	glcfg.OGL.Win = glXGetCurrentDrawable();
   162 #endif
   */
   if(hmd->HmdCaps & ovrHmdCap_ExtendDesktop) {
    		printf("running in \"extended desktop\" mode\n");
    	} else {
    		/* to sucessfully draw to the HMD display in "direct-hmd" mode, we have to
    		 * call ovrHmd_AttachToWindow
    		 * XXX: this doesn't work properly yet due to bugs in the oculus 0.4.1 sdk/driver
    		 */
			ovrHmd_AttachToWindow(hmd, glcfg.OGL.Window, 0, 0);
			/*
   #ifdef WIN32
    		ovrHmd_AttachToWindow(hmd, glcfg.OGL.Window, 0, 0);
   #else
    		ovrHmd_AttachToWindow(hmd, (void*)glcfg.OGL.Win, 0, 0);
   #endif
    		printf("running in \"direct-hmd\" mode\n");
			*/
    }
    
	/* enable low-persistence display and dynamic prediction for lattency compensation */
	hmd_caps = ovrHmdCap_LowPersistence | ovrHmdCap_DynamicPrediction;
	ovrHmd_SetEnabledCaps(hmd, hmd_caps);
    
    /* configure SDK-rendering and enable chromatic abberation correction, vignetting, and
    	* timewrap, which shifts the image before drawing to counter any lattency between the call
    	* to ovrHmd_GetEyePose and ovrHmd_EndFrame.
    	*/
    distort_caps = ovrDistortionCap_Chromatic | ovrDistortionCap_Vignette | ovrDistortionCap_TimeWarp |
    	ovrDistortionCap_Overdrive;
   if(!ovrHmd_ConfigureRendering(hmd, &glcfg.Config, distort_caps, hmd->DefaultEyeFov, eye_rdesc)) {
    		fprintf(stderr, "failed to configure distortion renderer\n");
			exit(0);
   }
    
	// Start the sensor which provides the Rift’s pose and motion.
	ovrHmd_ConfigureTracking(hmd, ovrTrackingCap_Orientation |
		ovrTrackingCap_MagYawCorrection |
		ovrTrackingCap_Position, 0);

   /* disable the retarded "health and safety warning" */
  //ovrhmd_EnableHSWDisplaySDKRender(hmd, 0);
	headWidget = new UIHeader();
	UIHeader * header1 = new UIHeader();
	header1->setLabel("hello");
	headWidget->addChild(header1);
	UIHeader * header2 = new UIHeader();
	header2->setLabel("world");
	headWidget->addChild(header2);
}


bool Renderer::exitstate()
{
	return bExit; 
}

int thread_func(void *data)
{
	Renderer * renderer = (Renderer*)data;
    int last_value = 0;

    while ( renderer->exitstate() != true ) {
		renderer->draw();
    }
    printf("Thread quitting\n");
    return(0);
}


void Renderer::drawFps()
{
	char s[256];
	sprintf(s,"FPS %.3d",this->getFps());
	drawMessage(s,ALIGNRIGHT,ALIGNTOP);
}

void Renderer::drawMessage(const char * message,float x,float y) {
	if (strlen(message) == 0)
		return;

	SDL_Color fontcolor = {255, 255, 255};

	SDL_Surface * textSur = TTF_RenderText_Solid(this->font,message,fontcolor);	//set the text surface
	if (textSur == NULL)
	{
		
        fprintf(stderr, "Unable to create draw surface: %s\n", SDL_GetError());
		printf("%s\n",message);
        return;
	}

	drawMessage(textSur,x,y);
	SDL_FreeSurface(textSur);	
}

void Renderer::drawMessage(SDL_Surface * textSur,float x,float y) {
	SDL_Rect rect;
	rect.x = x;
	rect.y = y;
	SDL_BlitSurface(textSur,NULL,this->textSurface,&rect);
}

void Renderer::drawMessage(const char * message,RendererTextAlign hAlign,RendererTextAlign vAlign)
{
	SDL_Color fontcolor = {255, 255, 255};
	const float hudScale = 0.75f;

	if (strlen(message) == 0)
		return;

	SDL_Surface * textSur = TTF_RenderText_Solid(this->font,message,fontcolor);	//set the text surface
	if (textSur == NULL)
	{
		
        fprintf(stderr, "Unable to create draw surface: %s\n", SDL_GetError());
		printf("%s\n",message);
        return;
	}
	SDL_Rect rect;
	switch (hAlign)
	{
	case ALIGNLEFT:
		rect.x = (this->textSurface->w - this->textSurface->w* hudScale );
		break;
	case ALIGNRIGHT:
		rect.x = (this->textSurface->w * hudScale - textSur->w) ;
		break;
	case ALIGNCENTER:
		rect.x = (this->textSurface->w - textSur->w)/2;
		break;
	}

	switch (vAlign)
	{
	case ALIGNTOP:
		rect.y = (this->textSurface->h - this->textSurface->h* hudScale );
		break;
	case ALIGNBOTTOM:
		rect.y = (this->textSurface->h * hudScale - textSur->h) ;
		break;
	case ALIGNCENTER:
		rect.y = (this->textSurface->h - textSur->h)/2;
		break;
	}

	drawMessage(textSur,rect.x,rect.y);
	SDL_FreeSurface(textSur);	
}

void Renderer::draw()
{
	// Remove warning message
	ovrHSWDisplayState hswDisplayState;
	ovrHmd_GetHSWDisplayState(hmd, &hswDisplayState);
	if (hswDisplayState.Displayed) {
		ovrHmd_DismissHSWDisplay(hmd);
	}

	/* the drawing starts with a call to ovrHmd_BeginFrame */
#ifdef OVR
   ovrHmd_BeginFrame(hmd, 0);
#endif

	SDL_FillRect(this->textSurface, NULL, 0x000000);

	if (this->fbDrawing == NULL){ 
		this->fbDrawing = new FrameBuffer(this->screenWidth,this->screenHeight);
		this->fbDrawing->do_register();
		this->spriteDrawing = new Sprite(this->fbDrawing->getTexture(),(float)this->screenWidth,(float)this->screenHeight,0,0,1,1);		

		#ifdef OVR
		for(int i=0; i<2; i++) {
			fb_ovr_tex[i].OGL.TexId = this->fbDrawing->getTextureId();	/* both eyes will use the same texture id */
    	}
		#endif
	}
	
	this->fbDrawing->bind();
	//OpenGL setup
	glDisable(GL_CULL_FACE);
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

	ovrPosef headPose[2];

	#ifdef OVR
	// Query the HMD for the current tracking state.
	ovrTrackingState ts = ovrHmd_GetTrackingState(hmd, ovr_GetTimeInSeconds());
	if (ts.StatusFlags & (ovrStatus_OrientationTracked | ovrStatus_PositionTracked))
	{
		ovrPoseStatef pose = ts.HeadPose;
		glm::quat quat = glm::quat(pose.ThePose.Orientation.w,pose.ThePose.Orientation.x,pose.ThePose.Orientation.y,pose.ThePose.Orientation.z);
		glm::vec3 pose3f = glm::eulerAngles(quat);
		this->camera->MoveOvr(-pose3f[0],pose3f[1],pose3f[2]);
	}
	#endif
	
	camera->Update();
	for (int eyeid = 0;eyeid < 2;eyeid++) {
	//this->fbDrawing->unbind(screenWidth,screenHeight);	

	//this->fbDrawing->bind();
		
	

	/*
	this->fbDrawing->bind();
	glClearColor(1, 1, 1, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
	this->fbDrawing->unbind(screenWidth,screenHeight);
	*/
	//GLfloat matrix[16];
	//glGetFloatv(GL_MODELVIEW_MATRIX, matrix);

	
	glViewport((GLint) (0) + eyeid*(GLsizei) this->screenWidth/2,
				(GLint) (0),
				(GLsizei) this->screenWidth/2,
				(GLsizei) this->screenHeight);
	
	
	camera->SetViewport((GLint) (0) + eyeid*(GLsizei) this->screenWidth/2,
				(GLint) (0),
				(GLsizei) this->screenWidth/2,
				(GLsizei) this->screenHeight);
	/*
	glViewport((GLint) (0),
				(GLint) (0),
				(GLsizei) this->screenWidth,
				(GLsizei) this->screenHeight);
	*/

	/*
	camera->SetViewport((GLint) (matrix[12]),
				(GLint) (-matrix[13]),
				(GLsizei) this->screenWidth,
				(GLsizei) this->screenHeight);
	*/
	glMatrixMode(GL_MODELVIEW);

	//
	glEnable(GL_DEPTH_TEST);

	//glm::mat4 projMat = glm::ortho( 0.f, (float)this->screenWidth, (float)this->screenHeight, 0.f, -1.f, 1.f );
	//memcpy(this->shaderTexturing->getProjectionMatrix(), glm::value_ptr(projMat),sizeof(float)*16);

	//glm::mat4 projection = glm::perspective(110.0f, (float)screenWidth/2/(float)screenHeight, 0.1f, 200.0f);
	
	
	glm::mat4 P = glm::mat4();
	glm::mat4 M = glm::mat4();
	glm::mat4 V = glm::mat4();


	camera->GetMatricies(P,V,M);

	if (eyeid == 0) {
		V = camera->TranslateToEye(-0.5f);
	} else {
		V = camera->TranslateToEye(0.5f);
	}

	memcpy(g_shader_debug->getProjectionMatrix(),glm::value_ptr(P),sizeof(float)*16);
	memcpy(g_shader_debug->getModelViewMatrix(),glm::value_ptr(V),sizeof(float)*16);
	
	g_shader_debug->bind();
	g_shader_debug->bind_attributes();


	for (int i = 0;i < g_renderableList.size();i++) {
		g_renderableList[i]->draw();
	}
	
	g_shader_debug->unbind();

	
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);
	
	this->shaderTexturing->bind();

	memcpy(this->shaderTexturing->getModelViewMatrix(), glm::value_ptr(glm::mat4()),sizeof(float)*16);
	this->shaderTexturing->bind_attributes();

	glDisable(GL_DEPTH_TEST);
	char s[1024];
	glm::vec3 cam_pos = camera->getPosition();
	sprintf(s,"Pos: %.1f %.1f %.1f",cam_pos[0],cam_pos[1],cam_pos[2]);
	this->drawMessage(s,RendererTextAlign::ALIGNRIGHT,RendererTextAlign::ALIGNBOTTOM);
	this->drawMessage("TOPLEFT",RendererTextAlign::ALIGNLEFT,RendererTextAlign::ALIGNTOP);
	this->drawMessage("BOTTOMLEFT",RendererTextAlign::ALIGNLEFT,RendererTextAlign::ALIGNBOTTOM);

	this->headWidget->drawChilds(this);

	this->drawFps();
	OutputConsole::render();

	glEnable(GL_BLEND);
	this->spriteTextSurface->updateTexture();
	this->spriteTextSurface->draw();
	glDisable(GL_BLEND);	
	this->shaderTexturing->unbind();
	
	

	#ifdef OVR
	ovrEyeType eye = hmd->EyeRenderOrder[eyeid];
	headPose[eyeid] = ovrHmd_GetHmdPosePerEye(hmd, eye);
	#endif
	
	}

	this->fbDrawing->unbind(screenWidth,screenHeight);
	this->shaderTexturing->bind();

	glm::mat4 projMat = glm::ortho( 0.f, (float)this->screenWidth, (float)this->screenHeight, 0.f, -1.f, 1.f );
	memcpy(this->shaderTexturing->getProjectionMatrix(), glm::value_ptr(projMat),sizeof(float)*16);
	memcpy(this->shaderTexturing->getModelViewMatrix(), glm::value_ptr(glm::mat4()),sizeof(float)*16);
	this->shaderTexturing->bind_attributes();
	this->spriteDrawing->draw();
	this->shaderTexturing->unbind();

	#ifndef OVR
	SDL_GL_SwapWindow(displayWindow);
	#endif

#ifdef OVR
	ovrHmd_EndFrame(hmd, headPose, &fb_ovr_tex[0].Texture);
#endif
}

void Renderer::loop()
{
	SDL_Event event;
	T_EFFECT effect;
	int told = SDL_GetTicks();

		int asyncup = 0;
		int asyncdown = 0;
		int asynleft = 0;
		int asynright = 0;
		int asyncspace = 0;

	while (!bExit)
	{
		frameCounter++;
		int tnew = SDL_GetTicks();
		int dt = tnew - told;
		told = tnew;

		if (dt != 0)
			this->fps = 1000/dt;

		while( SDL_PollEvent( &event ) )
		{
			switch( event.type )
			{
				case SDL_KEYDOWN:
					switch( event.key.keysym.sym )
					{
						case SDLK_SPACE :
							asyncspace = 1;
							break;
						case SDLK_UP:
							asyncup = 1;
							camera->Move(CameraDirection::FORWARD);
							break;
						case SDLK_DOWN:
							asyncdown = 1;
							camera->Move(CameraDirection::BACK);
							break;
						case SDLK_LEFT:
							asynleft = 1;
							camera->Move(CameraDirection::LEFT);
							break;
						case SDLK_RIGHT:
							asynright = 1;
							camera->Move(CameraDirection::RIGHT);
							break;
						case SDLK_ESCAPE:
							bExit = true;
							break;

						default:
							break;
					}
				break;
				case SDL_KEYUP:
					switch( event.key.keysym.sym )
					{
						case SDLK_SPACE :
							asyncspace = 0;
							break;
						case SDLK_UP:
							asyncup = 0;
							break;
						case SDLK_DOWN:
							asyncdown = 0;
							break;
						case SDLK_LEFT:
							asynleft = 0;
							break;
						case SDLK_RIGHT:
							asynright = 0;
							break;
						case SDLK_ESCAPE:
							bExit = true;
							break;
						case SDLK_7 :
							effect.duration = 30;
							effect.effectType = 3;
							addEffect(effect);			
							break;
						case SDLK_9 :
							effect.duration = 30;
							effect.effectType = 2;
							addEffect(effect);			
							break;
						case SDLK_0 :
							effect.duration = 30;
							effect.effectType = 1;
							addEffect(effect);
							break;
						default:
							break;
					}
				break;
				case SDL_MOUSEMOTION:
					camera->Move2D(event.motion.x,event.motion.y);
					break;
			}
                    
		}
		

		if (asyncup)
		{

		}

		if (asyncdown)
		{	
		}

		if (asynleft)
		{
		}

		if (asynright)
		{
		}

		if (asyncspace)
		{
		}


		this->draw();

		SDL_Delay(10);
	}

    
	SDL_GL_DeleteContext(contexteOpenGL);
    SDL_DestroyWindow(displayWindow);
    SDL_Quit();
}

int Renderer::getFps()
{
	return this->fps;
}

void Renderer::processEffect(T_EFFECT &effect) {
	effect.duration--;
	switch (effect.effectType) {
	case 4 : {
				this->drawMessage(effect.str_param,effect.fl_param1,effect.fl_param2);
			 }
			 break;
	case 3 : {
			}
			break;
	case 2  :
			break;
	case 1 : 
			break;
	}
}

void Renderer::addEffect(T_EFFECT effect) {
	for(int i = 0;i < 10;i++) {
		if (effectList[i].duration == 0) {
			effectList[i].duration = effect.duration;
			effectList[i].effectType = effect.effectType;
			strcpy(effectList[i].str_param, effect.str_param);
			effectList[i].fl_param1 = effect.fl_param1;
			effectList[i].fl_param2 = effect.fl_param2;
			return;
		}
	}
}