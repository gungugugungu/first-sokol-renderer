#pragma once
/*
    #version:1# (machine generated, don't edit!)

    Generated by sokol-shdc (https://github.com/floooh/sokol-tools)

    Cmdline:
        sokol-shdc --input ./shaders/mainshader.glsl --output ./shaders/mainshader.glsl.h --slang glsl430

    Overview:
    =========
    Shader program: 'simple':
        Get shader desc: simple_shader_desc(sg_query_backend());
        Vertex Shader: vs
        Fragment Shader: fs
        Attributes:
            ATTR_simple_aPos => 0
            ATTR_simple_aTexCoord => 1
    Bindings:
        Uniform block 'vs_params':
            C struct: vs_params_t
            Bind slot: UB_vs_params => 0
        Image '_texture1':
            Image type: SG_IMAGETYPE_2D
            Sample type: SG_IMAGESAMPLETYPE_FLOAT
            Multisampled: false
            Bind slot: IMG__texture1 => 0
        Image '_texture2':
            Image type: SG_IMAGETYPE_2D
            Sample type: SG_IMAGESAMPLETYPE_FLOAT
            Multisampled: false
            Bind slot: IMG__texture2 => 1
        Sampler 'texture1_smp':
            Type: SG_SAMPLERTYPE_FILTERING
            Bind slot: SMP_texture1_smp => 0
        Sampler 'texture2_smp':
            Type: SG_SAMPLERTYPE_FILTERING
            Bind slot: SMP_texture2_smp => 1
*/
#if !defined(SOKOL_GFX_INCLUDED)
#error "Please include sokol_gfx.h before mainshader.glsl.h"
#endif
#if !defined(SOKOL_SHDC_ALIGN)
#if defined(_MSC_VER)
#define SOKOL_SHDC_ALIGN(a) __declspec(align(a))
#else
#define SOKOL_SHDC_ALIGN(a) __attribute__((aligned(a)))
#endif
#endif
#define ATTR_simple_aPos (0)
#define ATTR_simple_aTexCoord (1)
#define UB_vs_params (0)
#define IMG__texture1 (0)
#define IMG__texture2 (1)
#define SMP_texture1_smp (0)
#define SMP_texture2_smp (1)
#pragma pack(push,1)
SOKOL_SHDC_ALIGN(16) typedef struct vs_params_t {
    HMM_Mat4 model;
    HMM_Mat4 view;
    HMM_Mat4 projection;
} vs_params_t;
#pragma pack(pop)
/*
    #version 430

    uniform vec4 vs_params[12];
    layout(location = 0) in vec3 aPos;
    layout(location = 0) out vec2 TexCoord;
    layout(location = 1) in vec2 aTexCoord;

    void main()
    {
        gl_Position = ((mat4(vs_params[8], vs_params[9], vs_params[10], vs_params[11]) * mat4(vs_params[4], vs_params[5], vs_params[6], vs_params[7])) * mat4(vs_params[0], vs_params[1], vs_params[2], vs_params[3])) * vec4(aPos, 1.0);
        TexCoord = aTexCoord;
    }

*/
static const uint8_t vs_source_glsl430[432] = {
    0x23,0x76,0x65,0x72,0x73,0x69,0x6f,0x6e,0x20,0x34,0x33,0x30,0x0a,0x0a,0x75,0x6e,
    0x69,0x66,0x6f,0x72,0x6d,0x20,0x76,0x65,0x63,0x34,0x20,0x76,0x73,0x5f,0x70,0x61,
    0x72,0x61,0x6d,0x73,0x5b,0x31,0x32,0x5d,0x3b,0x0a,0x6c,0x61,0x79,0x6f,0x75,0x74,
    0x28,0x6c,0x6f,0x63,0x61,0x74,0x69,0x6f,0x6e,0x20,0x3d,0x20,0x30,0x29,0x20,0x69,
    0x6e,0x20,0x76,0x65,0x63,0x33,0x20,0x61,0x50,0x6f,0x73,0x3b,0x0a,0x6c,0x61,0x79,
    0x6f,0x75,0x74,0x28,0x6c,0x6f,0x63,0x61,0x74,0x69,0x6f,0x6e,0x20,0x3d,0x20,0x30,
    0x29,0x20,0x6f,0x75,0x74,0x20,0x76,0x65,0x63,0x32,0x20,0x54,0x65,0x78,0x43,0x6f,
    0x6f,0x72,0x64,0x3b,0x0a,0x6c,0x61,0x79,0x6f,0x75,0x74,0x28,0x6c,0x6f,0x63,0x61,
    0x74,0x69,0x6f,0x6e,0x20,0x3d,0x20,0x31,0x29,0x20,0x69,0x6e,0x20,0x76,0x65,0x63,
    0x32,0x20,0x61,0x54,0x65,0x78,0x43,0x6f,0x6f,0x72,0x64,0x3b,0x0a,0x0a,0x76,0x6f,
    0x69,0x64,0x20,0x6d,0x61,0x69,0x6e,0x28,0x29,0x0a,0x7b,0x0a,0x20,0x20,0x20,0x20,
    0x67,0x6c,0x5f,0x50,0x6f,0x73,0x69,0x74,0x69,0x6f,0x6e,0x20,0x3d,0x20,0x28,0x28,
    0x6d,0x61,0x74,0x34,0x28,0x76,0x73,0x5f,0x70,0x61,0x72,0x61,0x6d,0x73,0x5b,0x38,
    0x5d,0x2c,0x20,0x76,0x73,0x5f,0x70,0x61,0x72,0x61,0x6d,0x73,0x5b,0x39,0x5d,0x2c,
    0x20,0x76,0x73,0x5f,0x70,0x61,0x72,0x61,0x6d,0x73,0x5b,0x31,0x30,0x5d,0x2c,0x20,
    0x76,0x73,0x5f,0x70,0x61,0x72,0x61,0x6d,0x73,0x5b,0x31,0x31,0x5d,0x29,0x20,0x2a,
    0x20,0x6d,0x61,0x74,0x34,0x28,0x76,0x73,0x5f,0x70,0x61,0x72,0x61,0x6d,0x73,0x5b,
    0x34,0x5d,0x2c,0x20,0x76,0x73,0x5f,0x70,0x61,0x72,0x61,0x6d,0x73,0x5b,0x35,0x5d,
    0x2c,0x20,0x76,0x73,0x5f,0x70,0x61,0x72,0x61,0x6d,0x73,0x5b,0x36,0x5d,0x2c,0x20,
    0x76,0x73,0x5f,0x70,0x61,0x72,0x61,0x6d,0x73,0x5b,0x37,0x5d,0x29,0x29,0x20,0x2a,
    0x20,0x6d,0x61,0x74,0x34,0x28,0x76,0x73,0x5f,0x70,0x61,0x72,0x61,0x6d,0x73,0x5b,
    0x30,0x5d,0x2c,0x20,0x76,0x73,0x5f,0x70,0x61,0x72,0x61,0x6d,0x73,0x5b,0x31,0x5d,
    0x2c,0x20,0x76,0x73,0x5f,0x70,0x61,0x72,0x61,0x6d,0x73,0x5b,0x32,0x5d,0x2c,0x20,
    0x76,0x73,0x5f,0x70,0x61,0x72,0x61,0x6d,0x73,0x5b,0x33,0x5d,0x29,0x29,0x20,0x2a,
    0x20,0x76,0x65,0x63,0x34,0x28,0x61,0x50,0x6f,0x73,0x2c,0x20,0x31,0x2e,0x30,0x29,
    0x3b,0x0a,0x20,0x20,0x20,0x20,0x54,0x65,0x78,0x43,0x6f,0x6f,0x72,0x64,0x20,0x3d,
    0x20,0x61,0x54,0x65,0x78,0x43,0x6f,0x6f,0x72,0x64,0x3b,0x0a,0x7d,0x0a,0x0a,0x00,

};
/*
    #version 430

    layout(binding = 16) uniform sampler2D _texture1_texture1_smp;
    layout(binding = 17) uniform sampler2D _texture2_texture2_smp;

    layout(location = 0) out vec4 FragColor;
    layout(location = 0) in vec2 TexCoord;

    void main()
    {
        FragColor = mix(texture(_texture1_texture1_smp, TexCoord), texture(_texture2_texture2_smp, TexCoord), vec4(0.5));
    }

*/
static const uint8_t fs_source_glsl430[358] = {
    0x23,0x76,0x65,0x72,0x73,0x69,0x6f,0x6e,0x20,0x34,0x33,0x30,0x0a,0x0a,0x6c,0x61,
    0x79,0x6f,0x75,0x74,0x28,0x62,0x69,0x6e,0x64,0x69,0x6e,0x67,0x20,0x3d,0x20,0x31,
    0x36,0x29,0x20,0x75,0x6e,0x69,0x66,0x6f,0x72,0x6d,0x20,0x73,0x61,0x6d,0x70,0x6c,
    0x65,0x72,0x32,0x44,0x20,0x5f,0x74,0x65,0x78,0x74,0x75,0x72,0x65,0x31,0x5f,0x74,
    0x65,0x78,0x74,0x75,0x72,0x65,0x31,0x5f,0x73,0x6d,0x70,0x3b,0x0a,0x6c,0x61,0x79,
    0x6f,0x75,0x74,0x28,0x62,0x69,0x6e,0x64,0x69,0x6e,0x67,0x20,0x3d,0x20,0x31,0x37,
    0x29,0x20,0x75,0x6e,0x69,0x66,0x6f,0x72,0x6d,0x20,0x73,0x61,0x6d,0x70,0x6c,0x65,
    0x72,0x32,0x44,0x20,0x5f,0x74,0x65,0x78,0x74,0x75,0x72,0x65,0x32,0x5f,0x74,0x65,
    0x78,0x74,0x75,0x72,0x65,0x32,0x5f,0x73,0x6d,0x70,0x3b,0x0a,0x0a,0x6c,0x61,0x79,
    0x6f,0x75,0x74,0x28,0x6c,0x6f,0x63,0x61,0x74,0x69,0x6f,0x6e,0x20,0x3d,0x20,0x30,
    0x29,0x20,0x6f,0x75,0x74,0x20,0x76,0x65,0x63,0x34,0x20,0x46,0x72,0x61,0x67,0x43,
    0x6f,0x6c,0x6f,0x72,0x3b,0x0a,0x6c,0x61,0x79,0x6f,0x75,0x74,0x28,0x6c,0x6f,0x63,
    0x61,0x74,0x69,0x6f,0x6e,0x20,0x3d,0x20,0x30,0x29,0x20,0x69,0x6e,0x20,0x76,0x65,
    0x63,0x32,0x20,0x54,0x65,0x78,0x43,0x6f,0x6f,0x72,0x64,0x3b,0x0a,0x0a,0x76,0x6f,
    0x69,0x64,0x20,0x6d,0x61,0x69,0x6e,0x28,0x29,0x0a,0x7b,0x0a,0x20,0x20,0x20,0x20,
    0x46,0x72,0x61,0x67,0x43,0x6f,0x6c,0x6f,0x72,0x20,0x3d,0x20,0x6d,0x69,0x78,0x28,
    0x74,0x65,0x78,0x74,0x75,0x72,0x65,0x28,0x5f,0x74,0x65,0x78,0x74,0x75,0x72,0x65,
    0x31,0x5f,0x74,0x65,0x78,0x74,0x75,0x72,0x65,0x31,0x5f,0x73,0x6d,0x70,0x2c,0x20,
    0x54,0x65,0x78,0x43,0x6f,0x6f,0x72,0x64,0x29,0x2c,0x20,0x74,0x65,0x78,0x74,0x75,
    0x72,0x65,0x28,0x5f,0x74,0x65,0x78,0x74,0x75,0x72,0x65,0x32,0x5f,0x74,0x65,0x78,
    0x74,0x75,0x72,0x65,0x32,0x5f,0x73,0x6d,0x70,0x2c,0x20,0x54,0x65,0x78,0x43,0x6f,
    0x6f,0x72,0x64,0x29,0x2c,0x20,0x76,0x65,0x63,0x34,0x28,0x30,0x2e,0x35,0x29,0x29,
    0x3b,0x0a,0x7d,0x0a,0x0a,0x00,
};
static inline const sg_shader_desc* simple_shader_desc(sg_backend backend) {
    if (backend == SG_BACKEND_GLCORE) {
        static sg_shader_desc desc;
        static bool valid;
        if (!valid) {
            valid = true;
            desc.vertex_func.source = (const char*)vs_source_glsl430;
            desc.vertex_func.entry = "main";
            desc.fragment_func.source = (const char*)fs_source_glsl430;
            desc.fragment_func.entry = "main";
            desc.attrs[0].base_type = SG_SHADERATTRBASETYPE_FLOAT;
            desc.attrs[0].glsl_name = "aPos";
            desc.attrs[1].base_type = SG_SHADERATTRBASETYPE_FLOAT;
            desc.attrs[1].glsl_name = "aTexCoord";
            desc.uniform_blocks[0].stage = SG_SHADERSTAGE_VERTEX;
            desc.uniform_blocks[0].layout = SG_UNIFORMLAYOUT_STD140;
            desc.uniform_blocks[0].size = 192;
            desc.uniform_blocks[0].glsl_uniforms[0].type = SG_UNIFORMTYPE_FLOAT4;
            desc.uniform_blocks[0].glsl_uniforms[0].array_count = 12;
            desc.uniform_blocks[0].glsl_uniforms[0].glsl_name = "vs_params";
            desc.images[0].stage = SG_SHADERSTAGE_FRAGMENT;
            desc.images[0].image_type = SG_IMAGETYPE_2D;
            desc.images[0].sample_type = SG_IMAGESAMPLETYPE_FLOAT;
            desc.images[0].multisampled = false;
            desc.images[1].stage = SG_SHADERSTAGE_FRAGMENT;
            desc.images[1].image_type = SG_IMAGETYPE_2D;
            desc.images[1].sample_type = SG_IMAGESAMPLETYPE_FLOAT;
            desc.images[1].multisampled = false;
            desc.samplers[0].stage = SG_SHADERSTAGE_FRAGMENT;
            desc.samplers[0].sampler_type = SG_SAMPLERTYPE_FILTERING;
            desc.samplers[1].stage = SG_SHADERSTAGE_FRAGMENT;
            desc.samplers[1].sampler_type = SG_SAMPLERTYPE_FILTERING;
            desc.image_sampler_pairs[0].stage = SG_SHADERSTAGE_FRAGMENT;
            desc.image_sampler_pairs[0].image_slot = 0;
            desc.image_sampler_pairs[0].sampler_slot = 0;
            desc.image_sampler_pairs[0].glsl_name = "_texture1_texture1_smp";
            desc.image_sampler_pairs[1].stage = SG_SHADERSTAGE_FRAGMENT;
            desc.image_sampler_pairs[1].image_slot = 1;
            desc.image_sampler_pairs[1].sampler_slot = 1;
            desc.image_sampler_pairs[1].glsl_name = "_texture2_texture2_smp";
            desc.label = "simple_shader";
        }
        return &desc;
    }
    return 0;
}
