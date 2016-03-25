#version 420
//#extension GL_EXT_gpu_shader4 : enable

uniform sampler2D tex;

layout ( binding = 0 ) uniform RayMarchingParam{
    vec3 iResolution;
    float iGlobalTime;
    vec4 iMouse;
};

in vec2 texCoord;
out vec4 fragColor;

#define R(p, a) p=cos(a)*p+sin(a)*vec2(p.y, -p.x)

float maxcomp( in vec3 p ){
        return max(p.x, max(p.y, p.z));
}

float sdSphere( vec3 p, float r ) {
    return length( p ) - r;
}

float sdBox( vec3 p, vec3 b, float r ){
    vec3 d = abs(p) - b;
    return min(maxcomp(d), 0.0) - r + length(max(d, 0.0));
}

float map( vec3 p ){
    
    //R(p.xz, iGlobalTime );
    //R(p.xy, iGlobalTime );
    return sdBox( p, vec3(40, 40, 40), 0 );
    //return sdSphere( p, 90.0);
}

vec3 GetCameraRay( vec3 ro, vec3 ra, vec2 fragCoord ){
    float fRation = iResolution.x / iResolution.y;
    
    vec3 f = normalize( ra - ro );
    vec3 vUp = vec3( 0.0, 1.0, 0.0);
    vec2 VVc = (texCoord - 0.5)/vec2(1.0, iResolution.z);
    
    vec3 r = normalize( cross(f, vUp) );
    vUp = cross( r, f );
    vec3 rd = normalize( r * VVc.x + vUp * VVc.y + f );
    return rd;
}

void main()
{
    float pi = 3.1415926;
    
    float theta = -pi * ( iMouse.x / iResolution.x  - 0.5 ) * 2.0;
    float theta1 = 0.5 * pi * ( 1.0 - iMouse.y / iResolution.y - 0.5)  * 2.0;
    
    vec3 ro = -200.0 * normalize( vec3( sin(theta) * cos(theta1), sin(theta1), cos(theta) * cos(theta1) )  );
    //vec3 rd = normalize( vec3( (texCoord - 0.5)/vec2(1.0, iResolution.z), .5 ) );
    vec3 rd = GetCameraRay( ro, vec3( 0.0 ), texCoord);
    
    float d = -1;
    float t = 0.01;
    vec4 color = vec4(0.0,0.0,0.0,1.0);
    for( int i = 0; i < 256; ++i )
    {
        vec3 p = ro + t * rd;
        d = map( p );
        if( d < 0.001 )
        {
            color = vec4( 1.0, 0.0, 0.0, 1.0 );
            break;
        }
        
        t += d;
    }
    fragColor  = color;
    
    
}