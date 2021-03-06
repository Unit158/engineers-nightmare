#pragma once

#include <unordered_map>
#include <string>
#include <array>

#include "mesh.h"
#include "block.h"
#include "textureset.h"

class asset_manager
{
    std::unordered_map<std::string, mesh_data> meshes;
    std::array<std::string, face_count> surface_index_to_mesh_name;

    std::unordered_map<std::string, unsigned> texture_to_index;

    texture_set *world_textures{nullptr};
    texture_set *skybox{nullptr};

public:
    asset_manager();
    ~asset_manager() = default;

    void load_meshes();

    void load_textures();

    unsigned get_texture_index(const std::string &) const;
    mesh_data & get_mesh(const std::string &);
    mesh_data & get_surface_mesh(unsigned);

    void bind_world_textures(int i);

    void bind_skybox(int i);
};
