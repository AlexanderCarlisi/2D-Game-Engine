#include <stdint.h>
#include <stdbool.h>

#ifndef SHAPE_H
#define SHAPE_H

#include "pose.h"
#include "vector.h"

#ifndef SHAPE_PI
#define SHAPE_PI 3.14159265358979323846
#endif // SHAPE_PI

// A normalized magnitude, representing the Vertex of a Shape. 
// (x, y) : [0, 1]
// Bottom Left = (0, 0)
// Top Right = (1, 1)
// Layout: [float, float]
// Size: 8
typedef struct Vertex {
    float x;
    float y;
} Vertex;

DECLARE_VECTOR(VertexVector, vector_vertex, struct Vertex)
DECLARE_VECTOR(PoseVector, vector_pose, struct Pose)

// Shape object, responsible for generating and storing 2D Shapes.
// The data here is used for both Rendering and Physics.
// This Shape structure is made to be malliable, and allow for a lot of freedom.
// Do note that having a lot of verticies is performance intensive, and its not suggested to run isConvex too much
// if possible know if the shape is convex or not, or just simplify the collisions to always be or not be convex
// 
// A dynamic array of Verticies. Normalized between [0, 1], (0, 0) being top left.
// This way the size of your Object doesn't matter, and can be transposed to whatever Size freely.
//
// @var normalized_vertices defines the shape. With a literal line being drawn from each point to the next.
//  The vertices don't need to be sorted in any specific order, since its however you want your shape to be defined as.
//  The last vertex in the vector of vertices will automatically be drawn to the [0]th index vertex.
//
// Shapes need to be deallocated.
//
// Layout: [Vector, Pose, uint32, bool]
// Size: 85(88), 53(56)
typedef struct Shape {
    struct VertexVector normalized_vertices;    // Dynamic Array of Normalized Verticies. This defines the shape.
    struct PoseVector vertex_poses;             // Dynamic Array of Vertex Positions. This is the vertices position in world. Don't manipulate
    struct Pose size;                           // Size of Shape, used to Transpose the Normalized Verticies.
    uint32_t color;                             // *Until Textures become a thing.
    bool convex;                                // Whether a Shape is Convex or Concave.
} Shape;

// Uses the Graham Scan algorithm to determine if the provided set of Vertices forms a convex shape.
// This engine allows users to have a lot of freedom with what
// shapes they want to use, but Convex Shapes vs. Concave Shapes have more 
// efficient algorithms, instead of running an algorithm to find if a shape
// is convex or concave, users will either need to run this to figure it out, or specify so when creating the shape.
// TODO: This.
bool shape_is_convex(struct VertexVector vertices);

/// @brief Initializes the provided Shape.
/// @param shape Ptr to Shape.
/// @param size Desired dimensions of Shape.
/// @param color To display the shape as in the Renderer.
/// @param convex Wether the shape is Convex or Concave.
/// @param vertices Dynamic Array of Normalized Vertices to define the outline of the Shape.
void shape_init(struct Shape* shape, size_t vertices);

/// @brief Initializes a new Shape, that's vertices are of a Square.
/// @param size To manipulate the width and height of the Box.
/// @param color To display the shape as in the Renderer.
struct Shape shape_create_box(struct Pose size, uint32_t color);

/// @brief Add a new Vertex to the normalized_vertices dyn arr.
/// Updates the vertex_poses count properly, which is why this func is preferred.
/// @param shape Shape to add to.
/// @param vertex Vertex to deepcopy into vertex_poses.
bool shape_add_vertex(struct Shape* shape, struct Vertex vertex);

/// @brief Create the vertices for a convex shape, adding them to the current shape.
void shape_add_convex_vertices(struct Shape* shape, size_t vertices);

/// @brief Deallocate provided shape, and its members
void shape_free(struct Shape* shape);

/// @brief Prints to term debug info on Shape.
void shape_health_check(struct Shape* shape);

#endif // SHAPE_H
