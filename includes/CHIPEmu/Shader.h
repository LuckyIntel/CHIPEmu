/*
    Custom GLSL 330 Core shaders for the
    CHIPEmu renderer. Feel free to change
    the code here but since I haven't added
    anything to the renderer code to debug
    any errors, it's prone to start displaying
    a black display with any tweak.
*/

#ifndef SHADER_H
#define SHADER_H

/*
    GLSL 330 Vertex Shader
*/
const char* vertex_shader_code = 
"#version 330 core\n"
"layout (location = 0) in vec2 position;\n"
"layout (location = 1) in vec2 texUV;\n"
"out vec2 textureUV;\n"
"void main()\n"
"{\n"
"textureUV = texUV;\n"
"gl_Position = vec4(position, 1.0, 1.0);\n"
"}\0";

/*
    GLSL 330 Fragment Shader
*/
const char* fragment_shader_code = 
"#version 330 core\n"
"in vec2 textureUV;\n"
"uniform sampler2D screenTexture;\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"FragColor = texture(screenTexture, textureUV);\n"
"}\0";

#endif