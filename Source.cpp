#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <cmath>

using namespace std;

#define numVAOs 1


GLuint renderingProgram;
GLuint vao[numVAOs];

GLuint vbo;
GLuint circleVBO;

float offsetX = 0.0f;
float velocity = 0.5f;

float lineOffsetY = 0.0f;

const int circleSegments = 60;

float offsetY = 0.0f;

float velX = 0.0f;
float velY = 0.0f;

bool isMoving = false;

//Kör pattogás G változoi
bool angledMovement = false;
float speed = 2.0f;

float angleDeg = 25.0f;


bool checkOpenGLError() {
	bool foundError = false;
	int glErr = glGetError();
	while (glErr != GL_NO_ERROR) {
		cout << "glError: " << glErr << endl;
		foundError = true;
		glErr = glGetError();
	}
	return foundError;
}

void printShaderLog(GLuint shader) {
	int len = 0;
	int chWrittn = 0;
	char* log;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
	if (len > 0) {
		log = (char*)malloc(len);
		glGetShaderInfoLog(shader, len, &chWrittn, log);
		cout << "Shader Info Log: " << log << endl;
		free(log);
	}
}

void printProgramLog(int prog) {
	int len = 0;
	int chWrittn = 0;
	char* log;
	glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &len);
	if (len > 0) {
		log = (char*)malloc(len);
		glGetProgramInfoLog(prog, len, &chWrittn, log);
		cout << "Program Info Log: " << log << endl;
		free(log);
	}
}

/** Függvény, a shader fájlok sorainak beolvasásához. */
string readShaderSource(const char* filePath) {
	string content;
	ifstream fileStream(filePath, ios::in);
	string line = "";

	while (!fileStream.eof()) {
		getline(fileStream, line);
		content.append(line + "\n");
	}
	fileStream.close();
	return content;
}

GLuint createShaderProgram() {

	GLint vertCompiled;
	GLint fragCompiled;
	GLint linked;

	/** Beolvassuk a shader fájlok tartalmát. */
	string vertShaderStr = readShaderSource("vertexShader.glsl");
	string fragShaderStr = readShaderSource("fragmentShader.glsl");

	/** Létrehozzuk a shader objektumainkat. */
	GLuint vShader = glCreateShader(GL_VERTEX_SHADER);
	GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);

	/** A shader fájlok tartalmát eltároló string objektum szöveggé konvertálásás is elvégezzük. */
	const char* vertShaderSrc = vertShaderStr.c_str();
	const char* fragShaderSrc = fragShaderStr.c_str();

	/** Ekkor a betöltött kódot hozzárendelhetjük a shader objektumainkhoz. */
	glShaderSource(vShader, 1, &vertShaderSrc, NULL);
	glShaderSource(fShader, 1, &fragShaderSrc, NULL);

	/** Fordítsuk le ezen shader objektumhoz rendelt kódot. */
	glCompileShader(vShader);
	/** Hibakeresési lépések. Például sikeres volt-e a fordítás? Ha nem, mi volt az oka? */
	checkOpenGLError();
	glGetShaderiv(vShader, GL_COMPILE_STATUS, &vertCompiled);
	if (vertCompiled != 1) {
		cout << "vertex compilation failed" << endl;
		printShaderLog(vShader);
	}

	/** A második shader objektumhoz rendelt kódunkat is lefordítjuk. */
	glCompileShader(fShader);
	/** Ismét hibakeresési lépések. Például sikeres volt-e a fordítás? Ha nem, mi volt az oka? */
	checkOpenGLError();
	glGetShaderiv(vShader, GL_COMPILE_STATUS, &fragCompiled);
	if (fragCompiled != 1) {
		cout << "fragment compilation failed" << endl;
		printShaderLog(fShader);
	}

	/** Shader program objektum létrehozása: õ fogja össze a shadereket. Eltároljuk az ID értéket. */
	GLuint vfProgram = glCreateProgram();
	/** Csatoljuk a shadereket az elõzõ lépésben létrehozott objektumhoz. */
	glAttachShader(vfProgram, vShader);
	glAttachShader(vfProgram, fShader);

	/** Végül a GLSL fordítónk ellenõrzi, hogy a csatolt shaderek kompatibilisek-e egymással. */
	glLinkProgram(vfProgram);
	/** Ha hiba lépett fel, nézzük meg mi volt ennek az oka. */
	checkOpenGLError();
	glGetProgramiv(vfProgram, GL_LINK_STATUS, &linked);
	if (linked != 1) {
		cout << "linking failed" << endl;
		printProgramLog(vfProgram);
	}

	/** Ha minden rendben ment a linkelés során, az objektumok leválaszthatóak a programról. */
	glDeleteShader(vShader);
	glDeleteShader(fShader);

	/** A kész program a visszatérési értékünk. */
	return vfProgram;
}

/** A jelenetünk utáni takarítás. */
void cleanUpScene()
{
	/** Töröljük a vertex array objektumokat. */
	glDeleteVertexArrays(1, vao);

	/** Töröljük a shader programot. */
	glDeleteProgram(renderingProgram);
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	//Vonal fel re mozgatása
	if (action == GLFW_PRESS || action == GLFW_REPEAT)
	{
		if (key == GLFW_KEY_UP)
		{
			lineOffsetY += 0.02f;
		}

		if (key == GLFW_KEY_DOWN)
		{
			lineOffsetY -= 0.02f;
		}
	}
	//ESC kilépés
	if ((action == GLFW_PRESS) && (key == GLFW_KEY_ESCAPE))
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	//Kör pattogásának indítása
	if (key == GLFW_KEY_S && action == GLFW_PRESS)
	{
		angledMovement = true;

		float angleRad = angleDeg * 3.14159f / 180.0f;

		velX = cos(angleRad) * speed;
		velY = sin(angleRad) * speed;
	}
}

void cursorPosCallback(GLFWwindow* window, double xPos, double yPos)
{

}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{

}

void init(GLFWwindow* window) {
	/** A rajzoláshoz használt shader programok betöltése. */
	renderingProgram = createShaderProgram();
	/** Vertex Array Objektum létrehozása. */
	glGenVertexArrays(numVAOs, vao);
	glBindVertexArray(vao[0]);

	//vonal x y z koordinátái
	float vertices[6] =
	{
		-0.3f, 0.0f, 0.0f,
		 0.3f, 0.0f, 0.0f
	};

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	// 50 pixel / fél ablakméret (600)
	float radius = 50.0f / 300.0f; 

	vector<float> circleVertices;

	// Kör középpntja
	circleVertices.push_back(0.0f);
	circleVertices.push_back(0.0f);
	circleVertices.push_back(0.0f);

	circleVertices.push_back(1.0f);
	circleVertices.push_back(0.0f);
	circleVertices.push_back(0.0f);

	// kör pontjai
	for (int i = 0; i <= circleSegments; i++)
	{
		float angle = 2.0f * 3.14159f * i / circleSegments;

		float x = cos(angle) * radius;
		float y = sin(angle) * radius;

		circleVertices.push_back(x);
		circleVertices.push_back(y);
		circleVertices.push_back(0.0f);

		circleVertices.push_back(0.0f);
		circleVertices.push_back(1.0f);
		circleVertices.push_back(0.0f);
	}

	glGenBuffers(1, &circleVBO);
	glBindBuffer(GL_ARRAY_BUFFER, circleVBO);

	glBufferData(
		GL_ARRAY_BUFFER,
		circleVertices.size() * sizeof(float),
		&circleVertices[0],
		GL_STATIC_DRAW
	);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
}

void display(GLFWwindow* window, double currentTime) {
	//Háttér színe 
	glClearColor(0.9, 0.75, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);

	GLuint lineLoc = glGetUniformLocation(renderingProgram, "lineOffsetY");
	glUniform1f(lineLoc, lineOffsetY);


	//A vonal határainak beállítása
	if (lineOffsetY > 1.0f) lineOffsetY = 1.0f;
	if (lineOffsetY < -1.0f) lineOffsetY = -1.0f;

	/** Aktiváljuk a shader-program objektumunkat. */
	glUseProgram(renderingProgram);
	//Megadja hogy mozogjon a körrel együtt
	GLuint movingLoc = glGetUniformLocation(renderingProgram, "moving");
	glUniform1i(movingLoc, 0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	/** Ha kirajzoltuk a pontunkat, kísérletezzünk a pont méretének növelésével. */
	//glPointSize(30.0);
	/** Ha vonalat rajzolunk ki, ezzel módosíthatjuk a láthatóságot. */
	//vonalvastagság
	glLineWidth(3.0);


	//glDrawArrays(GL_POINTS, 0, 1);
	glDrawArrays(GL_LINES, 0, 2);
	//glDrawArrays(GL_TRIANGLES, 0, 3);
	bool collision = false;

	if (!angledMovement)
	{
		//Balra-jobbra mozgás
		offsetX += velocity * 0.01f;
	}
	else
	{
		//Pattogo mozgás
		offsetX += velX * 0.01f;
		offsetY += velY * 0.01f;
	}

	float radius = 50.0f / 300.0f;

	// legközelebbi pont a vonalon
	float closestX = std::max(-0.3f, std::min(offsetX, 0.3f));
	float closestY = lineOffsetY;

	// Pitagorasz
	float dx = offsetX - closestX;
	float dy = offsetY - closestY;

	float distance = sqrt(dx * dx + dy * dy);

	// ütközés
	collision = (distance <= radius);

	GLuint collisionLoc = glGetUniformLocation(renderingProgram, "collision");
	glUniform1i(collisionLoc, collision ? 1 : 0);

	//Pattogás 
	if (!angledMovement)
	{
		//Bal-jobb mozgás pattogása
		if (offsetX > 1.0f - radius || offsetX < -1.0f + radius)
		{
			velocity = -velocity;
		}
	}
	else
	{
		//A 25 fokos pattogás
		//Jobb-bal fal
		if (offsetX > 1.0f - radius || offsetX < -1.0f + radius)
		{
			velX = -velX;
		}

		//Felső-alsó fal
		if (offsetY > 1.0f - radius || offsetY < -1.0f + radius)
		{
			velY = -velY;
		}
	}

	GLuint offsetYLoc = glGetUniformLocation(renderingProgram, "offsetY");
	glUniform1f(offsetYLoc, offsetY);

	glUniform1i(movingLoc, 1);

	GLuint offsetLoc = glGetUniformLocation(renderingProgram, "offsetX");
	glUniform1f(offsetLoc, offsetX);
	glBindBuffer(GL_ARRAY_BUFFER, circleVBO);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glDrawArrays(GL_TRIANGLE_FAN, 0, circleSegments + 2);
}

int main(void) {

	/** Próbáljuk meg inicializálni a GLFW-t! */
	if (!glfwInit()) { exit(EXIT_FAILURE); }

	/** A kívánt OpenGL verzió (4.3) */
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	/** Próbáljuk meg létrehozni az ablakunkat. */
	//Ablakméret és cím megadás
	GLFWwindow* window = glfwCreateWindow(600, 600, "Beadando1", NULL, NULL);

	/** Válasszuk ki az ablakunk OpenGL kontextusát, hogy használhassuk. */
	glfwMakeContextCurrent(window);
	cout << "Keyboard control" << endl;
	cout << "ESC\t\tkilepes" << endl;
	cout << "UP_KEY\t\tVonal felfele mozgatasa" << endl;
	cout << "DOWN_KEY\tVonal lefele mozgatasa" << endl;
	cout << "S\t\tA kor pattogasanak inditasa" << endl;
	/** Billentyûzethez köthetõ események kezelése. */
	glfwSetKeyCallback(window, keyCallback);
	/** A kurzor helyének vizsgálata. */
	//glfwSetCursorPosCallback(window, cursorPosCallback);
	/** Az egér gombjaihoz köthetõ események kezelése. */
	//glfwSetMouseButtonCallback(window, mouseButtonCallback);

	/** Incializáljuk a GLEW-t, hogy elérhetõvé váljanak az OpenGL függvények. */
	if (glewInit() != GLEW_OK) { exit(EXIT_FAILURE); }
	glfwSwapInterval(1);

	/** Az alkalmazáshoz kapcsolódó elõkészítõ lépések, pl. hozd létre a shader objektumokat. */
	init(window);

	while (!glfwWindowShouldClose(window)) {
		/** a kód, amellyel rajzolni tudunk a GLFWwindow ojektumunkba. */
		display(window, glfwGetTime());
		/** double buffered */
		glfwSwapBuffers(window);
		/** események kezelése az ablakunkkal kapcsolatban, pl. gombnyomás */
		glfwPollEvents();
	}

	/** Töröljük a GLFW ablakot. */
	glfwDestroyWindow(window);
	/** Leállítjuk a GLFW-t */
	glfwTerminate();

	/** Felesleges objektumok törlése. */
	cleanUpScene();

	exit(EXIT_SUCCESS);
}