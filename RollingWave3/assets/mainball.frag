//mainball.frag
precision highp float;
uniform sampler2D diffuseTex;

varying vec2 texCoordOUT;
varying vec4 colourOUT;
void main(void)
{
	//gl_FragColor = texture2D(diffuseTex, texCoordOUT);
	//gl_FragColor = vec4(1.0, 1.0, 0.0, 1.0);
	gl_FragColor = vec4(texCoordOUT, 0.0, 1.0);

}

