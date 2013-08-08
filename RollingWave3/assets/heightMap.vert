uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform mat4 textureMatrix;


attribute vec2 texCoord;
attribute vec3 position;
attribute vec3 normal;

varying vec2 texCoordOUT;
varying vec3 normalOUT;
varying vec3 worldPosOUT;

void main (void) {

	mat4 mvp = projMatrix * viewMatrix * modelMatrix ;
	gl_Position = mvp * vec4 ( position , 1.0);
	texCoordOUT = ( textureMatrix * vec4 ( texCoord , 0.0 , 1.0)). xy;
	normalOUT = normal;	
	
	worldPosOUT	= (modelMatrix * vec4(position,1)).xyz;
}