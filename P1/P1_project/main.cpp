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

//Yaw, Pitch (theta & phi) 각도를 입력으로 벡터 계산
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
	
	//------View tranform matrix 계산용 변수들--------
	glm::vec3 eyePos = glm::vec3(0, 0, 1); //EYE 위치
	glm::vec3 front = glm::vec3(0, 0, -1); //EYE가 바라보는 위치 (eyePos + front가 AT 역할)
	glm::vec3 up = glm::vec3(0, 1, 0); //UP 벡터
	glm::vec3 right; //Right 벡터. 카메라의 오른쪽 방향. A/D 키 입력 처리를 위해 필요함
	GLfloat movementSpeed = 1.0f; //이동 속도 조절 변수
	GLfloat yaw = -90; //pitch가 0, yaw가 -90도일때 front vector가 (0,0,-1)
	GLfloat pitch = 0;
	GLfloat rotationSpeed = 1.0f; //회전 속도 조절 변수
	glm::vec3 actualUp; //(추가점수) front 방향 변화에 따른 실제 up vector
	GLfloat deltaTime = 0.0f; //(추가점수) 프레임 갱신에 걸린 시간
	GLfloat lastTime = glfwGetTime(); //(추가점수) 지난번 프레임의 시간
	//-----------------------------------------------

	while (!mainWindow.getShouldClose())
	{
		//(추가점수) 
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		lastTime = now;

		glfwPollEvents();
				
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//-----View transform matrix 계산--------
		front = ForwardVecFromYawPitch(yaw, pitch); //현재 yaw,pitch 값으로 front 벡터 계산
		right = glm::cross(front, up); //오른쪽의 방향은 front와 up 벡터의 cross product
		glm::mat4 camera = glm::lookAt(eyePos, //EYE, AT, UP 벡터를 lookAt 함수에 넘겨 view transform matrix 결과 반환
									eyePos + front,
									glm::vec3(0, 1, 0));
		actualUp = glm::cross(right, front); //(추가점수용) 실제 up vector 방향을 right와 front 벡터 cross product로 다시 계산
		//---------------------------------------
		
		shaderList[0]->UseShader();
		uniformModel = shaderList[0]->GetModelLocation();
		uniformProjection = shaderList[0]->GetProjectionLocation();
		uniformView = shaderList[0]->GetViewLocation(); //<-------------------Shader에 추가된 view 위치

		//-----키보드, 마우스의 입력으로 해 주어야 할 것-------
		if (mainWindow.getKeys()[GLFW_KEY_W] == GL_TRUE)
		{
			eyePos += movementSpeed * front * deltaTime; //W가 입력되면, EYE 위치를 front 방향으로 이동. (movementSpeed로 이동 속도 조절)
		}
		if (mainWindow.getKeys()[GLFW_KEY_S] == GL_TRUE)
		{
			eyePos += movementSpeed * -front * deltaTime; //S가 입력되면, EYE 위치를 front 반대 방향으로 이동. (movementSpeed로 이동 속도 조절)
		}
		if (mainWindow.getKeys()[GLFW_KEY_A] == GL_TRUE)
		{
			eyePos += movementSpeed * -right * deltaTime; //A가 입력되면, EYE 위치를 right 반대 방향으로 이동. (movementSpeed로 이동 속도 조절)
		}
		if (mainWindow.getKeys()[GLFW_KEY_D] == GL_TRUE)
		{
			eyePos += movementSpeed * right* deltaTime; //D가 입력되면, EYE 위치를 right 방향으로 이동. (movementSpeed로 이동 속도 조절)
		}
		//(추가점수용) Q와 E를 누르면 actualUp 방향으로 이동
		if (mainWindow.getKeys()[GLFW_KEY_Q] == GL_TRUE)
		{
			eyePos += movementSpeed * -actualUp * deltaTime;
		}
		if (mainWindow.getKeys()[GLFW_KEY_E] == GL_TRUE)
		{
			eyePos += movementSpeed * actualUp * deltaTime;
		}

		yaw += rotationSpeed*mainWindow.getXChange(); //마우스가 x방향으로 움직인만큼 yaw 각도 변경
		pitch += rotationSpeed*mainWindow.getYChange(); //마우스가 y방향으로 움직인만큼 pitch 각도 변경
		//-------------------------------------------------

		glm::mat4 model = glm::mat4{ 1.0f }; 
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -2.5f));
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.0f)); 
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera)); //<-------------------uniformView 포인터로 view matrix 전달
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