
uniform vec3 u_camera_local;
uniform sampler3D u_volume;

varying vec3 v_world_position;
varying vec3 v_position;

bool isIn(vec3 pos){
	//vec3 v= step(vec3(-1.0,-1.0,-1.0),pos)-step(vec3(1.0,1.0,1.0),pos);
	
	return (any(lessThan(pos,vec3(-1.0,-1.0,-1.0))) || any(greaterThan(pos,vec3(1.0,1.0,1.0))));
}

void main()
{
	float raystep=0.001;
	vec3 raydir=normalize(v_position-u_camera_local)*raystep;
	vec3 samplepos=v_position;
	vec4 samplecolor;
	float d;
	vec4 finalcolor;
	int i =0;

	while(!isIn(samplepos)){
	d=texture3D(u_volume,(samplepos+1.0)/2.0).x;
	samplecolor=vec4(d);
	finalcolor+=raystep*(1.0 - finalcolor.a)*samplecolor;


	if(finalcolor.a>=1||i>100000)break;
	samplepos+=raydir;
	}






	gl_FragColor = (finalcolor*0.9)+0.1;
}
