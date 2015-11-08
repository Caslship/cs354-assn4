/*
* Assignment #4
* Name: Jason Palacios
* UT EID: jap4839
* UTCS: jason777
*/

#include "attributenode.h"

#include <GL/glut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>

std::string AttributeNode::getRenderType(void)
{
    return render_type;
}

void AttributeNode::setParams(std::string render_type)
{
    this->render_type = render_type;
}

void AttributeNode::traverseNode(glm::mat4 transform, std::string render_type)
{
    if (this->render_type == "Points")
    {
        // Point mode
        glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
    }
    else if (this->render_type == "Wireframe")
    {
        // Wireframe mode
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    else if (this->render_type == "Solid")
    {
        // Solid mode
        glDisable(GL_LIGHTING);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
    else if (this->render_type == "Shaded")
    {
        // Shaded mode
        glEnable(GL_LIGHTING);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
    else if (this->render_type == "Face Normals")
    {
        // Face normals mode
        glEnable(GL_LIGHTING);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
    else if (this->render_type == "Vertex Normals")
    {
        // Vertex normals mode
        glEnable(GL_LIGHTING);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    Node::traverseNode(transform, this->render_type);
    glEnable(GL_LIGHTING);
}