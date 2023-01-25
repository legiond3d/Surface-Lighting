#define GLEW_STATIC//используем статическую версию библиотеки GLEW
#include<GL/glew.h>//подключаем GLEW
#include<GLFW/glfw3.h>//подключаем GLFW
#include<SOIL.h>//подключаем SOIL для текстур
//подключаем GLM для работы с векторами и матрицами
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
//стандартные библиотеки С++
#include<iostream>
#include<cmath>
#include<Windows.h>
#include<clocale>
//подключаем свои заголовочные файлы (с классами для подключения шейдера и камеры)
#include"shader.h"
#include"camera.h"

//определяем разрешение окна
const GLuint WIDTH = 1280, HEIGHT = 720;
//камера
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

camera cam(glm::vec3(0.0f, 0.0f, 3.0f));//создаём объект класса камера
GLfloat lastX = WIDTH / 2, lastY = HEIGHT / 2;//задаём центр окна
bool firstMouse = true;//переменная для определения первое ли это считывание положения курсора после открытия окна
bool keys[1024];//хранит состояние клавиш

//атрибуты источника света
glm::vec3 lightPos(1.2f,1.0f,2.0f);//позиция лампы

//положение второго объекта, на котором будем демонстрировать плоское освещение
glm::vec3 ploskPos(-1.5f, 0.0f, 0.0f);

//положение третьего объекта, на котором будет демонстрироваться модель затенения Гуро
glm::vec3 guroPos(1.5f, 0.0f, 0.0f);

//delta-time:
GLfloat deltaTime = 0.0f;//интервал времени между последним и текущим кадром
GLfloat lastFrame = 0.0f;//время вывода последнего кадра

void intro();//прототип функции для вывода приветствия в консоль и описания возможных команд

//прототипы функций обратных вызовов
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);//прототип функции обратного вызова
void mouse_callback(GLFWwindow* window, double xpos, double ypos);//прототип функции обратного вызова для управления камерой при помощи мыши
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);//прототип функции обратного вызова для изменения масштаба изображения (фактически меняется угол обзора камеры)
void do_movement();//прототип функции для проверки, в каком направлении будет двигаться камера
void dinamic_lamp();//прототип функции для движения лампы

int main()
{
	intro();
	glfwInit();//инициализация GLFW
	//задаём минимальную требуемую версию OpenGL (OpenGL 3.3, т.к. последние версии OpenGL работают не на всех видеокартах)
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//установка профайла, для которого создаётся контекст
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);//отключаем возможность изменять размер окна

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "lab_9_program", nullptr, nullptr);//создание окна
	if (window == nullptr)//если не удалось создать окно
	{
		std::cout << " Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glewExperimental = GL_TRUE;//новейшии техники GLEW для управления OpenGL 
	glewInit();
	if (glewInit() != GLEW_OK)//если инициализация GLEW прошла неудачно 
	{
		std::cout << " Failed to initialize GLEW" << std::endl;
		return -1;
	}
	int width, height;//переменные для ширины и высоты окна
	glfwGetFramebufferSize(window, &width, &height);//получаем значение ширины и высоты окна
	glViewport(0, 0, width, height);//определяем размер отрисовываемого окна (определяется как будут отображаться данные и координаты относительно размеров окна)

	//устнавливаем необходимые функции обратного вызова
	glfwSetKeyCallback(window, key_callback);//передаём функцию обратного вызова в GLFW
	glfwSetCursorPosCallback(window, mouse_callback);//передаём функцию, считывающую положение курсора в GLFW
	glfwSetScrollCallback(window, scroll_callback);//передаём функцию для масштабирования в GLFW
	
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);//опции для GLFW, чтобы курсор не выходил за рамки окна

	glEnable(GL_DEPTH_TEST);//включаем буффер глубины
	
	//модель Фонга:
	Shader phong("phong.vsh", "phong.fsh");//создаём объект нашего класса шейдер (фактически считываем с файлов вершинный и фрагментный шейдер, создаём из них шейдерную программу)
	//модель плоского освещения:
	Shader plosk("plosk.vsh","plosk.fsh");//аналогично создаём объект класса шейдер, но для модели плоского освещения
	//модель затенения Гуро:
	Shader guro("guro.vsh", "guro.fsh");//создаём объект класса шейдер для модели затенения Гуро
	//шейдеры для лампы:
	Shader lampShader("lamp.vsh","lamp.fsh");//создаём объект класса шейдер для рисования лампы
	GLfloat tmp = sqrt(2) / 2;//значение для нахождения вектора нормали под грани пирамиды

	//устанавливаем координаты вершин для пирамиды (слева координаты вершин, а справа вектора нормали к поверхностям (всего 5 поверхностей))
	GLfloat vertices1[] = {
		//низ пирамиды (квадрат из 2 треугольников)
		-0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,
		0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,
		0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f,
		0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f,
		-0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f,
		-0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,
		//задний треугольник
		-0.5f, -0.5f, -0.5f, 0.0f, tmp, -tmp,
		0.5f, -0.5f, -0.5f, 0.0f, tmp, -tmp,
		0.0f, 0.5f, 0.0f, 0.0f, tmp, -tmp,
		//правый треугольник
		0.0f, 0.5f, 0.0f, tmp, tmp, 0.0f,
		0.5f, -0.5f, -0.5f, tmp, tmp, 0.0f,
		0.5f, -0.5f, 0.5f, tmp, tmp, 0.0f,
		//передний треугольник
		0.5f, -0.5f, 0.5f, 0.0f, tmp, tmp,
		0.0f, 0.5f, 0.0f, 0.0f, tmp, tmp,
		-0.5f, -0.5f, 0.5f, 0.0f, tmp, tmp,
		//левый треугольник
		-0.5f, -0.5f, 0.5f, -tmp, tmp, 0.0f,
		0.0f, 0.5f, 0.0f, -tmp, tmp, 0.0f,
		-0.5f, -0.5f, -0.5f, -tmp, tmp, 0.0f
	};

	//устанавливаем координаты вершин для куба (3 столбца слева - это координаты вершин, а 3 стобца справа - это есть вектор нормали)
	GLfloat vertices2[] = {
		-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
		0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
		0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
		0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
		-0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
		-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,

		-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
		0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
		0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
		0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
		-0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
		-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,

		-0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f,

		0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f,
		0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
		0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
		0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f,
		0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f,

		-0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,
		0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,
		0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f,
		0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f,
		-0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f,
		-0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,

		-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
		-0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
		-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f
	};
	GLuint VBO, VAO;
	glGenVertexArrays(1, &VAO);//создаём объект вершинного массива
	glGenBuffers(1, &VBO);//создаём объект вершинного буфера

	glBindBuffer(GL_ARRAY_BUFFER, VBO);//привязываем тип ARRAY_BUFFER к нашему буферу
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), vertices2, GL_STATIC_DRAW);//копируем вершинные данные в буфер

	//атрибуты позиции вершин
	glBindVertexArray(VAO);//привзяываем объёкт вершинного массива
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);//сообщаем OpenGL как интерпретироваться вершинные данные
	glEnableVertexAttribArray(0);//включаем атрибут
	
	//атрибуты нормалей вершин
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GL_FLOAT), (GLvoid*)(3 * sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);//отвязываем VAO

	GLuint lightVBO;//объект вершинного буфера для лампы
	glGenBuffers(1, &lightVBO);
	glBindBuffer(GL_ARRAY_BUFFER, lightVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), vertices2, GL_STATIC_DRAW);

	//объект вершинного массива для лампы
	GLuint lightVAO;

	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);

	glBindBuffer(GL_ARRAY_BUFFER, lightVBO);//связываем с VBO куба лампы

	//устанавливаем атрибуты лампы (указываем координаты вершин)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);//пропускаем все нормали
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);

	while (!glfwWindowShouldClose(window))//игровой цикл, пока не получили инструкцию к закрытию
	{
		GLfloat currentFrame = glfwGetTime();//замеряем время начала текущего кадра
		deltaTime = currentFrame - lastFrame;//узнаём интервал времени между текущим и предыдущим кадрами
		lastFrame = currentFrame;//в значение предыдущего кадра устанавливаем значением нынешнего

		glfwPollEvents();//проверяются не были ли вызваны какие-либо события
		do_movement();//вызываем функцию проверки клавиш движения камеры
		dinamic_lamp();//вызываем функцию для перемещения лампы

		glClearColor(0.05f, 0.05f, 0.05f, 1.0f);//устанавливаем состояние окна после очистки экрана
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);//используем состояние, полученное из предыдущей функции и чистим экран

		phong.use();//используем шейдерную программу для прорисовки объекта с использованием освещения по Фонгу

		//установка uniform переменных для шейдерной программы
		GLint objectColorLoc = glGetUniformLocation(phong.Program, "object_color");//цвет самого объекта
		GLint lightColorLoc = glGetUniformLocation(phong.Program, "light_color");//цвет источника света
		GLint lightPosLoc = glGetUniformLocation(phong.Program, "light_pos");//позиция источника освещения
		GLint viewPosLoc = glGetUniformLocation(phong.Program, "view_pos");//позиция наблюдателя (камеры)
		//glUniform3f(objectColorLoc, 0.352941f, 0.0f, 0.784313f);//задаём цвет объекта
		glUniform3f(objectColorLoc, 0.0f, 1.0f, 1.0f);//задаём цвет объекта
		glUniform3f(lightColorLoc, 1.0f, 1.0f, 1.0f);//задаём цвет источника света
		glUniform3f(lightPosLoc, lightPos.x, lightPos.y, lightPos.z);//задаём позицию источника
		glUniform3f(viewPosLoc, cam.Position.x, cam.Position.y, cam.Position.z);//задаём позицию наблюдателя (камеры)

		//создаём трансформации камеры
		glm::mat4 view;
		glm::mat4 projection;
		view = cam.get_view_matrix();//получаем матрицу вида (наблюдателя)
		projection = glm::perspective(cam.Zoom, (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);//получаем значение перспективы объектов сцены
		//устанавливаем матрицы
		GLint modelLoc = glGetUniformLocation(phong.Program, "model");
		GLint viewLoc = glGetUniformLocation(phong.Program, "view");
		GLint projLoc = glGetUniformLocation(phong.Program, "projection");

		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
		
		glBindVertexArray(VAO);
		glm::mat4 model;
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glDrawArrays(GL_TRIANGLES, 0, 36);//отрисовка объекта
		glBindVertexArray(0);

		//рисование объекта для плоского освещения:
		plosk.use();//используем шейдерную программу для прорисовки объекта с плоским освещением
		//установка uniform переменных для шейдерной программы
		GLint object_color_loc = glGetUniformLocation(plosk.Program, "object_color");//цвет самого объекта
		GLint light_color_loc = glGetUniformLocation(plosk.Program, "light_color");//цвет источника света
		GLint light_pos_loc = glGetUniformLocation(plosk.Program, "light_pos");//позиция источника освещения
		GLint view_pos_loc = glGetUniformLocation(plosk.Program, "view_pos");//позиция наблюдателя (камеры)
		//glUniform3f(object_color_loc, 0.803921f, 0.0f, 0.352941f);//задаём цвет объекта
		glUniform3f(object_color_loc, 1.0f, 0.0f, 0.0f);//задаём цвет объекта
		glUniform3f(light_color_loc, 1.0f, 1.0f, 1.0f);//задаём цвет источника света
		glUniform3f(light_pos_loc, lightPos.x, lightPos.y, lightPos.z);//задаём позицию источника
		glUniform3f(view_pos_loc, cam.Position.x, cam.Position.y, cam.Position.z);//задаём позицию наблюдателя (камеры)

		//устанавливаем матрицы
		modelLoc = glGetUniformLocation(plosk.Program, "model");
		viewLoc = glGetUniformLocation(plosk.Program, "view");
		projLoc = glGetUniformLocation(plosk.Program, "projection");

		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

		model = glm::mat4();
		model = glm::translate(model, ploskPos);

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);//отрисовка куба для плоского освещения
		glBindVertexArray(0);

		//рисование объекта для модели затенения Гуро:
		guro.use();//используем шейдерную программу для прорисовки объекта с моделью освещения по Гуро

		//установка uniform переменных для шейдерной программы
		GLint object_color_gur = glGetUniformLocation(guro.Program, "object_color");//цвет самого объекта
		GLint light_color_gur = glGetUniformLocation(guro.Program, "light_color");//цвет источника света
		GLint light_pos_gur = glGetUniformLocation(guro.Program, "light_pos");//позиция источника освещения
		GLint view_pos_gur = glGetUniformLocation(guro.Program, "view_pos");//позиция наблюдателя (камеры)
		//glUniform3f(object_color_loc, 0.803921f, 0.0f, 0.352941f);//задаём цвет объекта
		glUniform3f(object_color_gur, 1.0f, 0.0f, 1.0f);//задаём цвет объекта
		glUniform3f(light_color_gur, 1.0f, 1.0f, 1.0f);//задаём цвет источника света
		glUniform3f(light_pos_gur, lightPos.x, lightPos.y, lightPos.z);//задаём позицию источника
		glUniform3f(view_pos_gur, cam.Position.x, cam.Position.y, cam.Position.z);//задаём позицию наблюдателя (камеры)

		//создаём трансформации камеры
		view = glm::mat4();
		projection = glm::mat4();
		view = cam.get_view_matrix();//получаем матрицу вида (наблюдателя)
		projection = glm::perspective(cam.Zoom, (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);//получаем значение перспективы объектов сцены

		//устанавливаем матрицы
		modelLoc = glGetUniformLocation(guro.Program, "model");
		viewLoc = glGetUniformLocation(guro.Program, "view");
		projLoc = glGetUniformLocation(guro.Program, "projection");

		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

		model = glm::mat4();
		model = glm::translate(model, guroPos);

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);//отрисовка куба для модели затенения Гуро
		glBindVertexArray(0);

		//рисование объекта лампы
		lampShader.use();//вызываем шейдерную программу для прорисовки лампы (источника света)

		//устанавливаем матрицы
		modelLoc = glGetUniformLocation(lampShader.Program, "model");
		viewLoc = glGetUniformLocation(lampShader.Program, "view");
		projLoc = glGetUniformLocation(lampShader.Program, "projection");

		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

		model = glm::mat4();
		model = glm::translate(model, lightPos);
		model = glm::scale(model, glm::vec3(0.2f));//делаем куб лампы в 5 раз меньше куба объекта

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

		glBindVertexArray(lightVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);//отрисовка куба лампы
		glBindVertexArray(0);

		glfwSwapBuffers(window);//переключает буфер экрана
	}
	//освобождаем ресурсы
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &lightVBO);
	glDeleteVertexArrays(1, &lightVAO);
	glfwTerminate();//очищаем выделенные ресурсы (завершаем GLFW)
	return 0;
}
//реализация функции key_callback
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE&&action == GLFW_PRESS)//если пользователь нажимает escape, приложение закрывается
		glfwSetWindowShouldClose(window, GL_TRUE);
	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			keys[key] = true;
		else if (action == GLFW_RELEASE)
			keys[key] = false;
	}
}
void do_movement()//реализация функции, отвечающей за перемещение камеры в зависимости от нажатой клавиши
{
	GLfloat cameraSpeed = 5.0f*deltaTime;//задаём скорость полёта камеры
	if (keys[GLFW_KEY_W])
		cam.process_keyboard(FORWARD, deltaTime);
	if (keys[GLFW_KEY_S])
		cam.process_keyboard(BACKWARD, deltaTime);
	if (keys[GLFW_KEY_A])
		cam.process_keyboard(LEFT, deltaTime);
	if (keys[GLFW_KEY_D])
		cam.process_keyboard(RIGHT, deltaTime);
	if (keys[GLFW_KEY_LEFT_SHIFT])
		cam.process_keyboard(UP, deltaTime);
	if (keys[GLFW_KEY_LEFT_CONTROL])
		cam.process_keyboard(DOWN, deltaTime);
	if (keys[GLFW_KEY_E])
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);//отрисовываем без закрашивания, только границы
	if (keys[GLFW_KEY_Q])
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);//отрисовываем вместе с закрашиванием
}
void mouse_callback(GLFWwindow* window, double xpos, double ypos)//реализация функции для вращения камеры мышкой
{
	if (firstMouse)//если первое считывание положения курсора относительно окна
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}
	//смещенение мыши, произошедшее между последним и текущим кадром
	GLfloat xoffset = xpos - lastX;
	GLfloat yoffset = lastY - ypos;//оконные координаты y возрастают вниз 

	lastX = xpos;
	lastY = ypos;

	cam.process_mouse_movement(xoffset, yoffset);//вызываем соответствующий метод из класса камеры
}
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)//функция для изменения угла обзора колёсиком мышки
{
	cam.process_mouse_scroll(yoffset);//вызываем соответствующий метод из класса камеры
}
void dinamic_lamp()//реализация функции для перемещения лампы по осям координат
{
	GLfloat speed = 1.0f;
	if (keys[GLFW_KEY_UP])
		lightPos.z -= deltaTime*speed;
	if (keys[GLFW_KEY_DOWN])
		lightPos.z += deltaTime*speed;
	if (keys[GLFW_KEY_RIGHT])
		lightPos.x += deltaTime*speed;
	if (keys[GLFW_KEY_LEFT])
		lightPos.x -= deltaTime*speed;
	if (keys[GLFW_KEY_END])
		lightPos.y -= deltaTime*speed;
	if (keys[GLFW_KEY_HOME])
		lightPos.y += deltaTime*speed;
}
void intro()//функция, выводящая приветствие в консоль и инструкции по использованию
{
	setlocale(LC_ALL, "Rus");
	cout << " Данная программа демонстрирует различные модели освещения: " << endl;
	cout << " Модель Фонга и плоского освещения" << endl;
	cout << endl << " В рабочем окне доступны следующие команды:" << endl;
	cout << " Для закрытия окна - нажмите ESC" << endl;
	cout << " При помощи мыши можно осматривать сцену" << endl;
	cout << " Колёсико мышки позволяет уменьшить угол обзора (приблизить объект)" << endl;
	cout << " Клавиши W, A, S, D служат для перемещения камеры по сцене" << endl;
	cout << " Клавиша Left Shift позволяет камере подниматься вертикально" << endl;
	cout << " Клавиша Left Ctrl позволяет камере опускаться вертикально" << endl;
	cout << " При помощи стрелочек можно перемещать лампу по оси Z(вперёд/назад), по оси X (влево/вправо)" << endl;
	cout << " При помощи HOME можно поднимать лампу по оси Y, с помощью END опускать лампу по оси Y" << endl;
}