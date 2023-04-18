#ifndef SHADERPROGRAMBOX_H
#define SHADERPROGRAMBOX_H
#include <gl/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <map>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
//ShaderProgram for the Box
class ShaderProgramBox {
public:
    unsigned int ID;

    ShaderProgramBox() {
        ID = createShaderProgram();
    }
    //mainly to change the mvp
    void setUniformMatrix4fv(const std::string& name, glm::mat4 val) {
        glUniformMatrix4fv(getLocation(name), 1, GL_FALSE, glm::value_ptr(val));
    }
    unsigned int getLocation(const std::string& name) {
        return glGetUniformLocation(ID, name.c_str());
    }
    unsigned int createShaderProgram()
    {
        std::string VertexShaderCode = "\
    	#version 330 core\n\
		// Input vertex data, different for all executions of this shader.\n\
		layout(location = 0) in vec3 aPos;\n\
		layout(location = 1) in vec3 aCol;\n\
		// Output data ; will be interpolated for each fragment.\n\
		out vec3 color;\n\
		// Values that stay constant for the whole mesh.\n\
		uniform mat4 mvp;\n\
        void main(){ \n\
        color = aCol;\n\
        gl_Position = mvp * vec4(aPos, 1);\n\
		}\n";
        std::string FragmentShaderCode = "\
		#version 330 core\n\
        out vec4 fragCol;\n\
		in vec3 color; \n\
		void main() {\n\
			fragCol = vec4(color, 1);\n\
		}\n";

        const char* c_vscode = VertexShaderCode.c_str();
        const char* c_fscode = FragmentShaderCode.c_str();
        //generate the shaders and attach the code to it
        unsigned int VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
        unsigned int FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(VertexShaderID, 1, &c_vscode, NULL);
        glShaderSource(FragmentShaderID, 1, &c_fscode, NULL);
        //Compile both
        glCompileShader(VertexShaderID);
        glCompileShader(FragmentShaderID);
        //generate a program and attach both shader to it
        unsigned int program = glCreateProgram();
        glAttachShader(program, VertexShaderID);
        glAttachShader(program, FragmentShaderID);
        glLinkProgram(program);
        //delete the shader once finished
        glDeleteShader(VertexShaderID);
        glDeleteShader(FragmentShaderID);
        return program;
    }
};

#endif