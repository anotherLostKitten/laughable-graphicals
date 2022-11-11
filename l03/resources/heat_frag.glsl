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

  // TODO: 11 Implement your GLSL distance stripes here!

  float diffuse=max(dot(n,v),0);
  float specular=0.0;
  if(diffuse>0.0){
	vec3 hv=normalize(l+v);
	specular=max(0,dot(n,hv));
	specular=pow(specular,materialShininess);
  }
  vec3 diffLight=clamp(vec3(141+110*utv,205-50*utv,197-22*utv)/256,0,1)*diffuse;
  vec3 specLight=lightColor*specular;

  out_fragColor=clamp(vec4(diffLight+specLight,1),0,1);
}
