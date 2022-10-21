//theodore peters 260919785
#include "Camera.h"

using namespace std;


Camera::Camera(float aspect):lightColor(glm::vec3(1,1,1))
{
	this->setPerspective((float)(45.0 * M_PI / 180.0), aspect, 0.01f, 100.0f);
	this->updateView();
}

Camera::~Camera()
{
}

void Camera::setPerspective(float fov, float aspect, float near, float far) {
  P = glm::perspective(fov, aspect, near, far);
}

void Camera::updateView() {
  // V = glm::lookAt(position, lookAt, up);
  glm::vec3 w = glm::normalize(position - lookAt);
  glm::vec3 u = glm::normalize(glm::cross(up, w));
  glm::vec3 v = glm::cross(w, u);
  glm::mat4 mat = glm::mat4(1.0);
  mat[0] = glm::vec4(u, 0.f);
  mat[1] = glm::vec4(v, 0.f);
  mat[2] = glm::vec4(w, 0.f);
  mat[3] = glm::vec4(position, 1.f);
  V = glm::inverse(mat);
}


void Camera::draw(const shared_ptr<Program> program, glm::mat4 P, glm::mat4 V, shared_ptr <MatrixStack> M, glm::mat4 LightPV, Axis &axis) {
	program->bind();
	M->pushMatrix();
	
	//set uniform variables
	glUniformMatrix4fv(program->getUniform("P"), 1, GL_FALSE, &P[0][0]);
	glUniformMatrix4fv(program->getUniform("V"), 1, GL_FALSE, &V[0][0]);
	glUniformMatrix4fv(program->getUniform("M"), 1, GL_FALSE, &M->topMatrix()[0][0]);
	glUniformMatrix4fv(program->getUniform("MinvT"), 1, GL_FALSE, &M->topInvMatrix()[0][0]);
	glUniformMatrix4fv(program->getUniform("lightPV"), 1, GL_FALSE, &LightPV[0][0]);
    
	M->multMatrix(glm::inverse(this->V));
	
	axis.draw(program, M);

	//draw the camera's wirecube
	M->pushMatrix();

	M->multMatrix(glm::inverse(this->P));

	debugWireCube->draw(program, P, V, M, LightPV);

	program->unbind();

	M->pushMatrix();	
	// TODO: draw the light view on the near plane of the frustum. You must set up the right transformation! 
	// That is, translate and scale the x and y directions of the -1 to 1 quad so that the quad fits exactly the l r t b portion of the near plane
	M->translate(0,0,-1);
	debugDepthMapQuad->draw(quadShader, P, V, M, LightPV);

	M->popMatrix();
	M->popMatrix();
	M->popMatrix();
	
	GLSL::checkError(GET_FILE_LINE);
}
