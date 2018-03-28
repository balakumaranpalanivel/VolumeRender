#pragma once

#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <GL/glew.h>
#include <GL/freeglut.h>

class CVolume
{
protected:
	glm::ivec3 mVolumeDimension;
	int mMaxVolumeDimension;

	glm::ivec3 mGridDimension;
	int mMaxGridDimension;
	
	glm::ivec3 mVoxelsPerCell;
	GLuint mTextureId; // volumeTexId, gridTexId

	const float cubeSize = 30.f;
	glm::vec3 scaleWorldToVoltex;
	glm::vec3 scaleWorldToGrid;
	glm::vec3 maxGridPos;

	float mIsoValue, mMinIsoValue, mMaxIsoValue;

public:

	virtual void InitialiseParameters()
	{}

	virtual void LoadVolume()
	{}

	GLuint Generate3DTexture(GLenum texture, GLint minMagFilter)
	{
		GLuint result;
		glActiveTexture(texture);
		glGenTextures(1, &result);
		glBindTexture(GL_TEXTURE_3D, result);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, minMagFilter);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, minMagFilter);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
		glTexParameterfv(GL_TEXTURE_3D, GL_TEXTURE_BORDER_COLOR, value_ptr(glm::vec4(0)));
		return result;
	}

	void InitialiseDimensions()
	{
		//std::cout << "volumeDim = " << volumeDim << std::endl;
		std::cout << "Start: InitialiseDimensions" << std::endl;

		// Grid dimensions
		if (mGridDimension == glm::ivec3(0))
		{
			mGridDimension = glm::max(
				glm::ivec3(1),
				mVolumeDimension / 
	(glm::max(mVolumeDimension.x, mVolumeDimension.y, mVolumeDimension.z) / mMaxGridDimension));
		}

		mVoxelsPerCell = glm::ivec3(ceil(glm::vec3(mVolumeDimension) / glm::vec3(mGridDimension)));
		//std::cout << "gridDim = " << gridDim << std::endl;
		//std::cout << "voxelsPerCell = " << voxelsPerCell << std::endl;

		// Given a grid, we want to scale the max dimension to cubeSize (WCS).
		// E.g. volumeDim = (17,13), gridDim = (4,4), voxelsPerCell = (17/4, 13/4) = (5,4), cubeSize = 30.
		mMaxVolumeDimension = glm::max(mVolumeDimension.x, mVolumeDimension.y, mVolumeDimension.z); // 17
		std::cout << "maxVolumeDim " << mMaxVolumeDimension << std::endl;

		glm::vec3 scaleWorldToVolume = glm::vec3(cubeSize / mMaxVolumeDimension); // 30/17 = 1.77
		scaleWorldToVoltex = scaleWorldToVolume * glm::vec3(mVolumeDimension); // (30, 22.9)
																   //std::cout << "scaleWorldToVoltex " << scaleWorldToVoltex << std::endl;
		// TODO: CAMERA
		//ptWorldToGrid = -scaleWorldToVoltex / 2.0f;
		scaleWorldToGrid = scaleWorldToVolume * glm::vec3(mVoxelsPerCell); // 1.77*(5,4) = (8.82, 7.05)
																	 //std::cout << "scaleWorldToGrid " << scaleWorldToGrid << std::endl;

		maxGridPos = glm::vec3(mVolumeDimension) / glm::vec3(mVoxelsPerCell);

		std::cout << "End: InitialiseDimensions" << std::endl;
	}

	glm::ivec3 GetVolume()
	{
		return mVolumeDimension;
	}

	glm::ivec3 GetGrid()
	{
		return mGridDimension;
	}

	GLuint GetTextureId()
	{
		return mTextureId;
	}

	glm::ivec3 GetVoxelsPerCell()
	{
		return mVoxelsPerCell;
	}

	glm::vec3 GetMaxGridPos()
	{
		return maxGridPos;
	}

	int GetMaxVolumeDimension()
	{
		return mMaxVolumeDimension;
	}

	float GetIsoValue()
	{
		return mIsoValue;
	}

	float UpdateIsoValue(float delta)
	{
		mIsoValue += delta;
		mIsoValue = glm::clamp(mIsoValue, mMinIsoValue, mMaxIsoValue);
		return mIsoValue;
	}

	glm::vec3 GetScaleWorldToGrid()
	{
		return scaleWorldToGrid;
	}

	glm::vec3 GetScaleWorldToVoltex()
	{
		return scaleWorldToVoltex;
	}

};