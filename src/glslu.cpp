#include "glslu.hpp"

#include <iostream>
#include <iomanip>
#include <string>
#include <fstream>
#include <sstream>

#include <glm/glm.hpp>

using std::ifstream;
using std::ios;
using std::string;
using std::setw;
using std::endl;
using std::stringstream;
using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::mat2;
using glm::mat3;
using glm::mat4;

namespace glslu
{
    namespace ShaderInfo {
	struct shader_file_extension {
	    const string extension;
	    ShaderType type;
	};

	struct shader_file_extension extensions[] = {
	    {".vs", VERTEX},
	    {".vert", VERTEX},
	    {".fs", FRAGMENT},
	    {".frag", FRAGMENT},
	    {".gs", GEOMETRY},
	    {".geom", GEOMETRY},
	    {".tcs", TESS_CONTROL},
	    {".tes", TESS_EVALUATION},
	    {".cs", COMPUTE},
	    {".comp", COMPUTE}
	};
    }
    
    // Constructor
    Program::Program(void):
	handle(0), linked(false) {}
    
    // Deconstructor!
    Program::~Program(void)
    {
	if(handle == 0) return;
	
	// Query the attached shader count
	GLint shaderCount = 0;
	gl::GetProgramiv(handle, gl::ATTACHED_SHADERS, &shaderCount);
	
	// Query shader handles
	GLuint* shaders = new GLuint[shaderCount];
	gl::GetAttachedShaders(handle, shaderCount, NULL, shaders);
	
	// Delete and cleanup after each shader
	for(int current = 0; current < shaderCount; ++current)
	    gl::DeleteShader(shaders[current]);
	
	// Delete the program itself
	gl::DeleteProgram(handle);
	
	// Cleanup
	delete[] shaders;
    }
    
    // Handle accessor
    int Program::getHandle(void) { return handle; }
    
    // Link accessor
    bool Program::isLinked(void) { return linked; }
    
    // Get the location of a uniform based on its name
    int Program::getUniformLocation(const string& name)
    {
	std::map<string, int>::iterator position;
	position = uniformLocations.find(name);
	
	// If the name could not be found, query and store it for future checks.
	if(position == uniformLocations.end())
	    uniformLocations[name] = gl::GetUniformLocation(handle, name.c_str());
	
	return uniformLocations[name];
    }
    
    // Check to see if a file exists
    // NOTE: stat is considerably faster, look at using that if it isn't screwy as heck
    bool Program::fileExists(const string& filename)
    {
	if(FILE *file = fopen(filename.c_str(), "r")) {
	    fclose(file);
	    
	    return true;
	} else {
	    return false;
	}
    }
    
    // Retrieve the file extensions of a filename
    string Program::getExtension(const string& filename)
    {
	size_t location = filename.find_last_of('.');
	
	if(location != string::npos)
	    return filename.substr(location, string::npos);
	
	return "";
    }
    
    // String type translator
    string Program::getTypeString(GLenum type)
    {
	switch(type) {
	case gl::FLOAT:        return "float"; break;
	case gl::FLOAT_VEC2:   return "vec2"; break;
	case gl::FLOAT_VEC3:   return "vec3"; break;
	case gl::FLOAT_VEC4:   return "vec4"; break;
	case gl::DOUBLE:       return "double"; break;
	case gl::INT:          return "int"; break;
	case gl::UNSIGNED_INT: return "unsigned int"; break;
	case gl::BOOL:         return "boolean"; break;
	case gl::FLOAT_MAT2:   return "mat2"; break;
	case gl::FLOAT_MAT3:   return "mat3"; break;
	case gl::FLOAT_MAT4:   return "mat4"; break;
	default:               return "???"; break;
	}
    }
    
    // Type-smart shader compiler
    void Program::compileShader(const string& filename)
	throw(ProgramException)
    {
	int extensionCount = sizeof(ShaderInfo::extensions)/sizeof(ShaderInfo::shader_file_extension);
	
	// Check for valid file extension.
	string extension = getExtension(filename);
	ShaderType type;
	bool found = false;
	
	for(int ext = 0; ext < extensionCount; ++ext) {
	    if(extension == ShaderInfo::extensions[ext].extension) {
		found = true;
		type = ShaderInfo::extensions[ext].type;
		
		break;
	    }
	}
	
	// Throw an exception if not found!
	if(!found) {
	    stringstream buffer;
	    
	    buffer << "Unrecognized shader extension: \'" << extension << "\'!" << endl
		   << "\tExpected: ";
	    
	    for(int ext = 0; ext < extensionCount; ++ext)
		buffer << (ext != 0 ? ", " : "") << ShaderInfo::extensions[ext].extension;
	    
	    throw ProgramException(buffer.str());
	}
	
	// Pass the information onto the next compile function
	compileShader(filename, type);
    }
    
    // Source-retriving shader compiler
    void Program::compileShader(const string& filename, ShaderType type)
	throw(ProgramException)
    {
	// Test file existance!
	if(!fileExists(filename)) {
	    stringstream buffer;
	    
	    buffer << "Could not find shader \"" << filename << "\". Did it escape?";
	    
	    throw ProgramException(buffer.str());
	}
	
	// Create program if necessary.
	if(handle <= 0) {
	    handle = gl::CreateProgram();
	    
	    if(handle == 0)
		throw ProgramException("Could not create shader program.");
	}
	
	// Open file
	ifstream shader(filename.c_str(), ios::in);
	
	if(!shader) {
	    stringstream buffer;
	    
	    buffer << "Could not open \"" << filename << "\", access denied! :(";
	    
	    throw ProgramException(buffer.str());
	}
	
	// Read contents of file.
	stringstream source;
	source << shader.rdbuf();
	
	// Cleanup and pass on contents.
	shader.close();
	
	compileShaderSource(source.str(), type, filename);
    }
    
    // Compile shader with source
    void Program::compileShaderSource(const string& source, ShaderType type, const string& filename)
	throw(ProgramException)
    {
	// Create shader program if necessary
	if(handle <= 0) {
	    handle = gl::CreateProgram();
	    
	    if(handle == 0)
		throw ProgramException("Could not create shader program.");
	}
	
	// Create shader and attach source
	GLuint shaderHandle = gl::CreateShader(type);
        
	const char* c_source = source.c_str();
	gl::ShaderSource(shaderHandle, 1, &c_source, NULL);
	
	// Compile the shader
	gl::CompileShader(shaderHandle);
	
	// Check for compile errors
	int status;
	gl::GetShaderiv(shaderHandle, gl::COMPILE_STATUS, &status);
	
	if(status == gl::FALSE_) {
	    int length = 0;
	    string log;
	    stringstream exceptionMessage;
	    
	    gl::GetShaderiv(shaderHandle, gl::INFO_LOG_LENGTH, &length);
	    
	    if(length > 0) {
		char* c_log = new char[length];
		int written = 0;
		
		gl::GetShaderInfoLog(shaderHandle, length, &written, c_log);
		
		log = c_log;
		
		delete[] c_log;
	    }
	    
	    // Construct exception...
	    if(filename != "")
		exceptionMessage << "\"" << filename << "\" could not be compiled!";
	    else
		exceptionMessage << "Shader could not be compiled!";
	    
	    exceptionMessage << endl << log;
	    
	    throw ProgramException(exceptionMessage.str());
	} else
	    gl::AttachShader(handle, shaderHandle);
    }
    
    void Program::link(void)
	throw(ProgramException)
    {
	if(linked) return;
	else if(handle <= 0)
	    throw ProgramException("Program has not been initialized! (Have you attached shaders to it?)");
	
	// Linking is easy!
	gl::LinkProgram(handle);
	
	// Check link status
	int status;
	
	gl::GetProgramiv(handle, gl::LINK_STATUS, &status);
	
	if(status == gl::FALSE_) {
	    int length = 0;
	    string log;
	    stringstream exceptionMessage;
	    
	    gl::GetProgramiv(handle, gl::INFO_LOG_LENGTH, &length);
	    
	    if(length > 0) {
		char* c_log = new char[length];
		int written = 0;
		
		gl::GetProgramInfoLog(handle, length, &written, c_log);
		
		log = c_log;
		
		delete[] c_log;
	    }
	    
	    // Construct exception message...
	    exceptionMessage << "Could not link Program[" << handle << "]" << endl
			     << log;
		  
	    
	    throw ProgramException(exceptionMessage.str());
	} else {
	    uniformLocations.clear();
	    linked = true;
	}
    }
    
    // Validate the program's state
    void Program::validate(void)
	throw(ProgramException)
    {
	if(!linked)
	    throw ProgramException("Program has not been linked!");
	
	// Validate program
	GLint status;
	gl::ValidateProgram(handle);
	gl::GetProgramiv(handle, gl::VALIDATE_STATUS, &status);
	
	// Check validation status
	if(status == gl::FALSE_) {
	    // Get the program validation log
	    int length = 0;
	    string log;
	    stringstream exceptionMessage;
	    
	    gl::GetProgramiv(handle, gl::INFO_LOG_LENGTH, &length);
	    
	    if(length > 0) {
		char* c_log = new char[length];
		int written = 0;
		gl::GetProgramInfoLog(handle, length, &written, c_log);
		
		log = c_log;
		
		delete[] c_log;
	    }
	    
	    exceptionMessage << "Program did not validate: " << endl
			     << log;
	    
	    throw ProgramException(exceptionMessage.str());
	}
    }
    
    // Focuses this program to use for next pass!
    void Program::use(void)
	throw(ProgramException)
    {
	if(handle <= 0)
	    throw ProgramException("Program has not been initialized! (Have you attached shaders to it?)");
	else if(!linked)
	    throw ProgramException("Program has not been linked!");
	
	gl::UseProgram(handle);
    }
    
    // Attrib Bind Location
    void Program::bindAttribLocation(GLuint location, const string& name) { gl::BindAttribLocation(handle, location, name.c_str()); }
    
    // Frag Data Bind Location
    void Program::bindFragDataLocation(GLuint location, const string& name) { gl::BindFragDataLocation(handle, location, name.c_str()); }
    
    // Set Uniform for boolean value.
    void Program::setUniform(const string& name, bool value)
    {
	GLint location = getUniformLocation(name);
	gl::Uniform1i(location, value); // Possibly reference integer set inform...
    }
    
    // Set Uniform for integer value
    void Program::setUniform(const string& name, int value)
    {
	GLint location = getUniformLocation(name);
	gl::Uniform1i(location, value);
    }
    
    // Set Uniform for float value
    void Program::setUniform(const string& name, float value)
    {
	GLint location = getUniformLocation(name);
	gl::Uniform1f(location, value);
    }
    
    // Set Uniform for GL unsigned integer
    void Program::setUniform(const string& name, GLuint value)
    {
	GLint location = getUniformLocation(name);
	gl::Uniform1ui(location, value);
    }
    
    // Set Uniform for double float value
    void Program::setUniform(const string& name, float x, float y)
    {
	GLint location = getUniformLocation(name);
	gl::Uniform2f(location, x, y);
    }
    
    // Set Uniform for triple float value
    void Program::setUniform(const string& name, float x, float y, float z)
    {
	GLint location = getUniformLocation(name);
	gl::Uniform3f(location, x, y, z);
    }
    
    // Set Uniform for quad float value
    void Program::setUniform(const string& name, float x, float y, float z, float w)
    {
	GLint location = getUniformLocation(name);
	gl::Uniform4f(location, x, y, z, w);
    }
    
    // Set Uniform for 2-value vector
    void Program::setUniform(const string& name, const vec2& vector)
    {
        this->setUniform(name, vector.x, vector.y);
    }
    
    // Set Uniform for 3-value vector
    void Program::setUniform(const string& name, const vec3& vector)
    {
	this->setUniform(name, vector.x, vector.y, vector.z);
    }
    
    // Set Uniform for 4-value vector
    void Program::setUniform(const string& name, const vec4& vector)
    {
	this->setUniform(name, vector.x, vector.y, vector.z, vector.w);
    }
    
    // Set Uniform for 3x3 matrix
    void Program::setUniform(const string& name, const mat3& matrix)
    {
	GLint location = getUniformLocation(name);
	gl::UniformMatrix3fv(location, 1, gl::FALSE_, &matrix[0][0]);
    }
    
    // Set Uniform for 4x4 matrix
    void Program::setUniform(const string& name, const mat4& matrix)
    {
	GLint location = getUniformLocation(name);
	gl::UniformMatrix4fv(location, 1, gl::FALSE_, &matrix[0][0]);
    }
    
    // Get a string containing all active uniforms
    string Program::getActiveUniforms(void)
    {
	stringstream buffer;
	
	// Get a count of the uniforms
	GLint uniformCount = 0;
	gl::GetProgramInterfaceiv(handle, gl::UNIFORM, gl::ACTIVE_RESOURCES, &uniformCount);
	
	// Setup information query
	GLenum properties[] = {gl::NAME_LENGTH, gl::TYPE, gl::LOCATION, gl::BLOCK_INDEX};
	
	// Acquire uniform info
	buffer << "Active Uniforms" << endl;
	
	if(uniformCount <= 0) {
	    buffer << "\tNONE" << endl;
	} else {	
	    for(int curr = 0; curr < uniformCount; ++curr) {
		GLint results[4];
		char* name;
		GLint nameLength = 0;
		gl::GetProgramResourceiv(handle, gl::UNIFORM, curr, 4, properties, 4, NULL, results);
	    
		// Skip uniforms in blocks!
		if(results[3] != -1) continue;
	    
		// Read results and store them away!
		nameLength = results[0] + 1;
		name = new char[nameLength];
		gl::GetProgramResourceName(handle, gl::UNIFORM, curr, nameLength, NULL, name);
	    
		buffer << setw(5) << results[2] << setw(0) << " " << name << " (" << getTypeString(results[1]) << ")" << endl;
	    
		delete[] name;
	    }
	}
	
	return buffer.str();
    }
    
    // Get a string containing all active uniform blocks
    string Program::getActiveUniformBlocks(void)
    {
	stringstream buffer;
	
	// Get a count of the active uniform blocks
	GLint blockCount = 0;
	gl::GetProgramInterfaceiv(handle, gl::UNIFORM_BLOCK, gl::ACTIVE_RESOURCES, &blockCount);
	
	// Setup uniform block queries
	GLenum blockProperties[] = {gl::NUM_ACTIVE_VARIABLES, gl::NAME_LENGTH};
	GLenum blockIndex[] = {gl::ACTIVE_VARIABLES};
	GLenum properties[] = {gl::NAME_LENGTH, gl::TYPE, gl::BLOCK_INDEX};
	
	buffer << "Uniform Blocks" << endl;
	
	if(blockCount <= 0) {
	    buffer << "\tNONE" << endl;
	} else {
	    // Query for the information about each block!
	    for(int block = 0; block < blockCount; ++block) {
		GLint blockInfo[2];
		GLint uniformCount;
		gl::GetProgramResourceiv(handle, gl::UNIFORM_BLOCK, block, 2 , blockProperties, 2, NULL, blockInfo);
	    
		uniformCount = blockInfo[0];
	    
		// Get block name!
		GLint blockNameLength = blockInfo[1] + 1;
		char* blockName = new char[blockNameLength];
		gl::GetProgramResourceName(handle, gl::UNIFORM_BLOCK, block, blockNameLength, NULL, blockName);
	    
		// Print block name
		buffer << "Uniform block \"" << blockName << "\":" << endl;
	    
		delete[] blockName;
	    
		// Get the list of uniforms within the block
		GLint* uniformIndexes = new GLint[uniformCount];
		gl::GetProgramResourceiv(handle, gl::UNIFORM_BLOCK, block, 1, blockIndex, uniformCount, NULL, uniformIndexes);
	    
		// Iterate over each uniform and add it to the listing.
		for(int uniform = 0; uniform < uniformCount; ++uniform) {
		    GLint uniformIndex = uniformIndexes[uniform];
		    GLint results[3];
		    gl::GetProgramResourceiv(handle, gl::UNIFORM, uniformIndex, 3, properties, 3, NULL, results);
		
		    // Get uniform name
		    GLint uniformNameLength = results[0] + 1;
		    char* uniformName = new char[uniformNameLength];
		    gl::GetProgramResourceName(handle, gl::UNIFORM, uniformIndex, uniformNameLength, NULL, uniformName);
		
		    // Print out information
		    buffer << setw(5) << uniformIndex << setw(0) << " [" << uniform << "] " << uniformName << " (" << getTypeString(results[1]) << ")" << endl;
		
		    delete[] uniformName;
		}
	    
		delete[] uniformIndexes;
	    }
	}
	
	return buffer.str();
    }
    
    // Get a string of all active attributes
    string Program::getActiveAttribs(void)
    {
	stringstream buffer;
	
	// Get the number of attributes active
	GLint attributeCount;
	gl::GetProgramInterfaceiv(handle, gl::PROGRAM_INPUT, gl::ACTIVE_RESOURCES, &attributeCount);
	
	// Setup attribute query
	GLenum properties[] = {gl::NAME_LENGTH, gl::TYPE, gl::LOCATION};
	
	// Print and query each attribute
	buffer << "Active Attributes" << endl;
	
	if(attributeCount <= 0) {
	    buffer << "\tNONE" << endl;
	} else {
	    for(int attribute = 0; attribute < attributeCount; ++attribute) {
		GLint results[3];
		gl::GetProgramResourceiv(handle, gl::PROGRAM_INPUT, attribute, 3, properties, 3, NULL, results);
	    
		// Get attribute name
		GLint nameLength = results[0] + 1;
		char* name = new char[nameLength];
		gl::GetProgramResourceName(handle, gl::PROGRAM_INPUT, attribute, nameLength, NULL, name);
	    
		buffer << setw(5) << results[2] << setw(0) << " " << name << "(" << getTypeString(results[1]) << ")" << endl;
	    
		delete[] name;
	    }
	}
	
	return buffer.str();
    }	    
}
