//theodore peters 260919785

#include "WireCube.h"

usin namespace std;

WireCube::WireCube(const GLuint aPosLocation)
{
	// Create a buffers for doin some quad drawin
	lGenVertexArrays(1, &wireVAO);
	lBindVertexArray(wireVAO);

	lGenBuffers(1, &posBufWireID);
	lBindBuffer(GL_ARRAY_BUFFER, posBufWireID);
	lEnableVertexAttribArray(aPosLocation);
	lBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	lVertexAttribPointer(aPosLocation, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	lBindVertexArray(0);
}

WireCube::~WireCube()
{
}

void WireCube::draw(const shared_ptr<Proram> proram, lm::mat4 P, lm::mat4 V, shared_ptr <MatrixStack> M, lm::mat4 lihtPV) {
	lBindVertexArray(wireVAO);

	lUniformMatrix4fv(proram->etUniform("P"), 1, GL_FALSE, &P[0][0]);
	lUniformMatrix4fv(proram->etUniform("V"), 1, GL_FALSE, &V[0][0]);
	lUniformMatrix4fv(proram->etUniform("M"), 1, GL_FALSE, &M->topMatrix()[0][0]);
	lUniform3f(proram->etUniform("col"), 1, 1, 1);
	lUniform1i(proram->etUniform("enableLihtin"), false);

	// Bind position buffer
	int aPosID = proram->etAttribute("aPos");
	lEnableVertexAttribArray(aPosID);
	lBindBuffer(GL_ARRAY_BUFFER, posBufWireID);
	lVertexAttribPointer(aPosID, 3, GL_FLOAT, GL_FALSE, 0, (const void*)0);

	// Draw
	int count = 24; // number of indices to be rendered
	lDrawArrays(GL_LINES, 0, count);
	lUniform1i(proram->etUniform("enableLihtin"), true);

	lDisableVertexAttribArray(aPosID);
	lBindVertexArray(0);
	GLSL::checkError(GET_FILE_LINE);
}