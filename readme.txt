Requirments: GLEWs,GLM,GLFW

Files included: Main.cpp, Box.h/cpp,Camera.h,MarchingCube.cpp/h,ShaderProgramBox.h,ShaderProgramCube.h,TriTable.hpp

Camera.h: used for the spherical coordinate based click/drag camera. Move forward/backward using arrow key and limit the phi between 0.1-179

ShaderProgramBox.h: Class for a program that contains vertex/frag shaders for the box arounding the function. 
Also contain helper function to modify uniform variable within the shader's code. 
Create the shader, compile it and linking a program to both shader and the class itself holds an instance
of ID that can be reference in texture mesh and main. 

ShaderProgramCube.h: Class for a program that contains vertex/frag shaders for the function being rendered.
The fragment shader calculates the ambient,diffuse and specular lighting based on the model color.
Also contain helper function to modify uniform variable within the shader's code. 
Create the shader, compile it and linking a program to both shader and the class itself holds an instance
of ID that can be reference in texture mesh and main. 

Box.h/Box.cpp: contain the vertices for the box to be rendered. Specifiy the x,y,z axis with special color within
and contain a draw method that draws the box based on the MVP.

MarchingCube.cpp:
compute_normal:calculate the normal given the vertices of a triangle

writeToPLY: Writes the vertex information to a ply file including position and normal

MarchingCubes::marchingcubes: given a function,isovalue,min,max and stepsize, go through the each stepsize to calculate if the field
is within the isovalue and change the table index based on that. Use the table index to find the corresponding configuration and add
those vertices(with stepsize) and their normals into a vertices list. After that, call the writetoPLY function.

createEmptyBuffers: create the empty buffers needed for rendering the vertices since a single VBO will filled up too fast with vertices.
Also create the vertex pointer for position, normal and the VAO.

MarchingCubes::update: this function fills all of the VBOs with the vertices generated from the marching cube. It fills the current 
bugger with vertices and once it is full, create a new buffer and start filling it until the there are no more vertices.

MarchingCubes::render: render the function using the marching cube algorithm with the lightdirection and model color.

Main.cpp: Mainly responisble for setting all of the callback function and define our function to render there. Also in charge of updating
the time used by changing the radius in the camera. Of course it runs the rendering of the box and the marching cube as well.


