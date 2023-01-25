//вершинный шейдер модели Гуро (отличие Гуро от Фонга в том, что у Гуро все манипуляции происходят именно в вершинном шейдере, а у Фонга в фрагментном)
#version 330 core
layout (location = 0) in vec3 position;//получаем положение вершины
layout (location = 1) in vec3 normal;//получаем вектор нормали вершины
out vec3 color;//возвращаем в качестве результата вычисленное значение освещения вершины
//вектора из игрового цикла программы:
uniform vec3 light_pos;//положение источника света
uniform vec3 view_pos;//положение наблюдателя
uniform vec3 light_color;//значение цвета источника света
//матрицы из игрового цикла программы:
uniform mat4 model;//матрицы модели
uniform mat4 view;//матрица вида
uniform mat4 projection;//матрица проекции
void main()//тело шейдера
{
	gl_Position=projection*view*model*vec4(position, 1.0);
	vec3 Position = vec3(model*vec4(position, 1.0));//находим положение вершины в мировых координатах
	vec3 Normal = mat3(transpose(inverse(model)))*normal;//переводим вектор нормали в мировые координаты, домножая его на транспонированную матрицу модели
	//фоновое освещение:
	float ambient_strength = 0.1;
	vec3 ambient = ambient_strength * light_color;//вычисляем фоновую компоненту освещения
	//диффузное освещение:
	vec3 norm = normalize(Normal);
	vec3 light_dir = normalize(light_pos - Position);//определяем нормализированный вектор между вершиной и источником света
	float diff = max(dot(norm, light_dir), 0.0);//скалярное произведение найденного вектора направления и вектора нормали (за счёт max отбрасываем отрицательные значения)
	vec3 diffuse = diff * light_color;//вычисляем диффузную компоненту освещения
	//бликовое освещение:
	float spec_strength = 0.5;
	vec3 view_dir = normalize(view_pos - Position);//определяем нормализованный вектор между позицией вершины и позицией наблюдателя (камеры)
	vec3 reflect_dir = reflect(-light_dir, norm);//определяем вектор отражения
	int shine = 32;//сила блеска
	float spec = pow(max(dot(view_dir, reflect_dir), 0.0), shine);//скалярное произведение векторов от наблюдателя к вершине и вектора отражения. В скалярном произведении отбрасываем отрицательные значения, благодаря max, далее возводим всё в степень силы блеска.
	vec3 specular = spec_strength * spec * light_color;//вычисляем бликовую компоненту освещения
	//находим результирующее освещение, как сумму компонентов:
	color = ambient + diffuse + specular; 
}