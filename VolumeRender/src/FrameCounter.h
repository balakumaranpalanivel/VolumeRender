#pragma once
#include <iostream>
#include <list>
#include <sstream>

#include <GL\freeglut.h>

std::string fpsString;

void drawString(const std::string &stringToDraw, float& yPos)
{
	if (stringToDraw.empty())
		return;

	glColor3f(0, 0, 0);
	glWindowPos3f(12.f, 600 - yPos - 2, 0.5f);
	glutBitmapString(GLUT_BITMAP_HELVETICA_18, (const unsigned char*)stringToDraw.c_str());
	glColor3f(1, 1, 1);
	glWindowPos3f(10.f, 600 - yPos, 0.f);
	glutBitmapString(GLUT_BITMAP_HELVETICA_18, (const unsigned char*)stringToDraw.c_str());

	yPos += 20.f;
}

/*
	Method to display the FPS in the window
*/
void FrameCounter(bool showMoreAccurateFps)
{
	static int frameCounter = 0;
	static int previousTime = glutGet(GLUT_ELAPSED_TIME);

	frameCounter++;
	int currentTime = glutGet(GLUT_ELAPSED_TIME);

	if (currentTime - previousTime > 1000)
	{
		// Compute averages over the last 1 and 10 seconds.
		static std::list<int> frameNumbers;
		static std::list<double> frameTimes;
		if (frameNumbers.size() == 0)
		{
			frameNumbers.push_front(0);
			frameTimes.push_front(previousTime);
		}

		frameNumbers.push_front(frameCounter);
		frameTimes.push_front(currentTime);
		if (frameNumbers.size() > 11)
		{
			frameNumbers.pop_back();
			frameTimes.pop_back();
		}

		auto formatFPS = [&](double frame2, double time2) -> std::string
		{
			double numFrames = frameNumbers.front() - frame2;
			double dt = frameTimes.front() - time2;
			double msPerFrame = dt / numFrames;
			double fps = 1000.0 / msPerFrame;
			std::stringstream ss;
			ss << fps << " FPS, " << msPerFrame << " ms";
			return ss.str();
		};

		std::string fps1s = formatFPS((*++frameNumbers.begin()), (*++frameTimes.begin()));

		previousTime = currentTime;
		std::stringstream ss;
		ss << fps1s;
		if (showMoreAccurateFps)
		{
			std::string fps10s = formatFPS(frameNumbers.back(), frameTimes.back());
			ss << " (" << fps10s << /*" over "<< frameNumbers.size() - 1 << "s"*/")";
		}
		fpsString = ss.str();
		std::cout << fpsString << std::endl;
	}
}

/*
	Method that draws the FPS String
*/
void DrawFPS()
{
	glDisable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);

	float yPos = 20.f;
	drawString(fpsString, yPos);
}