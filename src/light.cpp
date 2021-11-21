#include "light.h"
#include "application.h"


/*esta clase sirve para poder dar una posicion, y colores difusos y especulares a una luz
la qual sera usada en el PhongMaterial*/

light::light()
{
}

light::light(Vector3 pos,  Vector3 color)
{
	this->set(pos,  color);
}

void light::set(Vector3 pos  , Vector3 color)
{
	
	this->pos=pos;
	this->color = color;
}



void light::renderInMenu()
{
	if (ImGui::TreeNode("Light"))
	{
		ImGui::DragFloat3("Position", &this->pos.x, 0.01f, -20, 20);
		ImGui::DragFloat3("Color", &this->color.x, 0.01f, 0, 1);
		ImGui::TreePop();
	}


}
