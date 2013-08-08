precision highp float;
uniform sampler2D diffuseTex;

varying lowp vec2 texCoordOUT;
varying lowp vec3 normalOUT;

varying vec3 worldPosOUT;

void main ( void ) {
	lowp vec4 outputColor = vec4(1,0,0,1);
	//if(worldPosOUT.y >= 180.0){
	//	outputColor = vec4(1,1,1,1);
	//} else if(worldPosOUT.y <180.0 && worldPosOUT.y > 110.0){
	//	float alpha = (worldPosOUT.y-110.0)/70.0;
	//	outputColor = mix(vec4(1,1,1,1),vec4(0,1,0,1),1.0-alpha);
	//}else if(worldPosOUT.y <= 110.0 && worldPosOUT.y > 70.0){
	//	outputColor = vec4(0,1,0,1);
	//}else if (worldPosOUT.y <=70.0 && worldPosOUT.y > 30.0){
	//	float alpha = (worldPosOUT.y-30.0)/40.0;
	//	outputColor = mix( vec4(0,1,0,1),vec4(0.93,0.788,0.686,1),1.0-alpha);
	//} else {
	//	outputColor = vec4(0.93,0.788,0.686,1);
	//}
	
	vec3 cameraPos = vec3(0,500,0);
	vec4 lightColour = vec4(1,1,1,1);
	vec3 lightPos = vec3(2000,500,-1500);
	float lightRadius = 4000.0;
	
	vec3 incident	= normalize(lightPos - worldPosOUT);
	float lambert 	= max(0.0, dot(incident, normalOUT));

	float dist 		= length(lightPos - worldPosOUT);
	float atten 	= 1.0 - clamp(dist / lightRadius, 0.0, 1.0);

	vec3 viewDir 	= normalize(cameraPos - worldPosOUT);
	vec3 halfDir	= normalize(incident + viewDir);

	float rFactor	= max(0.0, dot(halfDir, normalOUT));
	float sFactor	= pow(rFactor, 50.0);
	
	vec3 colour 	=   (outputColor.rgb * lightColour.rgb);
	colour 			+= (lightColour.rgb *sFactor) * 0.33;
	outputColor 	= vec4(colour * atten * lambert, 1);
	outputColor.rgb 	+= (outputColor.rgb * lightColour.rgb) * 0.1;	
	
	gl_FragColor = outputColor;

}