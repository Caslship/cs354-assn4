#ifndef __NODE_H__
#define __NODE_H__

#include <GL/glut.h>
#include <vector>
#include "enums.h"
#include "geom.h"

class Node
{
protected:
    std::vector<Node *> children_vec;
public:
    void addChild(Node *);
    virtual void traverse(void);
};

void Node::addChild(Node * child)
{
    children_vec.push_back(child);
}

void Node::traverse(void)
{
    int children_vec_size = children_vec.size();
    for (int i = 0; i < children_vec_size; i++)
    {
        children_vec[i]->traverse();
    }
}

class ObjectNode : public Node
{
public:
    void traverse(void);
};

void ObjectNode::traverse(void)
{
    glPushMatrix();

        Node::traverse();

    glPopMatrix();
}

class GeometryNode : public Node
{
private:
    Trimesh * mesh;
public:
    void traverse(void);
};

void GeomtryNode::traverse(void)
{
    mesh->render();
}

class TransformNode : public Node
{
private:
    TRANS_ID type;
    float xyz[3];
    float xyz_inc[3];
    float angle;
    float angle_inc;

public:
    void traverse(void);
};

void TransformNode::traverse(void)
{
    switch(type)
    {
        case SCALE:
        {
            glScalef(it->xyz[0], it->xyz[1], it->xyz[2]);
            break;
        }
        case TRANSLATE:
        {
            glTranslatef(it->xyz[0], it->xyz[1], it->xyz[2]);
            break;
        }
        case ROTATE:
        {
            glRotatef(it->angle, it->xyz[0], it->xyz[1], it->xyz[2]);
            break;
        }
    }

    Node::traverse();
}

class AttributeNode : public Node
{
private:
    MODE_ID render_mode;
public:
    void traverse(void);
};

void AttributeNode::traverse(void)
{
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

    Node::traverse();
}

class LightNode : public Node
{
private:
    LIGHT_ID type;
    float pos[3];
public:
    void traverse(void);
};

void LightNode::traverse(void)
{
    
}

class CameraNode : public Node
{

};

#endif