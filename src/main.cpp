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
    unsigned int pointCount = 1;
    vec2* points;
    vec2 vertices[3] = {
	vec2(-1.0f, -1.0f),
	vec2( 0.0f,  1.0f),
	vec2( 1.0f, -1.0f)
    };
    
    srand(time(NULL));
    
    // Get the number of points to generate
    cout << "Please enter the number of points to generate: ";
    cin >> pointCount;
    
    points = new vec2[pointCount];
    
    // Set first point and calculate the rest of the points from there.
    points[0] = vec2(0.25f, 0.5f);
    
    for(int point = 1; point < pointCount; ++point) {
	int vertex = rand()%3;
	
	points[point] = (points[point - 1] + vertices[vertex])/2.0f;
    }
    
    // Generate position data in normal array:
    float pointData[2*pointCount];
    
    for(int point = 0; point < pointCount; point++) {
	pointData[point*2] = points[point].x;
	pointData[point*2 + 1] = points[point].y;
    }
    
    delete[] points;
    
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
    GLuint buffer;
    gl::GenBuffers(1, &buffer);
    
    // Populate buffer
    gl::BindBuffer(gl::ARRAY_BUFFER, buffer);
    gl::BufferData(gl::ARRAY_BUFFER, 2*pointCount*sizeof(float), pointData, gl::STATIC_DRAW);
    
    // Bind the vertexes to the input buffer;
    GLuint vao;
    gl::GenVertexArrays(1, &vao);
    gl::BindVertexArray(vao);
    
    // Enable VAO for position
    gl::EnableVertexAttribArray(0);
    
    // Populate data
    gl::VertexAttribPointer(0, 2, gl::FLOAT, gl::FALSE_, 0, NULL);
    //gl::VertexAttribFormat(0, 2, gl::FLOAT, gl::FALSE_, 0);
    //gl::VertexAttribBinding(0, 0);
    
    gl::ClearColor(0.95f, 0.95f, 0.95f, 1.0f);

    // Enter main loop of application.
    while(!glfwWindowShouldClose(hWindow)) {
	// Clear window
	gl::Clear(gl::COLOR_BUFFER_BIT);
	
	// RENDER
	gl::BindVertexArray(vao);
	gl::DrawArrays(gl::POINTS, 0, pointCount);
	
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
