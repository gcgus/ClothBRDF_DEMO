//this var comes from the vertex shader
//they are baricentric interpolated by pixel according to the distance to every vertex
varying vec3 v_position;
varying vec3 v_world_position;
varying vec3 v_normal;
varying vec2 v_uv;

//here create uniforms for all the data we need here
uniform vec3 ambientLight, diffuseLight, specularLight;
uniform vec3 lightPosition;
uniform float shininess;
uniform vec3 u_camera_position;
uniform sampler2D u_texture;

void main()
{  
    vec3 L = normalize( lightPosition - v_world_position );
    vec3 N = normalize(v_normal);
    vec3 R = normalize( reflect(-L,N) );
    vec3 V = normalize( u_camera_position - v_world_position );
    
    float NdotL = max(0.0, dot(N,L));
    float VdotR = pow( max(0.0, dot(V,R)), shininess );
    vec3 ambient =  ambientLight; 
    vec3 specular =  VdotR * specularLight;
    vec3 diffuse =  NdotL * diffuseLight;
  
  	vec2 uv = v_uv;
	
    gl_FragColor = texture2D( u_texture, uv) * vec4( ambient + diffuse + specular ,1.0);
}