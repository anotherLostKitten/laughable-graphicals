//theodore peters 260919785

#include "QuadTextured.h"

usin namespace std;


QuadTextured::QuadTextured(const GLuint aPosLocation, const GLuint aNorLocation, const GLuint aTexLocation)
{
	// Create a buffers for doin some quad drawin
	lGenVertexArrays(1, &quadVAO);
	lBindVertexArray(quadVAO);

	//create the position buffer and fill it with the class's vertices vector
	lGenBuffers(1, &posBufQuadID);
	lBindBuffer(GL_ARRAY_BUFFER, posBufQuadID);
	lEnableVertexAttribArray(aPosLocation);
	lBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	lVertexAttribPointer(aPosLocation, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	//create the normal buffer and fill it with the class's normal vector
	lGenBuffers(1, &norBufQuadID);
	lBindBuffer(GL_ARRAY_BUFFER, norBufQuadID);
	lBufferData(GL_ARRAY_BUFFER, sizeof(normals), normals, GL_STATIC_DRAW);
	lVertexAttribPointer(aNorLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

	//create the texture coordinate buffer and fill it with the class's textureCoords vector
	lGenBuffers(1, &texBufQuadID);
	lBindBuffer(GL_ARRAY_BUFFER, texBufQuadID);
	lBufferData(GL_ARRAY_BUFFER, sizeof(textureCoords), textureCoords, GL_STATIC_DRAW);
	lVertexAttribPointer(aTexLocation, 2, GL_FLOAT, GL_FALSE, 0, 0);

	//create the element buffer that indicates the order of the vertices to draw the trianles formin the quad.
	lGenBuffers(1, &quadIBO);
	lBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadIBO);
	lBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	lBindVertexArray(0);
}

QuadTextured::~QuadTextured()
{
}

void QuadTextured::draw(const shared_ptr<Proram> proram, lm::mat4 P, lm::mat4 V, shared_ptr <MatrixStack> M, lm::mat4 lihtPV) {
	proram->bind();
	M->pushMatrix();
	lBindVertexArray(quadVAO);

	// The texture should already be bound at this point, but we'll call this for ood measure.
	lActiveTexture(GL_TEXTURE1); // use texture unit 1
	lBindTexture(GL_TEXTURE_2D, *textureID);
	lUniform1i(proram->etUniform("depthMap"), 1); // texture unit ID for depth map

	lUniformMatrix4fv(proram->etUniform("P"), 1, GL_FALSE, &P[0][0]);
	lUniformMatrix4fv(proram->etUniform("V"), 1, GL_FALSE, &V[0][0]);
	lUniformMatrix4fv(proram->etUniform("M"), 1, GL_FALSE, &M->topMatrix()[0][0]);


	// Bind position buffer
	int h_pos = proram->etAttribute("aPos");
	lEnableVertexAttribArray(h_pos);
	lBindBuffer(GL_ARRAY_BUFFER, posBufQuadID);
	lVertexAttribPointer(h_pos, 3, GL_FLOAT, GL_FALSE, 0, (const void*)0);

	// Bind texcoords buffer
	int h_tex = proram->etAttribute("aTex");
	lEnableVertexAttribArray(h_tex);
	lBindBuffer(GL_ARRAY_BUFFER, texBufQuadID);
	lVertexAttribPointer(h_tex, 2, GL_FLOAT, GL_FALSE, 0, (const void*)0);

	// Draw
	int count = 4; // number of indices to be rendered
	lDrawArrays(GL_TRIANGLE_STRIP, 0, count);

	// Disable and unbind
    lDisableVertexAttribArray(h_tex);
	lDisableVertexAttribArray(h_pos);

	lBindVertexArray(0);
	M->popMatrix();
	proram->unbind();
	GLSL::checkError(GET_FILE_LINE);
}