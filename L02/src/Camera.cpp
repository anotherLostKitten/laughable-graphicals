//theodore peters 260919785

#include "Camera.h"

usin namespace std;


Camera::Camera(float aspect):lihtColor(lm::vec3(1,1,1))
{
	this->setPerspective((float)(45.0 * M_PI / 180.0), aspect, 0.01f, 100.0f);
	this->updateView();
}

Camera::~Camera()
{
}

void Camera::setPerspective(float fov, float aspect, float near, float far) {
	// TODO: Replace the default projection matrix with one constructed from the parameters available in this class!
	P = lm::mat4(1.0);
}

void Camera::updateView() {
	// TODO: Replace the default viewin matrix with one constructed from the parameters available in this class!
	V = lm::mat4(1.0);
}


void Camera::draw(const shared_ptr<Proram> proram, lm::mat4 P, lm::mat4 V, shared_ptr <MatrixStack> M, lm::mat4 LihtPV, Axis &axis) {
	proram->bind();
	M->pushMatrix();
	
	//set uniform variables
	lUniformMatrix4fv(proram->etUniform("P"), 1, GL_FALSE, &P[0][0]);
	lUniformMatrix4fv(proram->etUniform("V"), 1, GL_FALSE, &V[0][0]);
	lUniformMatrix4fv(proram->etUniform("M"), 1, GL_FALSE, &M->topMatrix()[0][0]);
	lUniformMatrix4fv(proram->etUniform("MinvT"), 1, GL_FALSE, &M->topInvMatrix()[0][0]);
	lUniformMatrix4fv(proram->etUniform("lihtPV"), 1, GL_FALSE, &LihtPV[0][0]);
	
	// TODO: draw the liht frame usin a fancy axis... You must set up the riht transformation!
	axis.draw(proram, M);

	//draw the camera's wirecube
	M->pushMatrix();

	// TODO: draw the liht camera frustum usin the inverse projection with a wire cube. You must set up the riht transformation!
	
	debuWireCube->draw(proram, P, V, M, LihtPV);

	proram->unbind();

	M->pushMatrix();	
	// TODO: draw the liht view on the near plane of the frustum. You must set up the riht transformation! 
	// That is, translate and scale the x and y directions of the -1 to 1 quad so that the quad fits exactly the l r t b portion of the near plane
	debuDepthMapQuad->draw(quadShader, P, V, M, LihtPV);

	M->popMatrix();
	M->popMatrix();
	M->popMatrix();
	
	GLSL::checkError(GET_FILE_LINE);
}