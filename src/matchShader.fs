#version 330

// Input vertex attributes (from vertex shader)
layout(origin_upper_left) in vec4 gl_FragCoord;
in vec2 fragTexCoord;
in vec4 fragColor;

// Output fragment color
out vec4 finalColor;

// Custom variable
uniform float headRadius;
uniform float stickRadius;
uniform float stickLength;
uniform float outlineThickness;

#define MAX_TILES 64 // value must be above what is required
uniform vec2 headCentres[MAX_TILES];
uniform int matchCount;

#define STICK_COLOUR vec3(0.9, 0.8, 0.7)
#define HEAD_COLOUR vec3(0.9, 0.3, 0.3)


float opSmoothUnion( float a, float b, float k )
{
    k *= 4.0;
    float h = max(k-abs(a-b),0.0);
    return min(a, b) - h*h*0.25/k;
}
float sdCircle(vec2 p, float r){
    return length(p) - r;
}
float sdSegment(in vec2 p, in vec2 a, in vec2 b){
    vec2 pa = p-a, ba = b-a;
    float h = clamp( dot(pa,ba)/dot(ba,ba), 0.0, 1.0 );
    return length( pa - ba*h );
}


void main(){
    vec4 colour = vec4(0.0);

    for(int i = 0; i < matchCount; i++){
        // calculating sdf
        vec2 segV1 = headCentres[i];
        vec2 segV2 = vec2(segV1.x, segV1.y + stickLength);
        float segment = sdSegment(gl_FragCoord.xy, segV1, segV2) - stickRadius;
        float circle = sdCircle(gl_FragCoord.xy - segV1, headRadius);
        float match = opSmoothUnion(circle, segment, 2.0);

        // outline
        float outline = match - outlineThickness;
        float change = fwidth(outline);
        float alpha = 1.0 - smoothstep(-change, change, outline);
        colour = mix(colour, vec4(vec3(0.0), 1.0), alpha);
        
        // fill
        change = fwidth(match);
        alpha = 1.0 - smoothstep(-change, change, match);
        float headThreshold = segV1.y + headRadius*2.0;
        float factor = step(headThreshold, gl_FragCoord.y);
        vec3 fillColour = mix(HEAD_COLOUR, STICK_COLOUR, factor);
        colour = mix(colour, vec4(fillColour, 1.0), alpha);
    }

    finalColor = colour;
}
