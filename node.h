#ifndef __NODE_H__
#define __NODE_H__

#include <GL/glut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <vector>
#include <iostream>
#include "enums.h"
#include "geom.h"

class Node
{
protected:
    std::string node_type;
    Node * parent;
    std::vector<Node *> children_vec;
public:
    Node(void);
    Node(Node * parent, std::string node_type = "Root");
    ~Node(void);
    Node * getChild(int index = 0);
    Node * getParent(void);
    std::string getNodeType(void);
    std::vector<std::string> getChildNodeTypes(void);
    int getChildCount(void);
    void addChild(Node * child);
    void addParent(Node * new_parent);
    // void deleteChild(Node * child);
    virtual void traverseNode(glm::mat4 transform = glm::mat4(1.0));
};

Node::Node(void) : parent(NULL), node_type("Root") {}

Node::Node(Node * parent, std::string node_type)
{
    this->node_type = node_type;
    this->parent = NULL;

    if (parent != NULL)
        addParent(parent);
}

Node::~Node(void)
{
    // Delete all children
    int children_vec_size = children_vec.size();
    for (int i = 0; i < children_vec_size; i++)
    {
        // The child's deconstructor erases the child from this list, so we have to delete whatever is at the front
        Node * child = children_vec.front();
        delete child;
    }
    children_vec.clear();

    // Remove node from potential parent's list
    if (parent != NULL)
    {
        int parent_children_vec_size = parent->children_vec.size();
        for (int i = 0; parent_children_vec_size; i++)
        {
            if (parent->children_vec[i] == this)
            {
                // Erase from list
                parent->children_vec.erase(parent->children_vec.begin() + i);
                break;
            }
        }
        parent = NULL;
    }
}

Node * Node::getChild(int index)
{
    if (index < 0 || index >= children_vec.size())
        return NULL;
    else
        return children_vec[index];
}

Node * Node::getParent(void)
{
    return parent;
}

std::string Node::getNodeType(void)
{
    return node_type;
}

std::vector<std::string> Node::getChildNodeTypes(void)
{
    std::vector<std::string> children_node_type_vec;

    int children_vec_size = children_vec.size();
    for (int i = 0; i < children_vec_size; i++)
    {
        children_node_type_vec.push_back(children_vec[i]->node_type);
    }

    return children_node_type_vec;
}

int Node::getChildCount(void)
{
    return children_vec.size();
}

void Node::addChild(Node * child)
{
    child->addParent(this);
}

void Node::addParent(Node * new_parent)
{
    // We don't want breaks in the graph
    if (new_parent == NULL)
        return;

    // Inject  between current parent if one exists
    if (parent != NULL)
    {
        // Search for current node in old parent's list
        int parent_children_vec_size = parent->children_vec.size();
        for (int i = 0; i < parent_children_vec_size; i++)
        {
            if (parent->children_vec[i] == this)
            {
                // Set old parent's slot for node to that of the new parent
                parent->children_vec[i] = new_parent;
                // Set the new parent's parent to the old parent
                new_parent->parent = parent;
                break;
            }
        }
    }

    // Add to new parent's child list
    new_parent->children_vec.push_back(this);
    // Set as new parent
    parent = new_parent;
}

// void Node::deleteChild(Node * child)
// {
//     int children_vec_size = children_vec.size();
//     for (int i = 0; i < children_vec_size; i++)
//     {
//         if (children_vec[i] == child)
//         {
//             // Deconstructor takes care of deleting the element from the vector
//             delete child;
//             break;
//         }
//     }
// }

void Node::traverseNode(glm::mat4 transform)
{
    int children_vec_size = children_vec.size();
    for (int i = 0; i < children_vec_size; i++)
    {
        children_vec[i]->traverseNode(transform);
    }
}

class ObjectNode : public Node
{
public:
    ObjectNode(void) : Node(NULL, "Object") {}
    ObjectNode(Node * parent) : Node(parent, "Object") {}
    void traverseNode(glm::mat4 transform = glm::mat4(1.0));
};

void ObjectNode::traverseNode(glm::mat4 transform)
{
    glPushMatrix();

        Node::traverseNode(transform);

    glPopMatrix();
}

class GeometryNode : public Node
{
private:
    Trimesh model;
public:
    GeometryNode(void) : Node(NULL, "Geometry") {}
    GeometryNode(Node * parent) : Node(parent, "Geometry") {}
    void setMesh(Trimesh model);
    void traverseNode(glm::mat4 transform = glm::mat4(1.0));
};

void GeometryNode::setMesh(Trimesh new_model)
{
    model = new_model;
}

void GeometryNode::traverseNode(glm::mat4 transform)
{
    glMatrixMode(GL_MODELVIEW);

    vertex_t model_center = model.getCenter();

    glTranslatef(model_center.pos[0], model_center.pos[1], model_center.pos[2]);
    glMultMatrixf(glm::value_ptr(transform));
    glTranslatef(-model_center.pos[0], -model_center.pos[1], -model_center.pos[2]);

    model.render();
}

class TransformNode : public Node
{
private:
    TRANS_ID transformation_type;
    float xyz[3];
    float angle;

public:
    TransformNode(void) : Node(NULL, "Transform") {}
    TransformNode(Node * parent) : transformation_type(INVALID), Node(parent, "Transform") {}
    TRANS_ID getTransformType(void);
    void traverseNode(glm::mat4 transform = glm::mat4(1.0));
};

TRANS_ID TransformNode::getTransformType(void)
{
    return transformation_type;
}

void TransformNode::traverseNode(glm::mat4 transform)
{
    switch(transformation_type)
    {
        case SCALE:
        {
            glm::mat4 scale = glm::scale(glm::mat4(1.0), glm::vec3(xyz[0], xyz[1], xyz[2]));
            transform = scale * transform;
            break;
        }
        case TRANSLATE:
        {
            glm::mat4 translate = glm::translate(glm::mat4(1.0), glm::vec3(xyz[0], xyz[1], xyz[2]));
            transform = translate * transform;
            break;
        }
        case ROTATE:
        {
            glm::mat4 rotate = glm::rotate(glm::mat4(1.0), angle, glm::vec3(xyz[0], xyz[1], xyz[2]));
            transform = rotate * transform;
            break;
        }
    }

    Node::traverseNode(transform);
}

class AttributeNode : public Node
{
private:
    MODE_ID render_mode;
public:
    AttributeNode(void) : Node(NULL, "Attribute") {}
    AttributeNode(Node * parent) : Node(parent, "Attribute") {}
    void traverseNode(glm::mat4 transform = glm::mat4(1.0));
};

void AttributeNode::traverseNode(glm::mat4 transform)
{
    glPushAttrib(GL_POLYGON_BIT);

        switch(render_mode)
        {
            case MODE_POINTS:
            {
                // Point mode
                glDisable(GL_NORMALIZE);
                glDisable(GL_LIGHTING);
                glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
                break;
            }
            case MODE_WIREFRAME:
            {
                // Wireframe mode
                glDisable(GL_NORMALIZE);
                glDisable(GL_LIGHTING);
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                break;
            }
            case MODE_SOLID:
            {
                // Solid mode
                glDisable(GL_NORMALIZE);
                glDisable(GL_LIGHTING);
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                break;
            }
            case MODE_SHADED:
            {
                // Shaded mode
                glDisable(GL_NORMALIZE);
                glEnable(GL_LIGHTING);
                glEnable(GL_LIGHT0);
                glEnable(GL_COLOR_MATERIAL);
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                break;
            }
            case MODE_FACE_NORMS:
            {
                // Face normals mode
                glEnable(GL_NORMALIZE);
                glEnable(GL_LIGHTING);
                glEnable(GL_LIGHT0);
                glEnable(GL_COLOR_MATERIAL);
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                break;
            }
            case MODE_VERT_NORMS:
            {
                // Vertex normals mode
                glEnable(GL_NORMALIZE);
                glEnable(GL_LIGHTING);
                glEnable(GL_LIGHT0);
                glEnable(GL_COLOR_MATERIAL);
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                break;
            }
        }

        Node::traverseNode(transform);

    glPopAttrib();
}

class LightNode : public Node
{
private:
    LIGHT_ID type;
public:
    LightNode(void) : Node(NULL, "Light") {}
    LightNode(Node * parent) : Node(parent, "Light") {}
    void traverseNode(glm::mat4 transform = glm::mat4(1.0));
};

void LightNode::traverseNode(glm::mat4 transform)
{
    // Have lights always start off at origin
    glm::vec4 pos(0.0, 0.0, 0.0, 1.0);
    // Have transformation matrix define where it should be placed
    pos = transform * pos;

    // // Set light
    // glEnable(GL_LIGHTING);
}

class CameraNode : public Node
{
public:
    CameraNode(void) : Node(NULL, "Camera") {}
    CameraNode(Node * parent) : Node(parent, "Camera") {}
    void traverseNode(glm::mat4 transform = glm::mat4(1.0));
};

void CameraNode::traverseNode(glm::mat4 transform)
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    gluLookAt(  0.0, 0.0, -1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0 );

    glMultMatrixf(glm::value_ptr(transform));
}

#endif