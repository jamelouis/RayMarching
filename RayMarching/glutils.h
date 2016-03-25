#include <gl/glew.h>
#include <string>

GLuint GetQuadVAO();

GLint CreateShaderFromFile(char* const vs_file, char* const fs_file );

GLuint CreateTextureFromFile(std::string const& strFile);

std::pair<GLuint, GLuint> CreateFrameBuffer(int width, int height);