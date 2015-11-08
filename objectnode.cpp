/*
* Assignment #4
* Name: Jason Palacios
* UT EID: jap4839
* UTCS: jason777
*/

#include "objectnode.h"

#include <GL/glut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>

void ObjectNode::traverseNode(glm::mat4 transform, std::string render_type)
{
    glPushAttrib(GL_POLYGON_BIT);
    glPushMatrix();

        Node::traverseNode(transform, render_type);

    glPopMatrix();
    glPopAttrib();
}