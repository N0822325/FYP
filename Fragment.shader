#version 300 es
precision mediump float;

out vec4 colour;

in vec4 outColour;
in vec2 vTexCoord;

uniform bool isTextured;
uniform sampler2D u_Texture;
uniform vec4 u_Colour;

void main()
{
    if(isTextured){ colour = texture(u_Texture, vTexCoord) * outColour * u_Colour; }
    else { colour = outColour * u_Colour; }

    //vec4 texColour = texture(u_Texture, vTextCoord);
    //colour = texColour; //* vec4(outColour, 1.0)
};
