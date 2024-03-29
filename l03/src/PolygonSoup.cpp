//theodore peters 260919785

#include "PolygonSoup.h"

PolygonSoup::PolygonSoup(std::string filename)
{
  try
    {
	  std::ifstream ifs(filename);
	  if (!ifs) {
		cout << "Cannot open file [" << filename << "]" << std::endl;
	  }

	  std::string linebuf;
	  while (ifs.peek() != -1) {
		safeGetline(ifs, linebuf);
		if (linebuf[0] == 'v' && linebuf[1] == ' ') {
		  parseVertex(linebuf);
		}
		else if (linebuf[0] == 'f' && linebuf[1] == ' ') {
		  parseFace(linebuf);
		}
	  }

	  soupStatistics = filename + "\n" + "faces = " + std::to_string(faceList.size()) + "\nverts = " + std::to_string(vertexList->size()) + "\n";
	  for (auto const &e : faceSidesHistogram) {
		soupStatistics += to_string(e.second) + " ";
		if (e.first == 3) {
		  soupStatistics += "triangles\n";
		}
		else if (e.first == 4) {
		  soupStatistics += "quadrilaterals\n";
		}
		else {
		  soupStatistics += e.first + "-gons\n";
		}
	  }

	  std::cout << soupStatistics;

	  glm::vec3 max=(*vertexList)[0]->p;
	  glm::vec3 min=(*vertexList)[0]->p;
	  for(auto&vert:*vertexList){
		if(vert->p.x>max.x)
		  max.x=vert->p.x;
		if(vert->p.y>max.y)
		  max.y=vert->p.y;
		if(vert->p.z>max.z)
		  max.z=vert->p.z;
		if(vert->p.x < min.x)
		  min.x = vert->p.x;
		if(vert->p.y < min.y)
		  min.y = vert->p.y;
		if(vert->p.z < min.z)
		  min.z = vert->p.z;
	  }
	  //std::cout << "min point: (" << min.x << ", " << min.y << ", " << min.z << ") max point: (" << max.x << ", " << max.y << ", " << max.z << ")\n";
	  glm::vec3 offset=(max+min)*-0.5f;
	  float scaleby=20.f/(max.x-min.x>max.y-min.y?(max.x-min.x>max.z-min.z?max.x-min.x:max.z-min.z):(max.y-min.y>max.z-min.z?max.y-min.y:max.z-min.z));
	  //std::cout << "offset by: (" << offset.x << ", " << offset.y << ", " << offset.z << ") scaleby: " << scaleby << "\n";
	  for (auto &vert : *vertexList)
		vert->p=(vert->p+offset)*scaleby;

	  // reserve buffer data
	  this->norBuf.reserve(faceList.size() * 3);
	  this->posBuf.reserve(faceList.size() * 3 * 3);

	  // assume triangular faces when drawing
	  glm::vec3 v1;
	  glm::vec3 v2;
	  glm::vec3 n;

	  int indexCounter = 0;
	  for (auto &faceVertex : faceList) {
		indexCounter++;
		glm::vec3 p0 = vertexList->at(faceVertex[0])->p;
		glm::vec3 p1 = vertexList->at(faceVertex[1])->p;
		glm::vec3 p2 = vertexList->at(faceVertex[2])->p;
		v1 = p1 - p0;
		v2 = p2 - p1;
		n = glm::cross(v1, v2);
		n = glm::normalize(n);
		if (isnan(n.x) || isnan(n.y) || isnan(n.z)) {
		  n.x = 0;
		  n.y = 0;
		  n.z = 0;
		}
		this->norBuf.push_back(n.x);
		this->norBuf.push_back(n.y);
		this->norBuf.push_back(n.z);
		for (int i = 0; i < faceVertex.size(); i++) {
		  glm::vec3 p = vertexList->at(faceVertex[i])->p;
		  this->posBuf.push_back(p.x);
		  this->posBuf.push_back(p.y);
		  this->posBuf.push_back(p.z);
		}
	  }

	  // bind buffers

	  glGenVertexArrays(1, &vao);

	  // Send the position array to the GPU
	  glGenBuffers(1, &posBufID);
	  glBindBuffer(GL_ARRAY_BUFFER, posBufID);
	  glBufferData(GL_ARRAY_BUFFER, posBuf.size() * sizeof(float), &posBuf[0], GL_STATIC_DRAW);

	  // Send the normal array to the GPU
	  if (!norBuf.empty()) {
		glGenBuffers(1, &norBufID);
		glBindBuffer(GL_ARRAY_BUFFER, norBufID);
		glBufferData(GL_ARRAY_BUFFER, norBuf.size() * sizeof(float), &norBuf[0], GL_STATIC_DRAW);
	  }

	  // Unbind the arrays
	  glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
  catch (std::string e)
    {
	  std::cout << e << " failed to read.\n Please check the path." << std::endl;
	  exit(1);
    }
}

Vertex *PolygonSoup::parseVertex(std::string newline)
{
  vector<string> tokens;
  stringstream check1(newline);
  string intermediate;

  // Tokenizing w.r.t. space ' '
  while (getline(check1, intermediate, ' ')) {
	if (intermediate[0] != '\0') {
	  tokens.push_back(intermediate);
	}
  }

  Vertex *v = new Vertex();
  // ignore the tag "v " at index 0
  v->p.x = stof(tokens[1]);
  v->p.y = stof(tokens[2]);
  v->p.z = stof(tokens[3]);
  v->index = (int)vertexList->size();
  vertexList->emplace_back(v);
  return v;
}

std::vector<int> PolygonSoup::parseFace(std::string newline)
{
  // vertex/texture/normal tuples are separated by a spaces.

  vector<string> tokens;
  stringstream check1(newline);
  string intermediate;

  // Tokenizing w.r.t. space ' '
  while (getline(check1, intermediate, ' ')) {
	if (intermediate[0] != '\0') {
	  tokens.push_back(intermediate);
	}
  }

  int size = tokens.size();
  vector<int> v;
  for (int i = 1; i < size; i++) // ignore the tag "f " at i=0
    {
	  // first token is vertex index... we'll ignore the rest
	  v.push_back(stoi(tokens[i]) - 1); // want zero indexed vertices!
    }

  int count = size - 1;

  int finding = (int)faceSidesHistogram.count(count);
  if (finding == 0)
    {
	  faceSidesHistogram.emplace(count, 1);
    }
  else
    {
	  auto existing = faceSidesHistogram.find(count);
	  existing->second++;
    }
  faceList.push_back(v);
  return v;
}

void PolygonSoup::display(shared_ptr<Program> prog, shared_ptr<MatrixStack> P, shared_ptr<MatrixStack> MV)
{
  prog->bind();

  glBindVertexArray(vao);

  glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, &P->topMatrix()[0][0]);
  glUniformMatrix4fv(prog->getUniform("MV"), 1, GL_FALSE, &MV->topMatrix()[0][0]);

  // Bind position buffer
  int h_pos = prog->getAttribute("aPos");
  glEnableVertexAttribArray(h_pos);
  glBindBuffer(GL_ARRAY_BUFFER, posBufID);
  glVertexAttribPointer(h_pos, 3, GL_FLOAT, GL_FALSE, 0, (const void *)0);

  glUniform3f(prog->getUniform("col"), 0.1f, 0.1f, 0.1f);

  // Draw
  int count = posBuf.size() / 3; // number of indices to be rendered
  glDrawArrays(GL_TRIANGLES, 0, count);

  // Disable and unbind
  glDisableVertexAttribArray(h_pos);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  prog->unbind();
}
