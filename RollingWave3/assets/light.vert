//light.vert
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform mat4 textureMatrix;

attribute vec3 position;
attribute vec4 colour;
attribute vec3 normal;
attribute vec2 texCoord;

varying vec4 colourOUT;
varying vec4 texCoordOUT;
varying vec4 normalOUT;
varying vec4 worldPosOUT;


void main(void)	{

	colourOUT 	=colour;
	texCoordOUT	= (textureMatrix * vec4(texCoord, 0.0, 1.0)).xy;
 	
	mat3 normalMatrix = transpose(inverse(mat3(modelMatrix)));
	normalOUT	= normalize(normalMatrix * normalize(normal));
	worldPosOUT	= (modelMatrix * vec4(position,1)).xyz;
	gl_Position 	= (projMatrix * viewMatrix * modelMatrix) * vec4(position, 1.0);
}

