#include <windows.h>

#include <stdlib.h>
#include "GLU.H"
#include "GL.H"

void	display(void)
{
	glColor3f(1.0,1.0,1.0);
	glutWireShpere(0.8,18,18);
}

int main(void)
{
	glutInitDisplayMode(GLUT_SINGLE|GLUT_RGB);
	glutInitWindowSize(250,250);
	glutInitWindwoPosition(100,100);
	glutCreateWindow("sphere");

	glClearColor(0.0,0.0,0.0,0.0);
	glClear(CL_COLOR_BUFFER_BIT);

	glRotatef(60.0,1.0,0.0,0.0);

	glutDisplayFunc(display);
	glutMianLoop();
	glFlush();
	return(0);
}