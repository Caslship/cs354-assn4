/*
* Assignment #4
* Name: Jason Palacios
* UT EID: jap4839
* UTCS: jason777
*/

#include "lightnode.h"

#include <GL/glut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>

LightNode::~LightNode(void)
{
    glDisable(light_id);
}

std::string LightNode::getLightType(void)
{
    return light_type;
}

GLenum LightNode::getLightId(void)
{
    return light_id;
}

void LightNode::setId(GLenum light_id)
{
    this->light_id = light_id;
}

void LightNode::setType(std::string light_type)
{
    this->light_type = light_type;
}

void LightNode::traverseNode(glm::mat4 transform, std::string render_type)
{
    // Have lights always start off at origin
    glm::vec4 pos(0.0, 0.0, 0.0, 1.0);

    // Have transformation matrix define where it should be placed
    pos = transform * pos;
    GLfloat light_pos[] = { pos.x, pos.y, pos.z, pos.w };
    GLfloat light_diffuse_specular[] = { 1.0, 1.0, 1.0, 1.0 };

    // Set w component to 0 if we have a directional light
    if (light_type == "Directional")
        light_pos[3] = 0.0;

    // Enable light
    glEnable(light_id);
    glLightfv(light_id, GL_POSITION, light_pos);
    glLightfv(light_id, GL_DIFFUSE, light_diffuse_specular);
    glLightfv(light_id, GL_SPECULAR, light_diffuse_specular);

    glDisable(GL_LIGHTING);

    glPointSize(20.0);
    glBegin(GL_POINTS);

        glColor4f(1.0, 0.95, 0.3, 1.0);
        glVertex3f(light_pos[0], light_pos[1], light_pos[2]);
        glColor4f(1.0, 1.0, 1.0, 1.0);

    glEnd();
    glPointSize(1.0);

    glEnable(GL_LIGHTING);
}