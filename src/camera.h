#ifndef CAMERA_H//чтобы подключать директивы только 1 раз
#define CAMERA_H

#include<GL/glew.h>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>

enum camera_movement//перечисление с возможными вариантами направления движения камеры
{
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT,
	UP,
	DOWN
};
//характеристики камеры по умолчанию
const GLfloat YAW = -90.0f;//значение рысканья
const GLfloat PITCH = 0.0f;//значение тангажа
const GLfloat SPEED = 3.0f;//скорость движения(полёта) камеры
const GLfloat SENS = 0.1f;//чувствительность мыши
const GLfloat ZOOM = 45.0f;//начальное приближение угла обзора

class camera//класс свободной камеры 
{
private:
	void update_camera_vectors()//вычисляется новый Front вектор в зависимости от углов эйлера направления обзора камеры
	{
		glm::vec3 front;//вектор направления
		//вычисляем результирующий вектор направления
		front.x = cos(glm::radians(this->Yaw))*cos(glm::radians(this->Pitch));
		front.y = sin(glm::radians(this->Pitch));
		front.z = sin(glm::radians(this->Yaw))*cos(glm::radians(this->Pitch));
		this->Front = glm::normalize(front);//нормализируем вектор направления
		//пересчитываем вектора Right и UP и нормализуем их
		this->Up = glm::normalize(glm::cross(this->Right, this->Front));
		this->Right = glm::normalize(glm::cross(this->Front, this->World_up));
	}
public:
	//атрибуты камеры:
	glm::vec3 Position;//позиция камеры, некоторая точка с направлением обзора
	//мы используем правую систему координат
	glm::vec3 Front;//ось z, которая направлена на наблюдателя
	glm::vec3 Up;//ось y, которая направлена вверх
	glm::vec3 Right;//ось х, которая направлена вправо
	glm::vec3 World_up;//нормализованный вектор по у

	//эйлеровы углы:
	GLfloat Yaw;//рысканье
	GLfloat Pitch;//тангаж

	//характеристики камеры:
	GLfloat movement_speed;//скорость передвижения
	GLfloat mouse_sens;//чувствительность камеры (чувствительность мыши)
	GLfloat Zoom;//масштаб (угол обзора)

	camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), GLfloat yaw = YAW, GLfloat pitch = PITCH) :Front(glm::vec3(0.0f, 0.0f, -1.0f)), movement_speed(SPEED), mouse_sens(SENS), Zoom(ZOOM)//конструктор класса с векторами
	{
		this->Position = position;
		this->World_up = up;
		this->Yaw = yaw;
		this->Pitch = pitch;
		this->update_camera_vectors();
	}
	camera(GLfloat posX, GLfloat posY, GLfloat posZ, GLfloat upX, GLfloat upY, GLfloat upZ, GLfloat yaw, GLfloat pitch) :Front(glm::vec3(0.0f, 0.0f, -1.0f)), movement_speed(SPEED), mouse_sens(SENS), Zoom(ZOOM)//конструктор со скалярными значениями
	{
		this->Position = glm::vec3(posX, posY, posZ);
		this->World_up = glm::vec3(upX, upY, upZ);
		this->Yaw = yaw;
		this->Pitch = pitch;
		this->update_camera_vectors();
	}
	glm::mat4 get_view_matrix()//возвращаем матрицу вида (наблюдателя)
	{
		return glm::lookAt(this->Position, this->Position + this->Front, this->Up);
	}
	void process_keyboard(camera_movement direction, GLfloat delta_time)//метод, отвечающий за перемещение камеры в зависимости от выбранного направления движения
	{
		GLfloat velocity = this->movement_speed*delta_time;//определяем скорость движения камеры, как желаемая скорость умноженная на время, которое затрачивается на прорисовку кадра
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
	void process_mouse_movement(GLfloat xoffset, GLfloat yoffset, GLboolean extra_pitch = true)//функция для определения смещения взгляда камеры, полученное от движения мышью
	{
		xoffset *= this->mouse_sens;
		yoffset *= this->mouse_sens;
		this->Yaw += xoffset;
		this->Pitch += yoffset;
		//ограничения для угла тангажа, чтобы нельзя было смотреть под углом 90 градусов вверх или вниз
		if (extra_pitch)
		{
			if (this->Pitch > 89.0f)
				this->Pitch = 89.0f;
			if (this->Pitch < -89.0f)
				this->Pitch = -89.0f;
		}
		this->update_camera_vectors();//пересчитываем вектора с использованием углов эйлера
	}
	void process_mouse_scroll(GLfloat yoffset)//функия изменения угла обзора при прокрутки колёсика мышки
	{
		if (this->Zoom >= 1.0f&&this->Zoom <= 45.0f)
			this->Zoom -= yoffset;
		if (this->Zoom <= 1.0f)
			this->Zoom = 1.0f;//минимальный угол обзора 1 градус
		if (this->Zoom >= 45.0f)
			this->Zoom = 45.0f;//максимальный угол обзора 45 градусов
	}
};
#endif