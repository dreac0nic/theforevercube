#include <iostream>
#include <sstream>

#include <stdlib.h>
#include <time.h>

#include "gl_core_4_4.hpp"
#include <GLFW/glfw3.h>

#include "glslu.hpp"

#define ERRLOG(errstr) std::cerr << "ERR [" << __FILE__ << ":" << __LINE__ << "] " << errstr << std::endl;

void glfw_err_callback(int code, const char* message);

using namespace std;
using glm::vec2;
using glslu::Program;

int main(int argc, char* argv[])
{
    GLFWwindow* hWindow;
    
    // Set error callback, because GLFW is being persnickety.
    glfwSetErrorCallback([](int code, const char* message) -> void {
	    cerr << "GLFW ERR[" << code << "]: " << message; });
    
    // Load application framworks00...
    cerr << "INITIALIZING SYSTEMS" << endl
         << "--------------------" << endl;
    
    // Initialize GLFW and check for errors.
    cerr << "\tGLFW ... \t";
    
    if(!glfwInit()) {
	ERRLOG("Could not initialize GLFW!");
	
	glfwTerminate();
	
	return -1;
    } else {
	cerr << "OK" << endl;
    }

    // Window Creation
    cerr << "\tWindow ... \t";
    
    // Window hints to ensure GLFW context is proper.
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);

    // Attempt to create window based on context.
    hWindow = glfwCreateWindow(640, 480, "WINDOW", NULL, NULL);
    if(!hWindow) {
	ERRLOG("Could not create a window!");

	glfwTerminate();
	return -1;
    } else {
	cerr << "OK" << endl;
    }

    // Focus window context.
    glfwMakeContextCurrent(hWindow);

    // Initialize GL using created loader.
    cerr << "\tLoad GL ... \t";
    
    if(!gl::sys::LoadFunctions()) {
	ERRLOG("Could not load OpenGL!");

	glfwDestroyWindow(hWindow);
	glfwTerminate();
	return -1;
    } else {
	cerr << "OK [v" << gl::sys::GetMajorVersion() << "." << gl::sys::GetMinorVersion() << endl;
    }
    
    // Attempt to get context
    cerr << "\tGL Context ... \t";
    
    if(!glfwGetCurrentContext()) {
	ERRLOG("Could not get context!");
	
	glfwDestroyWindow(hWindow);
	glfwTerminate();
	return -1;
    } else {
	cerr << "OK [v" << (gl::GetString(gl::VERSION) != NULL ? (const char*)gl::GetString(gl::VERSION) : "NULL") << "; GLSL v" << gl::GetString(gl::SHADING_LANGUAGE_VERSION) << "]" << endl;
    }
    
    cerr << "SYSTEM ... OK" << endl
	 << "RUNNING" << endl;
    
    // Setup for Sierpinski Gasket
    float vertData[6];
    vec2 vertices[3] = {
	vec2(-1.0f, -1.0f),
	vec2( 0.0f,  1.0f),
	vec2( 1.0f, -1.0f)
    };
    
    for(int vert = 0; vert < 3; ++vert) {
	vertData[2*vert] = vertices[vert].x;
	vertData[2*vert + 1] = vertices[vert].y;
    }
    
    // Setup teh shaders
    Program basicProgram;
    basicProgram.compileShader("src/shaders/passthrough.glsl.vert");
    basicProgram.compileShader("src/shaders/red.glsl.frag");
    
    basicProgram.link();
    
    cout << basicProgram.getActiveUniforms() << endl
	 << basicProgram.getActiveUniformBlocks() << endl
	 << basicProgram.getActiveAttribs() << endl;
    
    if(basicProgram.isLinked())
	basicProgram.use();
    
    // Setup buffers and bind to shader attribute.
    GLuint vertBuffer;
    gl::GenBuffers(1, &vertBuffer);
    
    // Bind data to vertBuffer
    gl::BindBuffer(gl::ARRAY_BUFFER, vertBuffer);
    gl::BufferData(gl::ARRAY_BUFFER, 6*sizeof(float), vertData, gl::STATIC_DRAW);
    
    // Setup vertex array object
    GLuint vaoHandle;
    gl::GenVertexArrays(1, &vaoHandle);
    gl::BindVertexArray(vaoHandle);
    
    // Enable the position data
    gl::EnableVertexAttribArray(0);
    
    // Fill in buffer data
    gl::BindBuffer(gl::ARRAY_BUFFER, vertBuffer);
    gl::VertexAttribPointer(0, 2, gl::FLOAT, gl::FALSE_, 0, NULL);
    
    gl::ClearColor(0.95f, 0.95f, 0.95f, 1.0f);

    // Enter main loop of application.
    while(!glfwWindowShouldClose(hWindow)) {
	// Clear window
	gl::Clear(gl::COLOR_BUFFER_BIT);
	
	// RENDER
	gl::BindVertexArray(vaoHandle);
	gl::DrawArrays(gl::TRIANGLES, 0, 3);
	
	// Flush ouput
	gl::Flush();

	// Window housekeeping...
	glfwSwapBuffers(hWindow);
	glfwPollEvents();
	
	// SPAAAAAAAAACESHIP!
	[=](){;;;;};
    }

    // Cleanup application and exit.
    glfwTerminate();
    return 0;
}
