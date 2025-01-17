#pragma once

#include "glm/gtx/string_cast.hpp"
#include <map>
#include <vector>
#include "../assimp/Importer.hpp"
#include "animation.h"
#include "bone.h"

/**
 * 动画控制单元
 * 发挥如下作用：
 * 		1.存储动画类
 * 		2.更新并且计算变换矩阵
 */
class Animator
{
public:
	Animator(Animation* animation)
	{
		m_CurrentTime = 0.0;
		m_CurrentAnimation = animation;
        m_RootGlobalTransform = AssimpGLMHelpers::ConvertMatrixToGLMFormat(m_CurrentAnimation->GetGlobalTransformation());

		//将vector大小设置为上限100
		m_FinalBoneMatrices.reserve(100);

		for (int i = 0; i < 100; i++)
			m_FinalBoneMatrices.push_back(glm::mat4(1.0f));

        LOGCATE("Animator init, m_RootGlobalTransform[0]: %s", glm::to_string(m_RootGlobalTransform).c_str());

	}

	//刷新帧动画
	void UpdateAnimation(float dt)
	{
	    DEBUG_LOGCATE();
		m_DeltaTime = dt;
		if (m_CurrentAnimation)
		{
			//获取当前dt时间段右多少个ticks并加到当前时间上
			m_CurrentTime += m_CurrentAnimation->GetTicksPerSecond() * dt;
			//判断是否超过动画时间，否则进行取余
			m_CurrentTime = fmod(m_CurrentTime, m_CurrentAnimation->GetDuration());
			//计算骨骼的变换矩阵
			CalculateBoneTransform(&m_CurrentAnimation->GetRootNode(), glm::mat4(1.0f));
		}
	}

	void PlayAnimation(Animation* pAnimation)
	{
		m_CurrentAnimation = pAnimation;
		m_CurrentTime = 0.0f;
	}

	/**
	 * 计算某个骨骼 影响顶点的换算矩阵
	 * @param node 存骨骼名字，矩阵
	 * @param parentTransform 父节点的换算矩阵
	 */
	void CalculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform)
	{

		//拿到骨骼名
		std::string nodeName = node->name;
		//拿到该骨骼相对父骨骼的变换
		glm::mat4 nodeTransform = node->transformation;
        LOGCATE("CalculateBoneTransform nodeName %s", nodeName.c_str());

		//找到当前骨骼
		Bone* Bone = m_CurrentAnimation->FindBone(nodeName);

		//如果找到骨骼，则对骨骼进行Update并获取当前骨骼变换
		if (Bone){
            LOGCATE("CalculateBoneTransform Bone->Update %.4f", m_CurrentTime);
            //Bone对象根据时间，计算一个矩阵
			Bone->Update(m_CurrentTime);
			//得到矩阵
			nodeTransform = Bone->GetLocalTransform();
		}

		LOGCATE("CalculateBoneTransform, nodeTransform %s", glm::to_string(nodeTransform).c_str() );
		//当前骨骼的换算矩阵，会被父节点的矩阵影响，所以要相乘
		glm::mat4 globalTransformation = parentTransform * nodeTransform;

        LOGCATE("CalculateBoneTransform, parentTransform %s", glm::to_string(parentTransform).c_str() );
        LOGCATE("CalculateBoneTransform, globalTransformation %s", glm::to_string(globalTransformation).c_str() );


        std::map<std::string,BoneInfo> boneInfoMap = m_CurrentAnimation->GetBoneIDMap();

		//找到当前骨骼
		if (boneInfoMap.find(nodeName) != boneInfoMap.end())
		{
			int index = boneInfoMap[nodeName].id;
			//获取offsetMatrix
			glm::mat4 offsetMatrix = boneInfoMap[nodeName].offset;
			//某个骨骼影响顶点的换算矩阵，该矩阵将传递给vertex shader
			//需要再乘于根节点的m_GlobalTransform，根节点影响所有子节点的换算
			m_FinalBoneMatrices[index] = m_RootGlobalTransform * globalTransformation * offsetMatrix;
			LOGCATE("m_RootGlobalTransform %s, m_FinalBoneMatrices[%d]: %s, offset %s", glm::to_string(m_RootGlobalTransform).c_str(),
							index, glm::to_string(m_FinalBoneMatrices[index]).c_str(), glm::to_string(offsetMatrix).c_str());
		}

		//递归，计算子节点的矩阵
		for (int i = 0; i < node->childrenCount; i++)
			CalculateBoneTransform(&node->children[i], globalTransformation);
	}

	std::vector<glm::mat4> GetFinalBoneMatrices()
	{
		return m_FinalBoneMatrices;
	}

private:
	//所有骨骼最终的Matrice数组
	std::vector<glm::mat4> m_FinalBoneMatrices;
	//当前控制的动画对象
	Animation* m_CurrentAnimation;
	//记录当前时间
	float m_CurrentTime;
	//与上一帧相差的时间
	float m_DeltaTime;
	glm::mat4 m_RootGlobalTransform;

};