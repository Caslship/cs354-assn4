/*
* Assignment #4
* Name: Jason Palacios
* UT EID: jap4839
* UTCS: jason777
*/

#ifndef __GEOMETRY_NODE_H__
#define __GEOMETRY_NODE_H__

#include "node.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include "geom.h"

class GeometryNode : public Node
{
private:
    Trimesh model;
    std::string file_path;
public:
    GeometryNode(void) : file_path(""), Node(NULL, "Geometry") {}
    GeometryNode(Node * parent) : file_path(""), Node(parent, "Geometry") {}
    std::string getFilePath(void);
    void setMesh(const char * file_path_cstring);
    void traverseNode(glm::mat4 transform = glm::mat4(1.0), std::string render_type = "Solid");
};

#endif