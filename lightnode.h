/*
* Assignment #4
* Name: Jason Palacios
* UT EID: jap4839
* UTCS: jason777
*/

#ifndef __LIGHT_NODE_H__
#define __LIGHT_NODE_H__

#include "node.h"

#include <GL/glut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>

class LightNode : public Node
{
private:
    std::string light_type;
    GLenum light_id;
public:
    LightNode(void) : light_type("Point"), light_id(GL_LIGHT0), Node(NULL, "Light") {}
    LightNode(GLenum light_id, Node * parent) : light_type("Point"), light_id(light_id), Node(parent, "Light") {}
    ~LightNode();
    std::string getLightType(void);
    GLenum getLightId(void);
    void setId(GLenum light_id = GL_LIGHT0);
    void setType(std::string light_type = "Point");
    void traverseNode(glm::mat4 transform = glm::mat4(1.0), std::string render_type = "Solid");
};

#endif