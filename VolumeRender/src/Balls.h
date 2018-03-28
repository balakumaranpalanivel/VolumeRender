#pragma once
#include "Volume.h"
#include "Shader.h"

#include <iostream>

#include <GL/glew.h>
#include <GL/freeglut.h>
class CBalls : public CVolume
{
public:

	void InitialiseParameters()
	{
		std::cout << "Start: InitialiseParameters: Balls" << std::endl;

		CVolume::mVolumeDimension = glm::ivec3(256, 230, 150);
		CVolume::mGridDimension = glm::ivec3(0);
		CVolume::mMaxGridDimension = 16;

		std::cout << "Done: InitialiseParameters: Balls" << std::endl;
	}

	void LoadVolume()
	{
		std::cout << "Start: LoadVolume: Balls" << std::endl;

		CVolume::mTextureId = CVolume::Generate3DTexture(GL_TEXTURE0, GL_LINEAR);
		glTexStorage3D(GL_TEXTURE_3D, 1, GL_R8,
			CVolume::mVolumeDimension.x,
			CVolume::mVolumeDimension.y,
			CVolume::mVolumeDimension.z
		);

		CShader computeVolumeShader;
		computeVolumeShader.loadShader("compute_balls.comp.glsl");
		computeVolumeShader.link();

		glBindImageTexture(0, CVolume::mTextureId, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_R8);

		computeVolumeShader.use();
		computeVolumeShader.uniform("volume", 0);

		glDispatchCompute(
			CVolume::mVolumeDimension.x,
			CVolume::mVolumeDimension.y,
			CVolume::mVolumeDimension.z
		);

		computeVolumeShader.unUse();
		glMemoryBarrier(GL_ALL_BARRIER_BITS);

		std::cout << "Done: LoadVolume: Balls" << std::endl;
	}
};