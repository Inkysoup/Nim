#version 330

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;
in vec4 fragColor;

// Input uniform values
uniform sampler2D texture0;
uniform vec4 colDiffuse;

// Output fragment color
out vec4 finalColor;

// NOTE: Add your custom variables here
#define MAX_OUTLINES 3
struct Outline{
    float expansionFract;
    vec4 colour;
};
uniform Outline outlines[MAX_OUTLINES+1];


void main()
{
    // Texel color fetching from texture sampler
    // NOTE: Calculate alpha using signed distance field (SDF)

    vec4 colour = vec4(0.0);
    for(int i = 0; i < MAX_OUTLINES+1; i++){
        if(outlines[i].colour == vec4(0.0)){
            //continue;
            break;
        }
        float distanceFromOutline = texture(texture0, fragTexCoord).a - 0.475 + outlines[i].expansionFract;
        float distanceChangePerFragment = length(vec2(dFdx(distanceFromOutline), dFdy(distanceFromOutline)));
        float alpha = smoothstep(-distanceChangePerFragment, distanceChangePerFragment, distanceFromOutline);
        
        if(colour == vec4(0.0)){
            colour = vec4(outlines[i].colour.rgb, alpha);
        }
        else{
            colour = mix(colour, outlines[i].colour, alpha);
        }
        //colour = vec4(vec3(i*10), 1.0);
    }

    

    // Calculate final fragment color
    finalColor = vec4(colour.rgb, colour.a * fragColor.a);
}