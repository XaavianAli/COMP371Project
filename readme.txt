Super Hyper Cube game clone created by Team 4 for COMP 371 (Summer 2021) at Concordia University

Compilation/Running the game:

This game requires OpenGL, GLEW, GLFW libraries.

Windows: This folder contains a VS project which should allow you to run the program without any modifications.

Linux: Compile the source code from the Source directory using: g++ stb_image.h shader.h irrKlang/irrKlang.h A2_team4.cpp -L../Bin -lIrrKlang -Wl,-R../Bin $(pkg-config glfw3 glew --cflags --libs) -o SuperHyperCube.out

Controls:

Camera angle can be controlled by the mouse.

W/S rotate the object about the X axis.
A/D rotate the object about the Y axis.
Q/E rotate the object about the Z axis.

References:

The fragment shader, vertex shader and shader.h were based off of the tutorial provided at https://learnopengl.com/Getting-started/Shaders.
The lighting and shadows were based off of the tutorials provided at https://learnopengl.com/Lighting/Basic-Lighting and https://learnopengl.com/Advanced-Lighting/Shadows/Shadow-Mapping

IrrKlang sound library version 1.6.0 used for sound effects in game

The cube vao was based off the code provided for the labs of this class. 
The method for loading textures was also provided from the labs.

Team 5:
Donovan, Julien, Ryan, Xaavian, David