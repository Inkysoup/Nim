#version 330

out vec4 finalColor;

uniform float time;
uniform float screenMagnitude;

uniform vec3 colour1;
uniform vec3 colour2;

#define STRIPE_WIDTH screenMagnitude * 0.05

float sdInfiniteLineNormal(in vec2 p, in vec2 a, in vec2 n) {
    return dot(p - a, n);
}

void main(){
    vec2 uv = gl_FragCoord.xy;

    uv.x = uv.x + time * STRIPE_WIDTH * -0.5;

    float dist = mod(sdInfiniteLineNormal(uv.xy, vec2(0.0,0.0), vec2(-1.0,1.0)), STRIPE_WIDTH*2.0);
    dist = abs(dist - STRIPE_WIDTH)-STRIPE_WIDTH/2.0; // shift so boundaries are not at edges
    float change = fwidth(dist);
    float alpha = 1.0 - smoothstep(0.0, change, dist);

    vec3 colour = mix(colour1, colour2, alpha);

    // float stripe = mod((uv.x + uv.y) * 20.0, 1.0);
    // vec3 colour = mix(colour1, colour2, step(0.5, stripe));

    finalColor = vec4(colour, 1.0);
}