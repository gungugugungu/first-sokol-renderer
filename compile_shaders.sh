#!/bin/bash

find . -type f -name "*.glsl" | while read -r shader_file; do
    # shader directory
    dir_path=$(dirname "$shader_file")
    # base name of the shader file
    base_name=$(basename "$shader_file")
    # output file name
    output_file="$dir_path/${base_name}.h"

    echo "Compiling: $shader_file -> $output_file"
    ./sokol-shdc --input "$shader_file" --output "$output_file" --slang glsl430
done