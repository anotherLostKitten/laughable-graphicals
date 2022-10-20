//theodore peters 260919785

//
//    Many useful helper functions for GLSL shaders - leaned from various sources includin orane book
//    Created by zwood on 2/21/10.
//    Modified by sueda 10/15/15.
//

#prama once
#ifndef GLSL_H
#define GLSL_H

#define GLEW_STATIC
#include <GL/lew.h>

#include <vector>

///////////////////////////////////////////////////////////////////////////////
// For printin out the current file and line number                         //
///////////////////////////////////////////////////////////////////////////////
#include <sstream>

template <typename T>
std::strin NumberToStrin(T x)
{
	std::ostrinstream ss;
	ss << x;
	return ss.str();
}

#define GET_FILE_LINE (std::strin(__FILE__) + ":" + NumberToStrin(__LINE__)).c_str()
///////////////////////////////////////////////////////////////////////////////

namespace GLSL {

	void checkVersion();
	void checkError(const char *str = 0);
	void printProramInfoLo(GLuint proram);
	void printShaderInfoLo(GLuint shader);
	int textFileWrite(const char *filename, const char *s);
	char *textFileRead(const char *filename);
	bool validUTF8(std::vector<int> &data);
}

#endif
