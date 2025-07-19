#define SOKOL_IMPL
#define SOKOL_GLCORE
#include <array>
#include <cstdint>
#include <vector>
#include <string>
#include <cstring>
#include <iostream>
#include <cstdlib>
#include "sokol/sokol_app.h"
#include "sokol/sokol_gfx.h"
#include "sokol/sokol_glue.h"
#include "sokol/sokol_fetch.h"
#include "sokol/sokol_time.h"
#ifdef B32
#pragma message("B32 is defined as: " B32)
#undef B32
#endif
#include "HandmadeMath/HandmadeMath.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#define CGLTF_IMPLEMENTATION
#include "cgltf/cgltf.h"
// shaders
#include "shaders/mainshader.glsl.h"

using namespace std;

struct AppState {
    sg_pipeline pip{};
    sg_bindings bind{};
    sg_pass_action pass_action{};
    std::array<uint8_t, 512 * 1024> file_buffer{};
    HMM_Vec3 cube_positions[10];
    HMM_Vec3 camera_pos;
    HMM_Vec3 camera_front;
    HMM_Vec3 camera_up;
    uint64_t last_time;
    uint64_t delta_time;
    bool mouse_btn;
    bool first_mouse;
    float last_x;
    float last_y;
    float yaw;
    float pitch;
    float fov;
    bool inputs[348];
};

AppState state;
int texture_index = 0;

void fetch_callback(const sfetch_response_t* response);

class Mesh {
public:
    // Transform components
    HMM_Vec3 position = {0.0f, 0.0f, 0.0f};
    HMM_Quat rotation = {0.0f, 0.0f, 0.0f, 1.0f};  // w=1 identity
    HMM_Vec3 scale = {1.0f, 1.0f, 1.0f};

    // Vertex data: interleaved positions (3 floats) and texcoords (2 floats)
    std::vector<float> vertices;

    // Index data
    std::vector<unsigned int> indices;
};

// Helper to decompose matrix into TRS components
static void decompose_matrix(const float* matrix, HMM_Vec3& position, HMM_Quat& rotation, HMM_Vec3& scale) {
    // Extract translation
    position.X = matrix[12];
    position.Y = matrix[13];
    position.Z = matrix[14];

    // Extract scale from column lengths
    HMM_Vec3 col0 = {matrix[0], matrix[1], matrix[2]};
    HMM_Vec3 col1 = {matrix[4], matrix[5], matrix[6]};
    HMM_Vec3 col2 = {matrix[8], matrix[9], matrix[10]};

    scale.X = HMM_LenV3(col0);
    scale.Y = HMM_LenV3(col1);
    scale.Z = HMM_LenV3(col2);

    // Normalize columns to get rotation matrix
    if (scale.X > 0.0001f) col0 = HMM_DivV3F(col0, scale.X);
    if (scale.Y > 0.0001f) col1 = HMM_DivV3F(col1, scale.Y);
    if (scale.Z > 0.0001f) col2 = HMM_DivV3F(col2, scale.Z);

    // Create rotation matrix and convert to quaternion
    HMM_Mat4 rot_matrix = {
        col0.X, col1.X, col2.X, 0,
        col0.Y, col1.Y, col2.Y, 0,
        col0.Z, col1.Z, col2.Z, 0,
        0, 0, 0, 1
    };
    rotation = HMM_M4ToQ_LH(rot_matrix); // LH or RH ?????
}

Mesh load_gltf(const char* path) {
    Mesh mesh;
    cgltf_options options = {};
    cgltf_data* data = nullptr;

    // Parse GLB/GLTF file
    cgltf_result result = cgltf_parse_file(&options, path, &data);
    if (result != cgltf_result_success) {
        return mesh;  // Return empty mesh on failure
    }

    // Load buffer data
    result = cgltf_load_buffers(&options, data, path);
    if (result != cgltf_result_success) {
        cgltf_free(data);
        return mesh;
    }

    // Find first node with a mesh
    cgltf_node* target_node = nullptr;
    for (cgltf_size i = 0; i < data->nodes_count; ++i) {
        if (data->nodes[i].mesh) {
            target_node = &data->nodes[i];
            break;
        }
    }

    if (!target_node || !target_node->mesh) {
        cgltf_free(data);
        return mesh;
    }

    // Extract transform from node
    float matrix[16];
    if (target_node->has_matrix) {
        memcpy(matrix, target_node->matrix, sizeof(matrix));
    } else {
        cgltf_node_transform_local(target_node, matrix);
    }
    decompose_matrix(matrix, mesh.position, mesh.rotation, mesh.scale);

    // Get first primitive from mesh
    cgltf_primitive* primitive = &target_node->mesh->primitives[0];
    if (!primitive) {
        cgltf_free(data);
        return mesh;
    }

    // Find position and texcoord accessors
    cgltf_accessor* pos_accessor = nullptr;
    cgltf_accessor* tex_accessor = nullptr;

    for (cgltf_size i = 0; i < primitive->attributes_count; ++i) {
        cgltf_attribute* attr = &primitive->attributes[i];
        if (attr->type == cgltf_attribute_type_position) {
            pos_accessor = attr->data;
        }
        else if (attr->type == cgltf_attribute_type_texcoord && attr->index == 0) {
            tex_accessor = attr->data;
        }
    }

    if (!pos_accessor) {
        cgltf_free(data);
        return mesh;
    }

    // Extract vertex data
    const cgltf_size vertex_count = pos_accessor->count;
    mesh.vertices.reserve(vertex_count * 5);  // 3 pos + 2 uv per vertex

    // Load positions
    float* positions = (float*)malloc(sizeof(float) * 3 * vertex_count);
    cgltf_accessor_unpack_floats(pos_accessor, positions, vertex_count * 3);

    // Load texcoords (create default if missing)
    float* texcoords = nullptr;
    if (tex_accessor) {
        texcoords = (float*)malloc(sizeof(float) * 2 * vertex_count);
        cgltf_accessor_unpack_floats(tex_accessor, texcoords, vertex_count * 2);
    }

    // Interleave vertex data
    for (cgltf_size i = 0; i < vertex_count; ++i) {
        // Position
        mesh.vertices.push_back(positions[i * 3 + 0]);
        mesh.vertices.push_back(positions[i * 3 + 1]);
        mesh.vertices.push_back(positions[i * 3 + 2]);

        // Texcoord (default to 0 if missing)
        if (texcoords) {
            mesh.vertices.push_back(texcoords[i * 2 + 0]);
            mesh.vertices.push_back(texcoords[i * 2 + 1]);
        } else {
            mesh.vertices.push_back(0.0f);
            mesh.vertices.push_back(0.0f);
        }
    }

    // Extract indices
    if (primitive->indices) {
        const cgltf_size index_count = primitive->indices->count;
        uint32_t* temp_indices = (uint32_t*)malloc(sizeof(uint32_t) * index_count);
        cgltf_accessor_unpack_indices(primitive->indices, temp_indices, sizeof(uint32_t), index_count);

        mesh.indices.reserve(index_count);
        for (cgltf_size i = 0; i < index_count; ++i) {
            mesh.indices.push_back(temp_indices[i]);
        }
        free(temp_indices);
    }

    // Cleanup
    free(positions);
    if (texcoords) free(texcoords);
    cgltf_free(data);

    return mesh;
}

void init() {
    sg_desc desc = {};
    desc.environment = sglue_environment();
    sg_setup(&desc);
    stm_setup();

    // flip images after loading
    stbi_set_flip_vertically_on_load(true);

    sapp_show_mouse(false);

    Mesh loaded_mesh = load_gltf("test.glb");;

    state.camera_pos = HMM_V3(0.0f, 0.0f, 3.0f);
    state.camera_front = HMM_V3(0.0f, 0.0f, -1.0f);
    state.camera_up = HMM_V3(0.0f, 1.0f, 0.0f);
    state.yaw = -90.0f;  // Start looking down the negative Z axis
    state.pitch = 0.0f;
    state.first_mouse = true;
    state.last_time = stm_now();
    state.fov = 45.0f;

    state.cube_positions[0] = HMM_V3( 0.0f,  0.0f,  0.0f);
    state.cube_positions[1] = HMM_V3( 2.0f,  5.0f, -15.0f);
    state.cube_positions[2] = HMM_V3(-1.5f, -2.2f, -2.5f);
    state.cube_positions[3] = HMM_V3(-3.8f, -2.0f, -12.3f);
    state.cube_positions[4] = HMM_V3( 2.4f, -0.4f, -3.5f);
    state.cube_positions[5] = HMM_V3(-1.7f,  3.0f, -7.5f);
    state.cube_positions[6] = HMM_V3( 1.3f, -2.0f, -2.5f);
    state.cube_positions[7] = HMM_V3( 1.5f,  2.0f, -2.5f);
    state.cube_positions[8] = HMM_V3( 1.5f,  0.2f, -1.5f);
    state.cube_positions[9] = HMM_V3(-1.3f,  1.0f, -1.5f);

    sfetch_desc_t fetch_desc = {};
    fetch_desc.max_requests = 2;
    fetch_desc.num_channels = 1;
    fetch_desc.num_lanes = 1;
    sfetch_setup(&fetch_desc);

    // create sampler
    sg_sampler_desc sampler_desc = {};
    sampler_desc.min_filter = SG_FILTER_LINEAR;
    sampler_desc.mag_filter = SG_FILTER_LINEAR;
    sampler_desc.wrap_u = SG_WRAP_REPEAT;
    sampler_desc.wrap_v = SG_WRAP_REPEAT;
    state.bind.samplers[0] = sg_make_sampler(&sampler_desc);

    // again!!!
    sg_sampler_desc sampler_desc_2 = {};
    sampler_desc_2.min_filter = SG_FILTER_LINEAR;
    sampler_desc_2.mag_filter = SG_FILTER_LINEAR;
    sampler_desc_2.wrap_u = SG_WRAP_REPEAT;
    sampler_desc_2.wrap_v = SG_WRAP_REPEAT;
    state.bind.samplers[1] = sg_make_sampler(&sampler_desc_2);

    float vertices[] = {
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
        0.5f,  0.5f, -0.5f, 1.0f, 1.0f,
        0.5f,  0.5f, -0.5f, 1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f, 0.0f, 0.0f,
        0.5f, -0.5f,  0.5f, 1.0f, 0.0f,
        0.5f,  0.5f,  0.5f, 1.0f, 1.0f,
        0.5f,  0.5f,  0.5f, 1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f, 0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f, 1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f, 1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f, 0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f, 1.0f, 0.0f,

        0.5f,  0.5f,  0.5f, 1.0f, 0.0f,
        0.5f,  0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, -0.5f,  0.5f, 0.0f, 0.0f,
        0.5f,  0.5f,  0.5f, 1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, -0.5f,  0.5f, 1.0f, 0.0f,
        0.5f, -0.5f,  0.5f, 1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f, 0.0f, 1.0f,
        0.5f,  0.5f, -0.5f, 1.0f, 1.0f,
        0.5f,  0.5f,  0.5f, 1.0f, 0.0f,
        0.5f,  0.5f,  0.5f, 1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f, 0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };
    sg_buffer_desc vbuf_desc = {};
    vbuf_desc.size = sizeof(vertices);
    vbuf_desc.data = SG_RANGE(vertices);
    vbuf_desc.label = "cube-vertices";
    state.bind.vertex_buffers[0] = sg_make_buffer(&vbuf_desc);

    sg_shader shd = sg_make_shader(simple_shader_desc(sg_query_backend()));

    sg_pipeline_desc pip_desc = {};
    pip_desc.shader = shd;
    pip_desc.color_count = 1;
    pip_desc.colors[0].pixel_format = SG_PIXELFORMAT_RGBA8;
    pip_desc.layout.attrs[ATTR_simple_aPos].format = SG_VERTEXFORMAT_FLOAT3;
    pip_desc.layout.attrs[ATTR_simple_aTexCoord].format = SG_VERTEXFORMAT_FLOAT2;
    pip_desc.depth.compare = SG_COMPAREFUNC_LESS_EQUAL;
    pip_desc.depth.write_enabled = true;
    // TODO: add cull mode when model loading comes
    //pip_desc.cull_mode = SG_CULLMODE_BACK;
    pip_desc.label = "cube-pipeline";
    state.pip = sg_make_pipeline(&pip_desc);

    state.pass_action.colors[0].load_action = SG_LOADACTION_CLEAR;
    state.pass_action.colors[0].clear_value = { 0.2f, 0.3f, 0.3f, 1.0f };

    sfetch_request_t request = {};
    request.path = "container.jpg";
    request.callback = fetch_callback;
    request.buffer.ptr = state.file_buffer.data();
    request.buffer.size = state.file_buffer.size();
    sfetch_send(&request);

    request = {};
    request.path = "awesomeface.png";
    request.callback = fetch_callback;
    request.buffer.ptr = state.file_buffer.data();
    request.buffer.size = state.file_buffer.size();
    sfetch_send(&request);
}

void frame(void) {
    state.delta_time = stm_laptime(&state.last_time);
    sfetch_dowork();
    sg_pass pass = {};
    pass.action = state.pass_action;
    pass.swapchain = sglue_swapchain();

    float camera_speed = 5.f * (float) stm_sec(state.delta_time);
    if (state.inputs[SAPP_KEYCODE_W] == true) {
        HMM_Vec3 offset = HMM_MulV3F(state.camera_front, camera_speed);
        state.camera_pos = HMM_AddV3(state.camera_pos, offset);
    }
    if (state.inputs[SAPP_KEYCODE_S] == true) {
        HMM_Vec3 offset = HMM_MulV3F(state.camera_front, camera_speed);
        state.camera_pos = HMM_SubV3(state.camera_pos, offset);
    }
    if (state.inputs[SAPP_KEYCODE_A] == true) {
        HMM_Vec3 offset = HMM_MulV3F(HMM_NormV3(HMM_Cross(state.camera_front, state.camera_up)), camera_speed);
        state.camera_pos = HMM_SubV3(state.camera_pos, offset);
    }
    if (state.inputs[SAPP_KEYCODE_D] == true) {
        HMM_Vec3 offset = HMM_MulV3F(HMM_NormV3(HMM_Cross(state.camera_front, state.camera_up)), camera_speed);
        state.camera_pos = HMM_AddV3(state.camera_pos, offset);
    }

    // note that we're translating the scene in the reverse direction of where we want to move -- said zeromake from github
    HMM_Mat4 view = HMM_LookAt_RH(state.camera_pos, HMM_AddV3(state.camera_pos, state.camera_front), state.camera_up);
    HMM_Mat4 projection = HMM_Perspective_RH_NO(state.fov, (float)sapp_width() / (float)sapp_height(), 0.1f, 100.0f);

    sg_begin_pass(&pass);
    sg_apply_pipeline(state.pip);
    sg_apply_bindings(&state.bind);

    vs_params_t vs_params = {
        .view = view,
        .projection = projection
    };

    for(size_t i = 0; i < 10; i++) {
        HMM_Mat4 model = HMM_Translate(state.cube_positions[i]);
        float angle = 20.0f * i;
        model = HMM_MulM4(model, HMM_Rotate_RH(HMM_AngleDeg(angle), HMM_V3(1.0f, 0.3f, 0.5f)));
        vs_params.model = model;
        sg_apply_uniforms(UB_vs_params, SG_RANGE(vs_params));

        sg_draw(0, 36, 1);
    }
    sg_end_pass();
    sg_commit();
}

void cleanup(void) {
    sg_shutdown();
    sfetch_shutdown();
}

void event(const sapp_event* e) {
        if (e->type == SAPP_EVENTTYPE_MOUSE_DOWN) {
        state.mouse_btn = true;
    } else if (e->type == SAPP_EVENTTYPE_MOUSE_UP) {
        state.mouse_btn = false;
    } else if (e->type == SAPP_EVENTTYPE_KEY_DOWN) {
        state.inputs[e->key_code] = true;
        if (e->key_code == SAPP_KEYCODE_ESCAPE) {
            sapp_request_quit();
        }

        if (e->key_code == SAPP_KEYCODE_SPACE) {
            bool mouse_shown = sapp_mouse_shown();
            sapp_show_mouse(!mouse_shown);
        }

    } else if (e->type == SAPP_EVENTTYPE_KEY_UP) {
        state.inputs[e->key_code] = false;
    }  else if (e->type == SAPP_EVENTTYPE_MOUSE_MOVE && state.mouse_btn) {
        if(state.first_mouse) {
            state.last_x = e->mouse_x;
            state.last_y = e->mouse_y;
            state.first_mouse = false;
        }

        float xoffset = e->mouse_x - state.last_x;
        float yoffset = state.last_y - e->mouse_y;
        state.last_x = e->mouse_x;
        state.last_y = e->mouse_y;

        float sensitivity = 0.1f;
        xoffset *= sensitivity;
        yoffset *= sensitivity;

        state.yaw   += xoffset;
        state.pitch += yoffset;

        if(state.pitch > 89.0f) {
            state.pitch = 89.0f;
        }
        else if(state.pitch < -89.0f) {
            state.pitch = -89.0f;
        }

        HMM_Vec3 direction;
        direction.X = cosf(state.yaw * HMM_PI / 180.0f) * cosf(state.pitch * HMM_PI / 180.0f);
        direction.Y = sinf(state.pitch * HMM_PI / 180.0f);
        direction.Z = sinf(state.yaw * HMM_PI / 180.0f) * cosf(state.pitch * HMM_PI / 180.0f);
        state.camera_front = HMM_NormV3(direction);
    }
    else if (e->type == SAPP_EVENTTYPE_MOUSE_SCROLL) {
        if (state.fov >= 1.0f && state.fov <= 45.0f) {
            state.fov -= e->scroll_y;
        }
        if (state.fov <= 1.0f)
            state.fov = 1.0f;
        else if (state.fov >= 45.0f)
            state.fov = 45.0f;
    }
}

void fetch_callback(const sfetch_response_t* response) {
    if (response->fetched) {
        int img_width, img_height, num_channels;
        const int desired_channels = 4;
        stbi_uc* pixels = stbi_load_from_memory(
            static_cast<const stbi_uc*>(response->data.ptr),
            static_cast<int>(response->data.size),
            &img_width, &img_height,
            &num_channels, desired_channels);

        if (pixels) {
            // yay, memory safety!
            sg_destroy_image(state.bind.images[texture_index]);

            sg_image_desc img_desc = {};
            img_desc.width = img_width;
            img_desc.height = img_height;
            img_desc.pixel_format = SG_PIXELFORMAT_RGBA8;
            img_desc.data.subimage[0][0].ptr = pixels;
            img_desc.data.subimage[0][0].size = img_width * img_height * 4;
            state.bind.images[texture_index] = sg_make_image(&img_desc);

            stbi_image_free(pixels);
        }
    } else if (response->failed) {
        state.pass_action.colors[0].load_action = SG_LOADACTION_CLEAR;
        state.pass_action.colors[0].clear_value = { 1.0f, 0.0f, 0.0f, 1.0f };
        std::cout << "ohhh no, failed to fetch the texture =(" << std::endl;
    }
    texture_index += 1;
}

sapp_desc sokol_main(int argc, char* argv[]) {
    sapp_desc desc = {};
    desc.init_cb = init;
    desc.frame_cb = frame;
    desc.cleanup_cb = cleanup;
    desc.event_cb = event;
    desc.width = 800;
    desc.height = 600;
    desc.high_dpi = true;
    desc.window_title = "learnopengl.com but in sokol";
    return desc;
}