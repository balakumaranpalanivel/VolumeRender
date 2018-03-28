#pragma once
#include "Volume.h"
#include "Shader.h"

#include <iostream>

#include <GL/glew.h>
#include <GL/freeglut.h>

class CBunny : public CVolume
{
public:

	void InitialiseParameters()
	{
		std::cout << "Start: InitialiseParameters: Bunny" << std::endl;

		CVolume::mGridDimension = glm::ivec3(0); 
		CVolume::mMaxGridDimension = 512 / 2;

		std::cout << "End: InitialiseParameters: Bunny" << std::endl;
	}

	void LoadVolume()
	{
		std::cout << "Start: LoadVolume: Bunny" << std::endl;

		mIsoValue = 0.1f;
		mMinIsoValue = 0;
		mMaxIsoValue = 1;

		CVolume::mVolumeDimension = glm::ivec3(512, 512, 360); // = (64, 64, 45) * 8

		unsigned char* voxels = new unsigned char[
			CVolume::mVolumeDimension.x * CVolume::mVolumeDimension.y * CVolume::mVolumeDimension.z];
		{
			unsigned char* p = voxels;
			unsigned short* sliceData = new unsigned short[
				CVolume::mVolumeDimension.x * CVolume::mVolumeDimension.y];
			int sliceSize = CVolume::mVolumeDimension.x * CVolume::mVolumeDimension.y;
			for (int slice = 0; slice < CVolume::mVolumeDimension.z; slice++)
			{
				std::stringstream ss;
				ss << "C:/data/bunny/" << (slice + 1);
				std::ifstream f(ss.str(), std::ios::in | std::ios::binary);
				f.read((char*)sliceData, sliceSize * sizeof(unsigned short));

				unsigned char* q = (unsigned char*)sliceData;
				for (int i = 0; i < sliceSize; i++, q += 2, p++)
					*p = unsigned char(((q[0] << 8) + q[1]) >> 4); // Mac byte ordering, discard 4 of the 12 significant bits
			}
			delete sliceData;
		}

		std::cout << "Uploading to GPU" << std::endl;

		CVolume::mTextureId = Generate3DTexture(GL_TEXTURE0, GL_LINEAR);
		glTexImage3D(GL_TEXTURE_3D, 0, GL_R8,
			CVolume::mVolumeDimension.x,
			CVolume::mVolumeDimension.y,
			CVolume::mVolumeDimension.z, 0,
			GL_RED, GL_UNSIGNED_BYTE, voxels);
		delete[] voxels;

		std::cout << "Stripping bunny" << std::endl;

		CShader computeBunnyShader;
		computeBunnyShader.loadShader("strip_bunny.comp.glsl");
		computeBunnyShader.link();
		computeBunnyShader.use();
		computeBunnyShader.uniform("volume", 0);
		glBindImageTexture(0, CVolume::mTextureId, 0, GL_TRUE, 0, GL_READ_WRITE, GL_R8);
		glDispatchCompute(
			CVolume::mVolumeDimension.x, CVolume::mVolumeDimension.y, CVolume::mVolumeDimension.z);
		computeBunnyShader.unUse();
		glMemoryBarrier(GL_ALL_BARRIER_BITS);

		std::cout << "Done: LoadVolume: Bunny" << std::endl;
	}
};