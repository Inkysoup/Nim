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
uniform float burnProgress; // 0 to 1

uniform float time;
uniform float fireRadius;
uniform vec4 fireColour1;
uniform vec4 fireColour2;


#define PI 3.14159265359


#define MAX_TILES 64 // value must be above what is required
struct Tile{
    vec2 headCentre;
    int isMarked; // 0 == false, 1 == true
};
uniform Tile tiles[MAX_TILES];
uniform int matchCount;

#define STICK_COLOUR vec3(0.9, 0.8, 0.7)
#define HEAD_COLOUR vec3(0.9, 0.3, 0.3)

// sdf functions from https://iquilezles.org/articles/ //
float opSmoothUnion( float a, float b, float k )
{
    k *= 4.0;
    float h = max(k-abs(a-b),0.0);
    return min(a, b) - h*h*0.25/k;
}
float opUnion( float a, float b ){
  return min(a,b);
}
float opSubtraction( float a, float b ){
  return max(-a,b);
}
float opIntersection( float a, float b ){
  return max(a,b);
}
float sdCircle(vec2 p, float r){
    return length(p) - r;
}
float sdSegment(in vec2 p, in vec2 a, in vec2 b){
    vec2 pa = p-a, ba = b-a;
    float h = clamp( dot(pa,ba)/dot(ba,ba), 0.0, 1.0 );
    return length( pa - ba*h );
}
float sdUnevenCapsule(vec2 p, float r1, float r2, float h){
    p.x = abs(p.x);
    float b = (r1-r2)/h;
    float a = sqrt(1.0-b*b);
    float k = dot(p,vec2(-b,a));
    if( k < 0.0 ) return length(p) - r1;
    if( k > a*h ) return length(p-vec2(0.0,h)) - r2;
    return dot(p, vec2(a,b) ) - r1;
}
// --------------------------------------------------- // 

// https://github.com/ashima/webgl-noise/blob/master/src/noise2D.glsl, returns range -1.0 to 1.0 //
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

float easeIn(float t){
  return t * t;
}


void main(){
    vec4 colour = vec4(0.0);
    float noise = (snoise(gl_FragCoord.xy/(50.0*outlineThickness) + time)/2.0f + 0.5) * outlineThickness * 3.0f;

    bool isFirstSelected = true;
    float totalFireOutline;
    float totalFireFill1;
    float totalFireFill2;
    float matchCutouts;

    for(int i = 0; i < matchCount; i++){
        // match sdf
        float burnHeight = tiles[i].isMarked * stickLength * easeIn(burnProgress); // shrink burning matches
        float delayedBurnProgress = clamp((burnProgress - 0.5) / 0.5, 0.0, 1.0);

        vec2 segV1 = tiles[i].headCentre;
        segV1.y += burnHeight;
        vec2 segV2 = vec2(segV1.x, segV1.y + stickLength - burnHeight);
        float segment = sdSegment(gl_FragCoord.xy, segV1, segV2) - stickRadius;
        vec2 uv = gl_FragCoord.xy - segV1;
        float circle = sdCircle(uv, headRadius);
        circle += tiles[i].isMarked * headRadius * easeIn(burnProgress); // shrink head at start of burning
        float match = opSmoothUnion(circle, segment, outlineThickness);
        match += tiles[i].isMarked * delayedBurnProgress * stickRadius;

        // outline
        float matchOutline = match - outlineThickness;
        float change = fwidth(matchOutline);
        float alpha = 1.0 - smoothstep(-change, change, matchOutline);
        colour = mix(colour, vec4(vec3(0.0), 1.0), alpha);
        
        // fill
        change = fwidth(match);
        alpha = 1.0 - smoothstep(-change, change, match);
        float headThreshold = segV1.y + headRadius*2.0;
        float factor = step(headThreshold, gl_FragCoord.y);
        vec3 fillColour = mix(HEAD_COLOUR, STICK_COLOUR, factor);
        fillColour = mix(fillColour, vec3(0.0), burnProgress * tiles[i].isMarked); // charing
        colour = mix(colour, vec4(fillColour, 1.0), alpha);

        if(tiles[i].isMarked == 1){
            // fire sdf
            float fireHeight = pow(fireRadius * fireRadius * 2.0, 0.5);
            float fire1 = sdUnevenCapsule(vec2(uv.x, uv.y + fireHeight), 0.0, fireRadius, fireHeight);
            float localNoise = length(uv)/fireRadius * noise;
            fire1 += localNoise;
            fire1 += tiles[i].isMarked * fireRadius * delayedBurnProgress; // shrink fire towards end

            // outline
            float fireOutline = fire1 - outlineThickness;

            // fill 2
            float fire2 = fire1 + outlineThickness*2.0;

            float matchCutout = opIntersection(fire2, matchOutline);
            if(isFirstSelected){
              totalFireOutline = fireOutline;
              totalFireFill1 = fire1;
              totalFireFill2 = fire2;
              isFirstSelected = false;
              matchCutouts = matchCutout;
            }
            else{
              totalFireOutline = opSmoothUnion(totalFireOutline, fireOutline, outlineThickness*2.0);
              totalFireFill1 = opSmoothUnion(totalFireFill1, fire1, outlineThickness*2.0);
              totalFireFill2 = opSmoothUnion(totalFireFill2, fire2, outlineThickness*2.0);
              matchCutouts = opUnion(matchCutouts, matchCutout);
            }
        }
       
    }

    if(!isFirstSelected){
      // fire outline
      totalFireOutline = opSubtraction(matchCutouts, totalFireOutline);
      float change = fwidth(totalFireOutline);
      float alpha = 1.0 - smoothstep(-change, change, totalFireOutline);
      colour = mix(colour, vec4(vec3(0.0), 1.0), alpha);

      // fire fill1
      totalFireFill1 = opSubtraction(matchCutouts, totalFireFill1);
      change = fwidth(totalFireFill1);
      alpha = 1.0 - smoothstep(-change, change, totalFireFill1);
      colour = vec4(mix(colour.rgb, fireColour1.rgb, alpha*fireColour1.a), colour.a + alpha*fireColour1.a);

      // fire fill2
      totalFireFill2 = opSubtraction(matchCutouts, totalFireFill2);
      change = fwidth(totalFireFill2);
      alpha = 1.0 - smoothstep(-change, change, totalFireFill2);
      colour = vec4(mix(colour.rgb, fireColour2.rgb, alpha*fireColour2.a), colour.a + alpha*fireColour2.a);
    }

    finalColor = colour;
}
