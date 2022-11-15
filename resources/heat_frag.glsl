#version 410 core

in vec3 camSpacePosition;
in vec3 camSpaceNormal;
in float utv;
in float phiv;

out vec4 out_fragColor;

uniform vec3 lightCamSpacePosition;
uniform vec3 lightColor;
uniform vec3 materialDiffuse;
uniform float materialShininess; 

void main(void) {
	
	vec3 v = normalize(-camSpacePosition);
	vec3 n = normalize(camSpaceNormal);
	vec3 l = normalize(lightCamSpacePosition - camSpacePosition);

	// TODO: 4, 7, 11 Implement your GLSL per fragement lighting, heat colouring, and distance stripes here!
    
    //specular lighting
    vec3 ks = vec3( 1.0, 1.0, 1.0 );
    vec3 halfVector = normalize( l + v );
    float specular = max(0, dot(n, halfVector) );
    specular = pow(specular, materialShininess );
    vec3 reflectedLight = ks * lightColor * specular;
    
    

	// can use this to initially visualize the normal	
    out_fragColor =  clamp( vec4( n.xyz * 0.5 + vec3( 0.5, 0.5,0.5 ) + reflectedLight, 0.8 + clamp(utv + phiv, 0, 0) ), 0, 1 );
}
