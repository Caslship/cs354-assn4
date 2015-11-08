/*
* Assignment #4
* Name: Jason Palacios
* UT EID: jap4839
* UTCS: jason777
*/

#ifndef __ATTRIBUTE_NODE_H__
#define __ATTRIBUTE_NODE_H__

#include "node.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>

class AttributeNode : public Node
{
private:
    std::string render_type;
public:
    AttributeNode(void) : render_type("Shaded"), Node(NULL, "Attribute") {}
    AttributeNode(Node * parent) : render_type("Shaded"), Node(parent, "Attribute") {}
    std::string getRenderType(void);
    void setParams(std::string render_type);
    void traverseNode(glm::mat4 transform = glm::mat4(1.0), std::string render_type = "Solid");
};

#endif