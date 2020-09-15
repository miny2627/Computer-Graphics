#include <iostream>
#include <string>
#include <cmath>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <crtdbg.h>  

#include "WindowGL.h"
#include "Mesh.h"
#include "Shader.h"

const float toRadian = 3.14159265f / 180.0f;

WindowGL mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader*> shaderList;

static const char* vShader = "Shaders/shader.vert";
static const char* fShader = "Shaders/shader.frag";

void CreateShaders()
{
	Shader *shader = new Shader();
	//shader->CreateFromString(vShader, fShader);
	shader->CreateFromFiles(vShader, fShader);
	shaderList.push_back(shader);
}

void CreateObject()
{
	GLfloat vertices[] = {
		-1.0f, -1.0f, 0.0f,
		 0.0f, -1.0f, 1.0f,
		 1.0f, -1.0f, 0.0f,
		 0.0f,  1.0f, 0.0f
	};

	unsigned int indices[] = {
		0, 3, 1,
		1, 3, 2,
		2, 3, 0,
		0, 1, 2
	};

	Mesh *obj = new Mesh();
	obj->CreateMesh(vertices, indices, 12, 12);
	meshList.push_back(obj);

	Mesh *obj2 = new Mesh();
	obj2->CreateMesh(vertices, indices, 12, 12);
	meshList.push_back(obj2);

}

//Yaw, Pitch (theta & phi) ������ �Է����� ���� ���
glm::vec3 ForwardVecFromYawPitch(GLfloat yaw, GLfloat pitch)
{
	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	front = glm::normalize(front);

	return front;
}


int main()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	mainWindow = WindowGL(800, 600);
	mainWindow.Initialize();

	
	CreateObject();

	CreateShaders();

	GLuint uniformProjection = 0;
	GLuint uniformModel = 0;
	GLuint uniformView = 0;

	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / (GLfloat)mainWindow.getBufferHeight(), 0.1f, 100.0f);
	
	//------View tranform matrix ���� ������--------
	glm::vec3 eyePos = glm::vec3(0, 0, 1); //EYE ��ġ
	glm::vec3 front = glm::vec3(0, 0, -1); //EYE�� �ٶ󺸴� ��ġ (eyePos + front�� AT ����)
	glm::vec3 up = glm::vec3(0, 1, 0); //UP ����
	glm::vec3 right; //Right ����. ī�޶��� ������ ����. A/D Ű �Է� ó���� ���� �ʿ���
	GLfloat movementSpeed = 1.0f; //�̵� �ӵ� ���� ����
	GLfloat yaw = -90; //pitch�� 0, yaw�� -90���϶� front vector�� (0,0,-1)
	GLfloat pitch = 0;
	GLfloat rotationSpeed = 1.0f; //ȸ�� �ӵ� ���� ����
	glm::vec3 actualUp; //(�߰�����) front ���� ��ȭ�� ���� ���� up vector
	GLfloat deltaTime = 0.0f; //(�߰�����) ������ ���ſ� �ɸ� �ð�
	GLfloat lastTime = glfwGetTime(); //(�߰�����) ������ �������� �ð�
	//-----------------------------------------------

	while (!mainWindow.getShouldClose())
	{
		//(�߰�����) 
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		lastTime = now;

		glfwPollEvents();
				
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//-----View transform matrix ���--------
		front = ForwardVecFromYawPitch(yaw, pitch); //���� yaw,pitch ������ front ���� ���
		right = glm::cross(front, up); //�������� ������ front�� up ������ cross product
		glm::mat4 camera = glm::lookAt(eyePos, //EYE, AT, UP ���͸� lookAt �Լ��� �Ѱ� view transform matrix ��� ��ȯ
									eyePos + front,
									glm::vec3(0, 1, 0));
		actualUp = glm::cross(right, front); //(�߰�������) ���� up vector ������ right�� front ���� cross product�� �ٽ� ���
		//---------------------------------------
		
		shaderList[0]->UseShader();
		uniformModel = shaderList[0]->GetModelLocation();
		uniformProjection = shaderList[0]->GetProjectionLocation();
		uniformView = shaderList[0]->GetViewLocation(); //<-------------------Shader�� �߰��� view ��ġ

		//-----Ű����, ���콺�� �Է����� �� �־�� �� ��-------
		if (mainWindow.getKeys()[GLFW_KEY_W] == GL_TRUE)
		{
			eyePos += movementSpeed * front * deltaTime; //W�� �ԷµǸ�, EYE ��ġ�� front �������� �̵�. (movementSpeed�� �̵� �ӵ� ����)
		}
		if (mainWindow.getKeys()[GLFW_KEY_S] == GL_TRUE)
		{
			eyePos += movementSpeed * -front * deltaTime; //S�� �ԷµǸ�, EYE ��ġ�� front �ݴ� �������� �̵�. (movementSpeed�� �̵� �ӵ� ����)
		}
		if (mainWindow.getKeys()[GLFW_KEY_A] == GL_TRUE)
		{
			eyePos += movementSpeed * -right * deltaTime; //A�� �ԷµǸ�, EYE ��ġ�� right �ݴ� �������� �̵�. (movementSpeed�� �̵� �ӵ� ����)
		}
		if (mainWindow.getKeys()[GLFW_KEY_D] == GL_TRUE)
		{
			eyePos += movementSpeed * right* deltaTime; //D�� �ԷµǸ�, EYE ��ġ�� right �������� �̵�. (movementSpeed�� �̵� �ӵ� ����)
		}
		//(�߰�������) Q�� E�� ������ actualUp �������� �̵�
		if (mainWindow.getKeys()[GLFW_KEY_Q] == GL_TRUE)
		{
			eyePos += movementSpeed * -actualUp * deltaTime;
		}
		if (mainWindow.getKeys()[GLFW_KEY_E] == GL_TRUE)
		{
			eyePos += movementSpeed * actualUp * deltaTime;
		}

		yaw += rotationSpeed*mainWindow.getXChange(); //���콺�� x�������� �����θ�ŭ yaw ���� ����
		pitch += rotationSpeed*mainWindow.getYChange(); //���콺�� y�������� �����θ�ŭ pitch ���� ����
		//-------------------------------------------------

		glm::mat4 model = glm::mat4{ 1.0f }; 
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -2.5f));
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.0f)); 
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera)); //<-------------------uniformView �����ͷ� view matrix ����
		meshList[0]->RenderMesh();

		model = glm::mat4{ 1.0f };
		model = glm::translate(model, glm::vec3(0.0f, 1.0f, -2.5f));
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.0f)); 
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[1]->RenderMesh();
				
		glUseProgram(0); //unassign shader

		mainWindow.swapBuffers();
	}
	
	for (auto m : meshList)
	{
		delete m;
	}
	for (auto s : shaderList)
	{
		delete s;
	}

	return 0;
}