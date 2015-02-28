#ifndef GLSL_UTILITIES
#define GLSL_UTILITIES

#include <stdexcept>
#include <string>
#include <map>

#include <glm/glm.hpp>

#include "gl_core_3_3.hpp"

using std::string;

using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::mat3;
using glm::mat4;

namespace GLSLUtilities {
    enum ShaderType
    {
	VERTEX = gl::VERTEX_SHADER,
	FRAGMENT = gl::FRAGMENT_SHADER,
	GEOMETRY = gl::GEOMETRY_SHADER,
	TESS_CONTROL = gl::TESS_CONTROL_SHADER,
	TESS_EVALUATION = gl::TESS_EVALUATION_SHADER,
	COMPUTE = gl::COMPUTE_SHADER
    };
    
    class ProgramException: public std::runtime_error
    {
    public:
	ProgramException(const string &msg): std::runtime_error(msg) {}
    };
    
    class Program
    {
    private:
	int handle;
	bool linked;
	std::map<string, int> uniformLocations;
	
	// Minor helper functions for internals.
	GLint getUniformLocation(const string& name);
	bool fileExists(const string& filename);
	string getExtension(const string& filename);
	
	// Prevent object copying
	Program(const Program& other) {}
	Program& operator=(const Program& other) { return *this; }
	
    public:
	// Constructor/Destructor
	Program(void);
	~Program(void);

	// Status functions
	int getHandle(void);
	bool isLinked(void);
	
	// Compile functions
	void compileShader(const string& filename) throw (ProgramException);
	void compileShader(const string& filename, ShaderType type) throw (ProgramException);
	void compileShader(const string& source, ShaderType type, const string& filename = "") throw (ProgramException);
	
	// Shader/Program Management
	void link(void) throw (ProgramException);
	void validate(void) throw (ProgramException);
	void use(void) throw (ProgramException);
	
	// Attribute handlers
	void bindAttribLocation(GLuint location, const string& name);
	void bindFragDataLocation(GLuint location, const string& name);
	
	// Uniform handlers
	void setUniform(const string& name, bool value);
	void setUniform(const string& name, int value);
	void setUniform(const string& name, float value);
	void setUniform(const string& name, GLuint value);
	void setUniform(const string& name, float x, float y);
	void setUniform(const string& name, float x, float y, float z);
	void setUniform(const string& name, float x, float y, float z, float w);
	void setUniform(const string& name, const vec2& vector);
	void setUniform(const string& name, const vec3& vector);
	void setUniform(const string& name, const vec4& vector);
	void setUniform(const string& name, const mat3& matrix);
	void setUniform(const string& name, const mat4& matrix);
	
	// String functions
	string getActiveUniforms(void);
	string getActiveUniformBlock(void);
	string getActiveAttribs(void);
	
	// Type helper
	string getTypeString(GLenum type);
}

#endif
