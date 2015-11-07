/*
* Assignment #4
* Name: Jason Palacios
* UT EID: jap4839
* UTCS: jason777
*/

#ifndef __SCENEGRAPH_H__
#define __SCENEGRAPH_H__

#include <GL/glut.h>
#include <stack>
#include "node.h"

class SceneGraphContainer
{
private:
    Node * root_node;
    CameraNode * camera_node;
    Node * curr_node;
    int light_count;
    std::stack<GLenum> free_light_ids;

public:
    SceneGraphContainer(void);
    ~SceneGraphContainer(void);
    Node * getRootNode(void);
    CameraNode * getCameraNode(void);
    Node * getCurrentNode(void);;
    int getLightCount(void);
    bool removeLight(GLenum light_id = GL_LIGHT0);
    GLenum addLight(void);
    void setCurrentNode(Node * new_curr_node);
    void traverseGraph(void);
};

SceneGraphContainer::SceneGraphContainer(void)
{
    root_node = new Node();
    curr_node = root_node;
    camera_node = new CameraNode(root_node);

    LightNode * light_node = new LightNode(GL_LIGHT0, root_node);
    light_count = 1;

    for (int i = 7; i > 0 ; i--)
        free_light_ids.push(GL_LIGHT0 + i);

    ObjectNode * object_node = new ObjectNode(root_node);
}

SceneGraphContainer::~SceneGraphContainer(void)
{
    delete root_node;
}

Node * SceneGraphContainer::getRootNode(void)
{
    return root_node;
}

CameraNode * SceneGraphContainer::getCameraNode(void)
{
    return camera_node;
}

Node * SceneGraphContainer::getCurrentNode(void)
{
    return curr_node;
}

int SceneGraphContainer::getLightCount(void)
{
    return light_count;
}

bool SceneGraphContainer::removeLight(GLenum light_id)
{
    light_count--;

    if (light_count < 1)
    {
        light_count = 1;
        return false;
    }

    free_light_ids.push(light_id);

    return true;
}

GLenum SceneGraphContainer::addLight(void)
{
    light_count++;

    if (light_count > 8)
    {
        light_count = 8;
        return GL_INVALID_ENUM;
    }

    GLenum light_id = free_light_ids.top();
    free_light_ids.pop();

    return light_id;
}

void SceneGraphContainer::setCurrentNode(Node * new_curr_node)
{
    curr_node = new_curr_node;
}

void SceneGraphContainer::traverseGraph(void)
{
    root_node->traverseNode();
}

#endif