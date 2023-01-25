#define GLEW_STATIC//���������� ����������� ������ ���������� GLEW
#include<GL/glew.h>//���������� GLEW
#include<GLFW/glfw3.h>//���������� GLFW
#include<SOIL.h>//���������� SOIL ��� �������
//���������� GLM ��� ������ � ��������� � ���������
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
//����������� ���������� �++
#include<iostream>
#include<cmath>
#include<Windows.h>
#include<clocale>
//���������� ���� ������������ ����� (� �������� ��� ����������� ������� � ������)
#include"shader.h"
#include"camera.h"

//���������� ���������� ����
const GLuint WIDTH = 1280, HEIGHT = 720;
//������
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

camera cam(glm::vec3(0.0f, 0.0f, 3.0f));//������ ������ ������ ������
GLfloat lastX = WIDTH / 2, lastY = HEIGHT / 2;//����� ����� ����
bool firstMouse = true;//���������� ��� ����������� ������ �� ��� ���������� ��������� ������� ����� �������� ����
bool keys[1024];//������ ��������� ������

//�������� ��������� �����
glm::vec3 lightPos(1.2f,1.0f,2.0f);//������� �����

//��������� ������� �������, �� ������� ����� ��������������� ������� ���������
glm::vec3 ploskPos(-1.5f, 0.0f, 0.0f);

//��������� �������� �������, �� ������� ����� ����������������� ������ ��������� ����
glm::vec3 guroPos(1.5f, 0.0f, 0.0f);

//delta-time:
GLfloat deltaTime = 0.0f;//�������� ������� ����� ��������� � ������� ������
GLfloat lastFrame = 0.0f;//����� ������ ���������� �����

void intro();//�������� ������� ��� ������ ����������� � ������� � �������� ��������� ������

//��������� ������� �������� �������
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);//�������� ������� ��������� ������
void mouse_callback(GLFWwindow* window, double xpos, double ypos);//�������� ������� ��������� ������ ��� ���������� ������� ��� ������ ����
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);//�������� ������� ��������� ������ ��� ��������� �������� ����������� (���������� �������� ���� ������ ������)
void do_movement();//�������� ������� ��� ��������, � ����� ����������� ����� ��������� ������
void dinamic_lamp();//�������� ������� ��� �������� �����

int main()
{
	intro();
	glfwInit();//������������� GLFW
	//����� ����������� ��������� ������ OpenGL (OpenGL 3.3, �.�. ��������� ������ OpenGL �������� �� �� ���� �����������)
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//��������� ��������, ��� �������� �������� ��������
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);//��������� ����������� �������� ������ ����

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "lab_9_program", nullptr, nullptr);//�������� ����
	if (window == nullptr)//���� �� ������� ������� ����
	{
		std::cout << " Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glewExperimental = GL_TRUE;//�������� ������� GLEW ��� ���������� OpenGL 
	glewInit();
	if (glewInit() != GLEW_OK)//���� ������������� GLEW ������ �������� 
	{
		std::cout << " Failed to initialize GLEW" << std::endl;
		return -1;
	}
	int width, height;//���������� ��� ������ � ������ ����
	glfwGetFramebufferSize(window, &width, &height);//�������� �������� ������ � ������ ����
	glViewport(0, 0, width, height);//���������� ������ ��������������� ���� (������������ ��� ����� ������������ ������ � ���������� ������������ �������� ����)

	//������������ ����������� ������� ��������� ������
	glfwSetKeyCallback(window, key_callback);//������� ������� ��������� ������ � GLFW
	glfwSetCursorPosCallback(window, mouse_callback);//������� �������, ����������� ��������� ������� � GLFW
	glfwSetScrollCallback(window, scroll_callback);//������� ������� ��� ��������������� � GLFW
	
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);//����� ��� GLFW, ����� ������ �� ������� �� ����� ����

	glEnable(GL_DEPTH_TEST);//�������� ������ �������
	
	//������ �����:
	Shader phong("phong.vsh", "phong.fsh");//������ ������ ������ ������ ������ (���������� ��������� � ������ ��������� � ����������� ������, ������ �� ��� ��������� ���������)
	//������ �������� ���������:
	Shader plosk("plosk.vsh","plosk.fsh");//���������� ������ ������ ������ ������, �� ��� ������ �������� ���������
	//������ ��������� ����:
	Shader guro("guro.vsh", "guro.fsh");//������ ������ ������ ������ ��� ������ ��������� ����
	//������� ��� �����:
	Shader lampShader("lamp.vsh","lamp.fsh");//������ ������ ������ ������ ��� ��������� �����
	GLfloat tmp = sqrt(2) / 2;//�������� ��� ���������� ������� ������� ��� ����� ��������

	//������������� ���������� ������ ��� �������� (����� ���������� ������, � ������ ������� ������� � ������������ (����� 5 ������������))
	GLfloat vertices1[] = {
		//��� �������� (������� �� 2 �������������)
		-0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,
		0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,
		0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f,
		0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f,
		-0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f,
		-0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,
		//������ �����������
		-0.5f, -0.5f, -0.5f, 0.0f, tmp, -tmp,
		0.5f, -0.5f, -0.5f, 0.0f, tmp, -tmp,
		0.0f, 0.5f, 0.0f, 0.0f, tmp, -tmp,
		//������ �����������
		0.0f, 0.5f, 0.0f, tmp, tmp, 0.0f,
		0.5f, -0.5f, -0.5f, tmp, tmp, 0.0f,
		0.5f, -0.5f, 0.5f, tmp, tmp, 0.0f,
		//�������� �����������
		0.5f, -0.5f, 0.5f, 0.0f, tmp, tmp,
		0.0f, 0.5f, 0.0f, 0.0f, tmp, tmp,
		-0.5f, -0.5f, 0.5f, 0.0f, tmp, tmp,
		//����� �����������
		-0.5f, -0.5f, 0.5f, -tmp, tmp, 0.0f,
		0.0f, 0.5f, 0.0f, -tmp, tmp, 0.0f,
		-0.5f, -0.5f, -0.5f, -tmp, tmp, 0.0f
	};

	//������������� ���������� ������ ��� ���� (3 ������� ����� - ��� ���������� ������, � 3 ������ ������ - ��� ���� ������ �������)
	GLfloat vertices2[] = {
		-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
		0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
		0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
		0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
		-0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
		-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,

		-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
		0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
		0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
		0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
		-0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
		-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,

		-0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f,

		0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f,
		0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
		0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
		0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f,
		0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f,

		-0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,
		0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,
		0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f,
		0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f,
		-0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f,
		-0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,

		-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
		-0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
		-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f
	};
	GLuint VBO, VAO;
	glGenVertexArrays(1, &VAO);//������ ������ ���������� �������
	glGenBuffers(1, &VBO);//������ ������ ���������� ������

	glBindBuffer(GL_ARRAY_BUFFER, VBO);//����������� ��� ARRAY_BUFFER � ������ ������
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), vertices2, GL_STATIC_DRAW);//�������� ��������� ������ � �����

	//�������� ������� ������
	glBindVertexArray(VAO);//����������� ������ ���������� �������
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);//�������� OpenGL ��� ������������������ ��������� ������
	glEnableVertexAttribArray(0);//�������� �������
	
	//�������� �������� ������
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GL_FLOAT), (GLvoid*)(3 * sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);//���������� VAO

	GLuint lightVBO;//������ ���������� ������ ��� �����
	glGenBuffers(1, &lightVBO);
	glBindBuffer(GL_ARRAY_BUFFER, lightVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), vertices2, GL_STATIC_DRAW);

	//������ ���������� ������� ��� �����
	GLuint lightVAO;

	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);

	glBindBuffer(GL_ARRAY_BUFFER, lightVBO);//��������� � VBO ���� �����

	//������������� �������� ����� (��������� ���������� ������)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);//���������� ��� �������
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);

	while (!glfwWindowShouldClose(window))//������� ����, ���� �� �������� ���������� � ��������
	{
		GLfloat currentFrame = glfwGetTime();//�������� ����� ������ �������� �����
		deltaTime = currentFrame - lastFrame;//����� �������� ������� ����� ������� � ���������� �������
		lastFrame = currentFrame;//� �������� ����������� ����� ������������� ��������� ���������

		glfwPollEvents();//����������� �� ���� �� ������� �����-���� �������
		do_movement();//�������� ������� �������� ������ �������� ������
		dinamic_lamp();//�������� ������� ��� ����������� �����

		glClearColor(0.05f, 0.05f, 0.05f, 1.0f);//������������� ��������� ���� ����� ������� ������
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);//���������� ���������, ���������� �� ���������� ������� � ������ �����

		phong.use();//���������� ��������� ��������� ��� ���������� ������� � �������������� ��������� �� �����

		//��������� uniform ���������� ��� ��������� ���������
		GLint objectColorLoc = glGetUniformLocation(phong.Program, "object_color");//���� ������ �������
		GLint lightColorLoc = glGetUniformLocation(phong.Program, "light_color");//���� ��������� �����
		GLint lightPosLoc = glGetUniformLocation(phong.Program, "light_pos");//������� ��������� ���������
		GLint viewPosLoc = glGetUniformLocation(phong.Program, "view_pos");//������� ����������� (������)
		//glUniform3f(objectColorLoc, 0.352941f, 0.0f, 0.784313f);//����� ���� �������
		glUniform3f(objectColorLoc, 0.0f, 1.0f, 1.0f);//����� ���� �������
		glUniform3f(lightColorLoc, 1.0f, 1.0f, 1.0f);//����� ���� ��������� �����
		glUniform3f(lightPosLoc, lightPos.x, lightPos.y, lightPos.z);//����� ������� ���������
		glUniform3f(viewPosLoc, cam.Position.x, cam.Position.y, cam.Position.z);//����� ������� ����������� (������)

		//������ ������������� ������
		glm::mat4 view;
		glm::mat4 projection;
		view = cam.get_view_matrix();//�������� ������� ���� (�����������)
		projection = glm::perspective(cam.Zoom, (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);//�������� �������� ����������� �������� �����
		//������������� �������
		GLint modelLoc = glGetUniformLocation(phong.Program, "model");
		GLint viewLoc = glGetUniformLocation(phong.Program, "view");
		GLint projLoc = glGetUniformLocation(phong.Program, "projection");

		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
		
		glBindVertexArray(VAO);
		glm::mat4 model;
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glDrawArrays(GL_TRIANGLES, 0, 36);//��������� �������
		glBindVertexArray(0);

		//��������� ������� ��� �������� ���������:
		plosk.use();//���������� ��������� ��������� ��� ���������� ������� � ������� ����������
		//��������� uniform ���������� ��� ��������� ���������
		GLint object_color_loc = glGetUniformLocation(plosk.Program, "object_color");//���� ������ �������
		GLint light_color_loc = glGetUniformLocation(plosk.Program, "light_color");//���� ��������� �����
		GLint light_pos_loc = glGetUniformLocation(plosk.Program, "light_pos");//������� ��������� ���������
		GLint view_pos_loc = glGetUniformLocation(plosk.Program, "view_pos");//������� ����������� (������)
		//glUniform3f(object_color_loc, 0.803921f, 0.0f, 0.352941f);//����� ���� �������
		glUniform3f(object_color_loc, 1.0f, 0.0f, 0.0f);//����� ���� �������
		glUniform3f(light_color_loc, 1.0f, 1.0f, 1.0f);//����� ���� ��������� �����
		glUniform3f(light_pos_loc, lightPos.x, lightPos.y, lightPos.z);//����� ������� ���������
		glUniform3f(view_pos_loc, cam.Position.x, cam.Position.y, cam.Position.z);//����� ������� ����������� (������)

		//������������� �������
		modelLoc = glGetUniformLocation(plosk.Program, "model");
		viewLoc = glGetUniformLocation(plosk.Program, "view");
		projLoc = glGetUniformLocation(plosk.Program, "projection");

		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

		model = glm::mat4();
		model = glm::translate(model, ploskPos);

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);//��������� ���� ��� �������� ���������
		glBindVertexArray(0);

		//��������� ������� ��� ������ ��������� ����:
		guro.use();//���������� ��������� ��������� ��� ���������� ������� � ������� ��������� �� ����

		//��������� uniform ���������� ��� ��������� ���������
		GLint object_color_gur = glGetUniformLocation(guro.Program, "object_color");//���� ������ �������
		GLint light_color_gur = glGetUniformLocation(guro.Program, "light_color");//���� ��������� �����
		GLint light_pos_gur = glGetUniformLocation(guro.Program, "light_pos");//������� ��������� ���������
		GLint view_pos_gur = glGetUniformLocation(guro.Program, "view_pos");//������� ����������� (������)
		//glUniform3f(object_color_loc, 0.803921f, 0.0f, 0.352941f);//����� ���� �������
		glUniform3f(object_color_gur, 1.0f, 0.0f, 1.0f);//����� ���� �������
		glUniform3f(light_color_gur, 1.0f, 1.0f, 1.0f);//����� ���� ��������� �����
		glUniform3f(light_pos_gur, lightPos.x, lightPos.y, lightPos.z);//����� ������� ���������
		glUniform3f(view_pos_gur, cam.Position.x, cam.Position.y, cam.Position.z);//����� ������� ����������� (������)

		//������ ������������� ������
		view = glm::mat4();
		projection = glm::mat4();
		view = cam.get_view_matrix();//�������� ������� ���� (�����������)
		projection = glm::perspective(cam.Zoom, (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);//�������� �������� ����������� �������� �����

		//������������� �������
		modelLoc = glGetUniformLocation(guro.Program, "model");
		viewLoc = glGetUniformLocation(guro.Program, "view");
		projLoc = glGetUniformLocation(guro.Program, "projection");

		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

		model = glm::mat4();
		model = glm::translate(model, guroPos);

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);//��������� ���� ��� ������ ��������� ����
		glBindVertexArray(0);

		//��������� ������� �����
		lampShader.use();//�������� ��������� ��������� ��� ���������� ����� (��������� �����)

		//������������� �������
		modelLoc = glGetUniformLocation(lampShader.Program, "model");
		viewLoc = glGetUniformLocation(lampShader.Program, "view");
		projLoc = glGetUniformLocation(lampShader.Program, "projection");

		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

		model = glm::mat4();
		model = glm::translate(model, lightPos);
		model = glm::scale(model, glm::vec3(0.2f));//������ ��� ����� � 5 ��� ������ ���� �������

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

		glBindVertexArray(lightVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);//��������� ���� �����
		glBindVertexArray(0);

		glfwSwapBuffers(window);//����������� ����� ������
	}
	//����������� �������
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &lightVBO);
	glDeleteVertexArrays(1, &lightVAO);
	glfwTerminate();//������� ���������� ������� (��������� GLFW)
	return 0;
}
//���������� ������� key_callback
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE&&action == GLFW_PRESS)//���� ������������ �������� escape, ���������� �����������
		glfwSetWindowShouldClose(window, GL_TRUE);
	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			keys[key] = true;
		else if (action == GLFW_RELEASE)
			keys[key] = false;
	}
}
void do_movement()//���������� �������, ���������� �� ����������� ������ � ����������� �� ������� �������
{
	GLfloat cameraSpeed = 5.0f*deltaTime;//����� �������� ����� ������
	if (keys[GLFW_KEY_W])
		cam.process_keyboard(FORWARD, deltaTime);
	if (keys[GLFW_KEY_S])
		cam.process_keyboard(BACKWARD, deltaTime);
	if (keys[GLFW_KEY_A])
		cam.process_keyboard(LEFT, deltaTime);
	if (keys[GLFW_KEY_D])
		cam.process_keyboard(RIGHT, deltaTime);
	if (keys[GLFW_KEY_LEFT_SHIFT])
		cam.process_keyboard(UP, deltaTime);
	if (keys[GLFW_KEY_LEFT_CONTROL])
		cam.process_keyboard(DOWN, deltaTime);
	if (keys[GLFW_KEY_E])
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);//������������ ��� ������������, ������ �������
	if (keys[GLFW_KEY_Q])
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);//������������ ������ � �������������
}
void mouse_callback(GLFWwindow* window, double xpos, double ypos)//���������� ������� ��� �������� ������ ������
{
	if (firstMouse)//���� ������ ���������� ��������� ������� ������������ ����
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}
	//���������� ����, ������������ ����� ��������� � ������� ������
	GLfloat xoffset = xpos - lastX;
	GLfloat yoffset = lastY - ypos;//������� ���������� y ���������� ���� 

	lastX = xpos;
	lastY = ypos;

	cam.process_mouse_movement(xoffset, yoffset);//�������� ��������������� ����� �� ������ ������
}
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)//������� ��� ��������� ���� ������ �������� �����
{
	cam.process_mouse_scroll(yoffset);//�������� ��������������� ����� �� ������ ������
}
void dinamic_lamp()//���������� ������� ��� ����������� ����� �� ���� ���������
{
	GLfloat speed = 1.0f;
	if (keys[GLFW_KEY_UP])
		lightPos.z -= deltaTime*speed;
	if (keys[GLFW_KEY_DOWN])
		lightPos.z += deltaTime*speed;
	if (keys[GLFW_KEY_RIGHT])
		lightPos.x += deltaTime*speed;
	if (keys[GLFW_KEY_LEFT])
		lightPos.x -= deltaTime*speed;
	if (keys[GLFW_KEY_END])
		lightPos.y -= deltaTime*speed;
	if (keys[GLFW_KEY_HOME])
		lightPos.y += deltaTime*speed;
}
void intro()//�������, ��������� ����������� � ������� � ���������� �� �������������
{
	setlocale(LC_ALL, "Rus");
	cout << " ������ ��������� ������������� ��������� ������ ���������: " << endl;
	cout << " ������ ����� � �������� ���������" << endl;
	cout << endl << " � ������� ���� �������� ��������� �������:" << endl;
	cout << " ��� �������� ���� - ������� ESC" << endl;
	cout << " ��� ������ ���� ����� ����������� �����" << endl;
	cout << " ������� ����� ��������� ��������� ���� ������ (���������� ������)" << endl;
	cout << " ������� W, A, S, D ������ ��� ����������� ������ �� �����" << endl;
	cout << " ������� Left Shift ��������� ������ ����������� �����������" << endl;
	cout << " ������� Left Ctrl ��������� ������ ���������� �����������" << endl;
	cout << " ��� ������ ��������� ����� ���������� ����� �� ��� Z(�����/�����), �� ��� X (�����/������)" << endl;
	cout << " ��� ������ HOME ����� ��������� ����� �� ��� Y, � ������� END �������� ����� �� ��� Y" << endl;
}