#ifndef SHADER_H//����� ������������ ��������� ������ 1 ���
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
	GLuint Program;//������������� ���������
	Shader(const GLchar* vertexPath, const GLchar* fragmentPath)//����������� ��� ���������� � ������ � ������ ��������
	{
		string vertexCode;//������ ��� �������� ���� ���������� �������
		string fragmentCode;//������ ��� �������� ���� ������������ �������
		ifstream vShaderFile;//����� ��� ���������� �� ����� ���������� �������
		ifstream fShaderFile;//����� ��� ���������� �� ����� ������������ �������
		//��������� ����� �� ������ �������� ����������
		vShaderFile.exceptions(ifstream::badbit);
		fShaderFile.exceptions(ifstream::badbit);
		try//����, � ������� �������� ������������� ����������
		{
			vShaderFile.open(vertexPath);//��������� ���� � ��������� ��������
			fShaderFile.open(fragmentPath);//��������� ���� � ����������� ��������
			stringstream vShaderStream, fShaderStream;//������ ���������� �����
			//��������� ������ � ������
			vShaderStream << vShaderFile.rdbuf();
			fShaderStream << fShaderFile.rdbuf();
			//��������� ����� � ���������
			vShaderFile.close();
			fShaderFile.close();
			//����������� ������ � ������� GLchar
			vertexCode = vShaderStream.str();
			fragmentCode = fShaderStream.str();
		}
		catch (ifstream::failure e)//����������� ���������� � ������� ��������� �� ������
		{
			cout << " file_not_succesfully_read" << endl;
		}
		const GLchar* vShaderCode = vertexCode.c_str();//������ ��� ���������� ������� � ���� ������
		const GLchar* fShaderCode = fragmentCode.c_str();//������ ��� ������������ ������� � ���� ������
		GLuint vertex, fragment;//�������������� �������� ��������
		GLint success;//����������, ������� ���������� ���������� ������
		GLchar infoLog[512];//������ ��� �������� ������
		vertex = glCreateShader(GL_VERTEX_SHADER);//������ ������ ���������� �������
		glShaderSource(vertex, 1, &vShaderCode, NULL); //����������� �������� ��� ���������� ������� � ������� ���������� �������
		glCompileShader(vertex);//����������� ��������� ������
		glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);//��������� ���������� ����������
		if (!success)//���� ���������� �� �������
		{
			glGetShaderInfoLog(vertex, 512, NULL, infoLog);//�������� ������
			cout << " Vertex shader compilation_failed: " << endl << infoLog << endl;//������� ��������� �� ������
		}
		fragment = glCreateShader(GL_FRAGMENT_SHADER);//������ ������ ������������ �������
		glShaderSource(fragment, 1, &fShaderCode, NULL);//����������� �������� ��� ������������ ������� � ������� ������������ �������
		glCompileShader(fragment);//����������� ����������� ������
		glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);//��������� ���������� ����������
		if (!success)//���� ���������� �� �������
		{
			glGetShaderInfoLog(fragment, 512, NULL, infoLog);//�������� ������
			cout << " Fragment shader compilation_failed: " << endl << infoLog << endl;//������� ��������� �� ������
		}
		//��������� ���������
		this->Program = glCreateProgram();//������ ��������� ���������
		//������������ ������� � ���������
		glAttachShader(this->Program, vertex);
		glAttachShader(this->Program, fragment);
		glLinkProgram(this->Program);//��������� ������� � ���������
		glGetProgramiv(this->Program, GL_LINK_STATUS, &success);//��������� ���������� ��������
		if (!success)//���� �������� �� �������
		{
			glGetProgramInfoLog(this->Program, 512, NULL, infoLog);
			cout << " Program linking_failed: " << endl << infoLog << endl;//������� ��������� �� ������
		}
		//������� �������
		glDeleteShader(vertex);
		glDeleteShader(fragment);
	}
	void use()//������������� ���������
	{
		glUseProgram(this->Program);
	}
};

#endif