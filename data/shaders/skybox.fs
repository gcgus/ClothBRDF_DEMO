uniform vec3 u_camera_center;
uniform samplerCube u_cube;
uniform vec4 colorSkybox;

varying vec3 v_world_position;

vec3 toneMap(vec3 color){
    return color / (color + vec3(1.0));
}

vec3 gamma(vec3 c){
return pow(c,vec3(1.0/2.2));}

void main()
{
    colorSkybox = textureCube(u_cube, normalize(v_world_position-u_camera_center));
    colorSkybox.rgb = gamma(toneMap(colorSkybox.rgb));
  
	gl_FragColor = colorSkybox;
}
