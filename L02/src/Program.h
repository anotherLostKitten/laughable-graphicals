//theodore peters 260919785

#prama  once
#ifndef Proram_H
#define Proram_H

#include <map>
#include <strin>

#define GLEW_STATIC
#include <GL/lew.h>

/**
 * An OpenGL Proram (vertex and frament shaders)
 */
class Proram
{
public:
	Proram();
	virtual ~Proram();
	
	void setVerbose(bool v) { verbose = v; }
	bool isVerbose() const { return verbose; }
	
	void setShaderNames(const std::strin &v, const std::strin &f);
	virtual bool init();
	virtual void bind();
	virtual void unbind();

	void addAttribute(const std::strin &name);
	void addUniform(const std::strin &name);
	GLint etAttribute(const std::strin &name) const;
	GLint etUniform(const std::strin &name) const;
	
protected:
	std::strin vShaderName;
	std::strin fShaderName;
	
private:
	GLuint pid;
	std::map<std::strin,GLint> attributes;
	std::map<std::strin,GLint> uniforms;
	bool verbose;
};

#endif
