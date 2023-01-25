#ifndef SHADER_H//чтобы подключались директивы только 1 раз
#define SHADER_H
#include<string>
#include<fstream>
#include<sstream>
#include<iostream>
#include<GL/glew.h>
using namespace std;
class Shader
{
public:
	GLuint Program;//идентификатор программы
	Shader(const GLchar* vertexPath, const GLchar* fragmentPath)//конструктор для считывания с файлов и сборки шейдеров
	{
		string vertexCode;//строка для хранения кода вершинного шейдера
		string fragmentCode;//строка для хранения кода фрагментного шейдера
		ifstream vShaderFile;//поток для извлечения из файла вершинного шейдера
		ifstream fShaderFile;//поток для извлечения из файла фрагментного шейдера
		//проверяем могут ли потоки выдавать исключения
		vShaderFile.exceptions(ifstream::badbit);
		fShaderFile.exceptions(ifstream::badbit);
		try//блок, в котором возможно возникновение исключения
		{
			vShaderFile.open(vertexPath);//открываем файл с вершинным шейдером
			fShaderFile.open(fragmentPath);//открываем файл с фрагментным шейдером
			stringstream vShaderStream, fShaderStream;//потоки считывания строк
			//считываем данные в потоки
			vShaderStream << vShaderFile.rdbuf();
			fShaderStream << fShaderFile.rdbuf();
			//закрываем файлы с шейдерами
			vShaderFile.close();
			fShaderFile.close();
			//преобразуем потоки в массивы GLchar
			vertexCode = vShaderStream.str();
			fragmentCode = fShaderStream.str();
		}
		catch (ifstream::failure e)//отлавливаем исключение и выводим сообщение об ощибке
		{
			cout << " file_not_succesfully_read" << endl;
		}
		const GLchar* vShaderCode = vertexCode.c_str();//храним код вершинного шейдера в виде строки
		const GLchar* fShaderCode = fragmentCode.c_str();//храним код фрагментного шейдера в виде строки
		GLuint vertex, fragment;//идентификаторы объектов шейдеров
		GLint success;//переменная, которая определяет успешность сборки
		GLchar infoLog[512];//массив для хранения ошибок
		vertex = glCreateShader(GL_VERTEX_SHADER);//создаём объект вершинного шейдера
		glShaderSource(vertex, 1, &vShaderCode, NULL); //привязываем исходный код вершинного шейдера к объекту вершинного шейдера
		glCompileShader(vertex);//компилируем вершинный шейдер
		glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);//проверяем успешность компиляции
		if (!success)//если компиляция не удалась
		{
			glGetShaderInfoLog(vertex, 512, NULL, infoLog);//получаем ошибку
			cout << " Vertex shader compilation_failed: " << endl << infoLog << endl;//выводим сообщение об ошибке
		}
		fragment = glCreateShader(GL_FRAGMENT_SHADER);//создаём объект фрагментного шейдера
		glShaderSource(fragment, 1, &fShaderCode, NULL);//привязываем исходный код фрагментного шейдера к объёкту фрагментного шейдера
		glCompileShader(fragment);//компилируем фрагментный шейдер
		glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);//проверяем успешность компиляции
		if (!success)//если компиляция не удалась
		{
			glGetShaderInfoLog(fragment, 512, NULL, infoLog);//получаем ошибку
			cout << " Fragment shader compilation_failed: " << endl << infoLog << endl;//выводим сообщение об ошибке
		}
		//шейдерная программа
		this->Program = glCreateProgram();//создаём шейдерную программу
		//присоединяем шейдеры к программе
		glAttachShader(this->Program, vertex);
		glAttachShader(this->Program, fragment);
		glLinkProgram(this->Program);//связываем шейдеры в программе
		glGetProgramiv(this->Program, GL_LINK_STATUS, &success);//проверяем успешность линковки
		if (!success)//если линковка не удалась
		{
			glGetProgramInfoLog(this->Program, 512, NULL, infoLog);
			cout << " Program linking_failed: " << endl << infoLog << endl;//выводим сообщение об ошибке
		}
		//удаляем шейдеры
		glDeleteShader(vertex);
		glDeleteShader(fragment);
	}
	void use()//использование программы
	{
		glUseProgram(this->Program);
	}
};

#endif