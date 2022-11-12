#version 410 core
// theodore peters 260919785

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

  float diffuse=max(dot(n,v),0);
  float specular=0.0;
  if(diffuse>0.0){
	vec3 hv=normalize(l+v);
	specular=max(0,dot(n,hv));
	specular=pow(specular,materialShininess);
  }
  float rphiv=mod(clamp(70.*phiv,.15,1024.),1.);
  float smoothRing=smoothstep(-0.1,0.,-rphiv)+smoothstep(0.9,1.,rphiv);
  vec3 diffLight=clamp(vec3(2*(utv-0.5),0,2*(0.5-utv))+vec3(0.1,smoothRing,0.1),0,1)*diffuse;
  vec3 specLight=lightColor*specular;

  out_fragColor=clamp(vec4(diffLight+specLight,1),0,1);
}
