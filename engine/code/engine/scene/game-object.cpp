#include "game-object.h"

#include  "components/animation-component.h"
#include  "components/mesh-component.h"

#include "../engine.h"
#include "../graphics/vertex-layout.h"
#include "../render/mesh.h"
#include "../render/material.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <cgltf.h>

void GameObject::DoUpdate(f32 dt) {
    if (!m_active) {
        return;
    }

    // update ourself (derived)
    Update(dt);

    // update our components
    for (const auto& component : m_components)  {
        component->Update(dt);
    }

    // update our children (and they'll update their components)
    for (auto it = m_children.begin(); it != m_children.end();) {
        if ((*it)->IsAlive()) {
            (*it)->DoUpdate(dt);
            it++;
        } else {
            it = m_children.erase(it);
        }
    }
}

bool GameObject::IsAlive() const {
    return m_alive;
}

void GameObject::MarkForDestroy() {
    m_alive = false;
}

void GameObject::SetActive(bool active) {
    m_active = active;
}

bool GameObject::IsActive() const {
    return m_active;
}

void GameObject::AddComponent(Component* component) {
    m_components.emplace_back(component);
    component->m_owner = this;
}

GameObject* GameObject::FindChildByName(std::string_view child_name) {
    if (name == child_name) {
        return this;
    }
    for (auto& child : m_children) {
        if (GameObject* res = child->FindChildByName(child_name); res != nullptr) {
            return res;
        }
    }
    return nullptr;
}

Scene* GameObject::GetScene() const {
    return m_scene;
}

GameObject* GameObject::GetParent() const {
    return m_parent;
}

void GameObject::SetParent(GameObject* parent) {
    m_scene->SetParent(this, parent);
}

glm::vec3 GameObject::GetPosition() const {
    return m_position;
}

void GameObject::SetPosition(glm::vec3 pos) {
    m_position = pos;
}

glm::quat GameObject::GetRotation() const  {
    return m_rotation;
}

void GameObject::SetRotation(glm::quat rot) {
    m_rotation = rot;
}

glm::vec3 GameObject::GetScale() const {
    return m_scale;
}

void GameObject::SetScale(glm::vec3 scale) {
    m_scale = scale;
}

glm::mat4 GameObject::GetLocalTransform() const {
    glm::mat4 m = glm::mat4_cast(m_rotation);
    m[0] *= m_scale.x;
    m[1] *= m_scale.y;
    m[2] *= m_scale.z;
    m[3]  = glm::vec4(m_position, 1.0f);
    return m;
}

glm::mat4 GameObject::GetWorldTransform() const {
    if (!m_parent) {
        return GetLocalTransform();
    }

    return m_parent->GetWorldTransform() * GetLocalTransform();
}

glm::vec3 GameObject::GetWorldPosition() const {
    if (m_parent == nullptr) {
        return m_position;
    }

    return m_parent->GetWorldPosition() + m_parent->GetWorldRotation() * m_position;
}

glm::quat GameObject::GetWorldRotation() const {
    if (m_parent == nullptr) {
        return m_rotation;
    }
    return m_parent->GetWorldRotation() * m_rotation;
}

static auto ReadFloats(const cgltf_accessor* acc, cgltf_size i, f32* out, int n) {
    std::fill(out, out + n, 0.0f);
    return cgltf_accessor_read_float(acc, i, out, n);
};

static auto ReadIndex(const cgltf_accessor* acc, cgltf_size i) {
    u32 out = 0;
    cgltf_bool ok = cgltf_accessor_read_uint(acc, i, &out, 1);
    ASSERT(out < 0xFFFF);
    return ok ? (u16)out : 0;
};

static auto ReadScalar(const cgltf_accessor* acc, cgltf_size i) {
    f32 res = 0.0f;
    cgltf_accessor_read_float(acc, i, &res, 1);
    return res;
};

static auto ReadVec3(const cgltf_accessor* acc, cgltf_size i) {
    glm::vec3 res;
    cgltf_accessor_read_float(acc, i, glm::value_ptr(res), 3);
    return res;
};

static auto ReadQuat = [](const cgltf_accessor* acc, cgltf_size i) {
    glm::quat res;
    cgltf_accessor_read_float(acc, i, glm::value_ptr(res), 4);
    return res;
};

static auto ReadTimes(const cgltf_accessor* acc, std::vector<f32>& out_times) {
    out_times.resize(acc->count);
    for (cgltf_size i = 0; i < acc->count; ++i) {
        out_times[i] = ReadScalar(acc, i);
    }
};

static auto ReadOutputVec3(const cgltf_accessor* acc, std::vector<glm::vec3>& out_values) {
    out_values.resize(acc->count);
    for (cgltf_size i = 0; i < acc->count; ++i) {
        out_values[i] = ReadVec3(acc, i);
    }
};

static auto ReadOutputQuat(const cgltf_accessor* acc, std::vector<glm::quat>& out_values) {
    out_values.resize(acc->count);
    for (cgltf_size i = 0; i < acc->count; ++i) {
        out_values[i] = ReadQuat(acc, i);
    }
};


static bool ParseGLTFNode(cgltf_node* node, GameObject* parent, const std::filesystem::path& folder) {
    auto* object = parent->GetScene()->CreateObject(node->name, parent);

    // Matrix or translation/rotation/scale
    if (node->has_matrix) {
        auto mat = glm::make_mat4(node->matrix);
        glm::vec3 translation, scale, skew;
        glm::vec4 perspective;
        glm::quat orientation;

        glm::decompose(mat, scale, orientation, translation, skew, perspective);

        object->SetPosition(translation);
        object->SetRotation(orientation);
        object->SetScale(scale);
    } else {
        if (node->has_translation) {
            object->SetPosition(glm::vec3(node->translation[0], node->translation[1], node->translation[2]));
        }
        if (node->has_rotation) {
            object->SetRotation(glm::quat(node->rotation[3], node->rotation[0], node->rotation[1], node->rotation[2]));
        }
        if (node->has_scale) {
            object->SetScale(glm::vec3(node->scale[0], node->scale[1], node->scale[2]));
        }
    }

    // Mesh
    if (node->mesh) {
        for (cgltf_size pi = 0; pi < node->mesh->primitives_count; ++pi) {
            auto& primitive = node->mesh->primitives[pi];

            if (primitive.type != cgltf_primitive_type_triangles) {
                ERROR("Found primitive with non-triangle mesh in node: {}. It own't be loaded", node->name);
                continue;
            }

            VertexLayout vertex_layout = {};
            cgltf_accessor* accessors[4] = {};

            for (cgltf_size ai = 0; ai < primitive.attributes_count; ai++) {
                auto& attribute = primitive.attributes[ai];
                auto accessor = attribute.data;
                if (!accessor) {
                    continue;
                }

                VertexElement element;
                element.type = VertexElement::Type::F32;

                switch (attribute.type) {
                    case cgltf_attribute_type_position: {
                        accessors[VertexElement::PositionIndex] = accessor;
                        element.index = VertexElement::PositionIndex;
                        element.size = 3;
                    } break;

                    case cgltf_attribute_type_normal: {
                        accessors[VertexElement::NormalIndex] = accessor;
                        element.index = VertexElement::NormalIndex;
                        element.size = 3;
                    } break;

                    case cgltf_attribute_type_color: {
                        if (attribute.index != 0) {
                            // Only get the first color attribute
                            break;
                        }

                        accessors[VertexElement::ColorIndex] = accessor;
                        element.index = VertexElement::ColorIndex;
                        element.size = 3;
                    } break;

                    case cgltf_attribute_type_texcoord: {
                        if (attribute.index != 0) {
                            // Only get the first uv coordinates attribute
                            break;
                        }

                        accessors[VertexElement::UVIndex] = accessor;
                        element.index = VertexElement::UVIndex;
                        element.size = 2;
                    } break;

                    default:
                        break;
                }

                if (element.size > 0) {
                    element.offset = vertex_layout.stride;
                    vertex_layout.stride += element.size * sizeof(float);
                    vertex_layout.elements.push_back(element);
                }
            }

            if (!accessors[VertexElement::PositionIndex]) {
                ERROR("Missing attribute position for primitive {} in node {}.", pi, node->name);
                continue;
            }

            auto vertex_count = accessors[VertexElement::PositionIndex]->count;

            std::vector<f32> vertices;
            vertices.resize(vertex_layout.stride / sizeof(f32) * vertex_count);

            for (cgltf_size vi = 0; vi < vertex_count; vi++) {
                for (auto& element : vertex_layout.elements) {
                    if (!accessors[element.index]) {
                        continue;
                    }

                    auto index = (vi * vertex_layout.stride + element.offset) / sizeof(f32);
                    f32* out_data = &vertices[index];
                    ReadFloats(accessors[element.index], vi, out_data, element.size);
                }
            }

            std::shared_ptr<Mesh> mesh;
            if (primitive.indices) {
                auto index_count = primitive.indices->count;
                std::vector<u16> indices(index_count);
                for (cgltf_size i = 0; i < index_count; i++) {
                    indices[i] = ReadIndex(primitive.indices, i);
                }
                mesh = std::make_shared<Mesh>(vertex_layout, vertices, indices);
            }
            else {
                mesh = std::make_shared<Mesh>(vertex_layout, vertices);
            }

            auto mat = std::make_shared<Material>(Engine::GetInstance().graphics.GetDefaultShaderProgram());

            if (primitive.material) {
                auto gltf_mat = primitive.material;

                if (gltf_mat->has_pbr_metallic_roughness) {
                    auto pbr = gltf_mat->pbr_metallic_roughness;
                    auto texture = pbr.base_color_texture.texture;
                    if (texture && texture->image) {
                        if (texture->image->uri) {
                            auto texture_path = folder / texture->image->uri;
                            auto tex = Engine::GetInstance().texture_manager.GetOrLoadTexture(
                                (const char*)texture_path.u8string().c_str());
                            mat->SetParam(Material::PBR_TEXTURE_ALBEDO_SLOT, MOV(tex));
                        }
                    }
                } else if (gltf_mat->has_pbr_specular_glossiness) {
                    auto pbr = gltf_mat->pbr_specular_glossiness;
                    auto texture = pbr.diffuse_texture.texture;
                    if (texture && texture->image) {
                        if (texture->image->uri) {
                            auto texture_path = folder / texture->image->uri;
                            auto& texture_manager = Engine::GetInstance().texture_manager;
                            auto tex = texture_manager.GetOrLoadTexture((const char*)texture_path.u8string().c_str());
                            mat->SetParam(Material::PBR_TEXTURE_ALBEDO_SLOT, MOV(tex));
                        }
                    }
                }

                object->AddComponent(new MeshComponent(mat, mesh));
            }
        }
    }

    for (cgltf_size ci = 0; ci < node->children_count; ci++) {
        ParseGLTFNode(node->children[ci], object, folder);
    }

    return object;
}

GameObject* GameObject::LoadGLTF(std::string_view path) {
    auto& fs = Engine::GetInstance().file_system;
    auto contents = fs.LoadAssetTextFile(path);
    if (contents.empty()) {
        ERROR("GameObject::LoadGLTF - Failed to load file: {}", path);
        return nullptr;
    }

    cgltf_options options = {};
    cgltf_data* data = nullptr;
    cgltf_result res = cgltf_parse(&options, contents.data(), contents.size(), &data);
    if (res != cgltf_result_success) {
        ERROR("cgltf parse failed with error: {}", (int)res);
        return nullptr;
    }
    defer { cgltf_free(data); };

    auto fullpath = fs.GetAssetsDir() / path;
    auto full_dir_path = fullpath.remove_filename();
    auto relative_dir_path = std::filesystem::path(path).remove_filename();

    res = cgltf_load_buffers(&options, data, fullpath.remove_filename().string().c_str());
    if (res != cgltf_result_success) {
        ERROR("cgltf buffer load failed with error: {}", (int)res);
        return nullptr;
    }

    auto obj_name = std::filesystem::path(path).filename().replace_extension();
    auto result_object = Engine::GetInstance().GetCurrentScene()->CreateObject(obj_name.c_str());
    auto scene = &data->scenes[0];

    for (cgltf_size i = 0; i < scene->nodes_count; ++i) {
        auto node = scene->nodes[i];
        ParseGLTFNode(node, result_object, relative_dir_path);
    }

    std::vector<std::shared_ptr<AnimationClip>> clips;
    for (cgltf_size ai = 0; ai < data->animations_count; ++ai) {
        auto& anim  = data->animations[ai];
        auto clip = std::make_shared<AnimationClip>();
        clip->name = anim.name ? anim.name : "noname";
        clip->duration = 0.0f;

        std::unordered_map<cgltf_node*, usize> track_index_of;

        auto GetOrCreateTrack = [&](cgltf_node* node) -> TransformTrack& {
            if (auto it = track_index_of.find(node); it != track_index_of.end()) {
                return clip->tracks[it->second];
            }

            TransformTrack track;
            track.target_name = node->name;
            clip->tracks.push_back(track);
            usize idx = clip->tracks.size() - 1;
            track_index_of[node] = idx;
            return clip->tracks[idx];
        };

        for (cgltf_size ci = 0; ci < anim.channels_count; ++ci) {
            auto& channel  = anim.channels[ci];
            auto sampler = channel.sampler;

            if (!channel.target_node || !sampler || !sampler->input || !sampler->output) {
                continue;
            }

            std::vector<f32> times;
            ReadTimes(sampler->input, times);

            auto& track = GetOrCreateTrack(channel.target_node);

            switch (channel.target_path) {
                case cgltf_animation_path_type_translation: {
                    std::vector<glm::vec3> values;
                    ReadOutputVec3(sampler->output, values);
                    track.positions.resize(times.size());
                    ASSERT(times.size() == values.size());
                    for (usize i = 0; i < times.size(); ++i) {
                        track.positions[i].time = times[i];
                        track.positions[i].value = values[i];
                    }
                } break;

                case cgltf_animation_path_type_rotation: {
                    std::vector<glm::quat> values;
                    ReadOutputQuat(sampler->output, values);
                    track.rotations.resize(times.size());
                    ASSERT(times.size() == values.size());
                    for (usize i = 0; i < times.size(); ++i) {
                        track.rotations[i].time = times[i];
                        track.rotations[i].value = values[i];
                    }
                } break;

                case cgltf_animation_path_type_scale: {
                    std::vector<glm::vec3> values;
                    ReadOutputVec3(sampler->output, values);
                    track.scales.resize(times.size());
                    ASSERT(times.size() == values.size());
                    for (usize i = 0; i < times.size(); ++i) {
                        track.scales[i].time = times[i];
                        track.scales[i].value = values[i];
                    }
                } break;

                default: {
                    break;
                }
            }

            clip->duration = Max(clip->duration, times.back());
        }

        clips.push_back(MOV(clip));
    }

    if (!clips.empty()) {
        auto animation_component = new AnimationComponent();
        for (auto& clip : clips) {
            animation_component->RegisterClip(clip->name, MOV(clip));
        }
        result_object->AddComponent(animation_component);
    }

    return result_object;
}

