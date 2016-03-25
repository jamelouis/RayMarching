#include <gl/glew.h>
#include <FreeImagePlus.h>
#include <fstream>
#include <string>
#include <iostream>

std::string ReadShaderStrFromFile(char* const file)
{
    std::ifstream f(file);
    return std::string(std::istreambuf_iterator<char>(f),
        std::istreambuf_iterator<char>());
}

GLint CompileShaderFromStr(std::string const& strSource, GLenum enumShaderType)
{
    GLint enumShaderID = glCreateShaderObjectARB(enumShaderType);
    char* str= (char*)strSource.c_str();
    glShaderSource(enumShaderID, 1, &str, NULL);
    glCompileShader(enumShaderID);

    {
        GLint status;
        glGetShaderiv(enumShaderID, GL_COMPILE_STATUS, &status);
        if( GL_TRUE != status)
        {
            char buffer[512];
            glGetShaderInfoLog(enumShaderID, 512, nullptr, buffer);
            std::cout << buffer << std::endl;
            if( glIsShader( enumShaderID ) ) glDeleteShader( enumShaderID );
            enumShaderID = -1;
        }
    }
    return enumShaderID;
}

GLint CreateShaderFromStr(std::string const strVS, std::string const strFS )
{
    GLint enumVSID, enumFSID, enumProgramID;

    enumProgramID = -1;

    enumVSID = CompileShaderFromStr(strVS, GL_VERTEX_SHADER);
    if(enumVSID <= 0 ) goto Exit0;
    enumFSID = CompileShaderFromStr(strFS, GL_FRAGMENT_SHADER);
    if(enumFSID <= 0 ) goto Exit0;

    enumProgramID = glCreateProgram();
    glAttachObjectARB(enumProgramID, enumVSID);
    glAttachObjectARB(enumProgramID, enumFSID);
    glLinkProgram(enumProgramID);
    {
        GLint status;
        glGetProgramiv(enumProgramID, GL_LINK_STATUS, &status);
        if(GL_TRUE != status)
        {
            char buffer[512];
            glGetProgramInfoLog(enumProgramID, 512, nullptr, buffer);
            std::cout << buffer << std::endl;
            if( glIsProgram(enumProgramID)) glDeleteProgram(enumProgramID);
            enumProgramID = -1;
        }
    }
Exit0:
    return enumProgramID;
}

GLint CreateShaderFromFile(char* const vs_file, char* const fs_file )
{
    
    std::string strVSSource = ReadShaderStrFromFile(vs_file);
    std::string strFSSource = ReadShaderStrFromFile(fs_file);
    
    return CreateShaderFromStr(strVSSource, strFSSource);
}

GLuint GetQuadVAO()
{
    float vertices[] = {
        -1.0, -1.0,
        1.0, -1.0,
        1.0,  1.0,
        -1.0,  1.0
    };
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    GLuint indexs[] = {
        0, 1, 2, 
        0, 2, 3
    };

    GLuint vbo2;
    glGenBuffers(1, &vbo2);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo2);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexs), indexs, GL_STATIC_DRAW);


    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,2, GL_FLOAT, GL_FALSE,0,nullptr);

    return vao;
}


GLuint CreateTextureFromFile(std::string const& strFile)
{
    fipImage img;
    img.load(strFile.c_str());
    img.flipVertical();
    img.convertTo32Bits();    
    
    GLuint nTexId;
    glGenTextures(1, &nTexId);
    glBindTexture(GL_TEXTURE_2D, nTexId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.getWidth(), img.getHeight(), 0, GL_BGRA, GL_UNSIGNED_BYTE, img.accessPixels());

    glBindTexture(GL_TEXTURE_2D, 0);
    return nTexId;
}

std::pair<GLuint, GLuint> CreateFrameBuffer(int width, int height)
{
    GLuint nTexId;
    glGenTextures(1, &nTexId);
    glBindTexture(GL_TEXTURE_2D, nTexId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);

    GLuint fb;
    glGenFramebuffers(1, &fb);
    glBindFramebuffer(GL_FRAMEBUFFER, fb);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, nTexId, 0);

    {
        GLenum status;
        status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if(GL_FRAMEBUFFER_COMPLETE != status)
        {
            std::cout << "Failed to create Framebuffer!" << std::endl;
        }
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    return std::make_pair(fb, nTexId);
}