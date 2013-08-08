//mainball.vert
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform mat4 textureMatrix;

uniform float nodeColour;

attribute vec3 position;
attribute vec4 colour;
attribute vec2 texCoord;

varying vec4 colourOUT;
varying vec2 texCoordOUT;

void main(void) {
	mat4 mvp = projMatrix *viewMatrix * modelMatrix;
	gl_Position = mvp * vec4(position, 1.0);
	colourOUT = colour;
	texCoordOUT = ( textureMatrix * vec4(texCoord, 0.0, 1.0)).xy;
}