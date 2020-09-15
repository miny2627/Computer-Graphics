#pragma once

#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
class WindowGL
{
public:
	WindowGL();
	WindowGL(GLint windowWidth, GLint windowHeight);

	int Initialize();

	GLint getBufferWidth() { return bufferWidth; }
	GLint getBufferHeight() { return bufferHeight; }

	bool getShouldClose() { return glfwWindowShouldClose(mainWindow); }

	bool* getKeys() { return keys; }
	GLfloat getXChange();
	GLfloat getYChange();

	void swapBuffers() { glfwSwapBuffers(mainWindow); }

	~WindowGL();

private:
	GLFWwindow *mainWindow;

	GLint width, height;
	GLint bufferWidth, bufferHeight;

	//Ű���� �Է��� ������ ����
	bool keys[1024];

	//���� �����ӿ��� ���� �����ӱ��� ���콺 ��ġ ��ȭ�� ���
	GLfloat lastX, lastY; //Ŀ���� ȭ��� X,Y ��ǥ
	GLfloat xChange, yChange; //�� ��ȭ��
	bool mouseFirstMoved;

	static void handleKeys(GLFWwindow* window, int key, int code, int action, int mode);
	static void handleMouse(GLFWwindow* window, double xPos, double yPos);

	void createCallbacks();
};

