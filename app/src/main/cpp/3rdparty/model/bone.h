#pragma once

/* Container for bone data */

#include <vector>
#include "../assimp/scene.h"
#include <list>
#include "glm/glm.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/quaternion.hpp"
#include "assimp_glm_helpers.h"
/**
 *Bone类数据结构作用分析：
 * 1.负责存储每根骨骼的动画关键帧（平移、缩放、旋转）
 * 2.负责根据时间对关键帧进行插值，得到当前的平移、旋转、缩放矩阵影响总和
 *   glm库已经提供了插值接口，给到当前插值比例，给到两个关键帧矩阵，
 *   就可以计算到本比例下的插值矩阵
 */

//平移关键帧
struct KeyPosition
{
	glm::vec3 position;//当前骨骼相对于父骨骼的位置平移
	float timeStamp;//时间戳：动画开始播放时候，经过多少时间能够到达当前关键帧
};

//旋转关键帧
struct KeyRotation
{
	glm::quat orientation;//当前旋转关键帧的四元素
	float timeStamp;//时间戳：在动画运行到多少时间的时候播放
};

//缩放关键帧
struct KeyScale
{
	glm::vec3 scale;//缩放关键帧数据
	float timeStamp;//时间戳：在动画运行到多少时间的时候播放
};

class Bone
{
public:
	//初始化读取三种关键帧
	Bone(const std::string& name, int ID, const aiNodeAnim* channel)
		:
		m_Name(name),
		m_ID(ID),
		m_LocalTransform(1.0f)
	{
		//1. 提取关键帧的位移参数，放到m_Positions列表，后面可以用于计算插值
		//获取位置关键帧数量
		m_NumPositions = channel->mNumPositionKeys;
        LOGCATE("Bone created, m_NumPositions %d", m_NumPositions);
		for (int positionIndex = 0; positionIndex < m_NumPositions; positionIndex++)
		{
			aiVector3D aiPosition = channel->mPositionKeys[positionIndex].mValue;
			float timeStamp = channel->mPositionKeys[positionIndex].mTime;
			KeyPosition data;
			data.position = AssimpGLMHelpers::GetGLMVec(aiPosition);
			data.timeStamp = timeStamp;
			m_Positions.push_back(data);
            LOGCATE("get one key frame's position %s, timeStamp %f", glm::to_string(data.position).c_str(), data.timeStamp);
        }
		//2. 提取关键帧的旋转
		//获取旋转关键帧数量
		m_NumRotations = channel->mNumRotationKeys;
		for (int rotationIndex = 0; rotationIndex < m_NumRotations; ++rotationIndex)
		{
			aiQuaternion aiOrientation = channel->mRotationKeys[rotationIndex].mValue;
			float timeStamp = channel->mRotationKeys[rotationIndex].mTime;
			KeyRotation data;
			data.orientation = AssimpGLMHelpers::GetGLMQuat(aiOrientation);
			data.timeStamp = timeStamp;
			m_Rotations.push_back(data);
		}
		//3. 提取关键帧的缩放
		m_NumScalings = channel->mNumScalingKeys;
		for (int keyIndex = 0; keyIndex < m_NumScalings; ++keyIndex)
		{
			aiVector3D scale = channel->mScalingKeys[keyIndex].mValue;
			float timeStamp = channel->mScalingKeys[keyIndex].mTime;
			KeyScale data;
			data.scale = AssimpGLMHelpers::GetGLMVec(scale);
			data.timeStamp = timeStamp;
			m_Scales.push_back(data);
		}
	}

	//驱动函数，传入当前时间来计算LocalTransform矩阵
	//Update函数中的操作步骤：
	//1.interPolatexx()计算本地变换
	//2.GetxxxIndex找到当前时间last关键帧与next关键帧
	//3.GetScaleFactor得到两个时间的插值比例
	//4.使用插值比例以及glm库计算插值后的prs矩阵
	//5.将得到的prs矩阵进行相乘，得到LocalTransform变换矩阵
	void Update(float animationTime)
	{
		//根据时间，估算translation, rotation, scale，最后合并成一个矩阵
		glm::mat4 translation = InterpolatePosition(animationTime);
		glm::mat4 rotation = InterpolateRotation(animationTime);
		glm::mat4 scale = InterpolateScaling(animationTime);
		//先缩放、再旋转、再平移，这里矩阵是从右向左计算
		m_LocalTransform = translation * rotation * scale;
	}

	glm::mat4 GetLocalTransform(){ return m_LocalTransform; }

	std::string GetBoneName() const { return m_Name; }

	int GetBoneID() { return m_ID; }

private:

	//传入上一个时间戳和下一个时间戳，得到两个时间的插值比例
	float GetLerpFactor(float lastTimeStamp, float nextTimeStamp, float animationTime)
	{
		float scaleFactor = 0.0f;
		float midWayLength = animationTime - lastTimeStamp;
		float framesDiff = nextTimeStamp - lastTimeStamp;
		scaleFactor = midWayLength / framesDiff;
		return scaleFactor;
	}

	//根据当前时间，找到当前时间last平移关键帧与next平移关键帧
	int GetPositionIndexByTime(float animationTime)
	{
		for (int index = 0; index < m_NumPositions - 1; ++index)
		{
			if (animationTime < m_Positions[index + 1].timeStamp)
				return index;
		}
		return 0;
	}

	//根据当前时间，找到当前时间last旋转关键帧与next旋转关键帧
	int GetRotationIndexByTime(float animationTime)
	{
		for (int index = 0; index < m_NumRotations - 1; ++index)
		{
			if (animationTime < m_Rotations[index + 1].timeStamp)
				return index;
		}
		return 0;
	}

	//根据当前时间，找到当前时间last缩放关键帧与next缩放关键帧
	int GetScaleIndexByTime(float animationTime)
	{
		for (int index = 0; index < m_NumScalings - 1; ++index)
		{
			if (animationTime < m_Scales[index + 1].timeStamp)
				return index;
		}
		return 0;
	}

	//根据当前时间来插值Position矩阵，来得到平移变换
	glm::mat4 InterpolatePosition(float animationTime)
	{
		//如果只有一个Positions变换
		if (1 == m_NumPositions)
			return glm::translate(glm::mat4(1.0f), m_Positions[0].position);

		//找到当前时间的上一帧
		int p0Index = GetPositionIndexByTime(animationTime);
		//找到当前时间的下一帧
		int p1Index = p0Index + 1;
		//根据前后帧，计算当前时间插值系数
		float scaleFactor = GetLerpFactor(m_Positions[p0Index].timeStamp,
										  m_Positions[p1Index].timeStamp, animationTime);
		//计算插值。使用glm自带的mix函数
		glm::vec3 finalPosition = glm::mix(m_Positions[p0Index].position, m_Positions[p1Index].position
			, scaleFactor);
		return glm::translate(glm::mat4(1.0f), finalPosition);
	}

	//根据当前时间来插值Rotation矩阵，来得到旋转变换
	glm::mat4 InterpolateRotation(float animationTime)
	{
		//如果只有一个Rotations变换
		if (1 == m_NumRotations)
		{
			auto rotation = glm::normalize(m_Rotations[0].orientation);
			return glm::toMat4(rotation);
		}

		//找到当前时间的上一帧
		int p0Index = GetRotationIndexByTime(animationTime);
		//找到当前时间的下一帧
		int p1Index = p0Index + 1;
		//计算插值系数
		float scaleFactor = GetLerpFactor(m_Rotations[p0Index].timeStamp,
										  m_Rotations[p1Index].timeStamp, animationTime);
		//计算插值
		glm::quat finalRotation = glm::slerp(m_Rotations[p0Index].orientation, m_Rotations[p1Index].orientation
			, scaleFactor);
		finalRotation = glm::normalize(finalRotation);
		return glm::toMat4(finalRotation);

	}

	//根据当前时间来插值Scaling矩阵，来得到缩放变换
	glm::mat4 InterpolateScaling(float animationTime)
	{
		//如果只有一个Scaling变换
		if (1 == m_NumScalings)
			return glm::scale(glm::mat4(1.0f), m_Scales[0].scale);

		//找到当前时间的上一帧
		int p0Index = GetScaleIndexByTime(animationTime);
		//找到当前时间的下一帧
		int p1Index = p0Index + 1;
		//计算插值系数
		float scaleFactor = GetLerpFactor(m_Scales[p0Index].timeStamp,
										  m_Scales[p1Index].timeStamp, animationTime);
		//计算插值
		glm::vec3 finalScale = glm::mix(m_Scales[p0Index].scale, m_Scales[p1Index].scale
			, scaleFactor);
		return glm::scale(glm::mat4(1.0f), finalScale);
	}

	std::vector<KeyPosition> m_Positions;//平移关键帧数组
	std::vector<KeyRotation> m_Rotations;//旋转关键帧数组
	std::vector<KeyScale> m_Scales;//缩放关键帧数组
	int m_NumPositions;
	int m_NumRotations;
	int m_NumScalings;

	glm::mat4 m_LocalTransform;//每次刷帧的变换，从平移、旋转、缩放关键帧中计算出来
	std::string m_Name;//骨骼名称
	int m_ID;//骨骼ID
};

