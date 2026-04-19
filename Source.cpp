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

struct Point {
    float x, y;
};

vector<Point> controlPoints;

int selectedPoint = -1;
int windowWidth = 600;
int windowHeight = 600;

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


Point deCasteljau(float t) {
    vector<Point> temp = controlPoints;

    int n = temp.size();
    for (int k = 1; k < n; k++) {
        for (int i = 0; i < n - k; i++) {
            temp[i].x = (1 - t) * temp[i].x + t * temp[i + 1].x;
            temp[i].y = (1 - t) * temp[i].y + t * temp[i + 1].y;
        }
    }
    return temp[0];
}

void drawCircle(float cx, float cy, float r) {
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx, cy);

    int segments = 30; 
    for (int i = 0; i <= segments; i++) {
        float angle = 2.0f * 3.1415926f * i / segments;
        float x = cx + cos(angle) * r;
        float y = cy + sin(angle) * r;
        glVertex2f(x, y);
    }

    glEnd();
}

Point screenToGL(double x, double y) {
    Point p;
    p.x = (x / windowWidth) * 2.0f - 1.0f;
    p.y = 1.0f - (y / windowHeight) * 2.0f;
    return p;
}

float dist(Point a, Point b) {
    return sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
}
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if ((action == GLFW_PRESS) && (key == GLFW_KEY_ESCAPE))
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void cursorPosCallback(GLFWwindow* window, double x, double y) {
    if (selectedPoint != -1) {
        controlPoints[selectedPoint] = screenToGL(x, y);
    }
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    double x, y;
    glfwGetCursorPos(window, &x, &y);
    Point p = screenToGL(x, y);

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        selectedPoint = -1;
        for (int i = 0; i < controlPoints.size(); i++) {
            if (dist(p, controlPoints[i]) < 0.03f) {
                selectedPoint = i;
                return;
            }
        }
        controlPoints.push_back(p);
    }

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
        selectedPoint = -1;
    }

    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
        for (int i = 0; i < controlPoints.size(); i++) {
            if (dist(p, controlPoints[i]) < 0.05f) {
                controlPoints.erase(controlPoints.begin() + i);
                break;
            }
        }
    }
}

void init(GLFWwindow* window) {
    /** A rajzoláshoz használt shader programok betöltése. */
    renderingProgram = createShaderProgram();
    /** Vertex Array Objektum létrehozása. */
    glGenVertexArrays(numVAOs, vao);
    glBindVertexArray(vao[0]);
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    glColor3f(1.0f, 0.0f, 0.0f);

    for (auto& p : controlPoints) {
        drawCircle(p.x, p.y, 0.03f); 
    }


    if (controlPoints.size() >= 2) {
        glColor3f(0.0f, 1.0f, 0.0f);
        glBegin(GL_LINE_STRIP);
        for (auto& p : controlPoints)
            glVertex2f(p.x, p.y);
        glEnd();
    }

    if (controlPoints.size() >= 2) {
        glColor3f(0.0f, 0.5f, 1.0f);
        glBegin(GL_LINE_STRIP);
        for (int i = 0; i <= 200; i++) {
            float t = i / 200.0f;
            Point p = deCasteljau(t);
            glVertex2f(p.x, p.y);
        }
        glEnd();
    }
}

int main() {
    /** Próbáljuk meg inicializálni a GLFW-t! */
    if (!glfwInit()) { exit(EXIT_FAILURE); }

    GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "Beadando 2", NULL, NULL);
    glfwMakeContextCurrent(window);
    cout << "Keyboard control" << endl;
    cout << "ESC\t\t\tkilepes" << endl;
    cout << "left_mouse_button\t\t Kontrol pont letrehozas" << endl;
    cout << "right_mouse_button\t\t Kontrol pont torlese" << endl;
    cout << "left_mouse_button drag and drop\t Kontrol drag and drop" << endl;

    glewInit();

    /** Billentyûzethez köthetõ események kezelése. */
    glfwSetKeyCallback(window, keyCallback);
    /** A kurzor helyének vizsgálata. */
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    /** Az egér gombjaihoz köthetõ események kezelése. */
    glfwSetCursorPosCallback(window, cursorPosCallback);

    /** Incializáljuk a GLEW-t, hogy elérhetõvé váljanak az OpenGL függvények. */
    if (glewInit() != GLEW_OK) { exit(EXIT_FAILURE); }
    glfwSwapInterval(1);

    while (!glfwWindowShouldClose(window)) {
        display();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    /** Felesleges objektumok törlése. */
    cleanUpScene();

    exit(EXIT_SUCCESS);
}