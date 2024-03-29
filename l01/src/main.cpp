//theodore peters 260919785
#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "DAGNode.h"
#include "GLSL.h"
#include "MatrixStack.h"
#include "Motion.h"
#include "Program.h"
#include "Shape.h"

using namespace std;

GLFWwindow *window; // Main application window
string RES_DIR = ""; // Where data files live
int FILE_ID = 0;
shared_ptr<Program> prog;
shared_ptr<Program> prog2; // for drawing with colours
shared_ptr<Shape> teapot;
shared_ptr<Shape> sphere;
shared_ptr<Shape> cube;
shared_ptr<Motion> skeleton;

int refreshRate = 60;
int fc = 0;
int fi = 1;
int fd = 1;
bool play = true;
GLuint vao;	
GLuint posBufID; // position buffer for drawing a line loop
GLuint aPosLocation = 0; // location set in col_vert.glsl (or can be queried)
const GLuint NumVertices = 6;
GLfloat vertices[NumVertices][3] = {{0, 0, 0},
									{8, 0, 0},
									{0, 0, 0},
									{0, 8, 0},
									{0, 0, 0},
									{0, 0, 8}};


static void error_callback(int error, const char *description)
{
	cerr << description << endl;
}

static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
  if(action == GLFW_PRESS) {
	switch(key) {
	case GLFW_KEY_ESCAPE:
	  glfwSetWindowShouldClose(window, GL_TRUE);
	  break;
	case GLFW_KEY_SPACE:
	  play = !play;
	  break;
	case GLFW_KEY_UP:
	  fi++;
	  break;
	case GLFW_KEY_DOWN:
	  if(fi>1)fi--;
	  break;
	case GLFW_KEY_ENTER:
	  fd*=-1;
	  break;
	default:
	  break;
	}
  }
}

static void init()
{
	GLSL::checkVersion();

	// Check how many texture units are supported in the vertex shader
	int tmp;
	glGetIntegerv(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, &tmp);
	cout << "GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS = " << tmp << endl;
	// Check how many uniforms are supported in the vertex shader
	glGetIntegerv(GL_MAX_VERTEX_UNIFORM_COMPONENTS, &tmp);
	cout << "GL_MAX_VERTEX_UNIFORM_COMPONENTS = " << tmp << endl;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &tmp);
	cout << "GL_MAX_VERTEX_ATTRIBS = " << tmp << endl;

	// Set background color.
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	// Enable z-buffer test.
	glEnable(GL_DEPTH_TEST);

	// Initialize meshes.
	teapot = make_shared<Shape>();
	teapot->loadMesh(RES_DIR + "teapot.obj");
	teapot->init();
	sphere = make_shared<Shape>();
	sphere->loadMesh(RES_DIR + "sphere.obj");
	sphere->init();
	cube = make_shared<Shape>();
	cube->loadMesh(RES_DIR + "cube.obj");
	cube->init();	
	
	skeleton = make_shared<Motion>();
	std::string filename;
	switch(FILE_ID) {
	case 0:
	  filename = "0019_AdvanceBollywoodDance001.bvh";
	  break;
	case 1:
	  filename = "Cyrus_Take6.bvh";
	  break;
	case 2:
	  filename = "OptiTrack-IITSEC2007.bvh";
	  break;
	}
	skeleton->loadBVH(RES_DIR + filename);
	cout << "skeleton number of datapoints: " << skeleton->numChannels << "\n";
	
	// Initialize the GLSL programs.
	prog = make_shared<Program>();
	prog->setVerbose(true);
	prog->setShaderNames(RES_DIR + "nor_vert.glsl", RES_DIR + "nor_frag.glsl");
	prog->init();
	prog->addUniform("P");
	prog->addUniform("MV");
	prog->addAttribute("aPos");
	prog->addAttribute("aNor");
	prog->setVerbose(false);
	
	prog2 = make_shared<Program>();
	prog2->setVerbose(true);
	prog2->setShaderNames(RES_DIR + "axs_vert.glsl", RES_DIR + "col_frag.glsl");
	prog2->init();
	prog2->addUniform("P");
	prog2->addUniform("MV");
	prog2->addAttribute("aPos");
	prog2->setVerbose(false);
	
	// If there were any OpenGL errors, this will print something.
	// You can intersperse this line in your code to find the exact location
	// of your OpenGL error.
	GLSL::checkError(GET_FILE_LINE);

	// Create a buffers for doing some line drawing
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glGenBuffers(1, &posBufID);
	glBindBuffer(GL_ARRAY_BUFFER, posBufID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices),	vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(aPosLocation);
	glVertexAttribPointer(aPosLocation, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	
}

static void drawAxes(const std::shared_ptr<Program> prog2, std::shared_ptr<MatrixStack> MV) {
  	glUniformMatrix4fv(prog2->getUniform("MV"), 1, GL_FALSE, &MV->topMatrix()[0][0]);
	glDrawArrays(GL_LINES, 0, NumVertices);
}

static void render()
{
	// Get current frame buffer size.
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	float aspect = width/(float)height;
	if (isnan(aspect))
	{
		aspect = 0;
	}
	glViewport(0, 0, width, height);

	// Clear framebuffer.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Create matrix stacks.
	auto P = make_shared<MatrixStack>();
	auto MV = make_shared<MatrixStack>();
	// Apply projection.
	P->pushMatrix();
	P->multMatrix(glm::perspective((float)(45.0*M_PI/180.0), aspect, 0.01f, 100.0f));
	// Apply camera transform.
	MV->pushMatrix();
	MV->translate(glm::vec3(0, -1, -3));
	MV->scale(0.01);
	
	// Draw axes.
	prog2->bind();
	glUniformMatrix4fv(prog2->getUniform("P"), 1, GL_FALSE, &P->topMatrix()[0][0]);
	glBindVertexArray(vao);
	drawAxes(prog2, MV);
    prog2->unbind();

	prog->bind();
	glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, &P->topMatrix()[0][0]);
	prog->unbind();
	
	skeleton->root->draw(prog, prog2, MV, vao, FILE_ID, teapot, sphere, cube, skeleton->data + fc * skeleton->numChannels, true); // set this boolean to false for a truly cursed image
	
	// Pop matrix stacks.
	MV->popMatrix();
	P->popMatrix();
	
	GLSL::checkError(GET_FILE_LINE);

	fc = play? (fc + fd*fi + skeleton->numFrames) % skeleton->numFrames : fc;
}

int main(int argc, char **argv)
{
	if(argc < 2) {
		cout << "Please specify the resource directory." << endl;
		return 0;
	}
	RES_DIR = argv[1] + string("/");

	
	if(argc >= 3) {
	  if(!strcmp(argv[2], "1"))
		FILE_ID = 1;
	  else if(!strcmp(argv[2], "2"))
		FILE_ID = 2;
	}
	
	// Set error callback.
	glfwSetErrorCallback(error_callback);
	// Initialize the library.
	if(!glfwInit()) {
		return -1;
	}
    
	// https://en.wikipedia.org/wiki/OpenGL
    // hint to use OpenGL 4.1 on all paltforms
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    
	// Create a windowed mode window and its OpenGL context.
	window = glfwCreateWindow(640, 480, "Theoodore Peters 260919785", NULL, NULL);
	if(!window) {
		glfwTerminate();
		return -1;
	}
	// Make the window's context current.
	glfwMakeContextCurrent(window);
	// Initialize GLEW.
	glewExperimental = true;
	if(glewInit() != GLEW_OK) {
		cerr << "Failed to initialize GLEW" << endl;
		return -1;
	}
	glGetError(); // A bug in glewInit() causes an error that we can safely ignore.
	cout << "OpenGL version: " << glGetString(GL_VERSION) << endl;
	cout << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;
	// Set vsync.
	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(monitor);
	refreshRate = mode->refreshRate;
	glfwSwapInterval((int)((float)(refreshRate) / 60.0));
	// Set keyboard callback.
	glfwSetKeyCallback(window, key_callback);
	// Initialize scene.
	init();
	// Loop until the user closes the window.
	while(!glfwWindowShouldClose(window)) {
		// Render scene.
		render();
		// Swap front and back buffers.
		glfwSwapBuffers(window);
		// Poll for and process events.
		glfwPollEvents();
	}
	// Quit program.
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}
