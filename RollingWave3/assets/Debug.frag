precision highp float;
uniform sampler2D diffuseTex;

varying vec4 colourOUT;

void main(void)	{
	gl_FragColor = colourOUT;
}