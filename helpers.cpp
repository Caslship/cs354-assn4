/*
* Assignment #4
* Name: Jason Palacios
* UT EID: jap4839
* UTCS: jason777
*/

#include "helpers.h"

#include <GL/glut.h>
#include <GL/glui.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <string>
#include "enums.h"
#include "callbacks.h"
#include "geom.h"
#include "scenegraph.h"
#include "node.h"
#include "cameranode.h"
#include "lightnode.h"
#include "geometrynode.h"
#include "objectnode.h"
#include "transformnode.h"
#include "attributenode.h"

using namespace std;

/// Perspective variables
extern int g_height;
extern int g_width;

/// Window handle
extern int main_window;

// Initalize GLUT and window properties
void InitializeWindow(int& argc, char ** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(g_width, g_height);
    main_window = glutCreateWindow("Jason Palacios - Scene Graph Viewer");
}

// Initial settings for OpenGL
void InitializeGraphics(void)
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);
    glDepthFunc(GL_LESS);
    glShadeModel(GL_SMOOTH);
}

// Register all callbacks to be used with GLUT
void RegisterCallbacks(void)
{
    glutDisplayFunc(Display);
    glutReshapeFunc(Reshape);  
    glutKeyboardFunc(Keyboard);
    glutMouseFunc(MouseButton);
    glutMotionFunc(MouseMotion);
}

/// GUI (widgets)
extern GLUI * glui;
extern GLUI_Panel * scene_graph_panel;
extern GLUI_Rollout * select_node_panel;
extern GLUI_Listbox * child_node_select;
extern GLUI_Button * select_child_node;
extern GLUI_Button * select_parent_node;
extern GLUI_Rollout * add_node_panel;
extern GLUI_Listbox * node_type_select;
extern GLUI_Button * add_child_node;
extern GLUI_Button * add_parent_node;
extern GLUI_Panel * curr_node_panel;
extern GLUI_Rollout * attr_node_panel;
extern GLUI_Listbox * render_type_select;
extern GLUI_Rollout * geom_node_panel;
extern GLUI_EditText * geom_path;
extern GLUI_Rollout * transform_node_panel;
extern GLUI_Listbox * transform_type_select;
extern GLUI_EditText * x_param;
extern GLUI_EditText * y_param;
extern GLUI_EditText * z_param;
extern GLUI_EditText * theta_param;
extern GLUI_Checkbox * animation_param;
extern GLUI_Rollout * camera_node_panel;
extern GLUI_EditText * fov_param;
extern GLUI_EditText * vx_param;
extern GLUI_EditText * vy_param;
extern GLUI_Rollout * light_node_panel;
extern GLUI_Listbox * light_type_select;
extern GLUI_Button * update_node;
extern GLUI_Button * delete_node;

/// GUI (live variables)
extern int child_node_index;
extern int node_type_index;
extern int render_type_index;
extern int transform_type_index;
extern int light_type_index;

/// GUI (listbox and other constant contents)
extern string render_type_list[];
extern string node_type_list[];
extern string transform_type_list[];
extern string light_type_list[];

void InitializeGUI(void)
{
    glui = GLUI_Master.create_glui("GLUI");

    // Scene graph panel
    scene_graph_panel = glui->add_panel("Edit Scene Graph");

        // Node selection panel
        select_node_panel = glui->add_rollout_to_panel(scene_graph_panel, "Node Selection", true);
            
            // Child selection listbox
            child_node_select = glui->add_listbox_to_panel(select_node_panel, "Child ", &child_node_index);

            glui->add_separator_to_panel(select_node_panel);

            // Node selection buttons
            select_child_node = glui->add_button_to_panel(select_node_panel, "Select Child", CHILD_NODE_SELECT_B_ID, Control);
            select_parent_node = glui->add_button_to_panel(select_node_panel, "Select Parent", PARENT_NODE_SELECT_B_ID, Control);

        // Node addition panel
        add_node_panel = glui->add_rollout_to_panel(scene_graph_panel, "Node Addition", true);
            
            // Node type selection listbox
            node_type_select = glui->add_listbox_to_panel(add_node_panel, "Type ", &node_type_index, NODE_TYPE_LB_ID, Control);
            for (int i = 0; i < 5; i++)
                node_type_select->add_item(i, node_type_list[i].c_str());

            glui->add_separator_to_panel(add_node_panel);

            // Node addition buttons
            add_child_node = glui->add_button_to_panel(add_node_panel, "Add Child", CHILD_NODE_ADD_B_ID, Control);
            add_parent_node = glui->add_button_to_panel(add_node_panel, "Add Parent", PARENT_NODE_ADD_B_ID, Control);

    scene_graph_panel->set_alignment(GLUI_ALIGN_LEFT);

    // Current node panel
    curr_node_panel = glui->add_panel("Edit Current Node");

        // Attribute panel
        attr_node_panel = glui->add_rollout_to_panel(curr_node_panel, "Attribute", false);
            
            // Render mode selection listbox
            render_type_select = glui->add_listbox_to_panel(attr_node_panel, "Render ", &render_type_index);
            for(int i = 0; i < 6; i++ )
                render_type_select->add_item(i, render_type_list[i].c_str());

        // Geometry panel
        geom_node_panel = glui->add_rollout_to_panel(curr_node_panel, "Geometry", false);
            
            // Obj filepath textbox
            geom_path = glui->add_edittext_to_panel(geom_node_panel, "Obj", GLUI_EDITTEXT_TEXT);

        // Transform panel
        transform_node_panel = glui->add_rollout_to_panel(curr_node_panel, "Transformation", false);

            // Transform type selection listbox
            transform_type_select = glui->add_listbox_to_panel(transform_node_panel, "Type ", &transform_type_index, TRANSFORM_TYPE_LB_ID, Control);
            for(int i = 0; i < 3; i++)
                transform_type_select->add_item(i, transform_type_list[i].c_str());
            transform_type_select->set_alignment(GLUI_ALIGN_LEFT);

            // Animation flag checkbox
            animation_param = glui->add_checkbox_to_panel(transform_node_panel, "Animation");

            glui->add_separator_to_panel(transform_node_panel);

            // Transformation parameter textboxes
            x_param = glui->add_edittext_to_panel(transform_node_panel, "X", GLUI_EDITTEXT_FLOAT);
            y_param = glui->add_edittext_to_panel(transform_node_panel, "Y", GLUI_EDITTEXT_FLOAT);
            z_param = glui->add_edittext_to_panel(transform_node_panel, "Z", GLUI_EDITTEXT_FLOAT);
            theta_param = glui->add_edittext_to_panel(transform_node_panel, "Theta", GLUI_EDITTEXT_FLOAT);

        // Camera panel
        camera_node_panel = glui->add_rollout_to_panel(curr_node_panel, "Camera", false);

            // Field-of-View paramater textbox
            fov_param = glui->add_edittext_to_panel(camera_node_panel, "FOV", GLUI_EDITTEXT_FLOAT);

            // Viewport parameter textboxes
            vx_param = glui->add_edittext_to_panel(camera_node_panel, "Viewport X", GLUI_EDITTEXT_INT);
            vy_param = glui->add_edittext_to_panel(camera_node_panel, "Viewport Y", GLUI_EDITTEXT_INT);

        // Light panel
        light_node_panel = glui->add_rollout_to_panel(curr_node_panel, "Light", false);

            // Light type selection listbox
            light_type_select = glui->add_listbox_to_panel(light_node_panel, "Type ", &light_type_index);
            for (int i = 0; i < 2; i++)
                light_type_select->add_item(i, light_type_list[i].c_str());

        // Apply node modification buttons
        update_node = glui->add_button_to_panel(curr_node_panel, "Update", CURR_NODE_UPDATE_B_ID, Control);
        delete_node = glui->add_button_to_panel(curr_node_panel, "Delete", CURR_NODE_DELETE_B_ID, Control);

    curr_node_panel->set_alignment(GLUI_ALIGN_LEFT);

    // Update GUI according to current node
    UpdateGUI(0);

    // Link windows to GLUI, and register idle callback
    glui->set_main_gfx_window(main_window);
    GLUI_Master.set_glutIdleFunc(Idle);
}

/// Scene graph
extern SceneGraphContainer scenegraph;

void UpdateGUI(int old_children_vec_size)
{
    // Start certain components off disabled and then enable ones that are appropriate given program state
    child_node_select->disable();
    select_child_node->disable();
    select_parent_node->disable();

    add_child_node->disable();
    add_parent_node->disable();

    attr_node_panel->disable();
    attr_node_panel->close();

    geom_node_panel->disable();
    geom_node_panel->close();

    transform_node_panel->disable();
    transform_node_panel->close();

    camera_node_panel->disable();
    camera_node_panel->close();

    light_node_panel->disable();
    light_node_panel->close();

    update_node->disable();
    delete_node->disable();

    Node * root_node = scenegraph.getRootNode();
    Node * curr_node = scenegraph.getCurrentNode();
    string curr_node_type = curr_node->getNodeType();
    vector<string> children_node_type_vec = curr_node->getChildNodeTypes();
    int children_vec_size = curr_node->getChildCount();
    int light_count = scenegraph.getLightCount();

    bool has_children = (children_vec_size > 0);
    bool is_root = (curr_node == root_node);
    bool is_geom_type = (curr_node_type == "Geometry");
    bool is_transform_type = (curr_node_type == "Transform");
    bool is_attr_type = (curr_node_type == "Attribute");
    bool is_camera_type = (curr_node_type == "Camera");
    bool is_light_type = (curr_node_type == "Light");
    bool camera_node_selected = (node_type_list[node_type_index] == "Camera");
    bool geom_node_selected = (node_type_list[node_type_index] == "Geometry");
    bool light_node_selected = (node_type_list[node_type_index] == "Light");

    for (int i = 0; i < old_children_vec_size; i++)
        child_node_select->delete_item(i);

    if (has_children)
    {
        for (int i = 0; i < children_vec_size; i++)
            child_node_select->add_item(i, children_node_type_vec[i].c_str());

        child_node_select->enable();
        select_child_node->enable();

        child_node_index = child_node_select->get_item_ptr(
            children_node_type_vec[0].c_str()
            )->id;

        child_node_select->do_selection(child_node_index);
    }

    if (!is_root)
        select_parent_node->enable();

    if (!is_root && !camera_node_selected && !light_node_selected && !geom_node_selected)
        add_parent_node->enable();

    if (!is_camera_type && !is_geom_type && !is_light_type && !(light_node_selected && light_count == 8))
        add_child_node->enable();

    if (is_attr_type)
    {
        attr_node_panel->enable();
        attr_node_panel->open();

        render_type_select->enable();

        render_type_index = render_type_select->get_item_ptr(
            (((AttributeNode *)curr_node)->getRenderType()).c_str()
            )->id;

        render_type_select->do_selection(render_type_index);
    }
    else if (is_geom_type)
    {
        geom_node_panel->enable();
        geom_node_panel->open();

        geom_path->enable();

        geom_path->set_text(((GeometryNode *)curr_node)->getFilePath().c_str());
    }
    else if (is_transform_type)
    {
        transform_node_panel->enable();
        transform_node_panel->open();

        transform_type_select->enable();
        animation_param->enable();
        x_param->enable();
        y_param->enable();
        z_param->enable();
        theta_param->enable();

        transform_type_index = transform_type_select->get_item_ptr(
            (((TransformNode *)curr_node)->getTransformType()).c_str()
            )->id;

        transform_type_select->do_selection(transform_type_index);

        if (transform_type_list[transform_type_index] != "Rotate" && ((TransformNode *)curr_node)->getTransformType() != "Rotate")
            theta_param->disable();

        animation_param->set_int_val(((TransformNode *)curr_node)->getAnimationFlag());
        x_param->set_float_val(((TransformNode *)curr_node)->getX());
        y_param->set_float_val(((TransformNode *)curr_node)->getY());
        z_param->set_float_val(((TransformNode *)curr_node)->getZ());
        theta_param->set_float_val(((TransformNode *)curr_node)->getTheta());
    }
    else if (is_camera_type)
    {
        camera_node_panel->enable();
        camera_node_panel->open();

        fov_param->enable();
        vx_param->enable();
        vy_param->enable();

        fov_param->set_float_val(((CameraNode *)curr_node)->getFOV());
        vx_param->set_int_val(((CameraNode *)curr_node)->getViewportX());
        vy_param->set_int_val(((CameraNode *)curr_node)->getViewportY());
    }
    else if (is_light_type)
    {
        light_node_panel->enable();
        light_node_panel->open();

        light_type_select->enable();

        light_type_index = light_type_select->get_item_ptr(
            (((LightNode *)curr_node)->getLightType()).c_str()
            )->id;

        light_type_select->do_selection(light_type_index);
    }

    if (!is_camera_type && !is_root && !(is_light_type && light_count == 1))
        delete_node->enable();

    if (!is_root)
        update_node->enable();

    GLUI_Master.sync_live_all();
}

void UpdateCameraGivenMesh(Trimesh model)
{
    CameraNode * camera_node = scenegraph.getCameraNode();

    // Get new look at point
    glm::vec3 model_center = model.getCenter();

    // Get an appropriate starting orbit radius
    GLfloat orbit_radius = model.getBoundingLength() * 1.5;
    orbit_radius = (orbit_radius < VIEWING_DISTANCE_MIN ? VIEWING_DISTANCE_MIN : orbit_radius);

    // Update camera
    camera_node->updateCameraGivenParams(model_center, orbit_radius); 
}