precision highp float;

uniform sampler2D diffuseTex;
uniform int 	 useTexture;

varying	vec2 texCoordOUT;
varying	vec4 colourOUT;


void main(void) {
	gl_FragColor  = colourOUT;
	if(useTexture > 0) {
		gl_FragColor  *= texture2D(diffuseTex, texCoordOUT);
	}
	//gl_FragColor  = vec4(1);

	//gl_FragColor.a = 1;

	//gl_FragColor.rg = IN.texCoord;
}