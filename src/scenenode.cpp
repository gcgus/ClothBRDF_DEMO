#include "scenenode.h"
#include "application.h"
#include "texture.h"
#include "utils.h"

unsigned int SceneNode::lastNameId = 0;
unsigned int mesh_selected = 0;
//variables para poder cambiar los materiales y texturas 
unsigned int material_selected = 3;
unsigned int texture_selected = 0;
//array para almacenar las direcciones de los enviorements para poder cambiarlos
char* textSky[8];
unsigned int lab = 1;

//Con estos metodos creamos los diferentes materiales para luego con el ImGui poder cambiar entre ellos
SceneNode::SceneNode(bool sky = 0)
{

	this->name = std::string("Node" + std::to_string(lastNameId++));

	if (!sky) {
		mats[0] = new StandardMaterial();
		mats[1] = new PhongMaterial();
		mats[2] = new ReflectiveMaterial();
		mats[3] = new PBRMaterial();
		mats[4] = new ClothMaterial();

		

		this->material = mats[3];
	}




}

SceneNode::SceneNode(const char* name)
{
	bool sky = 0;
	this->name = name;
	if (!sky) {
		mats[0] = new StandardMaterial();
		mats[1] = new PhongMaterial();
		mats[2] = new ReflectiveMaterial();
		mats[3] = new PBRMaterial();
		mats[4] = new ClothMaterial();

		this->material = mats[3];
	}


}

SceneNode::~SceneNode()
{

}

void SceneNode::render(Camera* camera)
{
	if (material) {
		material->render(mesh, model, camera);
	}

}

void SceneNode::renderWireframe(Camera* camera)
{
	WireframeMaterial mat = WireframeMaterial();
	mat.render(mesh, model, camera);
}


void SceneNode::renderInMenu()
{


		//Model editImGui::DragFloat3("Diffuse", &this->diffuse.x, 0.01f, 0, 1);
		if (ImGui::TreeNode("Model"))
		{
			float matrixTranslation[3], matrixRotation[3], matrixScale[3];
			ImGuizmo::DecomposeMatrixToComponents(model.m, matrixTranslation, matrixRotation, matrixScale);
			ImGui::DragFloat3("Position", matrixTranslation, 0.1f);
			ImGui::DragFloat3("Rotation", matrixRotation, 0.1f);
			ImGui::DragFloat3("Scale", matrixScale, 0.1f);
			ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, model.m);

			ImGui::TreePop();
		}

		//Material
		if (material && ImGui::TreeNode("Material"))
		{
			bool changed = false;
			changed |= ImGui::Combo("Material", (int*)&material_selected, "TEXTURE\0PHONG\0REFLECTIVE\0PBR\0CLOTH");
			if (changed) {
				material = mats[material_selected];
			}
			material->renderInMenu();
			ImGui::TreePop();
		}

		//Geometry
		if (mesh && ImGui::TreeNode("Geometry"))
		{
			bool changed = false;
			changed |= ImGui::Combo("Mesh", (int*)&mesh_selected, "BENCH\0HELMET\0BALL\0LANTERN");
			if (changed) {
				PBRMaterial* PBRmat = dynamic_cast<PBRMaterial*>(material);
				if (mesh_selected == 0) {
					this->mesh = Mesh::Get("data/models/bench/bench.obj.mbin");
					PBRmat->texture = Texture::Get("data/models/bench/albedo.png");
					PBRmat->metalness = Texture::Get("data/models/bench/metalness.png");
					PBRmat->roughness = Texture::Get("data/models/bench/roughness.png");
					PBRmat->normalmap = Texture::Get("data/models/bench/normal.png");
					PBRmat->emissionmap = NULL;
					PBRmat->aomap = NULL;
					PBRmat->metalval = 1;
					PBRmat->roughval = 1;
					PBRmat->color_factor = vec4(1., 1., 1., 1.f);
				}
				else if (mesh_selected == 1) {
					this->mesh = Mesh::Get("data/models/helmet/helmet.obj.mbin");
					PBRmat->texture = Texture::Get("data/models/helmet/albedo.png");
					PBRmat->metalness = Texture::Get("data/models/helmet/metalness.png");
					PBRmat->roughness = Texture::Get("data/models/helmet/roughness.png");
					PBRmat->normalmap = Texture::Get("data/models/helmet/normal.png");
					PBRmat->emissionmap = Texture::Get("data/models/helmet/emissive.png");
					PBRmat->aomap = Texture::Get("data/models/helmet/ao.png");
					PBRmat->metalval = 1;
					PBRmat->roughval = 1;
					PBRmat->color_factor = vec4(1., 1., 1., 1.f);
				}
				else if (mesh_selected == 2) {
					this->mesh = Mesh::Get("data/meshes/sphere.obj.mbin");
					PBRmat->texture = Texture::Get("data/models/ball/albedo.png");
					PBRmat->metalness = Texture::Get("data/models/ball/metalness.png");
					PBRmat->roughness = Texture::Get("data/models/ball/roughness.png");
					PBRmat->normalmap = Texture::Get("data/models/ball/normal.png");
					PBRmat->emissionmap = NULL;
					PBRmat->aomap = NULL;
					PBRmat->metalval = 1;
					PBRmat->roughval = 1;
					PBRmat->color_factor = vec4(1., 1., 1., 1.f);
				}
				else if (mesh_selected == 3) {
					this->mesh = Mesh::Get("data/models/lantern/lantern.obj.mbin");
					PBRmat->texture = Texture::Get("data/models/lantern/albedo.png");
					PBRmat->metalness = Texture::Get("data/models/lantern/metalness.png");
					PBRmat->roughness = Texture::Get("data/models/lantern/roughness.png");
					PBRmat->normalmap = Texture::Get("data/models/lantern/normal.png");
					PBRmat->emissionmap = NULL;
					PBRmat->aomap = Texture::Get("data/models/lantern/ao.png");
					PBRmat->metalval = 1;
					PBRmat->roughval = 1;
					PBRmat->color_factor = vec4(1., 1., 1., 1.f);
				}
			}

			ImGui::TreePop();
		}
}


void SkyBox::SkyBoxC() {

	this->mesh = new Mesh();
	this->mesh = Mesh::Get("data/meshes/box.ASE");

	this->material = new SkyboxMaterial();
	this->material->texture = new Texture();
	this->material->shader = Shader::Get("data/shaders/basic.vs", "data/shaders/skybox.fs");

	HDRE* temp = HDRE::Get("data/environments/panorama.hdre");

	for (int i = 0; i < 5; i++) {
		hdre[i] = new Texture();
		this->hdre[i]->cubemapFromHDRE(temp, i);
	}
	this->material->texture = hdre[0];

	textSky[0] = "data/environments/panorama.hdre";
	textSky[1] = "data/environments/pisa.hdre";
	textSky[2] = "data/environments/san_giuseppe_bridge.hdre";
	textSky[3] = "data/environments/studio.hdre";
	textSky[4] = "data/environments/tv_studio.hdre";

}

SkyBox::~SkyBox() {

}

void SkyBox::renderInMenu()
{
	if (material && ImGui::TreeNode("SkyBox"))
	{
		bool changed = false;
		changed |= ImGui::Combo("Skybox", (int*)&texture_selected, "panorama\0pisa\0bridge\0studio\0tvstudio\0");
		if (changed) {


			HDRE* temp = HDRE::Get(textSky[texture_selected]);

			for (int i = 0; i < 5; i++) {
				hdre[i] = new Texture();
				this->hdre[i]->cubemapFromHDRE(temp, i);
			}
			this->material->texture = hdre[0];

		}
		material->renderInMenu();
		ImGui::TreePop();
	}
}
