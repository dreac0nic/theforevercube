#include <iostream>

#include "gl_comp_3_1.hpp"
#include <GLFW/glfw3.h>

void glfw_err_callback(int code, const char* message);

using namespace std;

int main(int argc, char* argv[])
{
    GLFWwindow* hWindow;

    // Set error callback, because GLFW is being persnickety.
    glfwSetErrorCallback(glfw_err_callback);

    // Initialize GLFW and check for errors.
    if(!glfwInit()) {
	cerr << "Could not initialize GLFW!" << endl;

	return -1;
    }

    // Window hints to ensure GLFW context is proper.
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

    // Attempt to create window based on context.
    hWindow = glfwCreateWindow(640, 480, "WINDOW", NULL, NULL);
    if(!hWindow) {
	cerr << "Could not create a window!" << endl;

	glfwTerminate();
	return -1;
    }

    // Focus window context.
    glfwMakeContextCurrent(hWindow);

    // Initialize GL using created loader.
    if(!gl::sys::LoadFunctions()) {
	cerr << "Could not load GL!" << endl;

	glfwTerminate();
	return -1;
    }

    // Enter main loop of application.
    while(!glfwWindowShouldClose(hWindow)) {
	// RENDER

	// Window housekeeping...
	glfwSwapBuffers(hWindow);
	glfwPollEvents();
    }

    // Cleanup application and exit.
    glfwTerminate();
    return 0;
}

void glfw_err_callback(int code, const char* message) { cerr << "GLFW ERR[" << code << "]: " << message << endl; }
