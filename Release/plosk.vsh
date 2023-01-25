//это вершинный шейдер модели плоского освещения
#version 330 core
layout (location = 0) in vec3 position;//позиция фрагмента
layout (location = 1) in vec3 normal;//нормаль
out vec3 Normal;//нормаль
out vec3 frag_pos; //позиция фрагмента в мировых координатах
//матрицы полученные из игрового цикла
uniform mat4 model;//матрица модели
uniform mat4 view;//матрица вида
uniform mat4 projection;//матрица проекции
void main()
{
	gl_Position = projection*view*model*vec4(position, 1.0f);
	frag_pos=vec3(model*vec4(position, 1.0f));//находим позицию фрагмента в мировых координатах
	Normal = mat3(transpose(inverse(model)))*normal;
}