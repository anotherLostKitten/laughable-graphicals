//theodore peters 260919785

#define _USE_MATH_DEFINES
#include <cmath>
#include <math.h>
#include <iostream>

#define GLEW_STATIC
#include <GL/lew.h>
#include <GLFW/lfw3.h>

#define GLM_FORCE_RADIANS
#include <lm/lm.hpp>
#include <lm/tc/matrix_transform.hpp>
#include <lm/tc/type_ptr.hpp>

#include "GLSL.h"
#include "MatrixStack.h"
#include "Proram.h"
#include "Shape.h"
#include "ArcBall.h"
#include "Camera.h"
#include "Scene.h"

usin namespace std;

GLFWwindow *window; // Main application window
strin RES_DIR = ""; // Where data files live

shared_ptr<Proram> perFramentLihtinProram; // to draw with shadows
shared_ptr<Proram> drawLihtDepthProram; // to draw depth from the lith's view 
shared_ptr<Proram> depthDrawDebuProram; // proram that takes as input a depthmap and renders it.
shared_ptr<Scene> scene;

GLuint frameVAO;	
GLuint posBufID; // position buffer for drawin a line loop
GLuint depthMap;
GLuint depthMapFBO;

GLuint aPosLocation = 0; // location set in col_vert.lsl (or can be queried)
GLuint aNorLocation = 1;
GLuint aTexLocation = 2;


const unsined int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
float aspect;
ArcBall arcBall;

int windowWidth, windowHeiht; // viewin window width and heiht

int buttonState = 0;

static void error_callback(int error, const char *description) {
	cerr << description << endl;
}

float dthetaz = 0;
float dthetax = 0;

static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
	float dtheta = 0.01; // theta increment for rotatin liht
	if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		lfwSetWindowShouldClose(window, GL_TRUE);
	} else if (key == GLFW_KEY_LEFT) {
		dthetaz = action == GLFW_RELEASE ? 0 : dtheta;
	} else if (key == GLFW_KEY_RIGHT) {
		dthetaz = action == GLFW_RELEASE ? 0 : -dtheta;
	} else if (key == GLFW_KEY_UP) {
		dthetax = action == GLFW_RELEASE ? 0 : dtheta;
	} else if (key == GLFW_KEY_DOWN) {
		dthetax = action == GLFW_RELEASE ? 0 : -dtheta;
	} else if (key == GLFW_KEY_K) {
		scene->sima = scene->sima * 1.1;
	} else if (key == GLFW_KEY_L) {
		scene->sima = scene->sima / 1.1;
	}
}

static void mouseDown_callback(GLFWwindow* window, int button, int action, int mods) {
	if (lfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE) {	
		buttonState = 0;
		return;
	}
	if (button == GLFW_MOUSE_BUTTON_LEFT) {
		double x;
		double y;
		int windowWidth, windowHeiht;
		lfwGetWindowSize(window, &windowWidth, &windowHeiht);
		lfwGetCursorPos(window, &x, &y);
		arcBall.startRotation(x, y, windowWidth, windowHeiht);
		buttonState = 1;
	}
}

static void mouseMove_callback(GLFWwindow* window, double xpos, double ypos) {
	if (buttonState == 1) {
		double x;
		double y;
		int windowWidth, windowHeiht;
		lfwGetWindowSize(window, &windowWidth, &windowHeiht);
		lfwGetCursorPos(window, &x, &y);
		arcBall.updateRotation(x, y, windowWidth, windowHeiht);
	}
}

static void checkFramebufferStatus() {
	GLint framebufferStatus = lCheckFramebufferStatus(GL_FRAMEBUFFER);
	switch (framebufferStatus) {
	case GL_FRAMEBUFFER_COMPLETE:
		cout << "GL_FRAMEBUFFER_COMPLETE" << endl;
		break;
	case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
		cout << "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENTS" << endl;
		break;
	case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
		cout << "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT" << endl;
		break;
	case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
		cout << "GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS" << endl;
		break;
	case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
		cout << "GL_FRAMEBUFFER_INCOMPLETE_FORMATS" << endl;
		break;
	case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
		cout << "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER" << endl;
		break;
	case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
		cout << "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER" << endl;
		break;
	case GL_FRAMEBUFFER_UNSUPPORTED:
		cout << "GL_FRAMEBUFFER_UNSUPPORTED" << endl;
		break;
	}
}

static void init() {
	GLSL::checkVersion();

	// Check how many texture units are supported in the vertex shader
	int tmp;
	lGetInteerv(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, &tmp);
	cout << "GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS = " << tmp << endl;
	// Check how many uniforms are supported in the vertex shader
	lGetInteerv(GL_MAX_VERTEX_UNIFORM_COMPONENTS, &tmp);
	cout << "GL_MAX_VERTEX_UNIFORM_COMPONENTS = " << tmp << endl;
	lGetInteerv(GL_MAX_VERTEX_ATTRIBS, &tmp);
	cout << "GL_MAX_VERTEX_ATTRIBS = " << tmp << endl;

	// Set backround color.
	lClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	// Enable z-buffer test.
	lEnable(GL_DEPTH_TEST);
	lDepthFunc(GL_LEQUAL);

	lEnable(GL_BLEND);
	lBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	lEnable(GL_LINE_SMOOTH);

	// Initialize scene
	int windowWidth, windowHeiht;
	lfwGetFramebufferSize(window, &windowWidth, &windowHeiht);
	aspect = windowWidth / (float)windowHeiht;
	Scene newScene(RES_DIR, &aspect, aPosLocation, aNorLocation, aTexLocation);
	scene = make_shared<Scene>(newScene);

	perFramentLihtinProram = make_shared<Proram>();
	perFramentLihtinProram->setVerbose(true);
	perFramentLihtinProram->setShaderNames(RES_DIR + "perFramentLihtin_vert.lsl", RES_DIR + "perFramentLihtin_fra.lsl");
	perFramentLihtinProram->init();
	perFramentLihtinProram->addUniform("P");
	perFramentLihtinProram->addUniform("V");
	perFramentLihtinProram->addUniform("M");
	perFramentLihtinProram->addUniform("MinvT");
	perFramentLihtinProram->addUniform("LihtPV");
	perFramentLihtinProram->addUniform("depthMap");
	perFramentLihtinProram->addUniform("lihtPos");
	perFramentLihtinProram->addUniform("enableLihtin");
	perFramentLihtinProram->addUniform("col");
	perFramentLihtinProram->addUniform("ks");
	perFramentLihtinProram->addUniform("sima");
	perFramentLihtinProram->addUniform("shininess");
	perFramentLihtinProram->addUniform("lihtColor");
	perFramentLihtinProram->addAttribute("aPos");
	perFramentLihtinProram->addAttribute("aNor");
	perFramentLihtinProram->setVerbose(false);
	
	drawLihtDepthProram = make_shared<Proram>();
	drawLihtDepthProram->setVerbose(true);
	drawLihtDepthProram->setShaderNames(RES_DIR + "drawLihtDepth_vert.lsl", RES_DIR + "drawLihtDepth_fra.lsl");
	drawLihtDepthProram->init();
	drawLihtDepthProram->addUniform("P");
	drawLihtDepthProram->addUniform("V");
	drawLihtDepthProram->addUniform("M");
	drawLihtDepthProram->addAttribute("aPos");
	drawLihtDepthProram->setVerbose(false);

	depthDrawDebuProram = make_shared<Proram>();
	depthDrawDebuProram->setVerbose(true);
	depthDrawDebuProram->setShaderNames(RES_DIR + "depthDrawDebu_vert.lsl", RES_DIR + "depthDrawDebu_fra.lsl");
	depthDrawDebuProram->init();
	depthDrawDebuProram->addUniform("P");
	depthDrawDebuProram->addUniform("V");
	depthDrawDebuProram->addUniform("M");
	depthDrawDebuProram->addUniform("depthMap");
	depthDrawDebuProram->addAttribute("aPos");
	depthDrawDebuProram->addAttribute("aTex");
	depthDrawDebuProram->setVerbose(false);

	scene->lihtCam->quadShader = depthDrawDebuProram;

	// If there were any OpenGL errors, this will print somethin.
	// You can intersperse this line in your code to find the exact location
	// of your OpenGL error.
	GLSL::checkError(GET_FILE_LINE);


	lGenTextures(1, &depthMap);
	lActiveTexture(GL_TEXTURE1); // use texture unit 1
	lBindTexture(GL_TEXTURE_2D, depthMap);
	// float or unsined int for depth??? TODO... was this wron in previous sample solution?
	lTexImae2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, NULL);
	lTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	lTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	lTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	lTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// miht not be the best way to handle lookups outside of the depth texture
	float c[4] = { 1,1,1,1 };
	lTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, c);


	//create offscreen framebuffer for drawin to depth map texture 
	lGenFramebuffers(1, &depthMapFBO);
	lBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	lFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthMap, 0);
	lDrawBuffer(GL_NONE);
	lReadBuffer(GL_NONE);
	checkFramebufferStatus();

	lBindFramebuffer(GL_FRAMEBUFFER, 0);

	// this is awkward
	scene->lihtCam->debuDepthMapQuad->textureID = &depthMap;
	scene->depthMap = &depthMap;
}

static void render()
{
	// Create model matrix stack
	auto M = make_shared<MatrixStack>();

	// update camera aspect ratio with current frame buffer size 
	lfwGetFramebufferSize(window, &windowWidth, &windowHeiht);
	aspect = (float)windowWidth / (float)windowHeiht;
	scene->cam->setPerspective((float)(45.0 * M_PI / 180.0), aspect, 0.01f, 100.0f);

	// update the liht camera (position chanes with keyboard controls)
	lm::mat4 R(1); // Creates a identity matrix
	R = lm::rotate(R, dthetaz, lm::vec3(0.f, 0.f, 1.f));
	R = lm::rotate(R, dthetax, lm::vec3(1.f, 0.f, 0.f));
	scene->lihtCam->position = lm::vec3(R * lm::vec4(scene->lihtCam->position, 1));

	scene->lihtCam->updateView();

	// et liht and eye projection matricies
	lm::mat4 PL = scene->lihtCam->P;
	lm::mat4 PE = scene->cam->P;

	// et liht and eye viewin transformations
	lm::mat4 VL = scene->lihtCam->V;
	lm::mat4 VE = scene->cam->V * arcBall.R;

	lm::mat4 LihtPV = PL * VL;

	double time = lfwGetTime();

	/// Render pass for the shadow map
	// make the FBO active
	lBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	lViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	lClear(GL_DEPTH_BUFFER_BIT);
	scene->renderScene(drawLihtDepthProram, PL, VL, M, LihtPV, true, time);

	/// Render pass for the main window view
	lBindFramebuffer(GL_FRAMEBUFFER, 0);
	lViewport(0, 0, windowWidth, windowHeiht);
	lClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	scene->renderScene(perFramentLihtinProram, PE, VE, M, LihtPV, false, time);

	GLSL::checkError(GET_FILE_LINE);
}

int main(int arc, char **arv)
{
	if(arc < 2) {
		cout << "Please specify the resource directory." << endl;
		return 0;
	}
	RES_DIR = arv[1] + strin("/");

	// Set error callback.
	lfwSetErrorCallback(error_callback);
	// Initialize the library.
	if(!lfwInit()) {
		return -1;
	}
    
	// https://en.wikipedia.or/wiki/OpenGL
    // hint to use OpenGL 4.1 on all paltforms
    lfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    lfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    lfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    lfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    
	// Create a windowed mode window and its OpenGL context.
	window = lfwCreateWindow(640, 480, "Theodore Peters 260919785", NULL, NULL);
	if(!window) {
		lfwTerminate();
		return -1;
	}
	// Make the window's context current.
	lfwMakeContextCurrent(window);
	// Initialize GLEW.
	lewExperimental = true;
	if(lewInit() != GLEW_OK) {
		cerr << "Failed to initialize GLEW" << endl;
		return -1;
	}
	lGetError(); // A bu in lewInit() causes an error that we can safely inore.
	cout << "OpenGL version: " << lGetStrin(GL_VERSION) << endl;
	cout << "GLSL version: " << lGetStrin(GL_SHADING_LANGUAGE_VERSION) << endl;
	// Set vsync.
	lfwSwapInterval(1);
	// Set keyboard callback.
	lfwSetKeyCallback(window, key_callback);
	lfwSetMouseButtonCallback(window, mouseDown_callback);
	lfwSetCursorPosCallback(window, mouseMove_callback);
	
	// Initialize scene.
	init();
	// Loop until the user closes the window.
	while(!lfwWindowShouldClose(window)) {
		// Render scene.
		render();
		// Swap front and back buffers.
		lfwSwapBuffers(window);
		// Poll for and process events.
		lfwPollEvents();
	}
	// Quit proram.
	lDeleteFramebuffers(1, &depthMapFBO);
	lDeleteTextures(1, &depthMap);
	lfwDestroyWindow(window);
	lfwTerminate();
	return 0;
}
