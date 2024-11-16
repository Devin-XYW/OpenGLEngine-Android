#pragma once

#include <vector>
#include <map>
#include "glm/glm.hpp"
#include "../assimp/scene.h"
#include "bone.h"
#include <functional>
#include "animdata.h"
#include "model_animation.h"
#include "LogUtil.h"

/**
 * 1.负责存储每一个动画的相关信息
 * 2.负责存储动画层级架构，存储所有骨骼相关信息
 * 3.读取如下信息：
 * 		1.动画时间ticks及粒度
 * 		2.每根骨骼信息
 * 		3.漏读的BoneId
 */

//节点数据结构
struct AssimpNodeData
{
	glm::mat4 transformation;//没有骨骼变换的情况下，会使用这个变换矩阵
	std::string name;
	int childrenCount;
	std::vector<AssimpNodeData> children;
};

class Animation
{
public:
	Animation() = default;

	Animation(const std::string& animationPath, ModelAnim* model)
	{
		LOGCATE("Animation created");
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(animationPath, aiProcess_Triangulate);

		auto animation = scene->mAnimations[0];//可以有多种动画，本例子只显示第一种动画

		m_Duration = animation->mDuration;
		m_TicksPerSecond = animation->mTicksPerSecond;
		LOGCATE("Animation created, m_Duration %d, m_TicksPerSecond %d", m_Duration, m_TicksPerSecond);
        m_GlobalTransformation = scene->mRootNode->mTransformation;
		m_GlobalTransformation = m_GlobalTransformation.Inverse();
		//读取层级结构
		ReadHeirarchyData(m_RootNode, scene->mRootNode);
		//读取动画骨骼数据
		ReadAnimationData(animation, *model);
	}

	~Animation()
	{
	}

	Bone* FindBone(const std::string& name)
	{
		LOGCATE("FindBone %s", name.c_str());
		auto iter = std::find_if(m_Bones.begin(), m_Bones.end(),
			[&](const Bone& Bone)
			{
				return Bone.GetBoneName() == name;
			}
		);
		if (iter == m_Bones.end()) return nullptr;
		else return &(*iter);
	}

	
	inline float GetTicksPerSecond() { return m_TicksPerSecond; }
	inline float GetDuration() { return m_Duration;}
	inline aiMatrix4x4& GetGlobalTransformation() { return m_GlobalTransformation;}

	inline const AssimpNodeData& GetRootNode() { return m_RootNode; }
	inline const std::map<std::string,BoneInfo>& GetBoneIDMap() 
	{
		LOGCATE("GetBoneIDMap, m_BoneInfoMap size %d", m_BoneInfoMap.size());
		return m_BoneInfoMap;
	}

private:

	float m_Duration;//动画持续时间
	int m_TicksPerSecond;//每秒多少个ticks
	std::vector<Bone> m_Bones;//存储所有动画相关骨骼
	AssimpNodeData m_RootNode;
	std::map<std::string, BoneInfo> m_BoneInfoMap;
	aiMatrix4x4 m_GlobalTransformation;
	
	void ReadAnimationData(const aiAnimation* animation, ModelAnim& model)
	{
		int size = animation->mNumChannels;
		//获得之前解析权重时所记录的骨骼map，其中key为骨骼名字
		m_BoneInfoMap = model.GetBoneInfoMap();//getting m_BoneInfoMap from Model class
		LOGCATE("ReadAnimationData, m_BoneInfoMap address %p, size %d, animation->mNumChannels %d", &m_BoneInfoMap,m_BoneInfoMap.size(), animation->mNumChannels);
		//获得骨骼计数器，用于分配id
		int& boneCount = model.GetBoneCount(); //getting the m_BoneCounter from Model class

		//reading channels(bones engaged in an animation and their keyframes)
		//读取通道列表，每个通道包括所有被该动画影响的骨骼，以及对应的关键帧
		for (int i = 0; i < size; i++)
		{
			auto channel = animation->mChannels[i];//一个channel代表某个骨骼
			std::string boneName = channel->mNodeName.data;//拿到骨骼名字

			if (m_BoneInfoMap.find(boneName) == m_BoneInfoMap.end())
			{//如果万一map不包括这个骨骼，则记录下来
				m_BoneInfoMap[boneName].id = boneCount;
				boneCount++;
			}
			//创建一个Bone对象，添加到m_Bones数组
			//将骨骼信息读取并存储
			m_Bones.push_back(Bone(channel->mNodeName.data,
								   m_BoneInfoMap[channel->mNodeName.data].id, channel));
		}
		LOGCATE("ReadAnimationData, m_BoneInfoMap size %d", m_BoneInfoMap.size());
//		m_BoneInfoMap = boneInfoMap;
		LOGCATE("ReadAnimationData, now m_BoneInfoMap size %d", m_BoneInfoMap.size());

	}

	void ReadHeirarchyData(AssimpNodeData& dest, const aiNode* src)
	{
		assert(src);

		dest.name = src->mName.data;
		//如果当前节点没有骨骼则使用此transform
		dest.transformation = AssimpGLMHelpers::ConvertMatrixToGLMFormat(src->mTransformation);
		dest.childrenCount = src->mNumChildren;

		//递归遍历子节点
		for (int i = 0; i < src->mNumChildren; i++)
		{
			AssimpNodeData newData;
			ReadHeirarchyData(newData, src->mChildren[i]);
			dest.children.push_back(newData);
		}
	}
};

