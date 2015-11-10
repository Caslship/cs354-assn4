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

// Initialize the scene graph to contain a camera, light, and object node with a child geometry node
SceneGraphContainer::SceneGraphContainer(void)
{
    root_node = new Node();
    curr_node = root_node;
    camera_node = new CameraNode(root_node);

    LightNode * light_node = new LightNode(GL_LIGHT0, root_node);
    light_count = 1;

    // Build vector of free light ids
    for (int i = 7; i > 0 ; i--)
        free_light_ids.push(GL_LIGHT0 + i);

    ObjectNode * object_node = new ObjectNode(root_node);
    GeometryNode * geom_node = new GeometryNode(object_node);
}

// Delete all of scene graph
SceneGraphContainer::~SceneGraphContainer(void)
{
    delete root_node;
}

// Get pointer to root node
Node * SceneGraphContainer::getRootNode(void)
{
    return root_node;
}

// Get pointer to camera node
CameraNode * SceneGraphContainer::getCameraNode(void)
{
    return camera_node;
}

// Get pointer to current node
Node * SceneGraphContainer::getCurrentNode(void)
{
    return curr_node;
}

// Get the amount of lights in the scene
int SceneGraphContainer::getLightCount(void)
{
    return light_count;
}

// Given a light id, decrease the light count and push its id onto the free light id vector
bool SceneGraphContainer::removeLight(GLenum light_id)
{
    light_count--;

    // We need at least one light
    if (light_count < 1)
    {
        light_count = 1;
        return false;
    }

    // Light id is now free, so push it onto vector
    free_light_ids.push(light_id);

    return true;
}

// Return the next free ligth id
GLenum SceneGraphContainer::addLight(void)
{
    light_count++;

    // We can't have more than 8 lights
    if (light_count > 8)
    {
        light_count = 8;
        return GL_INVALID_ENUM;
    }

    // Grab the next free light id
    GLenum light_id = free_light_ids.top();
    free_light_ids.pop();

    return light_id;
}

// Set the pointer to the current node
void SceneGraphContainer::setCurrentNode(Node * new_curr_node)
{
    curr_node = new_curr_node;
}

// Traverse the scene graph
void SceneGraphContainer::traverseGraph(void)
{
    root_node->traverseNode();
}
