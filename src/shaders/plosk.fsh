//??? ??????????? ?????? ?????? ???????? ?????????
#version 330 core
//??????? ?????????? ?? ?????????? ???????
in vec3 frag_pos;//??????? ????????? ? ??????? ???????????
in vec3 Normal;//??????? ????? ?????????
out vec4 color;//?????? ?? ?????, ???? ?????????
//???????, ??????? ???????? ?? ???????? ????? ?????????
uniform vec3 object_color;
uniform vec3 light_color;
uniform vec3 light_pos;
uniform vec3 view_pos;
void main()//?????????? ? ??????????? ??????? ? ?????????? ??? ?????????????? ??? ?????????? ?????? ???????? ?????????
{
	//??????? ?????????? ?????????
	float ambient_strength=0.1f;
	vec3 ambient = ambient_strength * light_color;//??????? ??????????
	//????????? ?????????? ?????????
	vec3 norm = normalize(Normal);
	vec3 light_dir = normalize(light_pos-frag_pos);//?????? ????? ?????????? ????? ? ??????????
	float diff = max(dot(norm,light_dir),0.0);//????????? ???????????? ??????????? ??????? ? ??????? ??????? (??????? max ?? 0.0 ? ?????????? ????????????, ????? ????????? ????????????? ???????? ?????)
	vec3 diffuse = diff*light_color;//????????? ??????????

	vec3 result = (ambient + diffuse) * object_color; //???????? ???????? ?????????, ??? ????? ???? ??????????? ?????????
	color = vec4(result, 1.0f);//?????????? ???? 
}