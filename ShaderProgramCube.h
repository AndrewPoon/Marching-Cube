#ifndef SHADERPROGRAMCUBE_H
#define SHADERPROGRAMCUBE_H
#include <gl/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <map>
#include <string>

#include <iostream>

class ShaderProgramCube {
public:
    unsigned int ID;

    ShaderProgramCube() {
        ID = createShaderProgram();
    }

    void setUniform3fv(const std::string& name, glm::vec3 val) {
        glProgramUniform3fv(ID, getLocation(name), 1, glm::value_ptr(val));
    }

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
		layout(location = 1) in vec3 aNormal;\n\
		// Output data ; will be interpolated for each fragment.\n\
		out vec3 fragpos;\n\
        out vec3 norm_v;\n\
        out vec3 lightdir_v;\n\
		uniform mat4 mvp;\n\
		uniform mat4 view; \n\
        void main(){ \n\
			gl_Position = mvp * vec4(aPos, 1);\n\
            //calculate the fragment and normal in view space\n\
			fragpos = vec3(view * vec4(aPos, 0));\n\
			norm_v = vec3(inverse(transpose(view)) * vec4(aNormal, 0));\n\
		}\n";
        std::string FragmentShaderCode = "\
		#version 330 core\n\
        in vec3 fragpos;\n\
        in vec3 norm_v;\n\
		out vec4 fragCol; \n\
		uniform vec3 modelColor;\n\
        uniform vec3 lightDirection;\n\
		void main() {\n\
            //ambient lighting\n\
            vec3 ambient = vec3(0.2, 0.2, 0.2);\n\
            //diffuse lighting\n\
            vec3 lightdir = normalize(-lightDirection);\n\
            vec3 normal = normalize(norm_v);\n\
            float diffuse_coef = max(0, dot(normal, lightdir));\n\
            vec3 diffuse = modelColor * diffuse_coef;\n\
            //specular lighting\n\
            vec3 specularColor = vec3(1, 1, 1);\n\
            vec3 reflection = reflect(-lightdir, normal);\n\
            vec3 fragpost = normalize(-fragpos);\n\
            float specular_coef = pow(max(0, dot(reflection, fragpost)), 64);\n\
            vec3 specular = specular_coef * specularColor;\n\
            vec3 resultColor = ambient + diffuse + specular;\n\
            fragCol = vec4(resultColor, 1);\n\
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