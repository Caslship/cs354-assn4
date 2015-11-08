/*
* Assignment #4
* Name: Jason Palacios
* UT EID: jap4839
* UTCS: jason777
*/

#ifndef __TRANSFORM_NODE_H__
#define __TRANSFORM_NODE_H__

#include "node.h"

#include <GL/glut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>

class TransformNode : public Node
{
private:
    std::string transform_type;
    glm::vec3 xyz;
    GLfloat theta;
    bool animation_flag;
    glm::mat4 animation_transform;

public:
    TransformNode(void);
    TransformNode(Node * parent);
    std::string getTransformType(void);
    GLfloat getX(void);
    GLfloat getY(void);
    GLfloat getZ(void);
    GLfloat getTheta(void);
    bool getAnimationFlag(void);
    void setType(std::string transform_type = "Scale");
    void setParams(std::string transform_type = "Scale", glm::vec3 xyz = glm::vec3(1.0, 1.0, 1.0), GLfloat theta = 0.0, bool animation_flag = false);
    void traverseNode(glm::mat4 transform = glm::mat4(1.0), std::string render_type = "Solid");
};

#endif