#pragma once

#include "glm/glm.hpp"

struct BoneInfo
{
	/*id is index in finalBoneMatrices*/
	int id;

	/*offset matrix transforms vertex from model space to bone space*/
	glm::mat4 offset;

	BoneInfo(): id(0),offset(1.0f){}

};
#pragma once
