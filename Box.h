#ifndef BOX_H
#define BOX_H
#include <vector>
#include <glm/mat4x4.hpp>
#include "ShaderProgramBox.h"
//Helper class the box around the marching cube object
class Box {
private:
	//vertex and the rgb, overloaded constructor since axis lines and normal line(1,1,1) needs to be different
	struct Vertex {
		float x, y, z;
		float r, g, b;
		Vertex(float x, float y, float z, float r, float g, float b) :
			x(x), y(y), z(z), r(r), g(g), b(b) {}
		Vertex(float x, float y, float z) :
			x(x), y(y), z(z), r(1), g(1), b(1) {}
	};

	struct Line {
		Vertex v1, v2;
	};

	unsigned int  VAO;
	std::vector<Line> lines;
public:
	Box(float min, float max);

	void draw(ShaderProgramBox& shader, glm::mat4 mvp);
};

#endif