uniform vec3 u_camera_eye;
uniform samplerCube u_cube;

varying vec3 v_world_position;
varying vec3 v_normal;

void main()
{
	gl_FragColor = textureCube(u_cube, reflect(normalize(v_world_position-u_camera_eye),v_normal));
}
