#include "mesh-utils.hpp"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tinyobj/tiny_obj_loader.h>

#include <iostream>
#include <vector>
#include <unordered_map>
#include <filesystem>

#include "common-vertex-types.hpp"
#include "common-vertex-attributes.hpp"

#define WHITE   our::Color(255, 255, 255, 255)
#define GRAY    our::Color(128, 128, 128, 255)
#define BLACK   our::Color(  0,   0,   0, 255)
#define RED     our::Color(255,   0,   0, 255)
#define GREEN   our::Color(  0, 255,   0, 255)
#define BLUE    our::Color(  0,   0, 255, 255)
#define MAGENTA our::Color(255,   0, 255, 255)
#define YELLOW  our::Color(255, 255,   0, 255)
#define CYAN    our::Color(  0, 255, 255, 255)

void our::mesh_utils::loadOBJ(our::Mesh &mesh, const char* filename) {

    auto parent_path_string = std::filesystem::path(filename).parent_path().string();


    std::vector<our::Vertex> vertices;
    std::vector<GLuint> elements;
    std::unordered_map<our::Vertex, GLuint> vertex_map;

    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filename, parent_path_string.c_str())) {
        throw std::runtime_error(err);
    }
    if (!warn.empty()) {
        std::cout << "WARN while loading obj file \"" << filename << "\": " << warn << std::endl;
    }

    for (const auto &shape : shapes) {
        for (const auto &index : shape.mesh.indices) {
            Vertex vertex = {};

            vertex.position = {
                    attrib.vertices[3 * index.vertex_index + 0],
                    attrib.vertices[3 * index.vertex_index + 1],
                    attrib.vertices[3 * index.vertex_index + 2]
            };

            vertex.normal = {
                    attrib.normals[3 * index.normal_index + 0],
                    attrib.normals[3 * index.normal_index + 1],
                    attrib.normals[3 * index.normal_index + 2]
            };

            vertex.tex_coord = {
                    attrib.texcoords[2 * index.texcoord_index + 0],
                    attrib.texcoords[2 * index.texcoord_index + 1]
            };


            vertex.color = {
                    attrib.colors[3 * index.vertex_index + 0] * 255,
                    attrib.colors[3 * index.vertex_index + 1] * 255,
                    attrib.colors[3 * index.vertex_index + 2] * 255,
                    255
            };

            auto it = vertex_map.find(vertex);
            if (it == vertex_map.end()) {
                auto new_vertex_index = static_cast<GLuint>(vertices.size());
                vertex_map[vertex] = new_vertex_index;
                elements.push_back(new_vertex_index);
                vertices.push_back(vertex);
            } else {
                elements.push_back(it->second);
            }
        }
    }

    if (mesh.isCreated()) mesh.destroy();
    mesh.create({our::setup_buffer_accessors<Vertex>});
    mesh.setVertexData(0, vertices);
    mesh.setElementData(elements);
}


void our::mesh_utils::Cuboid(Mesh& mesh,
            bool colored_faces,
            const glm::vec3& center,
            const glm::vec3& size,
            const glm::vec2& texture_offset,
            const glm::vec2& texture_tiling){

    glm::vec3 half_size = size * 0.5f;
    glm::vec3 bounds[] = {center - half_size, center + half_size};
    glm::vec3 corners[] = {
            {bounds[0].x, bounds[0].y, bounds[0].z},
            {bounds[0].x, bounds[0].y, bounds[1].z},
            {bounds[0].x, bounds[1].y, bounds[0].z},
            {bounds[0].x, bounds[1].y, bounds[1].z},
            {bounds[1].x, bounds[0].y, bounds[0].z},
            {bounds[1].x, bounds[0].y, bounds[1].z},
            {bounds[1].x, bounds[1].y, bounds[0].z},
            {bounds[1].x, bounds[1].y, bounds[1].z}
    };
    glm::vec2 tex_coords[] = {
            texture_offset,
            texture_offset + glm::vec2(0, texture_tiling.y),
            texture_offset + glm::vec2(texture_tiling.x, 0),
            texture_offset + texture_tiling
    };
    glm::vec3 normals[3][2] = {
            {{-1, 0, 0}, {1,0,0}},
            {{ 0,-1, 0}, {0,1,0}},
            {{ 0, 0,-1}, {0,0,1}}
    };

    std::vector<Vertex> vertices = {
            //Upper Face
            {corners[2], colored_faces ? GREEN : WHITE , tex_coords[0], normals[1][1]},
            {corners[3], colored_faces ? GREEN : WHITE, tex_coords[2], normals[1][1]},
            {corners[7], colored_faces ? GREEN : WHITE, tex_coords[3], normals[1][1]},
            {corners[6], colored_faces ? GREEN : WHITE, tex_coords[1], normals[1][1]},
            //Lower Face
            {corners[0], colored_faces ? MAGENTA : WHITE, tex_coords[0], normals[1][0]},
            {corners[4], colored_faces ? MAGENTA : WHITE, tex_coords[2], normals[1][0]},
            {corners[5], colored_faces ? MAGENTA : WHITE, tex_coords[3], normals[1][0]},
            {corners[1], colored_faces ? MAGENTA : WHITE, tex_coords[1], normals[1][0]},
            //Right Face
            {corners[4], colored_faces ? RED : WHITE, tex_coords[0], normals[0][1]},
            {corners[6], colored_faces ? RED : WHITE, tex_coords[2], normals[0][1]},
            {corners[7], colored_faces ? RED : WHITE, tex_coords[3], normals[0][1]},
            {corners[5], colored_faces ? RED : WHITE, tex_coords[1], normals[0][1]},
            //Left Face
            {corners[0], colored_faces ? CYAN : WHITE, tex_coords[0], normals[0][0]},
            {corners[1], colored_faces ? CYAN : WHITE, tex_coords[2], normals[0][0]},
            {corners[3], colored_faces ? CYAN : WHITE, tex_coords[3], normals[0][0]},
            {corners[2], colored_faces ? CYAN : WHITE, tex_coords[1], normals[0][0]},
            //Front Face
            {corners[1], colored_faces ? BLUE : WHITE, tex_coords[0], normals[2][1]},
            {corners[5], colored_faces ? BLUE : WHITE, tex_coords[2], normals[2][1]},
            {corners[7], colored_faces ? BLUE : WHITE, tex_coords[3], normals[2][1]},
            {corners[3], colored_faces ? BLUE : WHITE, tex_coords[1], normals[2][1]},
            //Back Face
            {corners[0], colored_faces ? YELLOW : WHITE, tex_coords[0], normals[2][0]},
            {corners[2], colored_faces ? YELLOW : WHITE, tex_coords[2], normals[2][0]},
            {corners[6], colored_faces ? YELLOW : WHITE, tex_coords[3], normals[2][0]},
            {corners[4], colored_faces ? YELLOW : WHITE, tex_coords[1], normals[2][0]},
    };
    std::vector<GLuint> elements = {
            //Upper Face
            0, 1, 2, 2, 3, 0,
            //Lower Face
            4, 5, 6, 6, 7, 4,
            //Right Face
            8, 9, 10, 10, 11, 8,
            //Left Face
            12, 13, 14, 14, 15, 12,
            //Front Face
            16, 17, 18, 18, 19, 16,
            //Back Face
            20, 21, 22, 22, 23, 20,
    };

    mesh.create({our::setup_buffer_accessors<Vertex>});
    mesh.setVertexData(0, vertices);
    mesh.setElementData(elements);
};

void our::mesh_utils::Sphere(our::Mesh& mesh, const glm::ivec2& segments, bool colored,
            const glm::vec3& center, float radius,
            const glm::vec2& texture_offset, const glm::vec2& texture_tiling){

    std::vector<our::Vertex> vertices;
    std::vector<GLuint> elements;

    for(int lat = 0; lat <= segments.y; lat++){
        float v = (float)lat / segments.y;
        float pitch = v * glm::pi<float>() - glm::half_pi<float>();
        float cos = glm::cos(pitch), sin = glm::sin(pitch);
        for(int lng = 0; lng <= segments.x; lng++){
            float u = (float)lng/segments.x;
            float yaw = u * glm::two_pi<float>();
            glm::vec3 normal = {cos * glm::cos(yaw), sin, cos * glm::sin(yaw)};
            glm::vec3 position = radius * normal + center;
            glm::vec2 tex_coords = texture_tiling * glm::vec2(u, v) + texture_offset;
            our::Color color = colored ? our::Color(127.5f * (normal + 1.0f), 255) : WHITE;
            vertices.push_back({position, color, tex_coords, normal});
        }
    }

    for(int lat = 1; lat <= segments.y; lat++){
        int start = lat*(segments.x+1);
        for(int lng = 1; lng <= segments.x; lng++){
            int prev_lng = lng-1;
            elements.push_back(lng + start);
            elements.push_back(lng + start - segments.x - 1);
            elements.push_back(prev_lng + start - segments.x - 1);
            elements.push_back(prev_lng + start - segments.x - 1);
            elements.push_back(prev_lng + start);
            elements.push_back(lng + start);
        }
    }

    mesh.create({our::setup_buffer_accessors<Vertex>});
    mesh.setVertexData(0, vertices);
    mesh.setElementData(elements);
}
