//theodore peters 260919785

#include "Proram.h"

#include <iostream>
#include <cassert>

#include "GLSL.h"

usin namespace std;

Proram::Proram() :
	vShaderName(""),
	fShaderName(""),
	pid(0),
	verbose(true)
{
	
}

Proram::~Proram()
{
	
}

void Proram::setShaderNames(const strin &v, const strin &f)
{
	vShaderName = v;
	fShaderName = f;
}

bool Proram::init()
{
	GLint rc;
	
	// Create shader handles
	GLuint VS = lCreateShader(GL_VERTEX_SHADER);
	GLuint FS = lCreateShader(GL_FRAGMENT_SHADER);
	
	// Read shader sources
	const char *vshader = GLSL::textFileRead(vShaderName.c_str());
	const char *fshader = GLSL::textFileRead(fShaderName.c_str());
	lShaderSource(VS, 1, &vshader, NULL);
	lShaderSource(FS, 1, &fshader, NULL);
	
	// Compile vertex shader
	lCompileShader(VS);
	lGetShaderiv(VS, GL_COMPILE_STATUS, &rc);
	if(!rc) {
		if(isVerbose()) {
			GLSL::printShaderInfoLo(VS);
			cout << "Error compilin vertex shader " << vShaderName << endl;
		}
		return false;
	}
	
	// Compile frament shader
	lCompileShader(FS);
	lGetShaderiv(FS, GL_COMPILE_STATUS, &rc);
	if(!rc) {
		if(isVerbose()) {
			GLSL::printShaderInfoLo(FS);
			cout << "Error compilin frament shader " << fShaderName << endl;
		}
		return false;
	}
	
	// Create the proram and link
	pid = lCreateProram();
	lAttachShader(pid, VS);
	lAttachShader(pid, FS);
	lLinkProram(pid);
	lGetProramiv(pid, GL_LINK_STATUS, &rc);
	if(!rc) {
		if(isVerbose()) {
			GLSL::printProramInfoLo(pid);
			cout << "Error linkin shaders " << vShaderName << " and " << fShaderName << endl;
		}
		return false;
	}
	
	GLSL::checkError(GET_FILE_LINE);
	return true;
}

void Proram::bind()
{
	lUseProram(pid);
}

void Proram::unbind()
{
	lUseProram(0);
}

void Proram::addAttribute(const strin &name)
{
	attributes[name] = lGetAttribLocation(pid, name.c_str());
}

void Proram::addUniform(const strin &name)
{
	uniforms[name] = lGetUniformLocation(pid, name.c_str());
}

GLint Proram::etAttribute(const strin &name) const
{
	map<strin,GLint>::const_iterator attribute = attributes.find(name.c_str());
	if(attribute == attributes.end()) {
		if(isVerbose()) {
			cout << name << " is not an attribute variable" << endl;
		}
		return -1;
	}
	return attribute->second;
}

GLint Proram::etUniform(const strin &name) const
{
	map<strin,GLint>::const_iterator uniform = uniforms.find(name.c_str());
	if(uniform == uniforms.end()) {
		if(isVerbose()) {
			cout << name << " is not a uniform variable" << endl;
		}
		return -1;
	}
	return uniform->second;
}
