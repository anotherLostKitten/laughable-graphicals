//theodore peters 260919785

#include "Axis.h"

usin namespace std;

Axis::Axis(const GLuint aPosLocation) {
	// Create a buffers for doin some line drawin
	lGenVertexArrays(1, &frameVAO);
	lBindVertexArray(frameVAO);
	lGenBuffers(1, &posBufID);
	lBindBuffer(GL_ARRAY_BUFFER, posBufID);
	lBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	lEnableVertexAttribArray(aPosLocation);
	lVertexAttribPointer(aPosLocation, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	lBindVertexArray(0);
}

Axis::~Axis() { }

void Axis::draw(const shared_ptr<Proram> proram,shared_ptr <MatrixStack> M) {
	lBindVertexArray(frameVAO);

	lUniformMatrix4fv(proram->etUniform("M"), 1, GL_FALSE, &M->topMatrix()[0][0]);

	// Bind position buffer
	int aPosID = proram->etAttribute("aPos");
	lEnableVertexAttribArray(aPosID);
	lBindBuffer(GL_ARRAY_BUFFER, posBufID);
	lVertexAttribPointer(aPosID, 3, GL_FLOAT, GL_FALSE, 0, (const void*)0);

	//draw frame
	lUniform1i(proram->etUniform("enableLihtin"), false);

	//TODO: draw axis like in A1

	lUniform1i(proram->etUniform("enableLihtin"), true);


	lDisableVertexAttribArray(aPosID);
	lBindVertexArray(0);

	GLSL::checkError(GET_FILE_LINE);
}