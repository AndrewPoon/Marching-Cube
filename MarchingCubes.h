#ifndef MARCHINGCUBES_H
#define MARCHINGCUBES_H
#include <vector>
#include <functional>
#include <glm/mat4x4.hpp>
#include "ShaderProgramCube.h"
//Marching Cubes helper
namespace MarchingCubes {
	//get all of the marching cube vertices
	void marchingcubes(std::function<float(float, float, float)> f, float isovalue,
		float min, float max, float stepsize);
	//update the buffer by adding vertices into it
	void update();
	//render the vertices within the buffer
	void draw(ShaderProgramCube& shader, glm::mat4 mvp);

	struct Vertex {
		glm::vec3 position;
		glm::vec3 normal;
		Vertex(glm::vec3 position, glm::vec3 normal) :
			position(position), normal(normal) {}
	};


};

#endif