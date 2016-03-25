#include "glutils.h"
#include <gL/glew.h>
#include <GL/freeglut.h>
#include <FreeImagePlus.h>
#include <cassert>
#include <vector>
#include <string>

GLdouble width = 640, height = 480;   /* window width and height */
int wd;                   /* GLUT window handle */

GLint PS0_ID;
GLint PS_ID;

GLuint QuadVAO;
GLuint iChannel0;
GLuint iChannel1;

GLuint RayMarchingParamID;

bool bReload = true;
bool bLeftButtonDown = false;
char *src = "texture/4.png";

std::string FSShaderPath;

struct vec3{
    float x;
    float y;
    float z;
};

struct vec4{
    float x;
    float y;
    float z;
    float w;
};

struct RayMarchingParam{
    vec3 iResolution;
    float iGlobalTime;
    vec4 iMouse;

//     struct RayMarchingParam( vec3 resolution, float globalTime, vec4 mouse )
//         : iResolution( resolution )
//         , iGlobalTime( globalTime )
//         , iMouse( mouse ) {}
};

RayMarchingParam iRayMarchingParam;


void    init(void)
{
    char demo_path[MAX_PATH];
    GetPrivateProfileStringA( "Demo", "DemoConfig", "./default.ini", demo_path, MAX_PATH, "./ray-marching.ini");
    char fs_shader_path[MAX_PATH];
    GetPrivateProfileStringA( "Demo", "FSShader", "", fs_shader_path, MAX_PATH, std::string("./"+ std::string(demo_path)).c_str() );
    FSShaderPath = fs_shader_path;

    char strChannel0[MAX_PATH];
    GetPrivateProfileStringA( "Demo", "iChannel0", "", strChannel0, MAX_PATH, std::string("./"+ std::string(demo_path)).c_str() );

    DWORD status =  GetLastError();
    glClearColor(1.0, 1.0, 1.0, 0.0);
    glColor3f(0.0, 0.0, 0.0);

    // Shader
    PS0_ID = CreateShaderFromFile( "shader/Quad_VS.glsl", "shader/color.glsl" );

    // Quad
    QuadVAO = GetQuadVAO();

    // Texture
    iChannel0 = CreateTextureFromFile(strChannel0);

    // ubo
    glGenBuffers( 1, &RayMarchingParamID );
    glBindBuffer( GL_UNIFORM_BUFFER, RayMarchingParamID );
    glBufferData( GL_UNIFORM_BUFFER, sizeof( RayMarchingParam ), &iRayMarchingParam, GL_DYNAMIC_DRAW );
}

void reload()
{
    if(PS_ID != PS0_ID && glIsProgram(PS_ID)) glDeleteProgram( PS_ID );
    PS_ID = -1;
    PS_ID = CreateShaderFromFile("shader/Quad_VS.glsl", (char*) FSShaderPath.c_str());
}

void    display(void)
{
    if(bReload)
    {
        reload();
        bReload = false;
    }
    int i;

    /* clear the screen to white */
    glClear(GL_COLOR_BUFFER_BIT);

    if( glIsProgram( PS_ID ) )
    {
        glUseProgram(PS_ID);

        iRayMarchingParam.iGlobalTime = glutGet( GLUT_ELAPSED_TIME ) / 1000.0;
        glBufferData( GL_UNIFORM_BUFFER, sizeof( RayMarchingParam ), &iRayMarchingParam, GL_DYNAMIC_DRAW );
        glBindBufferBase( GL_UNIFORM_BUFFER, 0, RayMarchingParamID );
        glBindTexture(GL_TEXTURE_2D, iChannel0);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    }else
    {
        glUseProgram( PS0_ID );
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }
    
    glUseProgram(0);
    glBindTexture(GL_TEXTURE_2D, 0);

    glutSwapBuffers();
    glutPostRedisplay();
    return;
}

void reshape(int w, int h)
{
    width = (GLdouble) w;
    height = (GLdouble) h;

    iRayMarchingParam.iResolution.x = w;
    iRayMarchingParam.iResolution.y = h;
    iRayMarchingParam.iResolution.z = (GLdouble)w/h;

    glViewport(0, 0, (GLsizei) width, (GLsizei) height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, width, 0.0, height, -1.f, 1.f);


    return;
}

void
    kbd(unsigned char key, int x, int y)
{
    switch((char)key) {
    case 'q':
    case 27:    /* ESC */
        glutDestroyWindow(wd);
        exit(0);
        break;
    case 'r':
        bReload = true;
        break;
    default:
        break;
    }

    return;
}

void mouse( int button, int state, int x, int y){
    if( GLUT_LEFT_BUTTON == button )
    {
        if( GLUT_DOWN == state){
            iRayMarchingParam.iMouse.z = iRayMarchingParam.iMouse.x;
            iRayMarchingParam.iMouse.w = iRayMarchingParam.iMouse.y;
            bLeftButtonDown = true;
        }else{
            bLeftButtonDown = false;
        }
        
        
    }
}

void motion( int x, int y )
{
    if( bLeftButtonDown ){
        iRayMarchingParam.iMouse.x = x;
        iRayMarchingParam.iMouse.y = y;
    }
}

int
    main(int argc, char *argv[])
{
    glutInit(&argc, argv);

    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize((int) width, (int) height);
    glutInitWindowPosition(100, 100);
    wd = glutCreateWindow( "RayMarching" );

    GLenum err = glewInit();
    if( GLEW_OK != err )
    {
        fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
        exit(0);
    }

    init();

    glutReshapeFunc(reshape);
    glutKeyboardFunc(kbd);
    glutMouseFunc(mouse);
    glutMotionFunc( motion );
    glutDisplayFunc(display);


    glutMainLoop();

    exit(0);
}