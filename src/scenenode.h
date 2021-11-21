#ifndef SCENENODE_H
#define SCENENODE_H

#include "framework.h"

#include "shader.h"
#include "mesh.h"
#include "camera.h"
#include "material.h"
#include "volume.h"



class SceneNode {
public:

	static unsigned int lastNameId;

	SceneNode(bool sky);
	SceneNode(const char* name);
	~SceneNode();

	Material * material = NULL;
	std::string name;

	Mesh* mesh = NULL;
	Matrix44 model;

	Material* mats[5];


	virtual void render(Camera* camera);
	virtual void renderWireframe(Camera* camera);
	virtual void renderInMenu();
};

class SkyBox : public SceneNode {
public:
	Texture* hdre[5];
	
	SkyBox(bool sky) :SceneNode(sky) {
		sky = TRUE;
		SkyBoxC();
	};
	void SkyBoxC();
	~SkyBox();
	void SkyBox::renderInMenu();

};

#endif