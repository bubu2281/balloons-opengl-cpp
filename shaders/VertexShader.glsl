#version 330

// Input
layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec2 v_texture_coord;
layout(location = 3) in vec3 v_color;

// Uniform properties
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;
uniform sampler2D texture1;

// Output
out vec2 texcoord;
out vec4 debugColor;

uniform int object_is_terrain;
flat out int is_terrain;

void main()
{
    vec3 displaced_position = v_position;

    if (object_is_terrain == 0) {
        if (displaced_position.y < 0.0)
        {
            displaced_position.y *= (1.0 + abs(displaced_position.y) * 0.9);  
        }
        else
        {
            displaced_position.x *= 1.1;  
            displaced_position.z *= 1.1;    
        }
    } else {
        vec2 uv = v_position.xz + (0.5, 0.5);
        vec4 texColor = texture(texture1, uv);
        displaced_position.y += texColor.x * 5;
        is_terrain = 1;
       // debugColor = texColor;
    }



    gl_Position = Projection * View * Model * vec4(displaced_position, 1.0);

    texcoord = v_texture_coord;
}
