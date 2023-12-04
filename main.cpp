#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <vector>
#include <unordered_map>
#include <cmath>
#include "shader.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <filesystem>
#include <chrono>
#include <thread>
#include "solve.h"
#include "random.h"
#include <iostream>


using namespace std;

//de este punto van a salir las coordenadas de los puntos x y z que le pasaremos al shader
//tambien va a tener los colores de cad cara con c1 c2 y c3
struct Punto{
	float x, y, z, c1, c2, c3;
	Punto(float _x, float _y, float _z, glm::vec3 color){
		x = _x;
		y = _y;
		z = _z;
		c1 = color.x;
		c2 = color.y;
		c3 = color.z;
	}
};


class Cubo{
	public:
		vector<Punto> vertices_centros; //almacenamos los vertices que van a salir de este centro
		float dimension; //que tan grande vana  ser los cubos
		unsigned int VAO, VBO;
		glm::vec3 centro; //para dibujar
		int flag=1;
		
		Cubo(float _dimension, glm::vec3 ubicacion,vector<glm::vec3> color){
			dimension = _dimension;
			centro = ubicacion;
			color[0].x=1.0f;
			color[0].y=0.0f;
			color[0].z=0.0f;
			
			//Caras
			//--------------------------------------------------------------------------------
			//vertices de en frente
			vertices_centros.push_back(Punto(-dimension, -dimension, dimension, color[0]));
			vertices_centros.push_back(Punto(dimension, -dimension, dimension,color[0]));
			vertices_centros.push_back(Punto(dimension, dimension, dimension,color[0]));
			vertices_centros.push_back(Punto(dimension, dimension, dimension,color[0]));
			vertices_centros.push_back(Punto(-dimension, dimension, dimension,color[0]));
			vertices_centros.push_back(Punto(-dimension, -dimension, dimension,color[0]));
			
			//--------------------------------------------------------------------------------
			//vertices de arriba
			vertices_centros.push_back(Punto(-dimension, dimension, -dimension,color[1]));
			vertices_centros.push_back(Punto(dimension, dimension, -dimension,color[1]));
			vertices_centros.push_back(Punto(dimension, dimension, dimension,color[1]));
			vertices_centros.push_back(Punto(dimension, dimension, dimension,color[1]));
			vertices_centros.push_back(Punto(-dimension, dimension, dimension,color[1]));
			vertices_centros.push_back(Punto(-dimension, dimension, -dimension,color[1]));
			
			//---------------------------------------------------------------------------------
			//vertices de la izquierda
			vertices_centros.push_back(Punto(-dimension, dimension, dimension,color[2]));
			vertices_centros.push_back(Punto(-dimension, dimension, -dimension,color[2]));
			vertices_centros.push_back(Punto(-dimension, -dimension, -dimension,color[2]));
			vertices_centros.push_back(Punto(-dimension, -dimension, -dimension,color[2]));
			vertices_centros.push_back(Punto(-dimension, -dimension, dimension,color[2]));
			vertices_centros.push_back(Punto(-dimension, dimension, dimension,color[2]));
			
			//---------------------------------------------------------------------------------
			//vertices de abajo
			vertices_centros.push_back(Punto(-dimension, -dimension, -dimension,color[3]));
			vertices_centros.push_back(Punto(dimension, -dimension, -dimension,color[3]));
			vertices_centros.push_back(Punto(dimension, -dimension, dimension,color[3]));
			vertices_centros.push_back(Punto(dimension, -dimension, dimension,color[3]));
			vertices_centros.push_back(Punto(-dimension, -dimension, dimension,color[3]));
			vertices_centros.push_back(Punto(-dimension, -dimension, -dimension,color[3]));
			
			//---------------------------------------------------------------------------------
			//vertices de la derecha
			vertices_centros.push_back(Punto(dimension, dimension, dimension,color[4]));
			vertices_centros.push_back(Punto(dimension, dimension, -dimension,color[4]));
			vertices_centros.push_back(Punto(dimension, -dimension, -dimension,color[4]));
			vertices_centros.push_back(Punto(dimension, -dimension, -dimension,color[4]));
			vertices_centros.push_back(Punto(dimension, -dimension, dimension,color[4]));
			vertices_centros.push_back(Punto(dimension, dimension, dimension,color[4]));
			
			//--------------------------------------------------------------------------------
			//vertices traseros
			vertices_centros.push_back(Punto(-dimension, -dimension, -dimension,color[5]));
			vertices_centros.push_back(Punto(dimension, -dimension, -dimension,color[5]));
			vertices_centros.push_back(Punto(dimension, dimension, -dimension,color[5]));
			vertices_centros.push_back(Punto(dimension, dimension, -dimension,color[5]));
			vertices_centros.push_back(Punto(-dimension, dimension, -dimension,color[5]));
			vertices_centros.push_back(Punto(-dimension, -dimension, -dimension,color[5]));
		
		}			
		
		//Funciones para manejar los buffers
		void generarBuffers() {
			glGenVertexArrays(1, &VAO);
			glGenBuffers(1, &VBO);

			glBindVertexArray(VAO);

			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, vertices_centros.size() * 6 * sizeof(float), &vertices_centros[0], GL_STATIC_DRAW);
			//vertex position
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(0);
			// color attribute
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
			glEnableVertexAttribArray(1);
		
		}
		
		
		
		void updateBuffers() { 
			glBindVertexArray(VAO);
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, vertices_centros.size() * 6 * sizeof(float), &vertices_centros[0], GL_STATIC_DRAW);
		}
		
		
		void deleteBuffers() {
			glDeleteVertexArrays(1, &VAO);
			glDeleteBuffers(1, &VBO);

		}

		void print()
		{
			if(flag==1)
			{
				for(int x=0;x<36;x++)
				{
					cout<<"-----------------------------"<<endl;
					cout<<"x: "<<vertices_centros[x].x<<endl;
					cout<<"y: "<<vertices_centros[x].y<<endl;
					cout<<"z: "<<vertices_centros[x].z<<endl;
				}
				flag=0;
			}
		}
		
		
		void dibujar_cubito(Shader* ourShader) { 

			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, centro);
			ourShader->setMat4("model", model);

			//print();
			
			glBindVertexArray(VAO);
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glDrawArrays(GL_TRIANGLES, 0, 36);

		}
		
		//Mover los cubos
		
		void move(glm::vec3 pivot, Shader* ourShader, float degrees) {

			glm::mat4 model = glm::mat4(1.0f);

			//aplicamos la matriz de modelo a nuestro shader para que todos los centros se muevan
			glm::mat4 rot = glm::mat4(1.0f);
			rot = glm::rotate(rot, glm::radians(degrees), pivot);
			glm::vec4 r = rot * glm::vec4(centro, 1.0f);
			centro = r;

			model = glm::translate(model, centro);
			
			//actualizamos tambien os vertices de cada cubito en particular para tener control sobre ellos
			glm::vec4 nuevos_vertices;
			glm::mat4 rotMatrix = glm::mat4(1.0f);
			for (int i = 0; i < vertices_centros.size(); i++)
			{
				nuevos_vertices = glm::vec4(vertices_centros[i].x, vertices_centros[i].y, vertices_centros[i].z, 1.0f);

				nuevos_vertices = glm::rotate(rotMatrix, glm::radians(degrees), pivot) * nuevos_vertices;

				vertices_centros[i].x = nuevos_vertices.x;
				vertices_centros[i].y = nuevos_vertices.y;
				vertices_centros[i].z = nuevos_vertices.z;
			}
			updateBuffers();

			
			ourShader->setMat4("model", model);


			glDrawArrays(GL_TRIANGLES, 0, 36);

		}
		
		
		
};


//-------------------------------------
class CuboRubik{
	private:
		vector<Cubo> cubitos;
		GLFWwindow* window;
		Shader* shader;
		
	public:
	//mapa para relacionar cada camada R L, etc con los colores que va a tener
		unordered_map<char, vector<int>>  camadas;

		vector<string> cambios;
		vector<string> solution;
		float degree = -1.0f;
		CuboRubik(GLFWwindow* _window,Shader* _shader,float dimension)
		{
			
			//parte derecha del cubito (positivo)
			float dimension_derecha = dimension * 2+(dimension/6);
			//parte izquierda del cubito (negativo)
			float dimension_izquierda = -1 * (dimension_derecha);
			window = _window;
			shader = _shader;
			
			
			
			//definimos las 26 posiciones de los 26 centros de los cubos que vamos a tener
			
			glm::vec3 cubePositions[] = {
				glm::vec3(dimension_izquierda,  dimension_derecha, dimension_derecha),					//1
				glm::vec3(0.0f, dimension_derecha, dimension_derecha),									//2
				glm::vec3(dimension_derecha,  dimension_derecha, dimension_derecha),					//3
				glm::vec3(dimension_izquierda,  0.0f,dimension_derecha),								//4
				glm::vec3(0.0f,  0.0f, dimension_derecha),												//5
				glm::vec3(dimension_derecha,  0.0f,dimension_derecha),									//6
				glm::vec3(dimension_izquierda,  dimension_izquierda, dimension_derecha),				//7
				glm::vec3(0.0f, dimension_izquierda, dimension_derecha),								//8
				glm::vec3(dimension_derecha,  dimension_izquierda, dimension_derecha),					//9

				glm::vec3(dimension_izquierda,  dimension_derecha, 0.0f),								//10
				glm::vec3(0.0f, dimension_derecha, 0.0f),												//11
				glm::vec3(dimension_derecha,  dimension_derecha, 0.0f),									//12
				glm::vec3(dimension_izquierda,  0.0f, 0.0f),											//13
				glm::vec3(dimension_derecha,  0.0f, 0.0f),												//14
				glm::vec3(dimension_izquierda,  dimension_izquierda, 0.0f),								//15
				glm::vec3(0.0f, dimension_izquierda, 0.0f),												//16
				glm::vec3(dimension_derecha,  dimension_izquierda, 0.0f),								//17


				glm::vec3(dimension_izquierda,  dimension_derecha, dimension_izquierda),				//18
				glm::vec3(0.0f, dimension_derecha, dimension_izquierda),								//19
				glm::vec3(dimension_derecha,  dimension_derecha, dimension_izquierda),					//20
				glm::vec3(dimension_izquierda,  0.0f,dimension_izquierda),								//21
				glm::vec3(0.0f, 0.0f,dimension_izquierda),												//22
				glm::vec3(dimension_derecha,  0.0f,dimension_izquierda),								//23
				glm::vec3(dimension_izquierda,  dimension_izquierda, dimension_izquierda),				//24
				glm::vec3(0.0f, dimension_izquierda, dimension_izquierda),								//25
				glm::vec3(dimension_derecha,  dimension_izquierda, dimension_izquierda)					//26
			};
			//definimos las camadas y los centros que va a tener cada una
			vector<int> F{0,1,2,3,4,5,6,7,8};
			vector<int> B{19,18,17,22,21,20,25,24,23};
			vector<int> L{17,9,0,20,12,3,23,14,6};
			vector<int> R{2,11,19,5,13,22,8,16,25};
			vector<int> U{17,18,19,9,10,11,0,1,2};
			vector<int> D{6,7,8,14,15,16,23,24,25};
			
			//definimos los colores que usaremos
			glm::vec3 colors[] = {
				glm::vec3(0.0f,0.0f,0.0f),			//Negro
				glm::vec3(1.0f,1.0f,1.0f),			//Blanco
				glm::vec3(1.0f,0.5f,0.0f),			//Naranja  
				glm::vec3(0.8f,0.15f,1.0f),			//Morado  
				glm::vec3(1.0f,0.025f,0.25f),		//Rojo
				glm::vec3(0.224f,1.0f,0.078f),		//Verde 
				glm::vec3(0.15f,0.35f,1.0f)			//Azul
			};
			
			//definimos que colores va a tener cada cara segun su ubicacion
			int color_correspondiente[][6] = {
				{4,1,5,0,0,0},	
				{4,1,0,0,0,0},
				{4,1,0,0,6,0},
				{4,0,5,0,0,0},
				{4,0,0,0,0,0},
				{4,0,0,0,6,0},
				{4,0,5,3,0,0},
				{4,0,0,3,0,0},
				{4,0,0,3,6,0},	
				{0,1,5,0,0,0},	
				{0,1,0,0,0,0},
				{0,1,0,0,6,0},
				{0,0,5,0,0,0},
				{0,0,0,0,6,0},
				{0,0,5,3,0,0},
				{0,0,0,3,0,0},
				{0,0,0,3,6,0},
				{0,1,5,0,0,2}, 
				{0,1,0,0,0,2},
				{0,1,0,0,6,2},
				{0,0,5,0,0,2},
				{0,0,0,0,0,2},
				{0,0,0,0,6,2},
				{0,0,5,3,0,2},
				{0,0,0,3,0,2},
				{0,0,0,3,6,2}	
			};
			
			
			
			camadas.insert({'F', F });
			camadas.insert({'B', B});
			camadas.insert({'L', L});
			camadas.insert({'R', R});
			camadas.insert({'U', U});
			camadas.insert({'D', D});
			
			vector<glm::vec3> color_cubito;
			
			for(int i=0;i<26;i++)
			{
				//rellenamos el color de cada cara de un cubitp
				for (int j = 0; j < 6; j++) 
				{
					color_cubito.push_back(colors[color_correspondiente[i][j]]);
				}	
				//lo almacenamos en un vector para luego cuando se dibuje tener guardado la posicion y el color
				cubitos.push_back(Cubo(dimension, cubePositions[i],color_cubito));
				color_cubito.clear();
			}

			generarBuffers();
		}
		
	
	//Para manejar los buffers
	void generarBuffers() {
		for (int i = 0; i < cubitos.size(); i++){
			cubitos[i].generarBuffers();
		}

	}
	
	
	void deleteBuffers() {
		for (int i = 0; i < cubitos.size(); i++)
		{
			cubitos[i].deleteBuffers();
		}

	}
	//----------------
	void dibujar_rubik() {			
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		for (int i = 0; i < cubitos.size(); i++)
		{
			cubitos[i].dibujar_cubito(shader);
		}
		glfwSwapBuffers(window);
		glfwPollEvents();
		return;
	}
	
	//Saber los movimientos
	void guardarMovimiento(char sideMove) {
		if (degree == -1) {
			cambios.push_back(string(1, sideMove));
			cout << sideMove << " ";
		}
		else {
			string s(1, sideMove);
			s.push_back('\'');
			cambios.push_back(s);
			cout << s << " ";
		}
		
	}
	
	
	
	//Funcion mover la camada
	void move(char sideMove) {
		
		vector<int>* pv = &(camadas.find(sideMove)->second); //puntero al mapa camadas para relacionar el movimiento que se ha hecho con sus 
															  //posiciones de centro (->second)
		vector<bool> moviendo(26, false);
		
		for (int k = 0; k < 90; k++) 
		{
			//movemos cada cubito de esa camada
			for (int j = 0; j < pv->size(); j++) 
			{
				//aplicamos el movimiento
				cubitos[(*pv)[j]].move(cubitos[(*pv)[4]].centro, shader, degree);  
				moviendo[(*pv)[j]] = true;
			}
			//dibujamos el mini movimiento de la camada y dibujamos
			dibujar_rubik();
			//cout<<"dibujar rubik\n";
			//esperamos un momento para que al momento de girar, se va de forma mas clara
			for(int x=0;x<10;x++)
			{
				this_thread::sleep_for(chrono::nanoseconds(900000));
			}
			
		}
		
		return;
	}
	
	//Funciones para actualizar las camadas----------------------
	
	
	void actualizar_camadas(char CamadaEscogida) 
	{  
		vector<int>* pMoved = &(camadas.find(CamadaEscogida)->second);         
		vector<char> camada_para_actualizar;   
		vector<vector<int>> indices_para_actualizar; 
		switch (CamadaEscogida)
		{
		case 'F':
			camada_para_actualizar = vector<char>{ 'U','R','D','L' };
			indices_para_actualizar = vector<vector<int>>{ {6,7,8}, {0,3,6}, {0,1,2}, {2,5,8} };
			break;
		case 'B':
			camada_para_actualizar = vector<char>{ 'U','L','D','R' };       //Actualizar desde B
			indices_para_actualizar = vector<vector<int>>{ {2,1,0}, {0,3,6}, {8,7,6}, {2,5,8} };
			break;
		case 'L':
			camada_para_actualizar = vector<char>{ 'U','F','D','B' };
			indices_para_actualizar = vector<vector<int>>{ {0,3,6}, {0,3,6}, {6,3,0}, {2,5,8} };
			break;
		case 'R':
			camada_para_actualizar = vector<char>{ 'U','B','D','F' };
			indices_para_actualizar = vector<vector<int>>{ {8,5,2}, {0,3,6}, {2,5,8}, {2,5,8} };
			break;
		case 'D':
			camada_para_actualizar = vector<char>{ 'F','R','B','L' };
			indices_para_actualizar = vector<vector<int>>{ {6,7,8}, {6,7,8}, {8,7,6}, {8,7,6} };
			break;
		case 'U':
			camada_para_actualizar = vector<char>{ 'B','R','F','L' };
			indices_para_actualizar = vector<vector<int>>{ {2,1,0}, {2,1,0}, {0,1,2}, {0,1,2} };
			break;
		default:
			break;
		}
		
		
		
		vector<int>* pv = &(camadas.find(CamadaEscogida)->second);
		vector<int> temp = (*pv);

		//actualizamos las posiciones de la camada escogida
		for (int i = 0; i < 3; i++) {
			(*pv)[i] = temp[6 - (i * 3)];                  
			(*pv)[i + 3] = temp[10 - ((i + 1) * 3)];     
			(*pv)[i + 6] = temp[i + 12 - ((i + 1) * 4)];   
		}
		
		
		vector<vector<int>> sortedIndex = { {0,1,2},{2,5,8},{6,7,8},{0,3,6} }; 
		vector<int>* side_updating;   
		
		//acualizamos los indices de las camadas para que se refieran a los nuevos centros
		for (int i = 0; i < 4; i++) 
		{
			side_updating = &(camadas.find(camada_para_actualizar[i])->second);
			for (int j = 0; j < 3; j++) 
			{
				(*side_updating)[indices_para_actualizar[i][j]] = (*pMoved)[sortedIndex[i][j]];
			}
		}

	}


		//----------------------------SOLVER--------------------------------------
	
	void solve(vector<string> sol) {
		solution = sol;
		char sideMoved;
		degree = -1.0f;
		for (int i = 0; i < solution.size(); i++) {
			//imprime la solucion
			if (i > 1) {
				std::cout << "\nSOLUTION: - "<<i<<": ";
				for (int j = i; j < solution.size(); j++) std::cout << solution[j] << " ";
			}
			//rotacion normal
			if (solution[i].size() == 1) {
				sideMoved = solution[i].c_str()[0];
				move(sideMoved);
				actualizar_camadas(sideMoved);
			}
			//sentido contrario (L+')
			else if (solution[i].size() == 2) {
				if (solution[i][1] == '\'') {
					degree = 1.0f;
					sideMoved = solution[i].c_str()[0];
					move(sideMoved);
					actualizar_camadas(sideMoved);
					actualizar_camadas(sideMoved);
					actualizar_camadas(sideMoved);
					degree = -1.0f;
				}
				else {
					sideMoved = solution[i].c_str()[0];
					move(sideMoved);
					actualizar_camadas(sideMoved);
					move(sideMoved);
					actualizar_camadas(sideMoved);
				}
			}
		}
		std::cout << "\n\nMovimientos: ";

		return;
	}

	void armarCubo() {
		//estado inicial del cubo
		vector<string> move = get_solution(to_cube_not(cambios));
		cout << "\nSolucion: ";
		for (int i = 0; i < move.size(); i++) cout << move[i] << " ";
		solve(move);
		cambios.clear();
		//"\nSHUFFLE: ";

		return;
	}
	//-------------------SOLVER-FIN-------------------------
	
	
	
	
};
		
		
//----------------------------------------------------------------------------------------------------------		
		
			
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);


// camera
glm::vec3 cameraPos   = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f, 0.0f);

bool firstMouse = true;
float yaw   = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float pitch =  0.0f;
float lastX =  800.0f / 2.0;
float lastY =  600.0 / 2.0;
float fov   =  45.0f;

float angle = 0.0f;
float angle1 = 0.0f;
float angle2 = 0.0f;
float angle3 = 0.0f;

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

//Movimientos
char moverCamada;
bool enMovimiento= false;
bool cambiarDireccion = false;
bool armar = false;

//---------------------------------------------------------------------------------------------------------------------------------------

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	
    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGL(glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

   //EILEEN
	Shader ourShader("D:/LabCompiler2023_II_CG/glfw-master/OwnProjects/Project_2Parte_2/camera.vs", "D:/LabCompiler2023_II_CG/glfw-master/OwnProjects/Project_2Parte_2/camera.fs");

    //LAB_Eileen
	//Shader ourShader("E:/LabCompiler2023_II_CG/glfw-master/OwnProjects/Project_2Parte_2/camera.vs", "E:/LabCompiler2023_II_CG/glfw-master/OwnProjects/Project_2Parte_2/camera.fs");
    //GRABRIEL
    //Shader ourShader("H:/LabCompiler2023_II_CG/glfw-master/OwnProjects/Cubo_Rot/camera.vs", "H:/LabCompiler2023_II_CG/glfw-master/OwnProjects/Cubo_Rot/camera.fs");

    // tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
    // -------------------------------------------------------------------------------------------
    ourShader.use();

	CuboRubik MyCube(window,&ourShader,0.2f);
	glm::mat4 projection = glm::perspective(glm::radians(fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    ourShader.setMat4("projection", projection);
		
    while (!glfwWindowShouldClose(window))
    {
		float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
		//cout << "frame\n";

        processInput(window);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
		//cout <<"color\n";
        // activate shader
        ourShader.use();
		//cout<<"shader\n";

        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        ourShader.setMat4("view", view);
		//cout<<"lookAt\n";

		MyCube.dibujar_rubik();
		//cout<<"dibujar\n";
		
		
		if(enMovimiento == true){

			MyCube.guardarMovimiento(moverCamada);
			//cout<<"guardar movimiento\n";
			MyCube.move(moverCamada);
			//cout<<"mover camada\n";
			enMovimiento = false;
			
			for (int i = 0; MyCube.degree == 1.0f && i < 2; i++) 
			{
				MyCube.actualizar_camadas(moverCamada);
			}
			MyCube.actualizar_camadas(moverCamada);
			//cout<<"Actualizar camadas\n";
			
		}
		
		 else {
			if(enMovimiento== false && cambiarDireccion == true) {
                cambiarDireccion = false;
                MyCube.degree *= -1.0f;
            }
			
			else{
				if (armar == true) {
					MyCube.armarCubo();
					armar = false;
				}
			}
		 }
		// cout <<"enMovimiento: "<<enMovimiento<<endl;

	}
	// optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    
    MyCube.deleteBuffers();

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly


void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

	float cameraSpeed = 0.005;

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){
		cameraPos += cameraSpeed * cameraFront;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){
		cameraPos -= cameraSpeed * cameraFront;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	}

	if(enMovimiento == false){
		if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS){
			enMovimiento = true;
			moverCamada = 'U';
		}
		if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		{
			enMovimiento = true;
			moverCamada = 'D';
		}
		if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
		{
			enMovimiento = true;
			moverCamada = 'L';
		}
		if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
		{
			enMovimiento = true;
			moverCamada = 'R';
		}
		if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
		{
			enMovimiento = true;
			moverCamada = 'F';
		}
		if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS)
		{
			enMovimiento = true;
			moverCamada = 'B';
		}

		if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
			cambiarDireccion = true;
        }
		if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) {
            armar = true;
        }
	}
    
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}
