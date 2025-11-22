#include "shape.h"
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include "logger.h"

DEFINE_VECTOR(VertexVector, vector_vertex, struct Vertex);
DEFINE_VECTOR(PoseVector, vector_pose, struct Pose);

/// TODO:
bool shape_is_convex(struct VertexVector vertices) {
    return false;
}

void shape_init(struct Shape* shape, size_t vertices) {
    vector_vertex_init(&shape->normalized_vertices, vertices);
    vector_pose_init(&shape->vertex_poses, vertices);
}

bool shape_add_vertex(struct Shape* shape, struct Vertex vertex) {
    if (shape == NULL) return false;

    vector_vertex_add(&shape->normalized_vertices, vertex);
    vector_pose_add(&shape->vertex_poses, (struct Pose) {0});
    return true;
}

void shape_add_convex_vertices(struct Shape* shape, size_t vertices) {
    for (size_t i = 1; i < vertices + 1; i++) {
        float theta = (2.0f * SHAPE_PI * i) / vertices;
        float x = cos(theta);
        float y = sin(theta);
        shape_add_vertex(shape, (struct Vertex) {x, y});
        // printf("%f, %f \n", x, y);
    }
}

struct Shape shape_create_box(struct Pose size, uint32_t color) {
    struct Shape shape;
    shape.size = size;
    shape.color = color;
    shape_init(&shape, 4);
    shape_add_convex_vertices(&shape, 4);
    
    return shape;
}

void shape_free(struct Shape* shape) {
    vector_vertex_free(&shape->normalized_vertices);
    vector_pose_free(&shape->vertex_poses);
}

void shape_health_check(struct Shape* shape) {
    if (shape == NULL) {
        logger_write(3, 4, "shape_health: obj is NULL", true);
        return;
    }
    
    struct Pose p = {0};
    if (pose_equals(&shape->size, &p)) {
        logger_write(3, 4, "shape_health: Size is zero", true);
    }

    if (shape->vertex_poses.size == 0) {
        logger_write(3, 4, "shape_health: vertex poses uninitialized", true);
    }

    if (shape->normalized_vertices.size == 0) {
        logger_write(3, 4, "shape_health: normalized vertices uninitialized", true);
    }

    // printf("\n<DEBUG>\t Shape vertices_normalized: %zu", shape->normalized_vertices.count);
    // printf("\n<DEBUG>\t Shape vertex poses: %zu", shape->vertex_poses.count);

    // for (size_t i = 0; i < shape->normalized_vertices.count; i++) {
    //     printf("\n<DEBUG>\t normalized_vertices (%zu) : {%f, %f}", i, shape->normalized_vertices.data[i].x, shape->normalized_vertices.data[i].y);
    // }
    
    // for (size_t i = 0; i < shape->vertex_poses.count; i++) {
    //     printf("\n<DEBUG>\t normalized_vertices (%zu) : {%f, %f}", i, shape->normalized_vertices.data[i].x, shape->normalized_vertices.data[i].y);
    // }
}
