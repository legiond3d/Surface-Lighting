//��������� ������ ������ ���� (������� ���� �� ����� � ���, ��� � ���� ��� ����������� ���������� ������ � ��������� �������, � � ����� � �����������)
#version 330 core
layout (location = 0) in vec3 position;//�������� ��������� �������
layout (location = 1) in vec3 normal;//�������� ������ ������� �������
out vec3 color;//���������� � �������� ���������� ����������� �������� ��������� �������
//������� �� �������� ����� ���������:
uniform vec3 light_pos;//��������� ��������� �����
uniform vec3 view_pos;//��������� �����������
uniform vec3 light_color;//�������� ����� ��������� �����
//������� �� �������� ����� ���������:
uniform mat4 model;//������� ������
uniform mat4 view;//������� ����
uniform mat4 projection;//������� ��������
void main()//���� �������
{
	gl_Position=projection*view*model*vec4(position, 1.0);
	vec3 Position = vec3(model*vec4(position, 1.0));//������� ��������� ������� � ������� �����������
	vec3 Normal = mat3(transpose(inverse(model)))*normal;//��������� ������ ������� � ������� ����������, �������� ��� �� ����������������� ������� ������
	//������� ���������:
	float ambient_strength = 0.1;
	vec3 ambient = ambient_strength * light_color;//��������� ������� ���������� ���������
	//��������� ���������:
	vec3 norm = normalize(Normal);
	vec3 light_dir = normalize(light_pos - Position);//���������� ����������������� ������ ����� �������� � ���������� �����
	float diff = max(dot(norm, light_dir), 0.0);//��������� ������������ ���������� ������� ����������� � ������� ������� (�� ���� max ����������� ������������� ��������)
	vec3 diffuse = diff * light_color;//��������� ��������� ���������� ���������
	//�������� ���������:
	float spec_strength = 0.5;
	vec3 view_dir = normalize(view_pos - Position);//���������� ��������������� ������ ����� �������� ������� � �������� ����������� (������)
	vec3 reflect_dir = reflect(-light_dir, norm);//���������� ������ ���������
	int shine = 32;//���� ������
	float spec = pow(max(dot(view_dir, reflect_dir), 0.0), shine);//��������� ������������ �������� �� ����������� � ������� � ������� ���������. � ��������� ������������ ����������� ������������� ��������, ��������� max, ����� �������� �� � ������� ���� ������.
	vec3 specular = spec_strength * spec * light_color;//��������� �������� ���������� ���������
	//������� �������������� ���������, ��� ����� �����������:
	color = ambient + diffuse + specular; 
}