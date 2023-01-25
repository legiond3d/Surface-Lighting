//фрагментный шейдер модели затенения Гуро 
#version 330 core
out vec4 frag_color;//возвращаемое значени - цвет фрагмента
in vec3 color;//входное значение - цвет источника света
uniform vec3 object_color;//вектор из игрового цикла - цвет объекта
void main()//тело шейдера
{
	frag_color = vec4(color * object_color, 1.0);//всё уже было расчитано в вершинном шейдере, поэтому остаётся только вернуть значение цвета фрагмента
}
