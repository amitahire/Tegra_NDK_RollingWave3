uniform mat4 modelMatrix;
attribute vec3 position;
attribute vec4 a_colour;

varying vec4 colourOUT;					
void main(void)
{											
	gl_Position = vec4(position, 1.0);		
	colourOUT = a_colour;
} 