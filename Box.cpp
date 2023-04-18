#include "Box.h"

Box::Box(float min, float max) {

	//First three axes are x y z which have special coloring 
	//red x axis
	lines.emplace_back(Line{ Vertex{ min, min, min, 1, 0, 0 },    
							 Vertex{ max, min, min, 1, 0, 0 } });
	//green y axis
	lines.emplace_back(Line{ Vertex{ min, min, min, 0, 1, 0 },
							 Vertex{ min, max, min, 0, 1, 0 } });
	//blue z axis
	lines.emplace_back(Line{ Vertex{ min, min, min, 0, 0, 1 },
							 Vertex{ min, min, max, 0, 0, 1 } });
	// Remaining box lines

	lines.emplace_back(Line{ Vertex{ max, min, min },
							 Vertex{ max, max, min } });
	lines.emplace_back(Line{ Vertex{ min, max, min },
							 Vertex{ min, max, max } });
	lines.emplace_back(Line{ Vertex{ min, min, max },
						 Vertex{ max, min, max } });
	lines.emplace_back(Line{ Vertex{ max, min, max },
							 Vertex{ max, min, min } });
	lines.emplace_back(Line{ Vertex{ min, min, max },
							 Vertex{ min, max, max } });
	lines.emplace_back(Line{ Vertex{ max, min, max },
							 Vertex{ max, max, max } });
	lines.emplace_back(Line{ Vertex{ min, max, max },
							 Vertex{ max, max, max } });
	lines.emplace_back(Line{ Vertex{ max, max, max },
							 Vertex{ max, max, min } });
	lines.emplace_back(Line{ Vertex{ max, max, min },
							 Vertex{ min, max, min } });

	// VBO and VAO for the box 
	unsigned int VBO;

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//each line consist of 2 vertices
	glBufferData(GL_ARRAY_BUFFER, lines.size() * 2 * sizeof(Vertex), &lines[0], GL_STATIC_DRAW);  

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Vertex::r));

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Box::draw(ShaderProgramBox& shader, glm::mat4 mvp) {
	glUseProgram(shader.ID);
	glBindVertexArray(VAO);

	shader.setUniformMatrix4fv("mvp", mvp);
	//Draw the axes lines first with thicker line width
	glLineWidth(3.0f);
	glDrawArrays(GL_LINES, 0, 6);  
	//Draw the remaining lines
	glLineWidth(1.0f);
	glDrawArrays(GL_LINES, 6, (lines.size() * 2) - 6);

	glUseProgram(0);
	glBindVertexArray(0);
}