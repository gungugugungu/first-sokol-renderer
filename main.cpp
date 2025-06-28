#define SOKOL_IMPL
#define SOKOL_GLCORE
#include "sokol_app.h"
#include "sokol_gfx.h"
#include "sokol_glue.h"
#include "triangle.glsl.h"

/* application state */
static struct {
    sg_pipeline pip;
    sg_bindings bind;
    sg_pass_action pass_action;
} state;

static void init(void) {
    /* create setup descriptor */
    sg_desc desc = {};
    desc.environment = sglue_environment();
    sg_setup(&desc);

    /* create shader from code-generated sg_shader_desc */
    sg_shader shd = sg_make_shader(simple_shader_desc(sg_query_backend()));

    /* a vertex buffer with 3 vertices */
    float vertices[] = {
        // positions
        -0.5f, -0.5f, 0.0f,     // bottom left
        0.5f, -0.5f, 0.0f,      // bottom right
        0.0f,  0.5f, 0.0f,      // top
    };

    float colors[] = {
        1.0f, 0.0f, 0.0f,       // bottom left
        0.0f, 1.0f, 0.0f,       // bottom right
        0.0f, 0.0f, 1.0f        // top
    };

    // vertex buffer
    sg_buffer_desc buf_desc = {};
    buf_desc.size = sizeof(vertices);
    buf_desc.data = SG_RANGE(vertices);
    buf_desc.label = "triangle-vertices";
    state.bind.vertex_buffers[0] = sg_make_buffer(&buf_desc);
    // color buffer
    sg_buffer_desc color_buf_desc = {};
    color_buf_desc.size = sizeof(vertices);
    color_buf_desc.data = SG_RANGE(vertices);
    color_buf_desc.label = "triangle-vertices";
    state.bind.vertex_buffers[1] = sg_make_buffer(&color_buf_desc);

    // Create pipeline descriptor
    sg_pipeline_desc pip_desc = {};
    pip_desc.shader = shd;
    pip_desc.layout.attrs[ATTR_simple_position].format = SG_VERTEXFORMAT_FLOAT3;
    pip_desc.label = "triangle-pipeline";
    state.pip = sg_make_pipeline(&pip_desc);

    // Setup pass action
    state.pass_action = {};
    state.pass_action.colors[0].load_action = SG_LOADACTION_CLEAR;
    state.pass_action.colors[0].clear_value = {0.2f, 0.3f, 0.3f, 1.0f};
}

void frame(void) {
    // Create pass descriptor
    sg_pass pass = {};
    pass.action = state.pass_action;
    pass.swapchain = sglue_swapchain();
    
    sg_begin_pass(&pass);
    sg_apply_pipeline(state.pip);
    sg_apply_bindings(&state.bind);
    sg_draw(0, 3, 1);
    sg_end_pass();
    sg_commit();
}

void cleanup(void) {
    sg_shutdown();
}

void event(const sapp_event* e) {
    if (e->type == SAPP_EVENTTYPE_KEY_DOWN) {
        if (e->key_code == SAPP_KEYCODE_ESCAPE) {
            sapp_request_quit();
        }
    }
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