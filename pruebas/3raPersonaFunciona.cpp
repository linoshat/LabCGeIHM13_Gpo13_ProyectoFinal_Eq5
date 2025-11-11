/* 
Animación por keyframes
La textura del skybox fue conseguida desde la página https ://opengameart.org/content/elyvisions-skyboxes?page=1
y edité en Gimp rotando 90 grados en sentido antihorario la imagen  sp2_up.png para poder ver continuidad.
Fuentes :
	https ://www.khronos.org/opengl/wiki/Keyframe_Animation
	http ://what-when-how.com/wp-content/uploads/2012/07/tmpcd0074_thumb.png
	*/
//para cargar imagen
#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>
#include <math.h>

#include<fstream>

#include <glew.h>
#include <glfw3.h>

#include <glm.hpp>
#include <gtc\matrix_transform.hpp>
#include <gtc\type_ptr.hpp>
//para probar el importer
//#include<assimp/Importer.hpp>

#include "Window.h"
#include "Mesh.h"
#include "Shader_light.h"
#include "Camera.h"
#include "Texture.h"
#include "Sphere.h"
#include"Model.h"
#include "Skybox.h"

//para iluminación
#include "CommonValues.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Material.h"
const float toRadians = 3.14159265f / 180.0f;
#define NUM_BASE_KEYFRAMES 100

//variables para animación
float movCoche;
float movOffset;
float rotllanta;
float rotllantaOffset;
bool avanza;
float angulovaria = 0.0f;

//variables para keyframes
float reproduciranimacion, habilitaranimacion, guardoFrame, reinicioFrame, ciclo, ciclo2, contador = 0;
float banderaX = 0.0f, banderaX_ = 0.0f, banderaY = 0.0f, banderaY_ = 0.0f, banderaY2 = 0.0f, banderaY2_ = 0.0f;
float banderaGiro = 0.0f, banderaGiro_ = 0.0f;
bool orientacion = true;

Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

Camera camera;


Texture plainTexture;
Texture pisoTexture;
Texture AgaveTexture;
Texture FlechaTexture;
Texture balonTexture;	



Model Kitt_M;
Model Llanta_M;
Model Blackhawk_M;

Model tubo;
Model puerta;
Model banca;
Model ring;
Model lampara;
Model lampara1;
Model casa2;
Model edificios;
Model balon;
Model casas;
Model Carro;

//modelos para avatar mike
Model cuerpoM;
Model ArmR;
Model ArmL;
Model LegR; //pierna derecha superior
Model LegL; //pierna izquierda superior



Skybox skybox;

//materiales
Material Material_brillante;
Material Material_opaco;


//Sphere cabeza = Sphere(0.5, 20, 20);
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;

// luz direccional
DirectionalLight mainLight;
//para declarar varias luces de tipo pointlight
PointLight pointLights[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];

// Vertex Shader
static const char* vShader = "shaders/shader_light.vert";

// Fragment Shader
static const char* fShader = "shaders/shader_light.frag";

//función para teclado de keyframes 
void inputKeyframes(bool* keys);

//cálculo del promedio de las normales para sombreado de Phong
void calcAverageNormals(unsigned int* indices, unsigned int indiceCount, GLfloat* vertices, unsigned int verticeCount,
	unsigned int vLength, unsigned int normalOffset)
{
	for (size_t i = 0; i < indiceCount; i += 3)
	{
		unsigned int in0 = indices[i] * vLength;
		unsigned int in1 = indices[i + 1] * vLength;
		unsigned int in2 = indices[i + 2] * vLength;
		glm::vec3 v1(vertices[in1] - vertices[in0], vertices[in1 + 1] - vertices[in0 + 1], vertices[in1 + 2] - vertices[in0 + 2]);
		glm::vec3 v2(vertices[in2] - vertices[in0], vertices[in2 + 1] - vertices[in0 + 1], vertices[in2 + 2] - vertices[in0 + 2]);
		glm::vec3 normal = glm::cross(v1, v2);
		normal = glm::normalize(normal);

		in0 += normalOffset; in1 += normalOffset; in2 += normalOffset;
		vertices[in0] += normal.x; vertices[in0 + 1] += normal.y; vertices[in0 + 2] += normal.z;
		vertices[in1] += normal.x; vertices[in1 + 1] += normal.y; vertices[in1 + 2] += normal.z;
		vertices[in2] += normal.x; vertices[in2 + 1] += normal.y; vertices[in2 + 2] += normal.z;
	}

	for (size_t i = 0; i < verticeCount / vLength; i++)
	{
		unsigned int nOffset = i * vLength + normalOffset;
		glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
		vec = glm::normalize(vec);
		vertices[nOffset] = vec.x; vertices[nOffset + 1] = vec.y; vertices[nOffset + 2] = vec.z;
	}
}


void CreateObjects()
{
	unsigned int indices[] = {
		0, 3, 1,
		1, 3, 2,
		2, 3, 0,
		0, 1, 2
	};

	GLfloat vertices[] = {
		//	x      y      z			u	  v			nx	  ny    nz
			-1.0f, -1.0f, -0.6f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, -1.0f, 1.0f,		0.5f, 0.0f,		0.0f, 0.0f, 0.0f,
			1.0f, -1.0f, -0.6f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,		0.5f, 1.0f,		0.0f, 0.0f, 0.0f
	};

	unsigned int floorIndices[] = {
		0, 2, 1,
		1, 2, 3
	};

	GLfloat floorVertices[] = {
		-10.0f, 0.0f, -10.0f,	0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, -10.0f,	10.0f, 0.0f,	0.0f, -1.0f, 0.0f,
		-10.0f, 0.0f, 10.0f,	0.0f, 10.0f,	0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, 10.0f,		10.0f, 10.0f,	0.0f, -1.0f, 0.0f
	};
	unsigned int vegetacionIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	   4,5,6,
	   4,6,7
	};

	GLfloat vegetacionVertices[] = {
		-0.5f, -0.5f, 0.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.0f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, 0.5f, 0.0f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, 0.0f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,

		0.0f, -0.5f, -0.5f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.5f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.5f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, -0.5f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,


	};
	

	unsigned int flechaIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	};

	GLfloat flechaVertices[] = {
		-0.5f, 0.0f, 0.5f,		0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, 0.5f,		1.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, -0.5f,		1.0f, 1.0f,		0.0f, -1.0f, 0.0f,
		-0.5f, 0.0f, -0.5f,		0.0f, 1.0f,		0.0f, -1.0f, 0.0f,

	};

	unsigned int scoreIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	};

	GLfloat scoreVertices[] = {
		-0.5f, 0.0f, 0.5f,		0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, 0.5f,		1.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, -0.5f,		1.0f, 1.0f,		0.0f, -1.0f, 0.0f,
		-0.5f, 0.0f, -0.5f,		0.0f, 1.0f,		0.0f, -1.0f, 0.0f,

	};

	unsigned int numeroIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	};

	GLfloat numeroVertices[] = {
		-0.5f, 0.0f, 0.5f,		0.0f, 0.67f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, 0.5f,		0.25f, 0.67f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, -0.5f,		0.25f, 1.0f,		0.0f, -1.0f, 0.0f,
		-0.5f, 0.0f, -0.5f,		0.0f, 1.0f,		0.0f, -1.0f, 0.0f,

	};

	Mesh *obj1 = new Mesh();
	obj1->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj1);

	Mesh *obj2 = new Mesh();
	obj2->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj2);

	Mesh *obj3 = new Mesh();
	obj3->CreateMesh(floorVertices, floorIndices, 32, 6);
	meshList.push_back(obj3);


	Mesh* obj4 = new Mesh();
	obj4->CreateMesh(vegetacionVertices, vegetacionIndices, 64, 12);
	meshList.push_back(obj4);

	Mesh* obj5 = new Mesh();
	obj5->CreateMesh(flechaVertices, flechaIndices, 32, 6);
	meshList.push_back(obj5);

	Mesh* obj6 = new Mesh();
	obj6->CreateMesh(scoreVertices, scoreIndices, 32, 6);
	meshList.push_back(obj6);

	Mesh* obj7 = new Mesh();
	obj7->CreateMesh(numeroVertices, numeroIndices, 32, 6);
	meshList.push_back(obj7);

}


void CreateShaders()
{
	Shader *shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}


///////////////////////////////KEYFRAMES/////////////////////


bool animacion = false;



//NEW// Keyframes
float posXavion = 2.0, posYavion = 5.0, posZavion = -3.0;
float movAvion_x = 0.0f, movAvion_y = 0.0f;
float giroAvion = 0;
float giroX = 0.0f;


float posMike_X = 0.0f;
float posMike_Z = 0.0f;
float rotMike_Y = 0.0f; // Rotación sobre el eje Y (giro horizontal)

float anguloCaminata = 0.0f; // angulo que oscila entre -MAX y +MAX
float velocidadCaminata = 8.0f; // ajustar para naturalidad)
const float MAX_SWING_ANGLE = 30.0f;


// Constantes de velocidad
const float velocidadMovimiento = 0.5f; // Unidades por segundo
const float velocidadGiro = 5.0f; // Grados por segundo


// tercera persona
bool camaraTerceraPersona = true;   // activar/desactivar v
float distanciaCamara = 8.0f;     
float alturaCamara = 5.0f;   
float desfaseGiroCamara = 0.0f;    
float suavizadoCamara = 10.0f;    
bool teclaC_bloqueada = false;
bool permitirMovimientoAvatar = true;


#define MAX_FRAMES 100 //Número de cuadros máximos -> depende de la animacion
int i_max_steps = 1; //Número de pasos entre cuadros para interpolación, a mayor número , más lento será el movimiento -> minimo para el ojo 30
int i_curr_steps = 0;
typedef struct _frame
{
	//Variables para GUARDAR Key Frames
	float movAvion_x;		//Variable para PosicionX
	float movAvion_y;		//Variable para PosicionY
	float movAvion_xInc;		//Variable para IncrementoX
	float movAvion_yInc;		//Variable para IncrementoY
	float giroAvion;		//Variable para GiroAvion
	float giroAvionInc;		//Variable para IncrementoGiroAvion
	float giroX;
	float giroXInc;
}FRAME;

FRAME KeyFrame[MAX_FRAMES];
int FrameIndex = 100;			//El número de cuadros guardados actualmente desde 0 para no sobreescribir
bool play = false;
int playIndex = 0;

void saveFrame(void) //tecla L
{

	printf("frameindex %d\n", FrameIndex);


	KeyFrame[FrameIndex].movAvion_x = movAvion_x;
	KeyFrame[FrameIndex].movAvion_y = movAvion_y;
	KeyFrame[FrameIndex].giroAvion = giroAvion;
	KeyFrame[FrameIndex].giroX = giroX;
	//Se agregan nuevas líneas para guardar más variables si es necesario


	std::ofstream archivoKeyframes;
	// Abre el archivo "keyframes.txt" en modo append (agregar al final)
	archivoKeyframes.open("archivoKeyframes.txt", std::ios::app);

	if (archivoKeyframes.is_open())
	{
		//  formato[indice] [posX] [posY] [giro]
		archivoKeyframes << FrameIndex << " "
			<< KeyFrame[FrameIndex].movAvion_x << " "
			<< KeyFrame[FrameIndex].movAvion_y << " "
			<< KeyFrame[FrameIndex].giroAvion << " "
			<< KeyFrame[FrameIndex].giroX
			<< std::endl; // Salto de lnea para el siguiente frame

		archivoKeyframes.close(); //  cerrar 
	}
	else
	{
		printf("Error al abrir el archivo keyframes.txt\n");
	}


	//no volatil,se requiere agregar una forma de escribir a un archivo para guardar los frames
	FrameIndex++;
}


void saveBaseKeyframes(int numFrames)
{

	std::ofstream archivoKeyframes("archivoKeyframes.txt", std::ios::out);

	if (archivoKeyframes.is_open())
	{
		for (int i = 0; i < numFrames; i++)
		{
			// Formato: [ndice] [posX] [posY] [giro]
			archivoKeyframes << i << " "
				<< KeyFrame[i].movAvion_x << " "
				<< KeyFrame[i].movAvion_y << " "
				<< KeyFrame[i].giroAvion << " "
				<< KeyFrame[i].giroX
				<< std::endl;
		}
		archivoKeyframes.close();
		printf("Keyframes base guardados -> indices 0 a %d\n", numFrames - 1);
	}
	else
	{
		printf("Error al abrir archiv\n");
	}
}

void loadKeyframes()
{
	std::ifstream archivoKeyframes("archivoKeyframes.txt");
	int frameLeido = 0;

	if (archivoKeyframes.is_open())
	{
	
		while (archivoKeyframes >> frameLeido) 
		{
			if (frameLeido < MAX_FRAMES)
			{
				
				archivoKeyframes >> KeyFrame[frameLeido].movAvion_x
					>> KeyFrame[frameLeido].movAvion_y
					>> KeyFrame[frameLeido].giroAvion
					>> KeyFrame[frameLeido].giroX;

			}
		}
		archivoKeyframes.close();

		//ultimo frame cargado + 1
		FrameIndex = frameLeido + 1;

		if (FrameIndex <= NUM_BASE_KEYFRAMES)
		{
			// está vacío.
			FrameIndex = NUM_BASE_KEYFRAMES;
		}

		printf("Keyframes cargados\n");
	}
	
	else
	{
		saveBaseKeyframes(NUM_BASE_KEYFRAMES);
		loadKeyframes();
		printf("Archivo keyframes.txt no encontrado.\n");
	}
}

void resetElements(void) //Tecla 0
{

	movAvion_x = KeyFrame[0].movAvion_x;
	movAvion_y = KeyFrame[0].movAvion_y;
	giroAvion = KeyFrame[0].giroAvion;
	giroX = KeyFrame[0].giroX;
}

void interpolation(void)
{
	KeyFrame[playIndex].movAvion_xInc = (KeyFrame[playIndex + 1].movAvion_x - KeyFrame[playIndex].movAvion_x) / i_max_steps;
	KeyFrame[playIndex].movAvion_yInc = (KeyFrame[playIndex + 1].movAvion_y - KeyFrame[playIndex].movAvion_y) / i_max_steps;
	KeyFrame[playIndex].giroAvionInc = (KeyFrame[playIndex + 1].giroAvion - KeyFrame[playIndex].giroAvion) / i_max_steps;
	KeyFrame[playIndex].giroXInc = (KeyFrame[playIndex + 1].giroX - KeyFrame[playIndex].giroX) / i_max_steps;


}

void animate(void)
{
	//Movimiento del objeto con barra espaciadora
	if (play)
	{
		if (i_curr_steps >= i_max_steps) //fin de animación entre frames?
		{
			playIndex++;

			printf("playindex : %d\n", playIndex);

			if (playIndex > FrameIndex - 2)	//Fin de toda la animación con último frame?
			{
				
					printf("Frame index= %d\n", FrameIndex);
					printf("termino la animacion\n");

					playIndex = 0;
					play = false;

			}


			else //Interpolación del próximo cuadro
			{
				
				i_curr_steps = 0; //Resetea contador
				interpolation();
			}
		}
		else
		{
			//Dibujar Animación
			movAvion_x += KeyFrame[playIndex].movAvion_xInc ;
			movAvion_y += KeyFrame[playIndex].movAvion_yInc ;
			giroAvion += KeyFrame[playIndex].giroAvionInc;
			giroX += KeyFrame[playIndex].giroXInc;
			i_curr_steps++;
		}

	}
}

///////////////* FIN KEYFRAMES*////////////////////////////



int main()
{
	mainWindow = Window(1366, 768); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();
	
	CreateObjects();
	CreateShaders();

	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.5f, 0.5f);

	plainTexture = Texture("Textures/plain.png");
	plainTexture.LoadTextureA();
	pisoTexture = Texture("Textures/calle.jpg");
	pisoTexture.LoadTextureA();

	balonTexture = Texture("Textures/10536_soccerball_V1_diffuse.jpg");
	balonTexture.LoadTextureA();
	//bancaTexture = Texture("Textures/MaderaTablonesBlanca.jpg");
	//bancaTexture.LoadTextureA();



	Kitt_M = Model();
	Kitt_M.LoadModel("Models/kitt_optimizado.obj");
	Llanta_M = Model();
	Llanta_M.LoadModel("Models/llanta_optimizada.obj");
	Blackhawk_M = Model();
	Blackhawk_M.LoadModel("Models/uh60.obj");


	tubo = Model();
	tubo.LoadModel("Models/tubo.obj");
	
	banca = Model();
	banca.LoadModel("Models/Banca.fbx");
	ring = Model();	
	ring.LoadModel("Models/ringT.obj");
	lampara1 = Model();
	lampara1.LoadModel("Models/Lamp_Text.obj");
	lampara = Model();
	lampara.LoadModel("Models/lampara.fbx");
	puerta = Model();
	puerta.LoadModel("Models/puerta.obj");
	casa2 = Model();
	casa2.LoadModel("Models/casa2.fbx");
	edificios = Model();
	edificios.LoadModel("Models/edificiosEjemplo.fbx");
	balon = Model();
	balon.LoadModel("Models/10536_soccerball_V1_iterations-2.obj");

	cuerpoM = Model();
	cuerpoM.LoadModel("Models/mikeCuerpo.obj");
	ArmR = Model();
	ArmR.LoadModel("Models/mikeArmR.obj");
	ArmL = Model();
	ArmL.LoadModel("Models/mikeArmL.obj");
	LegR = Model();
	LegR.LoadModel("Models/mikeLegR.obj");
	LegL = Model();
	LegL.LoadModel("Models/mikeLegL.obj");



	Carro = Model();
	Carro.LoadModel("Models/CarroceriaCangre.obj");




	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("Textures/Skybox/sp2_rt.png");
	skyboxFaces.push_back("Textures/Skybox/sp2_lf.png");
	skyboxFaces.push_back("Textures/Skybox/sp2_dn.png");
	skyboxFaces.push_back("Textures/Skybox/sp2_up.png");
	skyboxFaces.push_back("Textures/Skybox/sp2_bk.png");
	skyboxFaces.push_back("Textures/Skybox/sp2_ft.png");

	skybox = Skybox(skyboxFaces);

	Material_brillante = Material(4.0f, 256);
	Material_opaco = Material(0.3f, 4);


	//luz direccional, sólo 1 y siempre debe de existir
	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,
		0.3f, 0.3f,
		0.0f, 0.0f, -1.0f);
	//contador de luces puntuales
	unsigned int pointLightCount = 0;
	//Declaración de primer luz puntual
	pointLights[0] = PointLight(1.0f, 0.0f, 0.0f,
		0.0f, 1.0f,
		0.0f, 2.5f, 1.5f,
		0.3f, 0.2f, 0.1f);
	pointLightCount++;

	unsigned int spotLightCount = 0;
	//linterna
	spotLights[0] = SpotLight(1.0f, 1.0f, 1.0f,
		0.0f, 2.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		5.0f);
	spotLightCount++;

	//luz fija
	spotLights[1] = SpotLight(0.0f, 0.0f, 1.0f,
		1.0f, 2.0f,
		5.0f, 10.0f, 0.0f,
		0.0f, -5.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		15.0f);
	spotLightCount++;


	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0, uniformTextureOffset=0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);
	
	movCoche = 0.0f;
	movOffset = 0.01f;
	rotllanta = 0.0f;
	rotllantaOffset = 10.0f;
	glm::vec3 posblackhawk = glm::vec3(2.0f, 0.0f, 0.0f);
	
	//---------PARA TENER KEYFRAMES GUARDADOS NO VOLATILES QUE SIEMPRE SE UTILIZARAN SE DECLARAN AQUÍ
	//deberian de estar almacenados en un archivo externo para no ser volatiles
	/*
	KeyFrame[0].movAvion_x = 0.0f;
	KeyFrame[0].movAvion_y = 0.0f;
	KeyFrame[0].giroAvion = 0;


	KeyFrame[1].movAvion_x = -2.0f;
	KeyFrame[1].movAvion_y = 4.0f;
	KeyFrame[1].giroAvion = 0;


	KeyFrame[2].movAvion_x = -4.0f;
	KeyFrame[2].movAvion_y = 0.0f;
	KeyFrame[2].giroAvion = 0;


	KeyFrame[3].movAvion_x = -6.0f;
	KeyFrame[3].movAvion_y = -4.0f;
	KeyFrame[3].giroAvion = 0;


	KeyFrame[4].movAvion_x = -8.0f;
	KeyFrame[4].movAvion_y = 0.0f;
	KeyFrame[4].giroAvion = 0.0f;

	KeyFrame[5].movAvion_x = -10.0f;
	KeyFrame[5].movAvion_y = 4.0f;
	KeyFrame[5].giroAvion = 0.0f;
	
	//Se agregan nuevos frames  //el ultimo cuadro y el primero siempre deben de ser iguales para que la animación sea cíclica
	//agregar cuadros necesarios para regresar en la direccion contraria -> generar la animacion por input de teclado NO pegar los frames
	//agregar tecla para mover en X y Y y giro 
	KeyFrame[6].movAvion_x = 0.0f;
	KeyFrame[6].movAvion_y = 0.0f;
	KeyFrame[6].giroAvion = 0.0f;//por teclado modificar estos valores
	*/
	//saveBaseKeyframes(NUM_BASE_KEYFRAMES);

	loadKeyframes();

		printf("\nTeclas para uso de Keyframes:\n1.-Presionar barra espaciadora para reproducir animacion.\n2.-Presionar 0 para volver a habilitar reproduccion de la animacion\n");
		printf("3.-Presiona L para guardar frame\n4.-Presiona P para habilitar guardar nuevo frame\n5.-Presiona 1 para mover en X\n6.-Presiona 2 para habilitar mover en X\n7.- Presiona 3 para mover en X negativo\n8.- presiona 4 para habilitar mover en X\n");
		printf("9.-Presiona 5 para mover en Y\n10.-Presiona 6 para habilitar mover en Y\n11.- Presiona 7 para mover en Y negativo\n12.- presiona 8 para habilitar mover en Y\n 13.- Presiona R para girar\n14.- Presiona T para habilitar giro");


		glm::mat4 model(1.0);
		glm::mat4 modelaux(1.0);
		glm::mat4 modelMike(1.0);
		glm::mat4 modelMikeAux(1.0);
		glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
		glm::vec2 toffset = glm::vec2(0.0f, 0.0f);
		glm::vec3 lowerLight = glm::vec3(0.0f,0.0f,0.0f);
		float swingAngle;

		
	////Loop mientras no se cierra la ventana
	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

		angulovaria += 0.5f*deltaTime;

		if (movCoche < 30.0f)
		{
			movCoche -= movOffset * deltaTime;
			//printf("avanza%f \n ",movCoche);
		}
		rotllanta += rotllantaOffset * deltaTime;


		//Recibir eventos del usuario
		glfwPollEvents();
		//camera.keyControl(mainWindow.getsKeys(), deltaTime);
		//camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		if (!camaraTerceraPersona) {
			camera.keyControl(mainWindow.getsKeys(), deltaTime);
			camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());
		}



		//-------Para Keyframes
		inputKeyframes(mainWindow.getsKeys());
		animate();

		// Clear the window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4  vista(1.0f);
		glm::vec3  ojo(0.0f), dirCam(0.0f);

		if (camaraTerceraPersona) {
			// Posición del avatar en mundo (tu base es -15,3,-15 + offsets)
			glm::vec3 objetivo = glm::vec3(-15.0f + posMike_X, 3.0f, -15.0f + posMike_Z);
			glm::vec3 objetivoMirar = objetivo + glm::vec3(0.0f, 1.5f, 0.0f); // punto de mira (cabeza/pecho)

			// “Forward” horizontal del avatar (solo XZ), desde rotMike_Y
			float yawRad = rotMike_Y * toRadians;
			glm::vec3 adelanteXZ = glm::vec3(glm::cos(yawRad), 0.0f, -glm::sin(yawRad));

			// Orbitado horizontal relativo (camara alrededor del avatar)
			float yawCam = (rotMike_Y + desfaseGiroCamara) * toRadians;
			glm::vec3 adelanteOrbita = glm::vec3(glm::cos(yawCam), 0.0f, -glm::sin(yawCam));

			// Posición deseada del ojo (detrás del avatar), altura fija
			glm::vec3 ojoDeseado = objetivoMirar - adelanteOrbita * distanciaCamara
				+ glm::vec3(0.0f, alturaCamara, 0.0f);

			// Suavizado exponencial
			static glm::vec3 ojoSuavizado = ojoDeseado;
			float a = 1.0f - std::exp(-suavizadoCamara * deltaTime); // 0..1
			ojoSuavizado = glm::mix(ojoSuavizado, ojoDeseado, a);

			ojo = ojoSuavizado;
			dirCam = glm::normalize(objetivoMirar - ojo);

			vista = glm::lookAt(ojo, objetivoMirar, glm::vec3(0, 1, 0));
		}
		else {
			// Modo libre actual
			vista = camera.calculateViewMatrix();
			ojo = camera.getCameraPosition();
			dirCam = camera.getCameraDirection();
		}

		// ----- Subir uniforms usando 'vista' y 'ojo' calculados -----
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(vista));
		glUniform3f(uniformEyePosition, ojo.x, ojo.y, ojo.z);

		// Linterna/flash atada a la cámara
		glm::vec3 luzBaja = ojo; luzBaja.y -= 0.3f;
		spotLights[0].SetFlash(luzBaja, dirCam);


		//skybox.DrawSkybox(camera.calculateViewMatrix(), projection);
		skybox.DrawSkybox(vista, projection);

		shaderList[0].UseShader();


		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformEyePosition = shaderList[0].GetEyePositionLocation();
		uniformColor = shaderList[0].getColorLocation();
		uniformTextureOffset = shaderList[0].getOffsetLocation();

		//información en el shader de intensidad especular y brillo	
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		//glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(vista));
		//glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);
		glUniform3f(uniformEyePosition, ojo.x, ojo.y, ojo.z);

		// luz ligada a la cámara de tipo flash
		//lowerLight = camera.getCameraPosition();
		lowerLight = ojo;
		lowerLight.y -= 0.3f;
		//spotLights[0].SetFlash(lowerLight, camera.getCameraDirection());
		spotLights[0].SetFlash(luzBaja, dirCam);

		//información al shader de fuentes de iluminación
		shaderList[0].SetDirectionalLight(&mainLight);
		shaderList[0].SetPointLights(pointLights, pointLightCount);
		shaderList[0].SetSpotLights(spotLights, spotLightCount);


		model=glm::mat4(1.0);
		modelaux= glm::mat4(1.0);
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		toffset = glm::vec2(0.0f, 0.0f);
		
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
		pisoTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);

		meshList[2]->RenderMesh();

		//prueba lampara1

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(100.0f, -1.0f, 15.0f));
		model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		lampara1.RenderModel();


		
		//prueba casa2
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(100.0f, 0.0f, 100.0f));
		model = glm::scale(model, glm::vec3(2.5f, 2.5f, 2.5f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		casa2.RenderModel();
		

		//prueba edificios

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(100.0f, -1.0f, -60.0f));
		model = glm::scale(model, glm::vec3(10.0f, 10.0f, 10.0f));
		//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		edificios.RenderModel();

		/*
		//prueba casas

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-30.0f, 15.0f, -100.0f));
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		casas.RenderModel();
		*/

		//prueba tubo
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		tubo.RenderModel();


	
		
		
		//prueba avatar mike
		modelMike = glm::mat4(1.0);
		modelMike = glm::translate(modelMike, glm::vec3(-15.0f, 3.0f, -15.0f)); // Posición inicial fija
		modelMike = glm::translate(modelMike, glm::vec3(posMike_X, 0.0f, posMike_Z));

		modelMike = glm::rotate(modelMike, rotMike_Y * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));

		modelMike = glm::scale(modelMike, glm::vec3(1.0f, 1.0f, 1.0f)); // Escala
		modelMikeAux = modelMike;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelMike));
		cuerpoM.RenderModel();
		
		swingAngle = glm::sin(anguloCaminata * toRadians) * MAX_SWING_ANGLE;

		//brazo derecho
		modelMike = modelMikeAux;
		modelMike = glm::rotate(modelMike, -swingAngle * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelMike));
		ArmR.RenderModel();

		//brazo izq
		modelMike = modelMikeAux;
		modelMike = glm::rotate(modelMike, swingAngle * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelMike));
		ArmL.RenderModel();

		//piernas
		//derecha
		modelMike = modelMikeAux;
		modelMike = glm::rotate(modelMike, swingAngle * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelMike));
		LegR.RenderModel();

		
		//izquierda
		modelMike = modelMikeAux;
		modelMike = glm::rotate(modelMike, -swingAngle * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelMike));
		LegL.RenderModel();

	

		

		//prueba puerta monster inc
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(10.0f, 0.0f, -10.0f));
		model = glm::scale(model, glm::vec3(0.08f, 0.08f, 0.08f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		puerta.RenderModel();

		//prueba banca
		
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-10.0f, 0.0f, 10.0f));
		model = glm::scale(model, glm::vec3(10.0f, 10.0f, 10.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		banca.RenderModel();

		//prueba banca1

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-20.0f, 0.0f, 20.0f));
		model = glm::scale(model, glm::vec3(10.0f, 10.0f, 10.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		banca.RenderModel();

		//prueba banca2

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-20.0f, 0.0f, 5.0f));
		model = glm::scale(model, glm::vec3(10.0f, 10.0f, 10.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		banca.RenderModel();

		
		//prueba lampara

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(100.0f, -2.0f, 30.0f));
		model = glm::scale(model, glm::vec3(1.5f, 1.5f, 1.5f));
		model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		lampara.RenderModel();

		


		//prueba ring
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(10.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		ring.RenderModel();

		//prueba carro
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-10.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(3.0f, 3.0f, 3.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Carro.RenderModel();

		
		//prueba balon
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(5.0f, 5.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		balonTexture.UseTexture();
		balon.RenderModel();


		


		model = glm::mat4(1.0);
		posblackhawk=glm::vec3(posXavion + movAvion_x, -1.5 + movAvion_y, posZavion);
		model = glm::translate(model, posblackhawk);
		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		
		model = glm::rotate(model, giroAvion * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, giroX * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, 90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		//color = glm::vec3(0.0f, 1.0f, 0.0f);
		//glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//Blackhawk_M.RenderModel();
		tubo.RenderModel();
		
		
		glUseProgram(0);

		mainWindow.swapBuffers();
	}
	//saveBaseKeyframes(NUM_BASE_KEYFRAMES);
	return 0;
}


void inputKeyframes(bool* keys)
{
	if (keys[GLFW_KEY_SPACE])
	{
		if (reproduciranimacion < 1)
		{
			if (play == false && (FrameIndex > 1))
			{
				resetElements();
				//First Interpolation				
				interpolation();
				
				playIndex = 0;
				i_curr_steps = 0;
				play = true;
				reproduciranimacion++;
				printf("\n presiona 0 para habilitar reproducir de nuevo la animación'\n");
				habilitaranimacion = 0;

			}
			
			else
			{
				play = false;

			}
		}
	}
	if (keys[GLFW_KEY_0])
	{
		if (habilitaranimacion < 1 && reproduciranimacion>0)
		{
			printf("Ya puedes reproducir de nuevo la animación con la tecla de barra espaciadora'\n");
			reproduciranimacion = 0;
			//FrameIndex = 0; // Reinicia el contador 
			
		}
	}

	if (keys[GLFW_KEY_L])
	{
		if (guardoFrame < 1)
		{
			saveFrame();
			printf("movAvion_x es: %f\n", movAvion_x);
			printf("movAvion_y es: %f\n", movAvion_y);
			printf("presiona P para habilitar guardar otro frame'\n");
			guardoFrame++;
			reinicioFrame = 0;
		}
	}
	if (keys[GLFW_KEY_P]) //para cada tecla debe de incluir banderas para evitar múltiples entradas por latencia
	{
		if (reinicioFrame < 1)
		{
			guardoFrame = 0;
			reinicioFrame++; //si se coloca aquí el press, por latencia entrará más de una vez el print
			printf("Ya puedes guardar otro frame presionando la tecla L'\n");
		}
	}

	if (keys[GLFW_KEY_R])
	{
		if (banderaGiro < 1)
		{
			//saveFrame();
			printf("movAvion_giro\n");
			printf("presiona T para habilitar giro'\n");
			giroAvion += 180.0f;
			//if (orientacion == false) giroAvion -= 180.0f;
			banderaGiro++;
			banderaGiro_ = 0;
		}
	}
	if (keys[GLFW_KEY_T]) 
	{
		if (banderaGiro_ < 1)
		{
			banderaGiro = 0;
			banderaGiro_++; 
			printf("Ya puedes continuar'\n");
		}
	}


	if (keys[GLFW_KEY_1])
	{
		if (ciclo < 1)
		{
			//printf("movAvion_x es: %f\n", movAvion_x);
			movAvion_x += 1.0f;
			printf("\n movAvion_x es: %f\n", movAvion_x);
			ciclo++;
			ciclo2 = 0;
			printf("\n Presiona la tecla 2 para poder habilitar la variable\n");
		}

	}
	if (keys[GLFW_KEY_2])
	{
		if (ciclo2 < 1)
		{
			ciclo = 0;
			ciclo2++;
			printf("\n Ya puedes modificar tu variable presionando la tecla 1\n");
		}
	}
	//teclas para movimiento en X regreso
	if (keys[GLFW_KEY_3])
	{
		if (banderaX < 1)
		{
			//printf("movAvion_x es: %f\n", movAvion_x);
			movAvion_x -= 1.0f;
			printf("\n movAvion_x es: %f\n", movAvion_x);
			banderaX++;
			banderaX_ = 0;
			printf("\n Presiona la tecla 4 para poder habilitar la variable\n");
		}

	}
	if (keys[GLFW_KEY_4])
	{
		if (banderaX_ < 1)
		{
			banderaX = 0;
			banderaX_++;
			printf("\n Ya puedes modificar X  presionando la tecla 3\n");
		}
	}

	if (keys[GLFW_KEY_5])
	{
		if (banderaY < 1)
		{
			//printf("movAvion_x es: %f\n", movAvion_x);
			movAvion_y += 1.0f;
			printf("\n movAvion_x es: %f\n", movAvion_y);
			banderaY++;
			banderaY_ = 0;
			printf("\n Presiona la tecla 6 para poder habilitar la variable\n");
		}

	}
	if (keys[GLFW_KEY_6])
	{
		if (banderaY_ < 1)
		{
			banderaY = 0;
			banderaY_++;
			printf("\n Ya puedes modificar Y  presionando la tecla 5\n");
		}
	}
	if (keys[GLFW_KEY_7])
	{
		if (banderaY2 < 1)
		{
			//printf("movAvion_x es: %f\n", movAvion_x);
			movAvion_y -= 1.0f;
			printf("\n movAvion_x es: %f\n", movAvion_y);
			banderaY2++;
			banderaY2_ = 0;
			printf("\n Presiona la tecla 8 para poder habilitar la variable\n");
		}

	}
	if (keys[GLFW_KEY_8])
	{
		if (banderaY2_ < 1)
		{
			banderaY2 = 0;
			banderaY2_++;
			printf("\n Ya puedes modificar Y  presionando la tecla 7\n");
		}
	}

	

	float yawRad = rotMike_Y * toRadians;
	glm::vec3 adelante = glm::vec3(glm::cos(yawRad), 0.0f, -glm::sin(yawRad));
	bool isMoving = false;

	if (camaraTerceraPersona) {
		// Rotación A/D
		if (keys[GLFW_KEY_A]) { rotMike_Y += velocidadGiro * deltaTime; }
		if (keys[GLFW_KEY_D]) { rotMike_Y -= velocidadGiro * deltaTime; }

		float yawRad = rotMike_Y * toRadians;
		glm::vec3 adelante = glm::vec3(glm::cos(yawRad), 0.0f, -glm::sin(yawRad));

		// Traslación W/S
		if (keys[GLFW_KEY_W]) {
			posMike_X += adelante.x * velocidadMovimiento * deltaTime;
			posMike_Z += adelante.z * velocidadMovimiento * deltaTime;
			isMoving = true;
		}
		if (keys[GLFW_KEY_S]) {
			posMike_X -= adelante.x * velocidadMovimiento * deltaTime;
			posMike_Z -= adelante.z * velocidadMovimiento * deltaTime;
			isMoving = true;
		}
	}

	// Oscilación de caminata
	if (isMoving) {
		anguloCaminata += velocidadCaminata * deltaTime;
		if (anguloCaminata > 360.0f) anguloCaminata -= 360.0f;
	}
	else {
		anguloCaminata = 0.0f;
	}


	
	static bool c_estadoAnterior = false;          
	bool c_estadoActual = keys[GLFW_KEY_C];          

	if (c_estadoActual && !c_estadoAnterior) {
		//se acaba de presionar C
		camaraTerceraPersona = !camaraTerceraPersona;

		// salir de 3ra persona deteneranimacin
		if (!camaraTerceraPersona) {
			anguloCaminata = 0.0f;
		}
	}
	//  estado
	c_estadoAnterior = c_estadoActual;
	
	
}
