#ifndef GLSL_UTILITIES
#define GLSL_UTILITIES

#include <stdexcept>
#include <string>
#include <map>

#include <glm/glm.hpp>

#include "gl_core_4_4.hpp"

namespace glslu
{
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
	ProgramException(const std::string &msg): std::runtime_error(msg) {}
    };
    
    class Program
    {
    private:
	int handle;
	bool linked;
	std::map<std::string, int> uniformLocations;
	
	// Minor helper functions for internals.
	GLint getUniformLocation(const std::string& name);
	bool fileExists(const std::string& filename);
	std::string getExtension(const std::string& filename);
	
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
	void compileShader(const std::string& filename) throw (ProgramException);
	void compileShader(const std::string& filename, ShaderType type) throw (ProgramException);
	void compileShaderSource(const std::string& source, ShaderType type, const std::string& filename = "") throw (ProgramException);
	
	// Shader/Program Management
	void link(void) throw (ProgramException);
	void validate(void) throw (ProgramException);
	void use(void) throw (ProgramException);
	
	// Attribute handlers
	void bindAttribLocation(GLuint location, const std::string& name);
	void bindFragDataLocation(GLuint location, const std::string& name);
	
	// Uniform handlers
	void setUniform(const std::string& name, bool value);
	void setUniform(const std::string& name, int value);
	void setUniform(const std::string& name, float value);
	void setUniform(const std::string& name, GLuint value);
	void setUniform(const std::string& name, float x, float y);
	void setUniform(const std::string& name, float x, float y, float z);
	void setUniform(const std::string& name, float x, float y, float z, float w);
	void setUniform(const std::string& name, const glm::vec2& vector);
	void setUniform(const std::string& name, const glm::vec3& vector);
	void setUniform(const std::string& name, const glm::vec4& vector);
	void setUniform(const std::string& name, const glm::mat3& matrix);
	void setUniform(const std::string& name, const glm::mat4& matrix);
	
	// String functions
	std::string getActiveUniforms(void);
	std::string getActiveUniformBlocks(void);
	std::string getActiveAttribs(void);
	
	// Type helper
	std::string getTypeString(GLenum type);
    };
}

#endif
