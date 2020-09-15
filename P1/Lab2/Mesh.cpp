#include "Mesh.h"



Mesh::Mesh()
{
	VAO = 0;
	VBO = 0;
	IBO = 0;
	indexCount = 0;
}

void Mesh::CreateMesh(GLfloat * vertices, unsigned int * indices, unsigned int numOfVertices, unsigned int numOfIndices)
{
	indexCount = numOfIndices;

	glGenVertexArrays(1, &VAO); //Vertex Array Object(VAO) ID ����
	glBindVertexArray(VAO); //VAO�� Bind --> ���ݺ��� VAO ���� �����͸� �����ϰڴٴ� �ǹ�

	glGenBuffers(1, &IBO); //Index Buffer Object(IBO) ID ����
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO); //IBO�� Bind �ϸ鼭, Index Array��� ���
	//���� ������ �Ҵ�
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * numOfIndices, indices, GL_STATIC_DRAW); 

	glGenBuffers(1, &VBO); //Vertex Buffer Object(VBO) ID ����
	glBindBuffer(GL_ARRAY_BUFFER, VBO); //VBO�� Bind �ϸ鼭, Vertex Array��� ���
	//���� ������ �Ҵ�
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0])*numOfVertices, vertices, GL_STATIC_DRAW); 

	glEnableVertexAttribArray(0); //Attribute Enable
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0); //ù ���ڰ� layout location ����. 5��° 0�� stride, ������ 0�� offset
	
	glBindVertexArray(0); //Unbind VAO

	glBindBuffer(GL_ARRAY_BUFFER, 0); //Unbind VAO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); //Unbind IBO
}

void Mesh::RenderMesh()
{
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void Mesh::ClearMesh()
{
	if (IBO != 0)
	{
		glDeleteBuffers(1, &IBO); //������ Buffer data�� �����Ͽ� GPU �޸� ���� Ȯ��
		IBO = 0;
	}

	if (VBO != 0)
	{
		glDeleteBuffers(1, &VBO);
		VBO = 0;
	}

	if (VAO != 0)
	{
		glDeleteVertexArrays(1, &VAO);
		VAO = 0;
	}

	indexCount = 0;
}


Mesh::~Mesh()
{
	ClearMesh();
}
