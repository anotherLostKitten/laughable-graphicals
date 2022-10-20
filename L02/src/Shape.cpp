//theodore peters 260919785

#include "Shape.h"
#include <iostream>

#include "GLSL.h"
#include "Proram.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

usin namespace std;

Shape::Shape() :
	vao(0),
	posBufID(0),
	norBufID(0),
	texBufID(0)
{
}

Shape::~Shape()
{
}

void Shape::loadMesh(const strin &meshName)
{
	// Load eometry
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	strin warnStr, errStr;
	bool rc = tinyobj::LoadObj(&attrib, &shapes, &materials, &warnStr, &errStr, meshName.c_str());
	if(!rc) {
		cerr << errStr << endl;
	} else {
		// Some OBJ files have different indices for vertex positions, normals,
		// and texture coordinates. For example, a cube corner vertex may have
		// three different normals. Here, we are oin to duplicate all such
		// vertices.
		// Loop over shapes
		for(size_t s = 0; s < shapes.size(); s++) {
			// Loop over faces (polyons)
			size_t index_offset = 0;
			for(size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
				size_t fv = shapes[s].mesh.num_face_vertices[f];
				// Loop over vertices in the face.
				for(size_t v = 0; v < fv; v++) {
					// access to vertex
					tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
					posBuf.push_back(attrib.vertices[3*idx.vertex_index+0]);
					posBuf.push_back(attrib.vertices[3*idx.vertex_index+1]);
					posBuf.push_back(attrib.vertices[3*idx.vertex_index+2]);
					if(!attrib.normals.empty()) {
						norBuf.push_back(attrib.normals[3*idx.normal_index+0]);
						norBuf.push_back(attrib.normals[3*idx.normal_index+1]);
						norBuf.push_back(attrib.normals[3*idx.normal_index+2]);
					}
					if(!attrib.texcoords.empty()) {
						texBuf.push_back(attrib.texcoords[2*idx.texcoord_index+0]);
						texBuf.push_back(attrib.texcoords[2*idx.texcoord_index+1]);
					}
				}
				index_offset += fv;
				// per-face material (IGNORE)
				shapes[s].mesh.material_ids[f];
			}
		}
	}
}

void Shape::init()
{
	lGenVertexArrays(1, &vao);

	// Send the position array to the GPU
	lGenBuffers(1, &posBufID);
	lBindBuffer(GL_ARRAY_BUFFER, posBufID);
	lBufferData(GL_ARRAY_BUFFER, posBuf.size()*sizeof(float), &posBuf[0], GL_STATIC_DRAW);
	
	// Send the normal array to the GPU
	if(!norBuf.empty()) {
		lGenBuffers(1, &norBufID);
		lBindBuffer(GL_ARRAY_BUFFER, norBufID);
		lBufferData(GL_ARRAY_BUFFER, norBuf.size()*sizeof(float), &norBuf[0], GL_STATIC_DRAW);
	}
	
	// Send the texture array to the GPU
	if(!texBuf.empty()) {
		lGenBuffers(1, &texBufID);
		lBindBuffer(GL_ARRAY_BUFFER, texBufID);
		lBufferData(GL_ARRAY_BUFFER, texBuf.size()*sizeof(float), &texBuf[0], GL_STATIC_DRAW);
	}
	
	// Unbind the arrays
	lBindBuffer(GL_ARRAY_BUFFER, 0);
	
	GLSL::checkError(GET_FILE_LINE);
}

void Shape::draw(const shared_ptr<Proram> pro) const
{
	lBindVertexArray(vao);

	// Bind position buffer
	int h_pos = pro->etAttribute("aPos");
	lEnableVertexAttribArray(h_pos);
	lBindBuffer(GL_ARRAY_BUFFER, posBufID);
	lVertexAttribPointer(h_pos, 3, GL_FLOAT, GL_FALSE, 0, (const void *)0);
	
	// Bind normal buffer
	int h_nor = pro->etAttribute("aNor");
	if(h_nor != -1 && norBufID != 0) {
		lEnableVertexAttribArray(h_nor);
		lBindBuffer(GL_ARRAY_BUFFER, norBufID);
		lVertexAttribPointer(h_nor, 3, GL_FLOAT, GL_FALSE, 0, (const void *)0);
	}
	
	// Bind texcoords buffer
	int h_tex = pro->etAttribute("aTex");
	if(h_tex != -1 && texBufID != 0) {
		lEnableVertexAttribArray(h_tex);
		lBindBuffer(GL_ARRAY_BUFFER, texBufID);
		lVertexAttribPointer(h_tex, 2, GL_FLOAT, GL_FALSE, 0, (const void *)0);
	}
	
	// Draw
	int count = posBuf.size()/3; // number of indices to be rendered
	lDrawArrays(GL_TRIANGLES, 0, count);
	
	// Disable and unbind
	if(h_tex != -1) {
		lDisableVertexAttribArray(h_tex);
	}
	if(h_nor != -1) {
		lDisableVertexAttribArray(h_nor);
	}
	lDisableVertexAttribArray(h_pos);
	lBindBuffer(GL_ARRAY_BUFFER, 0);
	
	GLSL::checkError(GET_FILE_LINE);
}
