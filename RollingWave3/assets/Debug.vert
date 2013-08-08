uniform mat4 viewProjMatrix;

attribute vec3 position;
attribute vec3 colour;

varying vec4 colourOUT;

void main(void)	{
	gl_Position	  = viewProjMatrix * vec4(position, 1.0);
	colourOUT    = vec4(colour,1);
}