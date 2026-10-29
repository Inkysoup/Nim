#version 330

// Input vertex attributes (from vertex shader)
layout(origin_upper_left) in vec4 gl_FragCoord;
in vec2 fragTexCoord;
in vec4 fragColor;

// Output fragment color
out vec4 finalColor;

// Custom variable
uniform float fireRadius;
uniform float time;
uniform vec4 fireColour1;
uniform vec4 fireColour2;
uniform float outlineThickness;

#define MAX_SELECTED 15 // value must be above what is required (15 is the full bottom row if 64 matches are used)
uniform vec2 headCentres[MAX_SELECTED];
uniform int selectedCount;


float sdUnevenCapsule(vec2 p, float r1, float r2, float h){
    p.x = abs(p.x);
    float b = (r1-r2)/h;
    float a = sqrt(1.0-b*b);
    float k = dot(p,vec2(-b,a));
    if( k < 0.0 ) return length(p) - r1;
    if( k > a*h ) return length(p-vec2(0.0,h)) - r2;
    return dot(p, vec2(a,b) ) - r1;
}


// copied from https://github.com/ashima/webgl-noise/blob/master/src/noise2D.glsl, returns range -1.0 to 1.0 //
vec3 mod289(vec3 x) {
  return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec2 mod289(vec2 x) {
  return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec3 permute(vec3 x) {
  return mod289(((x*34.0)+10.0)*x);
}

float snoise(vec2 v)
  {
  const vec4 C = vec4(0.211324865405187,  // (3.0-sqrt(3.0))/6.0
                      0.366025403784439,  // 0.5*(sqrt(3.0)-1.0)
                     -0.577350269189626,  // -1.0 + 2.0 * C.x
                      0.024390243902439); // 1.0 / 41.0
// First corner
  vec2 i  = floor(v + dot(v, C.yy) );
  vec2 x0 = v -   i + dot(i, C.xx);

// Other corners
  vec2 i1;
  //i1.x = step( x0.y, x0.x ); // x0.x > x0.y ? 1.0 : 0.0
  //i1.y = 1.0 - i1.x;
  i1 = (x0.x > x0.y) ? vec2(1.0, 0.0) : vec2(0.0, 1.0);
  // x0 = x0 - 0.0 + 0.0 * C.xx ;
  // x1 = x0 - i1 + 1.0 * C.xx ;
  // x2 = x0 - 1.0 + 2.0 * C.xx ;
  vec4 x12 = x0.xyxy + C.xxzz;
  x12.xy -= i1;

// Permutations
  i = mod289(i); // Avoid truncation effects in permutation
  vec3 p = permute( permute( i.y + vec3(0.0, i1.y, 1.0 ))
		+ i.x + vec3(0.0, i1.x, 1.0 ));

  vec3 m = max(0.5 - vec3(dot(x0,x0), dot(x12.xy,x12.xy), dot(x12.zw,x12.zw)), 0.0);
  m = m*m ;
  m = m*m ;

// Gradients: 41 points uniformly over a line, mapped onto a diamond.
// The ring size 17*17 = 289 is close to a multiple of 41 (41*7 = 287)

  vec3 x = 2.0 * fract(p * C.www) - 1.0;
  vec3 h = abs(x) - 0.5;
  vec3 ox = floor(x + 0.5);
  vec3 a0 = x - ox;

// Normalise gradients implicitly by scaling m
// Approximation of: m *= inversesqrt( a0*a0 + h*h );
  m *= 1.79284291400159 - 0.85373472095314 * ( a0*a0 + h*h );

// Compute final noise value at P
  vec3 g;
  g.x  = a0.x  * x0.x  + h.x  * x0.y;
  g.yz = a0.yz * x12.xz + h.yz * x12.yw;
  return 130.0 * dot(m, g);
}
// ------------------------------------------------------------------------------------------------------ //

void main(){
    vec4 colour = vec4(0.0);
    float noise = (snoise(gl_FragCoord.xy/(50.0*outlineThickness) + time)/2.0f + 0.5) * outlineThickness * 3.0f;

    for(int i = 0; i < selectedCount; i++){
        // calculating sdf
        vec2 centre = headCentres[i];
        float fireHeight = sqrt(fireRadius*fireRadius*2.0);
        vec2 uv = gl_FragCoord.xy - centre;
        float fire = sdUnevenCapsule(vec2(uv.x, uv.y + fireHeight), 0.0, fireRadius, fireHeight);
        float localNoise = length(uv)/fireRadius * noise;
        fire += localNoise;


        // outline
        float outline = abs(fire) - outlineThickness;
        float change = fwidth(outline);
        float alpha = 1.0 - smoothstep(-change, change, outline);
        colour = mix(colour, vec4(vec3(0.0), 1.0), alpha);

        // fill 1
        change = fwidth(fire);
        alpha = 1.0 - smoothstep(-change, change, fire);
        colour = vec4(mix(colour.rgb, fireColour1.rgb, alpha*fireColour1.a), colour.a + alpha*fireColour1.a);
        // fill 2
        float fire2 = fire + outlineThickness*2.0;
        change = fwidth(fire2);
        alpha = 1.0 - smoothstep(-change, change, fire2);
        colour = vec4(mix(colour.rgb, fireColour2.rgb, alpha*fireColour2.a), colour.a + alpha*fireColour2.a);
    }
    
    finalColor = colour;
}
