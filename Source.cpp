#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;

GLuint renderingProgram;
GLuint vao[1];
GLuint vbo[2]; // 0: kocka, 1: gömb

// Kamera és fény változók
float r = 9.0f;
float theta = 0.0f;
float camH = 0.0f;
float lightAngle = 0.0f;
bool lightOn = true;

// Uniform helyszínek
GLuint mvpLoc, mLoc, lpLoc, lightOnLoc, isSunLoc;

// Egységnyi kocka adatai
float cubeVertices[] = {
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f,  0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f,  0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f,  0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f,  0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f,  0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f,  0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f,  0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f,  0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f,  0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f,  0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f,  0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f,  0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f, -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f
};

// Gömb adatai
vector<float> sphereVertices;
vector<int> sphereIndices;

void setupSphere(float radius, int precision) {
    sphereVertices.clear();
    for (int i = 0; i < precision; i++) {
        for (int j = 0; j < precision; j++) {
            float phi1 = glm::pi<float>() * (float)i / (float)precision;
            float phi2 = glm::pi<float>() * (float)(i + 1) / (float)precision;
            float theta1 = 2.0f * glm::pi<float>() * (float)j / (float)precision;
            float theta2 = 2.0f * glm::pi<float>() * (float)(j + 1) / (float)precision;

            // Négy pont kiszámítása egy "négyzethez" a gömbön
            glm::vec3 p1(radius * sin(phi1) * cos(theta1), radius * cos(phi1), radius * sin(phi1) * sin(theta1));
            glm::vec3 p2(radius * sin(phi2) * cos(theta1), radius * cos(phi2), radius * sin(phi2) * sin(theta1));
            glm::vec3 p3(radius * sin(phi1) * cos(theta2), radius * cos(phi1), radius * sin(phi1) * sin(theta2));
            glm::vec3 p4(radius * sin(phi2) * cos(theta2), radius * cos(phi2), radius * sin(phi2) * sin(theta2));

            // Két háromszög (p1, p2, p3) és (p3, p2, p4)
            glm::vec3 pts[] = { p1, p2, p3, p3, p2, p4 };
            for (int k = 0; k < 6; k++) {
                sphereVertices.push_back(pts[k].x);
                sphereVertices.push_back(pts[k].y);
                sphereVertices.push_back(pts[k].z);
                // Normálvektor (gömbnél a középpontból kifelé mutat)
                glm::vec3 n = glm::normalize(pts[k]);
                sphereVertices.push_back(n.x);
                sphereVertices.push_back(n.y);
                sphereVertices.push_back(n.z);
                // UV koordináták (üresen is hagyható, ha nincs textúra)
                sphereVertices.push_back(0.0f);
                sphereVertices.push_back(0.0f);
            }
        }
    }
}

string readShaderSource(const char* filePath) {
    string content;
    ifstream fileStream(filePath, ios::in);
    if (!fileStream.is_open()) return "";
    string line = "";
    while (getline(fileStream, line)) content.append(line + "\n");
    fileStream.close();
    return content;
}

GLuint createShaderProgram() {
    string vertShaderStr = readShaderSource("vertexShader.glsl");
    string fragShaderStr = readShaderSource("fragmentShader.glsl");
    const char* vSrc = vertShaderStr.c_str();
    const char* fSrc = fragShaderStr.c_str();
    GLuint vShader = glCreateShader(GL_VERTEX_SHADER);
    GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(vShader, 1, &vSrc, NULL);
    glShaderSource(fShader, 1, &fSrc, NULL);
    glCompileShader(vShader);
    glCompileShader(fShader);
    GLuint vfProgram = glCreateProgram();
    glAttachShader(vfProgram, vShader);
    glAttachShader(vfProgram, fShader);
    glLinkProgram(vfProgram);
    return vfProgram;
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        if (key == GLFW_KEY_LEFT)  theta -= 0.1f;
        if (key == GLFW_KEY_RIGHT) theta += 0.1f;
        if (key == GLFW_KEY_UP)    camH += 0.2f;
        if (key == GLFW_KEY_DOWN)  camH -= 0.2f;
        if (key == GLFW_KEY_L)     lightOn = !lightOn;
        if ((action == GLFW_PRESS) && (key == GLFW_KEY_ESCAPE))
            glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

void init(GLFWwindow* window) {
    renderingProgram = createShaderProgram();
    glGenVertexArrays(1, vao);
    glBindVertexArray(vao[0]);
    glGenBuffers(2, vbo); // KÉT buffer kell!

    // Kocka feltöltése
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

    // Gömb generálása és feltöltése
    setupSphere(1.0f, 48);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, sphereVertices.size() * sizeof(float), &sphereVertices[0], GL_STATIC_DRAW);

    mvpLoc = glGetUniformLocation(renderingProgram, "mvp_matrix");
    mLoc = glGetUniformLocation(renderingProgram, "model_matrix");
    lpLoc = glGetUniformLocation(renderingProgram, "lightPos");
    lightOnLoc = glGetUniformLocation(renderingProgram, "useLighting");
    isSunLoc = glGetUniformLocation(renderingProgram, "isSun");

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
}

void display(GLFWwindow* window, double currentTime) {
    // 1. Törlés
    glClearColor(0.05f, 0.05f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(renderingProgram);

    // 2. Mátrixok beállítása
    glm::mat4 pMat = glm::perspective(glm::radians(55.0f), 1.0f, 0.1f, 1000.0f);
    glm::vec3 camPos(r * cos(theta), r * sin(theta), camH);
    glm::mat4 vMat = glm::lookAt(camPos, glm::vec3(0, 0, 0), glm::vec3(0, 0, 1));

    // Fény mozgása
    lightAngle += 0.01f;
    glm::vec3 lp(2.0f * r * cos(lightAngle), 2.0f * r * sin(lightAngle), 0.0f);

    // Alap uniformok küldése
    glUniform3fv(lpLoc, 1, glm::value_ptr(lp));
    glUniform1i(lightOnLoc, lightOn);

    // --- 3. KOCKÁK RAJZOLÁSA ---
    glUniform1i(isSunLoc, 0); // Kockákra hat a fény
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]); // KOCKA BUFFER AKTIVÁLÁSA

    // Attribútumok megadása a KOCKÁHOZ
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)(sizeof(float) * 3));
    glEnableVertexAttribArray(1);

    float zOffsets[] = { 0.0f, 2.0f, -2.0f };
    for (int i = 0; i < 3; i++) {
        glm::mat4 mMat = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, zOffsets[i]));
        glm::mat4 mvp = pMat * vMat * mMat;
        glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(mvp));
        glUniformMatrix4fv(mLoc, 1, GL_FALSE, glm::value_ptr(mMat));
        glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertex a kockának
    }

    // --- 4. GÖMB (FÉNYFORRÁS) RAJZOLÁSA ---
    glUniform1i(isSunLoc, 1); // Gömb "önvilágító"
    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]); // GÖMB BUFFER AKTIVÁLÁSA

    // Attribútumok ÚJRA-megadása a GÖMBHÖZ (Mert a VBO megváltozott!)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)(sizeof(float) * 3));
    glEnableVertexAttribArray(1);

    glm::mat4 sMat = glm::translate(glm::mat4(1.0f), lp);
    sMat = glm::scale(sMat, glm::vec3(0.5f)); // dsphere = 0.5
    glm::mat4 sMvp = pMat * vMat * sMat;

    glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(sMvp));
    glUniformMatrix4fv(mLoc, 1, GL_FALSE, glm::value_ptr(sMat));

    // Rajzolás (A generált gömb vertexeinek száma)
    glDrawArrays(GL_TRIANGLES, 0, (GLsizei)sphereVertices.size() / 8);
}

int main(void) {
    if (!glfwInit()) exit(EXIT_FAILURE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    GLFWwindow* window = glfwCreateWindow(800, 800, "Beadando_3", NULL, NULL);
    cout << "Keyboard control" << endl;
    cout << "ESC\t\t kilepes" << endl;
    cout << "KEY_LEFT\t Kamera balra" << endl;
    cout << "KEY_RIGHT\t Kamera jobbra" << endl;
    cout << "KEY_UP\t\t Kamera fel" << endl;
    cout << "KEY_DOWN\t Kamera le" << endl;
    cout << "KEY_L\t\t Vilagitas kikapcsolasa" << endl;
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, keyCallback);
    glewInit();
    glfwSwapInterval(1);
    init(window);
    while (!glfwWindowShouldClose(window)) {
        display(window, glfwGetTime());
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}