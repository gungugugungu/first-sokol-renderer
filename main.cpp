#define SOKOL_IMPL
#define SOKOL_GLCORE
#include "sokol_app.h"
#include "sokol_gfx.h"
#include "sokol_glue.h"

static sg_pass_action pass_action;

void init(void) {
    sg_desc desc = {};
    desc.environment = sglue_environment();
    sg_setup(&desc);

    pass_action = {};
    pass_action.colors[0] = { .load_action = SG_LOADACTION_CLEAR,
                              .clear_value = {0.39f,0.58f,0.93f,1.0f} };
}

void frame(void) {
    sg_pass pass = {};
    pass.action = pass_action;
    pass.swapchain = sglue_swapchain();
    sg_begin_pass(&pass);
    sg_end_pass();
    sg_commit();
}

void cleanup(void) {
    sg_shutdown();
}

sapp_desc sokol_main(int argc, char* argv[]) {
    (void)argc; (void)argv;
    return {
        .init_cb = init,
        .frame_cb = frame,
        .cleanup_cb = cleanup,
        .width = 800,
        .height = 600,
        .window_title = "sokol_gfx engine",
    };
}
