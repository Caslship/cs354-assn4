/*
* Assignment #4
* Name: Jason Palacios
* UT EID: jap4839
* UTCS: jason777
*/

#include "scenegraph.h"

#include <GL/glut.h>
#include <stack>
#include "node.h"
#include "cameranode.h"
#include "lightnode.h"
#include "objectnode.h"
#include "geometrynode.h"

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
    GeometryNode * geom_node = new GeometryNode(object_node);
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
