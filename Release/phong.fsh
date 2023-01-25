//это фрагментный шейдер модели ‘онга
#version 330 core
//входные переменные из вершинного шейдера
in vec3 frag_pos;//позици€ фрагмента в мировых координатах
in vec3 Normal;//нормаль этого фрагмента
out vec4 color;//вектор на выход, цвет фрагмента
//вектора, которые получаем из игрового цикла программы
uniform vec3 object_color;
uniform vec3 light_color;
uniform vec3 light_pos;
uniform vec3 view_pos;
void main()//фактически в фрагментном шейдере и происход€т все преобразовани€ дл€ построени€ модели освещени€ по ‘онгу
{
	//фонова€ компонента освещени€
	float ambient_strength=0.1f;
	vec3 ambient = ambient_strength * light_color;//фонова€ компонента
	//диффузна€ компонента освещени€
	vec3 norm = normalize(Normal);
	vec3 light_dir = normalize(light_pos-frag_pos);//вектор между источником света и фрагментом
	float diff = max(dot(norm,light_dir),0.0);//скал€рное произведение полученного вектора и вектора нормали (берЄтс€ max от 0.0 и скал€рного произведени€, чтобы отбросить отрицательные значени€ света)
	vec3 diffuse = diff*light_color;//диффузна€ компонента
	//бликова€ компонента освещени€
	float specular_strength = 0.5f;
	vec3 view_dir=normalize(view_pos-frag_pos);//вектор от наблюдател€ к фрагменту
	vec3 reflect_dir=reflect(-light_dir, norm);//вектор отражени€ 
	int shine = 32;//сила блеска
	float spec = pow(max(dot(view_dir,reflect_dir),0.0),shine);//скал€рное произведение векторов: от наблюдател€ и отражЄнного, в котором отбрасываютс€ отрицательные значени€, далее всЄ возводим в степень силы блеска
	vec3 specular = specular_strength*spec*light_color;//бликова€ составл€юща€

	vec3 result = (ambient + diffuse + specular) * object_color; //получаем значение освещени€, как сумму всех компонентов освещени€
	color = vec4(result, 1.0f);//возвращаем цвет 
}