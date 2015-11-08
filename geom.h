/*
* Assignment #4
* Name: Jason Palacios
* UT EID: jap4839
* UTCS: jason777
*/

#ifndef __GEOM_H__
#define __GEOM_H__

#include <GL/glut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <string>

/// Vertex
struct vertex_t
{
    glm::vec3 pos;
    glm::vec3 norm;
    std::vector<int> tri_ids;

    void setPos(float pos[]);
    void addTriId(int tri_id);
    vertex_t& operator=(const vertex_t& src);
};

/// Triangle
struct triangle_t
{
    int vert_ids[3];
    glm::vec3 center;
    glm::vec3 norm;

    void setVertIds(int vert_ids[]);
    triangle_t& operator=(const triangle_t& src);
};

/// Trimesh
class Trimesh
{
private:
    bool _changed;
    std::vector<vertex_t> _vertices;
    std::vector<triangle_t> _triangles;
    GLfloat _bounds[3][2];
    glm::vec3 _center;

    void _constructObject(void);
    void _calculateFaceNormals(void);
    void _calculateVertNormals(void);
    void _calculateNormals(void);
    void _calculateCenter(void);

public:
    Trimesh(void);
    GLfloat getBoundingLength(void);
    glm::vec3 getCenter(void);
    void addVertex(float pos[]);
    void addFace(int vert_ids[]);
    void render(std::string render_type = "Solid");
    Trimesh& operator=(const Trimesh& src);
};

#endif
