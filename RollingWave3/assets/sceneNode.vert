uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform vec4 nodeColour;

attribute vec3 position;
attribute vec2 texCoord;

varying vec2 texCoordOUT;
varying vec4 colourOUT;


void main(void)	{
	gl_Position	= (projMatrix  * viewMatrix * modelMatrix) * vec4(position, 1.0);
	texCoordOUT	= texCoord;
	colourOUT	= nodeColour;
}
