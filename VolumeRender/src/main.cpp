#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <GL/glew.h>
#include <GL/freeglut.h>

using namespace glm;

const int glut_profile = GLUT_COMPATIBILITY_PROFILE;

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_SINGLE | GLUT_RGBA);
	//glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA); // caps FPS
	glutInitContextVersion(4, 3);
	glutInitContextProfile(glut_profile);

	glutInitContextFlags(GLUT_DEBUG);

	glutInitWindowSize(800, 600);
	glutInitWindowPosition(500, 200);

	glutCreateWindow("Volume Rendering");

	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION);

	// Initialize GLEW to setup the OpenGL Function pointers
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		fprintf(stderr, "Failed to initialize GLEW\n");
		return 0;
	}

	glutMainLoop();

	return 0;
}


