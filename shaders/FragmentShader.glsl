#version 330 core

out vec4 FragColor;
in vec4 debugColor;


// Uniforme
uniform sampler2D texture1; 
uniform sampler2D texture2; 
uniform sampler2D texture3; 
uniform vec3 color; 

in vec2 texcoord;
flat in int is_terrain;
uniform int object_is_terrain;


void main()
{
    vec4 texColor1 = texture(texture1, texcoord * 15.0f);
    vec4 texColor2 = texture(texture2, texcoord * 15.0f);
    vec4 texColor3 = texture(texture3, texcoord * 15.0f);
    
    vec4 texColor;
    if (object_is_terrain == 0) {
        texColor = texture(texture1, texcoord); 
    } else {

        vec4 color = texture(texture1, texcoord); 
        float height = color.x;

        if (height > 0.1f && height < 0.5f) {
            
            texColor = mix(texColor2, texColor3, height * 2); 

        }
        if (height >= 0.5f){
            
            texColor = texColor3; 
        
        }
        if (height <= 0.1f){
            
            texColor = texColor2; 
        
        }



    }


        
    FragColor = texColor; 
}
