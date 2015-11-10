/*
* Assignment #4
* Name: Jason Palacios
* UT EID: jap4839
* UTCS: jason777
*/

#include "transformnode.h"

#include <GL/glut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>

// Set transform node parameters to sensible defaults
TransformNode::TransformNode(void) : animation_transform(1.0), Node(NULL, "Transform")
{
    setParams();
}

// Set transform node parameters to sensible defaults
TransformNode::TransformNode(Node * parent) : animation_transform(1.0), Node(parent, "Transform")
{
    setParams();
}

// Get transform type
std::string TransformNode::getTransformType(void)
{
    return transform_type;
}

// Get X param
GLfloat TransformNode::getX(void)
{
    return xyz.x;
}

// Get Y param
GLfloat TransformNode::getY(void)
{
    return xyz.y;
}

// Get Z param
GLfloat TransformNode::getZ(void)
{
    return xyz.z;
}

// Get Theta param
GLfloat TransformNode::getTheta(void)
{
    return theta;
}

// Get flag indicating whether the transformation is an animation
bool TransformNode::getAnimationFlag(void)
{
    return animation_flag;
}

// Set transformation type
void TransformNode::setType(std::string transform_type)
{
    this->transform_type = transform_type;
}

// Set transformation parameters
void TransformNode::setParams(std::string transform_type, glm::vec3 xyz, GLfloat theta, bool animation_flag)
{
    this->transform_type = transform_type;
    this->xyz = xyz;
    this->theta = theta;
    this->animation_flag = animation_flag;
}

// Apply a transform to the current transform and then pass it down to children
void TransformNode::traverseNode(glm::mat4 transform, std::string render_type)
{
    // Animation transform matrix
    if (!animation_flag)
        animation_transform = glm::mat4(1.0);
    
    if (transform_type == "Scale")
    {
        // Build scale matrix
        glm::mat4 scale = glm::scale(glm::mat4(1.0), xyz);

        // Update animation matrix and set transform to be passed to children
        if (animation_flag)
        {
            animation_transform = scale * animation_transform;
            transform = animation_transform * transform;
        }
        else
            transform = scale * transform;
    }
    else if (transform_type == "Translate")
    {
        // Build translation matrix
        glm::mat4 translate = glm::translate(glm::mat4(1.0), xyz);

        // Update animation matrix and set transform to be passed to children
        if (animation_flag)
        {
            animation_transform = translate * animation_transform;
            transform = animation_transform * transform;
        }
        else     
            transform = translate * transform;
    }
    else if (transform_type == "Rotate")
    {
        // Build rotation matrix
        glm::mat4 rotate = glm::rotate(glm::mat4(1.0), glm::radians(theta), xyz);
        
        // Update animation matrix and set transform to be passed to children
        if (animation_flag)
        {
            animation_transform = rotate * animation_transform;
            transform = animation_transform * transform;
        }
        else     
            transform = rotate * transform;
    }

    Node::traverseNode(transform, render_type);
}