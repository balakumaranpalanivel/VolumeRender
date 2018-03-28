#pragma once
#include "Volume.h"
#include "Shader.h"

#include <iostream>

#include <GL/glew.h>
#include <GL/freeglut.h>

class CBeetle : public CVolume
{
public:

	void InitialiseParameters()
	{
		std::cout << "Start: InitialiseParameters: Beetle" << std::endl;

		CVolume::mGridDimension = glm::ivec3(0);
		CVolume::mMaxGridDimension = 17;

		std::cout << "Done: InitialiseParameters: Beetle" << std::endl;
	}

	void LoadVolume()
	{
		std::cout << "Start: LoadVolume: Beetle" << std::endl;

		mIsoValue = 0.1625f;
		mMinIsoValue = 0;
		mMaxIsoValue = 1;

		std::ifstream f("C:/data/stagbeetle832x832x494.dat", std::ios::in | std::ios::binary);

		unsigned short vuSize[3];
		f.read((char*)vuSize, 3 * sizeof(unsigned short));
		CVolume::mVolumeDimension = glm::ivec3(vuSize[0], vuSize[1], vuSize[2]);

		unsigned long numVoxels =
			CVolume::mVolumeDimension.x * CVolume::mVolumeDimension.y * CVolume::mVolumeDimension.z;
		unsigned short *filedata = new unsigned short[numVoxels];
		f.read((char*)filedata, numVoxels * sizeof(unsigned long));

		std::cout << "Scaling data" << std::endl;
		unsigned char *voxels = new unsigned char[numVoxels];
		for (int i = 0; i < numVoxels; i++)
			voxels[i] = unsigned char(filedata[i] >> 4); // 12 significant bits, discard 4
		delete[] filedata;

		std::cout << "Uploading to GPU" << std::endl;

		CVolume::mTextureId = Generate3DTexture(GL_TEXTURE0, GL_LINEAR);
		glTexImage3D(GL_TEXTURE_3D, 0, GL_R8, 
			CVolume::mVolumeDimension.x, CVolume::mVolumeDimension.y, CVolume::mVolumeDimension.z, 0,
			GL_RED, GL_UNSIGNED_BYTE, voxels);

		std::cout << "Deleting data" << std::endl;
		delete[] voxels;

		std::cout << "Done: LoadVolume: Beetle" << std::endl;
	}
};