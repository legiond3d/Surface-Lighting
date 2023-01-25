#ifndef CAMERA_H//����� ���������� ��������� ������ 1 ���
#define CAMERA_H

#include<GL/glew.h>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>

enum camera_movement//������������ � ���������� ���������� ����������� �������� ������
{
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT,
	UP,
	DOWN
};
//�������������� ������ �� ���������
const GLfloat YAW = -90.0f;//�������� ��������
const GLfloat PITCH = 0.0f;//�������� �������
const GLfloat SPEED = 3.0f;//�������� ��������(�����) ������
const GLfloat SENS = 0.1f;//���������������� ����
const GLfloat ZOOM = 45.0f;//��������� ����������� ���� ������

class camera//����� ��������� ������ 
{
private:
	void update_camera_vectors()//����������� ����� Front ������ � ����������� �� ����� ������ ����������� ������ ������
	{
		glm::vec3 front;//������ �����������
		//��������� �������������� ������ �����������
		front.x = cos(glm::radians(this->Yaw))*cos(glm::radians(this->Pitch));
		front.y = sin(glm::radians(this->Pitch));
		front.z = sin(glm::radians(this->Yaw))*cos(glm::radians(this->Pitch));
		this->Front = glm::normalize(front);//������������� ������ �����������
		//������������� ������� Right � UP � ����������� ��
		this->Up = glm::normalize(glm::cross(this->Right, this->Front));
		this->Right = glm::normalize(glm::cross(this->Front, this->World_up));
	}
public:
	//�������� ������:
	glm::vec3 Position;//������� ������, ��������� ����� � ������������ ������
	//�� ���������� ������ ������� ���������
	glm::vec3 Front;//��� z, ������� ���������� �� �����������
	glm::vec3 Up;//��� y, ������� ���������� �����
	glm::vec3 Right;//��� �, ������� ���������� ������
	glm::vec3 World_up;//��������������� ������ �� �

	//�������� ����:
	GLfloat Yaw;//��������
	GLfloat Pitch;//������

	//�������������� ������:
	GLfloat movement_speed;//�������� ������������
	GLfloat mouse_sens;//���������������� ������ (���������������� ����)
	GLfloat Zoom;//������� (���� ������)

	camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), GLfloat yaw = YAW, GLfloat pitch = PITCH) :Front(glm::vec3(0.0f, 0.0f, -1.0f)), movement_speed(SPEED), mouse_sens(SENS), Zoom(ZOOM)//����������� ������ � ���������
	{
		this->Position = position;
		this->World_up = up;
		this->Yaw = yaw;
		this->Pitch = pitch;
		this->update_camera_vectors();
	}
	camera(GLfloat posX, GLfloat posY, GLfloat posZ, GLfloat upX, GLfloat upY, GLfloat upZ, GLfloat yaw, GLfloat pitch) :Front(glm::vec3(0.0f, 0.0f, -1.0f)), movement_speed(SPEED), mouse_sens(SENS), Zoom(ZOOM)//����������� �� ���������� ����������
	{
		this->Position = glm::vec3(posX, posY, posZ);
		this->World_up = glm::vec3(upX, upY, upZ);
		this->Yaw = yaw;
		this->Pitch = pitch;
		this->update_camera_vectors();
	}
	glm::mat4 get_view_matrix()//���������� ������� ���� (�����������)
	{
		return glm::lookAt(this->Position, this->Position + this->Front, this->Up);
	}
	void process_keyboard(camera_movement direction, GLfloat delta_time)//�����, ���������� �� ����������� ������ � ����������� �� ���������� ����������� ��������
	{
		GLfloat velocity = this->movement_speed*delta_time;//���������� �������� �������� ������, ��� �������� �������� ���������� �� �����, ������� ������������� �� ���������� �����
		if (direction == FORWARD)
			this->Position += this->Front*velocity;
		if (direction == BACKWARD)
			this->Position -= this->Front*velocity;
		if (direction == LEFT)
			this->Position -= this->Right*velocity;
		if (direction == RIGHT)
			this->Position += this->Right*velocity;
		if (direction == UP)
			this->Position += this->Up*velocity;
		if (direction == DOWN)
			this->Position -= this->Up*velocity;
	}
	void process_mouse_movement(GLfloat xoffset, GLfloat yoffset, GLboolean extra_pitch = true)//������� ��� ����������� �������� ������� ������, ���������� �� �������� �����
	{
		xoffset *= this->mouse_sens;
		yoffset *= this->mouse_sens;
		this->Yaw += xoffset;
		this->Pitch += yoffset;
		//����������� ��� ���� �������, ����� ������ ���� �������� ��� ����� 90 �������� ����� ��� ����
		if (extra_pitch)
		{
			if (this->Pitch > 89.0f)
				this->Pitch = 89.0f;
			if (this->Pitch < -89.0f)
				this->Pitch = -89.0f;
		}
		this->update_camera_vectors();//������������� ������� � �������������� ����� ������
	}
	void process_mouse_scroll(GLfloat yoffset)//������ ��������� ���� ������ ��� ��������� ������� �����
	{
		if (this->Zoom >= 1.0f&&this->Zoom <= 45.0f)
			this->Zoom -= yoffset;
		if (this->Zoom <= 1.0f)
			this->Zoom = 1.0f;//����������� ���� ������ 1 ������
		if (this->Zoom >= 45.0f)
			this->Zoom = 45.0f;//������������ ���� ������ 45 ��������
	}
};
#endif