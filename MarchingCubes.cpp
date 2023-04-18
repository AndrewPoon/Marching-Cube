#include "MarchingCubes.h"
#include "TriTable.hpp"
#include <iostream>
#include <fstream>
#include <glm/gtx/string_cast.hpp>
#include <thread>
#include <mutex>
#include <string>

typedef MarchingCubes::Vertex Vertex;

struct BufferIdentifiers {
	unsigned int VAO, VBO;
	int vert_count = 0;
};
// Computes the normal given 3 vertices with CCW winding order
glm::vec3 compute_normal(const Vertex& v1, const Vertex& v2,
	const MarchingCubes::Vertex& v3) {
	//get two vector and cross it. then normalize it
	return glm::normalize(glm::cross(v3.position - v1.position, v2.position - v1.position));
}
	
// Writes the vertex information to a ply file
void writeToPLY(std::vector<Vertex>& vertices, std::string filename) {

	std::ofstream plyFile(filename + ".ply");
	if (!plyFile.is_open()) {
		std::cerr << "Error: Failed to open file " << filename << std::endl;
		return;
	}

	// Write the PLY file header
	plyFile << "ply" << std::endl;
	plyFile << "format ascii 1.0" << std::endl;
	plyFile << "element vertex " << vertices.size() << std::endl;
	plyFile << "property float x" << std::endl;
	plyFile << "property float y" << std::endl;
	plyFile << "property float z" << std::endl;
	plyFile << "property float nx" << std::endl;
	plyFile << "property float ny" << std::endl;
	plyFile << "property float nz" << std::endl;
	plyFile << "element face " << std::to_string(vertices.size() / 3) << std::endl;
	plyFile << "property list uchar int vertex_index" << std::endl;
	plyFile << "end_header" << std::endl;
	// Write the vertex and normal data
	for (size_t i = 0; i < vertices.size(); i++) {
		plyFile << std::to_string(vertices[i].position.x) <<
			" " << std::to_string(vertices[i].position.y) <<
			" " << std::to_string(vertices[i].position.z) <<
			" " << std::to_string(vertices[i].normal.x) <<
			" " << std::to_string(vertices[i].normal.y) <<
			" " << std::to_string(vertices[i].normal.z) << std::endl;
	}
	// Write the face data
	for (size_t i = 0; i < vertices.size(); i += 3) {
		plyFile << "3 " << std::to_string(i) << " " << std::to_string(i + 1) << " " << std::to_string(i + 2) << std::endl;
	}

	plyFile.close();
}
std::vector<Vertex> vertices;
// generate all of the vertices using marching cube to model the function
void MarchingCubes::marchingcubes(std::function<float(float, float, float)> f, float isovalue,
	float min,
	float max,
	float stepsize) {
	// 1st letter is either bottom(b) or top(t), same for 2nd letter and 3rd is left right
	float bbl, bbr, btr, btl, tbl, tbr, ttr, ttl;
	int tableindex = 0;
	for (float z = min; z < max; z += stepsize)
		for (float x = min; x < max; x += stepsize)
			for (float y = min; y < max; y += stepsize) {
				//run the vertices of cube through the function.
				bbl = f(x, y, z);
				bbr = f(x + stepsize, y, z);
				btr = f(x + stepsize, y, z + stepsize);
				btl = f(x, y, z + stepsize);
				tbl = f(x, y + stepsize, z);
				tbr = f(x + stepsize, y + stepsize, z);
				ttr = f(x + stepsize, y + stepsize, z + stepsize);
				ttl = f(x, y + stepsize, z + stepsize);

				tableindex = 0;
				//this checks if a certain corner is wihtin the isovalue
				//which update the table index
				if (bbl < isovalue)
					tableindex |= 1;
				if (bbr < isovalue)
					tableindex |= 2;
				if (btr < isovalue)
					tableindex |= 4;
				if (btl < isovalue)
					tableindex |= 8;
				if (tbl < isovalue)
					tableindex |= 16;
				if (tbr < isovalue)
					tableindex |= 32;
				if (ttr < isovalue)
					tableindex |= 64;
				if (ttl < isovalue)
					tableindex |= 128;

				// use table index to get the correct config
				int* lut_indices = marching_cubes_lut[tableindex];
				glm::vec3 temp_normal = { 0,0,0 };
				// incerment by three since vertices are added three at a time
				for (int i = 0; i < 16; i += 3) {
					//
					if (lut_indices[i] < 0)
						break; 

					//each vertex consist of x(0), y(1), z(2) with each one adjusted by stepsize
					Vertex vert1({ x + stepsize * vertTable[lut_indices[i]][0],
								   y + stepsize * vertTable[lut_indices[i]][1],
								   z + stepsize * vertTable[lut_indices[i]][2] },
						temp_normal);
					Vertex vert2({ x + stepsize * vertTable[lut_indices[i + 1]][0],
								   y + stepsize * vertTable[lut_indices[i + 1]][1],
								   z + stepsize * vertTable[lut_indices[i + 1]][2] },
						temp_normal);
					Vertex vert3({ x + stepsize * vertTable[lut_indices[i + 2]][0],
								  y + stepsize * vertTable[lut_indices[i + 2]][1],
								  z + stepsize * vertTable[lut_indices[i + 2]][2] },
						temp_normal);

					//calculate the normal using compute normal
					glm::vec3 norm = compute_normal(vert1, vert2, vert3);
					vert1.normal = norm;
					vert2.normal = norm;
					vert3.normal = norm;

					// add vertices to the list
					vertices.emplace_back(vert1);
					vertices.emplace_back(vert2);
					vertices.emplace_back(vert3);
				}
			}
	std::cout << "Writing vertices to output file" << std::endl;
	writeToPLY(vertices, "output");
	std::cout << "Finished writing to file." << std::endl;
}
const int VERTS_PER_BATCH = 30000;
const size_t BYTES_PER_BATCH = VERTS_PER_BATCH * sizeof(Vertex);
size_t buffer_bytes_occupied = 0;
int lastIndex = 0;
std::vector<BufferIdentifiers> buffers;


// Create an empty buffer and VAO of a specified size and return those IDS
BufferIdentifiers createEmptyBuffers(int bufferSize) {
	unsigned int VAO, VBO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, bufferSize, nullptr, GL_STREAM_DRAW);
	//Position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	glEnableVertexAttribArray(0);
	//Normal
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Vertex::normal));
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	buffer_bytes_occupied = 0; // New empty buffer

	return BufferIdentifiers{ VAO, VBO };
}
void MarchingCubes::update() {
	//do nothing if there are no vertices
	if (lastIndex == vertices.size())
		return;

	// create a new buffer if we don't have one
	if (buffers.size() == 0)
		buffers.emplace_back(createEmptyBuffers(VERTS_PER_BATCH * sizeof(Vertex)));

	// part of the vertex that a buffer could hold
	std::vector<Vertex> vertex_partition;

	// Fill the buffer with vertices until fully occupied which then make a new buffer for data.
	size_t partition_bytes = 0;
	for (int i = lastIndex; i < vertices.size(); i++) {
		vertex_partition.emplace_back(vertices[i]);
		partition_bytes = vertex_partition.size() * sizeof(Vertex);

		// Buffer is full, make a new one and fill it
		if (buffer_bytes_occupied + partition_bytes >= BYTES_PER_BATCH) {
			glBindVertexArray(buffers.back().VAO);
			glBindBuffer(GL_ARRAY_BUFFER, buffers.back().VBO);
			glBufferSubData(GL_ARRAY_BUFFER, buffer_bytes_occupied,
				partition_bytes, &vertex_partition[0]);

			buffers.back().vert_count += vertex_partition.size();
			buffers.emplace_back(createEmptyBuffers(BYTES_PER_BATCH));

			vertex_partition.clear();
		}
	}

	//Now,add the remainder from the vertex partition into the buffer
	if (vertex_partition.size() > 0) {
		glBindVertexArray(buffers.back().VAO);
		glBindBuffer(GL_ARRAY_BUFFER, buffers.back().VBO);
		glBufferSubData(GL_ARRAY_BUFFER, buffer_bytes_occupied,
			partition_bytes, &vertex_partition[0]);

		buffers.back().vert_count += vertex_partition.size();
		buffer_bytes_occupied += partition_bytes;
	}

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	lastIndex = vertices.size(); 
}
glm::vec3 base_color = glm::vec3(0, 1, 1);
void MarchingCubes::draw(ShaderProgramCube& shader, glm::mat4 mvp) {

	glUseProgram(shader.ID);
	shader.setUniform3fv("lightDirection", glm::vec3(1,1,1));
	shader.setUniformMatrix4fv("mvp", mvp);
	shader.setUniform3fv("modelColor", base_color);

	// Draw each buffer that we are able
	for (int i = 0; i < buffers.size(); i++) {
		glBindVertexArray(buffers[i].VAO);
		glDrawArrays(GL_TRIANGLES, 0, buffers[i].vert_count);
		glBindVertexArray(0);
	}
}