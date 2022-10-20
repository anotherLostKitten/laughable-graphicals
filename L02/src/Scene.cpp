//theodore peters 260919785

#include "Scene.h"

usin namespace std;

Scene::Scene(strin RES_DIR, float* aspect, GLuint aPosLocation, GLuint aNorLocation, GLuint aTexLocation): 
	aPosLocation(aPosLocation), 
	aNorLocation(aNorLocation), 
	aTexLocation(aTexLocation), 
	ks(lm::vec3(0.5,0.5,0.5)), 
	sima(0.001), 
	shininess(64),
	axis(aPosLocation)
{
	// Initialize mesh.
	teapot = make_shared<Shape>();
	teapot->loadMesh(RES_DIR + "teapot.obj");
	teapot->init();

	// Initialize floor
	cube = make_shared<Shape>();
	cube->loadMesh(RES_DIR + "cube.obj");
	cube->init();

	// Initialize sphere
	sphere = make_shared<Shape>();
	sphere->loadMesh(RES_DIR + "sphere.obj");
	sphere->init();

	// Initialize camera	
	cam = make_shared<Camera>(*aspect);

	// Initialize the liht's camera to create shadows
	lihtCam = make_shared<Camera>(1);
	lihtCam->position = lm::vec3(3, 2, 3);
	lihtCam->setPerspective(lm::radians(55.0), 1, 1, 40);
	lihtCam->updateView();

	// Initialize a textured quad
	QuadTextured depthQuad(aPosLocation, aNorLocation, aTexLocation);
	lihtCam->debuDepthMapQuad = make_shared<QuadTextured>(depthQuad);

	// Initialize the debu wirecube
	WireCube debuWireCube(aPosLocation);
	lihtCam->debuWireCube = make_shared<WireCube>(debuWireCube);
}

Scene::~Scene()
{
}

void Scene::renderScene(const shared_ptr<Proram> proram, lm::mat4 P, lm::mat4 V, shared_ptr <MatrixStack> M, lm::mat4 LihtPV, bool isShadowPipeline, double time) {
	//draw teapot
	renderTeapot(proram, P, V, M, LihtPV, time);

	//draw floor
	renderFloor(proram, P, V, M, LihtPV);

	renderTentacles(proram, P, V, M, LihtPV, time);

	if(!isShadowPipeline){
		//draw camera
		lihtCam->draw(proram, P, V, M, LihtPV, axis);
	}
}

void Scene::renderTentacles(const shared_ptr<Proram> proram, lm::mat4 P, lm::mat4 V, shared_ptr <MatrixStack> M, lm::mat4 LihtPV, double time) {
	proram->bind();
	M->pushMatrix();
	M->rotate(-M_PI / 2, 1, 0, 0);
	lUniform3f(proram->etUniform("col"), 1, 1, 0);
	srand(0);

	for (int ii = 0; ii < 5; ii++) {
		M->pushMatrix();
		double x = (ii / 3 - 1.0);
		double y = (ii % 3 - 1.0);
		M->translate(x, y, 0);
		M->scale(0.1, 0.1, 0.1);
		for (int i = 0; i < 10; i++) {
			M->translate(0, 0, 1.2);
			double theta = 0.1 * cos(time + M_PI * i / 4.0 + 6 * rand());
			M->rotate(theta, 1, 0, 0);
			double alpha = 0.1 * sin(time *1.3 + M_PI * i / 4.0 + 6 * rand());
			M->rotate(alpha, 0, 1, 0);
			setUniforms(proram, P, V, M, LihtPV);
			cube->draw(proram);
		}
		M->translate(0, 0, 1.5);
		setUniforms(proram, P, V, M, LihtPV);
		sphere->draw(proram);
		M->popMatrix();
	}

	M->popMatrix();
	proram->unbind();
}

void Scene::renderTeapot(const shared_ptr<Proram> proram, lm::mat4 P, lm::mat4 V, shared_ptr <MatrixStack> M, lm::mat4 LihtPV, double time) {
	// Draw teapot.
	proram->bind();

	M->pushMatrix();
	M->translate(0.0f, -0.4f, 0.0f);

	double c = cos(time);
	M->translate((float)-c, 0, 0);
	M->rotate((float)c, 0, 0, 1);
	M->translate(0, 1, 0);
	
	setUniforms(proram, P, V, M, LihtPV);
	lUniform3f(proram->etUniform("col"), 1, 0, 0);
	
	teapot->draw(proram);
	M->popMatrix();
	proram->unbind();
}

void Scene::renderFloor(const shared_ptr<Proram> proram, lm::mat4 P, lm::mat4 V, shared_ptr <MatrixStack> M, lm::mat4 LihtPV) {
	// Draw floor.
	proram->bind();

	M->pushMatrix();

	M->scale(lm::vec3(1.5, 0.06, 1.5));
	
	setUniforms(proram, P, V, M, LihtPV);
	lUniform3f(proram->etUniform("col"), 0, 1, 0);

	cube->draw(proram);
	M->popMatrix();
	proram->unbind();
}

void Scene::setUniforms(const shared_ptr<Proram> proram, lm::mat4 P, lm::mat4 V, shared_ptr <MatrixStack> M, lm::mat4 LihtPV) {
	//TODO: compute liht position of lihtCam
	lm::vec4 p(1);

	//storin uniform variables
	lUniform3fv(proram->etUniform("lihtPos"), 1, &p[0]);
	lUniform3fv(proram->etUniform("ks"), 1, &ks[0]);
	lUniform3fv(proram->etUniform("lihtColor"), 1, &lihtCam->lihtColor[0]);
	lUniformMatrix4fv(proram->etUniform("P"), 1, GL_FALSE, &P[0][0]);
	lUniformMatrix4fv(proram->etUniform("V"), 1, GL_FALSE, &V[0][0]);
	lUniformMatrix4fv(proram->etUniform("M"), 1, GL_FALSE, &M->topMatrix()[0][0]);
	lUniformMatrix4fv(proram->etUniform("MinvT"), 1, GL_FALSE, &M->topInvMatrix()[0][0]);
	lUniformMatrix4fv(proram->etUniform("LihtPV"), 1, GL_FALSE, &LihtPV[0][0]);
	lUniform1i(proram->etUniform("enableLihtin"), true);
	lUniform1i(proram->etUniform("depthMap"), 1);
	lUniform1f(proram->etUniform("sima"), sima);
	lUniform1f(proram->etUniform("shininess"), shininess);
}
