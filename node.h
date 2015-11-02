#ifndef __NODE_H__
#define __NODE_H__

#include <GL/glut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include "enums.h"
#include "geom.h"

class Node
{
protected:
    Node * parent;
    std::vector<Node *> children_vec;
public:
    Node(void);
    Node(Node * parent);
    ~Node(void);
    void addChild(Node * child);
    void addParent(Node * new_parent);
    void deleteChild(Node * child);
    virtual void traverseNode(void);
};

Node::Node(void) : parent(NULL) {}

Node::Node(Node * parent) : parent(parent) {}

Node::~Node(void)
{
    // Delete all children
    int children_vec_size = children_vec.size();
    for (int i = 0; i < children_vec_size; i++)
    {
        delete children_vec[i];
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
                parent->children_vec.erase(children_vec.begin() + i);
                break;
            }
        }
        parent = NULL;
    }
}

void Node::addChild(Node * child)
{
    child->addParent(this);
}

void Node::addParent(Node * new_parent)
{
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

void Node::deleteChild(Node * child)
{
    int children_vec_size = children_vec.size();
    for (int i = 0; i < children_vec_size; i++)
    {
        if (children_vec[i] == child)
        {
            // Deconstructor takes care of deleting the element from the vector
            delete child;
            break;
        }
    }
}

void Node::traverseNode(void)
{
    int children_vec_size = children_vec.size();
    for (int i = 0; i < children_vec_size; i++)
    {
        children_vec[i]->traverseNode();
    }
}

class ObjectNode : public Node
{
public:
    void traverseNode(void);
};

void ObjectNode::traverseNode(void)
{
    glPushMatrix();

        Node::traverseNode();

    glPopMatrix();
}

class GeometryNode : public Node
{
private:
    Trimesh * mesh;
public:
    GeometryNode(void);
    void setMesh(Trimesh * mesh);
    void traverseNode(void);
};

GeometryNode::GeometryNode(void) : Node(NULL), mesh(NULL) {}

void GeometryNode::setMesh(Trimesh * new_mesh)
{
    if (mesh != NULL)
        delete mesh;

    mesh = new_mesh;
}

void GeometryNode::traverseNode(void)
{
    mesh->render();
}

class TransformNode : public Node
{
private:
    TRANS_ID type;
    float xyz[3];
    float angle;

public:
    void traverseNode(void);
};

void TransformNode::traverseNode(void)
{
    switch(type)
    {
        case SCALE:
        {
            glScalef(xyz[0], xyz[1], xyz[2]);
            break;
        }
        case TRANSLATE:
        {
            glTranslatef(xyz[0], xyz[1], xyz[2]);
            break;
        }
        case ROTATE:
        {
            glRotatef(angle, xyz[0], xyz[1], xyz[2]);
            break;
        }
    }

    Node::traverseNode();
}

class AttributeNode : public Node
{
private:
    MODE_ID render_mode;
public:
    void traverseNode(void);
};

void AttributeNode::traverseNode(void)
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

        Node::traverseNode();

    glPopAttrib();
}

class LightNode : public Node
{
private:
    LIGHT_ID type;
    float pos[3];
public:
    void traverseNode(void);
};

void LightNode::traverseNode(void)
{
    
}

class CameraNode : public Node
{

};

#endif