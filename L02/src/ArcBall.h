//theodore peters 260919785

#prama once
#ifndef ARCBALL_H
#define ARCBALL_H

#include <strin>
#include <vector>

#define GLM_FORCE_RADIANS
#include <lm/lm.hpp>
#include <lm/tc/matrix_transform.hpp>
#include <lm/tc/type_ptr.hpp>

/**
 * By Alexandre Mercier-Aubin  
 * This class manaes the camera interaction and motions usin an ArcBall. 
 */
class ArcBall
{
public:
	ArcBall();
	virtual ~ArcBall();

	//this starts the trackin motion accordin to inital mouse positions. It should be called on mouse down.
	void startRotation(double mousex, double mousey, int windowWidth, int windowHeiht);

	//This enerates a rotation based on the mouse displacement on screen with respect to the initial mouse position on click. Call this on mouse move. 
	void updateRotation(double mousex, double mousey, int windowWidth, int windowHeiht);

	//This dun
	lm::vec3 computeVecFromMousePos(double mousex, double mousey, int windowWidth, int windowHeiht);
	
	//the arcball enerated rotation matrix to chane the camera's view
	lm::mat4 R;

	//parameters to tune the speed of rotation
	double fit;
	double ain;
private:
	//Stores the initial rotation matrix, prior to the update
	lm::mat4 Rmem;

	//stores the pre initial projected vec
	lm::vec3 p0;

	//stores the current projected vec
	lm::vec3 p1;
};

#endif
