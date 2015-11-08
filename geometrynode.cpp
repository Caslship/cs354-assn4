/*
* Assignment #4
* Name: Jason Palacios
* UT EID: jap4839
* UTCS: jason777
*/

#include "geometrynode.h"

#include <GL/glut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include "helpers.h"
#include "geom.h"
#include "loader.h"

std::string GeometryNode::getFilePath(void)
{
    return file_path;
}

void GeometryNode::setMesh(const char * file_path_cstring)
{
    file_path = file_path_cstring;
    int file_path_length = file_path.length();
    int pos = 0;

    // Skip spaces but make sure not to hit the end of the string 
    while (pos < file_path_length && file_path[pos] ==  ' ')
        pos++;

    if (pos == file_path_length)
        return;

    // Read file name and update position
    std::string file_name = "";
    while (pos < file_path_length && file_path[pos] != ' ')
    {
        file_name += file_path[pos++];
    }

    // Append .obj extension to the string if needed
    int ext_pos = file_name.find(".obj");
    if (ext_pos == std::string::npos || ext_pos != file_name.length() - 4)
        file_name += ".obj";

    // Load object file into Trimesh object
    TrimeshLoader model_loader;
    Trimesh new_model;

    // Fail if the object failed to load
    if (!model_loader.loadOBJ(file_name.c_str(), &new_model))
        return;

    model = new_model;

    // We need to update the camera
    UpdateCameraGivenMesh(model);
}

void GeometryNode::traverseNode(glm::mat4 transform, std::string render_type)
{
    glMatrixMode(GL_MODELVIEW);

    glm::vec3 model_center = model.getCenter();

    glm::mat4 model_mat = glm::translate(glm::mat4(1.0), model_center) * transform * glm::translate(glm::mat4(1.0), (GLfloat)-1.0 * model_center);

    glMultMatrixf(glm::value_ptr(model_mat));

    model.render(render_type);
}