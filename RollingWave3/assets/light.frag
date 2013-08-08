//light.frag

precision highp float;
uniform sampler2D diffuseTex;

uniform vec3 cameraPos;
uniform vec4 lightColour;
uniform vec3 lightPos;
uniform float lightRadius;


varying	vec3 colourOUT;
varying	vec2 texCoordOUT;
varying	vec3 normalOUT;
varying	vec3 worldPosOUT;


void main(void) {

	vec4 diffuse	= texture2D(diffuseTex, texCoordOUT);
	vec3 incident	= normalize(lightPos - worldPosOUT);
	float lambert 	= max(0.0, dot(incident, normalOUT));

	float dist 		= length(lightPos - worldPosOUT);
	float atten 	= 1.0 - clamp(dist / lightRadius, 0.0, 1.0);

	vec3 viewDir 	= normalize(cameraPos - worldPosOUT);
	vec3 halfDir	= normalize(incident + viewDir);

	float rFactor	= max(0.0, dot(halfDir, normalOUT));
	float sFactor	= pow(rFactor, 50.0);
	
	vec3 colour 	=   (diffuse.rgb * lightColour.rgb);
	colour 		+= (lightColour.rgb *sFactor) * 0.33;
	gl_FragColor 	= vec4(colour * atten * lambert, diffuse.a);
	gl_FragColor.rgb 	+= (diffuse.rgb * lightColour.rgb) * 0.1;
}


