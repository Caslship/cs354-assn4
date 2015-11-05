/*
* Assignment #4
* Name: Jason Palacios
* UT EID: jap4839
* UTCS: jason777
*/

#ifndef __SCENEGRAPH_H__
#define __SCENEGRAPH_H__

#include "node.h"

class SceneGraphContainer
{
private:
    Node * root_node;
    CameraNode * camera_node;
    Node * curr_node;
    int light_count;
    int free_light_id;

public:
    SceneGraphContainer(void);
    ~SceneGraphContainer(void);
    Node * getRootNode(void);
    CameraNode * getCameraNode(void);
    Node * getCurrentNode(void);;
    int getLightCount(void);
    int getFreeLightId(void);
    void setCurrentNode(Node * new_curr_node);
    bool decLightCount(void);
    bool incLightCount(void);
    void traverseGraph(void);
};

SceneGraphContainer::SceneGraphContainer(void)
{
    root_node = new Node();
    curr_node = root_node;
    camera_node = new CameraNode(root_node);

    LightNode * light_node = new LightNode(GL_LIGHT0, root_node);
    light_count = 1;
    free_light_id = 1;
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

int SceneGraphContainer::getFreeLightId(void)
{
    return free_light_id;
}

void SceneGraphContainer::setCurrentNode(Node * new_curr_node)
{
    curr_node = new_curr_node;
}

bool SceneGraphContainer::decLightCount(void)
{
    light_count--;

    if (light_count < 1)
    {
        light_count = 1;
        return false;
    }

    return true;
}

bool SceneGraphContainer::incLightCount(void)
{
    light_count++;

    if (light_count > 8)
    {
        light_count = 8;
        return false;
    }

    return true;
}

void SceneGraphContainer::traverseGraph(void)
{
    root_node->traverseNode();
}

#endif