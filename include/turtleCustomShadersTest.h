/* openGL implementation of turtle and the turtletools module
features:
adjustable pen (size and colour)
resizable window support
keyboard and mouse presses
*/

#include <math.h>
#include "glad.h"
#include "glfw3.h"
#include "list.h"

typedef struct {
    GLFWwindow* window; // the window
    char close;
    list_t *keyPressed; // global keyPressed and mousePressed list
    int *screenbounds; // list of screen bounds (pixels)
    int *lastscreenbounds; // list of screen bounds last frame
    int *initscreenbounds; // screenbounds at initialisation
    int *bounds; // list of coordinate bounds (minX, minY, maxX, maxY)
    double mouseX; // mouseX and mouseY variables
    double mouseY;
    double scrollY;
    double mouseScaX;
    double mouseScaY;
    double mouseAbsX;
    double mouseAbsY;
    list_t *penPos; // a list of where to draw
    double altSum; // a first line of defense to check if anything has changed frame by frame (unused)
    list_t *penPosOld; // this list syncs with penPos every frame and if nothing changes from one frame to the next, the screen is not redrawn
    double x; // x and y position of the turtle
    double y;
    char pen; // pen status (1 for down, 0 for up)
    char penshape; // 0 for circle, 1 for square, 2 for triangle
    double circleprez; // how precise circles are (specifically, the number of sides of a circle with diameter e)
    double pensize; // turtle pen size
    double penr;
    double peng;
    double penb;
    double pena;
} turtle; // all globals are conSTRUCTed here

turtle turtools; // the global turtle struct

const char *turtleVertexShaderSource =
    "#version 330 core\n"
    "layout(location = 0) in vec2 vPosition;\n"
    "layout(location = 1) in vec4 vColor;\n"
    "out vec4 shadeColor;\n"
    "void main() {\n"
    "   shadeColor = vColor;\n"
    "   gl_Position = vec4(vPosition, 0.0, 1.0);\n"
    "}\0";
const char *turtleFragmentShaderSource = 
    "#version 330 core\n"
    "out vec4 color;\n"
    "in vec4 shadeColor;\n;"
    "void main() {\n"
    "   color = vec4(shadeColor);\n"
    "}\0";
void turtleSetWorldCoordinates(int minX, int minY, int maxX, int maxY) { // run this to set the bounds of the window in coordinates
    glfwGetWindowSize(turtools.window, &turtools.screenbounds[0], &turtools.screenbounds[1]);
    turtools.initscreenbounds[0] = turtools.screenbounds[0];
    turtools.initscreenbounds[1] = turtools.screenbounds[1];
    turtools.bounds[0] = minX;
    turtools.bounds[1] = minY;
    turtools.bounds[2] = maxX;
    turtools.bounds[3] = maxY;
}
void keySense(GLFWwindow* window, int key, int scancode, int action, int mods) { // detect key presses
    if (action == GLFW_PRESS) {
        list_append(turtools.keyPressed, (unitype) key, 'i');
    }
    if (action == GLFW_RELEASE) {
        list_remove(turtools.keyPressed, (unitype) key, 'i');
    }
}
void mouseSense(GLFWwindow* window, int button, int action, int mods) { // detect mouse clicks
    if (action == GLFW_PRESS) {
        switch(button) {
            case GLFW_MOUSE_BUTTON_LEFT:
            list_append(turtools.keyPressed, (unitype) "m1", 's');
            break;
            case GLFW_MOUSE_BUTTON_RIGHT:
            list_append(turtools.keyPressed, (unitype) "m2", 's');
            break;
            case GLFW_MOUSE_BUTTON_MIDDLE:
            list_append(turtools.keyPressed, (unitype) "m3", 's');
            break;
        }
    }
    if (action == GLFW_RELEASE) {
        switch(button) {
            case GLFW_MOUSE_BUTTON_LEFT:
            list_remove(turtools.keyPressed, (unitype) "m1", 's');
            break;
            case GLFW_MOUSE_BUTTON_RIGHT:
            list_remove(turtools.keyPressed, (unitype) "m2", 's');
            break;
            case GLFW_MOUSE_BUTTON_MIDDLE:
            list_remove(turtools.keyPressed, (unitype) "m3", 's');
            break;
        }
    }
}
void scrollSense(GLFWwindow* window, double xoffset, double yoffset) {
    turtools.scrollY = yoffset;
}
double turtleMouseWheel() { // the behavior with the mouse wheel is different since it can't be "on" or "off"
    double temp = turtools.scrollY;
    turtools.scrollY = 0;
    return temp;
}
char turtleKeyPressed(int key) { // top level boolean output call to check if the key with code [key] is currently being held down
    return list_count(turtools.keyPressed, (unitype) key, 'c');
}
char turtleMouseDown() { // top level boolean output call to check if the left click button is currently being held down
    return list_count(turtools.keyPressed, (unitype) "m1", 's');
}
char turtleMouseRight() { // top level boolean output call to check if the right click button is currently being held down
    return list_count(turtools.keyPressed, (unitype) "m2", 's');
}
char turtleMouseMiddle() { // top level boolean output call to check if the middle mouse button is currently being held down
    return list_count(turtools.keyPressed, (unitype) "m3", 's');
}
char turtleMouseMid() { // alternate duplicate of above
    return list_count(turtools.keyPressed, (unitype) "m3", 's');
}
void turtoolsInit(GLFWwindow* window, int minX, int minY, int maxX, int maxY) { // initializes the turtletools module
    glfwMakeContextCurrent(window); // various glfw things
    glEnable(GL_MULTISAMPLE); // enable multisampling (if it wasn't already)
    glEnable(GL_ALPHA); // enable pen transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA);
    
    /* set up shaders */
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void *) 0); // position attribute
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void *) (2 * sizeof(float))); // color attribute

    int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &turtleVertexShaderSource, NULL);
    glCompileShader(vertexShader);
    char errorMessage[512];
    int success;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, errorMessage);
        printf("Error compiling vertex shader\n");
        printf("%s\n", errorMessage);
    }
    int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &turtleFragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, errorMessage);
        printf("Error compiling fragment shader\n");
        printf("%s\n", errorMessage);
    }
    int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shaderProgram, 512, NULL, errorMessage);
        printf("Error linking shaders\n");
        printf("%s\n", errorMessage);
    }
    
    glUseProgram(shaderProgram);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    glDeleteProgram(shaderProgram);

    glClearColor(1.0, 1.0, 1.0, 0.0); // white background by default
    turtools.window = window;
    turtools.close = 0;
    turtools.keyPressed = list_init();
    turtools.screenbounds = malloc(sizeof(int) * 2);
    turtools.lastscreenbounds = malloc(sizeof(int) * 2);
    turtools.lastscreenbounds[0] = 0;
    turtools.lastscreenbounds[1] = 0;
    turtools.initscreenbounds = malloc(sizeof(int) * 2);
    turtools.bounds = malloc(sizeof(int) * 4);
    turtools.penPos = list_init();
    turtools.penPosOld = list_init();
    turtools.x = 0;
    turtools.y = 0;
    turtools.pensize = 1;
    turtools.penshape = 0;
    turtools.circleprez = 9;
    turtools.pen = 0;
    turtools.penr = 0.0;
    turtools.peng = 0.0;
    turtools.penb = 0.0;
    turtools.pena = 0.0;
    turtleSetWorldCoordinates(-240, -180, 240, 180);
    glfwSetKeyCallback(window, keySense); // initiate mouse and keyboard detection
    glfwSetMouseButtonCallback(window, mouseSense);
    glfwSetScrollCallback(window, scrollSense);
}
void turtleGetMouseCoords() { // gets the mouse coordinates
    glfwGetWindowSize(turtools.window, &turtools.screenbounds[0], &turtools.screenbounds[1]); // get screenbounds
    glfwGetCursorPos(turtools.window, &turtools.mouseAbsX, &turtools.mouseAbsY); // get mouse positions (absolute)
    turtools.mouseX = turtools.mouseAbsX;
    turtools.mouseY = turtools.mouseAbsY;
    turtools.mouseScaX = turtools.mouseAbsX;
    turtools.mouseScaY = turtools.mouseAbsY;
    turtools.mouseX -= (turtools.initscreenbounds[0] >> 1) - ((turtools.bounds[2] + turtools.bounds[0]) >> 1);
    turtools.mouseX *= ((double) (turtools.bounds[2] - turtools.bounds[0]) / (double) turtools.initscreenbounds[0]);
    turtools.mouseY -= (turtools.initscreenbounds[1] >> 1) - ((turtools.bounds[3] + turtools.bounds[1]) >> 1) + (turtools.screenbounds[1] - turtools.initscreenbounds[1]);
    turtools.mouseY *= ((double) (turtools.bounds[1] - turtools.bounds[3]) / (double) turtools.initscreenbounds[1]);
    turtools.mouseScaX -= (turtools.screenbounds[0] >> 1) - ((turtools.bounds[2] + turtools.bounds[0]) >> 1);
    turtools.mouseScaX *= ((double) (turtools.bounds[2] - turtools.bounds[0]) / (double) turtools.screenbounds[0]);
    turtools.mouseScaY -= (turtools.screenbounds[1] >> 1) - ((turtools.bounds[3] + turtools.bounds[1]) >> 1);
    turtools.mouseScaY *= ((double) (turtools.bounds[1] - turtools.bounds[3]) / (double) turtools.screenbounds[1]);
}
void turtleBgColor(double r, double g, double b) { // set the background color
    glClearColor(r / 255, g / 255, b / 255, 0.0);
}
void turtlePenColor(double r, double g, double b) { // set the pen color
    turtools.penr = r / 255;
    turtools.peng = g / 255;
    turtools.penb = b / 255;
    turtools.pena = 0.0;
}
void turtlePenColorAlpha(double r, double g, double b, double a) { // set the pen color (with transparency)
    turtools.penr = r / 255;
    turtools.peng = g / 255;
    turtools.penb = b / 255;
    turtools.pena = a / 255;
}
void turtlePenSize(double size) {
    turtools.pensize = size * 0.25;
}
void turtleClear() { // clears all the pen drawings
    list_free(turtools.penPos);
    turtools.penPos = list_init();
}
void turtlePenDown() {
    if (turtools.pen == 0) {
        turtools.pen = 1;
        char changed = 0;
        int len = turtools.penPos -> length;
        if (len > 0) {
            unitype *ren = turtools.penPos -> data;
            if (ren[len - 9].d != turtools.x) {changed = 1;}
            if (ren[len - 8].d != turtools.y) {changed = 1;}
            if (ren[len - 7].d != turtools.pensize) {changed = 1;}
            if (ren[len - 6].d != turtools.penr) {changed = 1;}
            if (ren[len - 5].d != turtools.peng) {changed = 1;}
            if (ren[len - 4].d != turtools.penb) {changed = 1;}
            if (ren[len - 3].d != turtools.pena) {changed = 1;}
            if (ren[len - 2].h != turtools.penshape) {changed = 1;}
            if (ren[len - 1].d != turtools.circleprez) {changed = 1;}
        } else {
            changed = 1;
        }
        if (changed == 1) {
            list_append(turtools.penPos, (unitype) turtools.x, 'd');
            list_append(turtools.penPos, (unitype) turtools.y, 'd');
            list_append(turtools.penPos, (unitype) turtools.pensize, 'd');
            list_append(turtools.penPos, (unitype) turtools.penr, 'd');
            list_append(turtools.penPos, (unitype) turtools.peng, 'd');
            list_append(turtools.penPos, (unitype) turtools.penb, 'd');
            list_append(turtools.penPos, (unitype) turtools.pena, 'd');
            list_append(turtools.penPos, (unitype) turtools.penshape, 'h');
            list_append(turtools.penPos, (unitype) turtools.circleprez, 'd');
        }
    }
}
void turtlePenUp() {
    if (turtools.pen == 1) {
        turtools.pen = 0;
        if (turtools.penPos -> length > 0 && turtools.penPos -> type[turtools.penPos -> length - 1] != 'c') {
            list_append(turtools.penPos, (unitype) 0, 'c');
            list_append(turtools.penPos, (unitype) 0, 'c');
            list_append(turtools.penPos, (unitype) 0, 'c');
            list_append(turtools.penPos, (unitype) 0, 'c');
            list_append(turtools.penPos, (unitype) 0, 'c');
            list_append(turtools.penPos, (unitype) 0, 'c');
            list_append(turtools.penPos, (unitype) 0, 'c');
            list_append(turtools.penPos, (unitype) 0, 'c');
            list_append(turtools.penPos, (unitype) 0, 'c');
        }
    }
}
void turtlePenShape(char *selection) {
    if (strcmp(selection, "circle") == 0 || strcmp(selection, "Circle") == 0) {
        turtools.penshape = 0;
    }
    if (strcmp(selection, "square") == 0 || strcmp(selection, "Square") == 0) {
        turtools.penshape = 1;
    }
    if (strcmp(selection, "triangle") == 0 || strcmp(selection, "Triangle") == 0) {
        turtools.penshape = 2;
    }
    if (strcmp(selection, "none") == 0 || strcmp(selection, "None") == 0) {
        turtools.penshape = 3;
    }
    if (strcmp(selection, "connected") == 0 || strcmp(selection, "Connected") == 0) {
        turtools.penshape = 4;
    }
}
void turtlePenPrez(double prez) {
    turtools.circleprez = prez;
}
void turtleGoto(double x, double y) { // moves the turtle to a coordinate
    if (fabs(turtools.x - x) > 0.01 || fabs(turtools.y - y) > 0.01) {
        turtools.x = x;
        turtools.y = y;
        if (turtools.pen == 1) {
            char changed = 0;
            int len = turtools.penPos -> length;
            if (len > 0) {
                unitype *ren = turtools.penPos -> data;
                if (ren[len - 9].d != turtools.x) {changed = 1;}
                if (ren[len - 8].d != turtools.y) {changed = 1;}
                if (ren[len - 7].d != turtools.pensize) {changed = 1;}
                if (ren[len - 6].d != turtools.penr) {changed = 1;}
                if (ren[len - 5].d != turtools.peng) {changed = 1;}
                if (ren[len - 4].d != turtools.penb) {changed = 1;}
                if (ren[len - 3].d != turtools.pena) {changed = 1;}
                if (ren[len - 2].h != turtools.penshape) {changed = 1;}
                if (ren[len - 1].d != turtools.circleprez) {changed = 1;}
            } else {
                changed = 1;
            }
            if (changed == 1) {
                list_append(turtools.penPos, (unitype) x, 'd');
                list_append(turtools.penPos, (unitype) y, 'd');
                list_append(turtools.penPos, (unitype) turtools.pensize, 'd');
                list_append(turtools.penPos, (unitype) turtools.penr, 'd');
                list_append(turtools.penPos, (unitype) turtools.peng, 'd');
                list_append(turtools.penPos, (unitype) turtools.penb, 'd');
                list_append(turtools.penPos, (unitype) turtools.pena, 'd');
                list_append(turtools.penPos, (unitype) turtools.penshape, 'h');
                list_append(turtools.penPos, (unitype) turtools.circleprez, 'd');
            }
        }
    }
}
int turtleCircle(float *buffer, int start, double x, double y, double rad, double r, double g, double b, double a, double xfact, double yfact, double prez) { // draws a circle at the specified x and y (coordinates)
    int indprez = (int) prez * 6;
    for (int i = 0; i < indprez; i += 6) {
        buffer[i + start] = (x + rad * sin(1.0 / 3 * i * M_PI / prez)) * xfact;
        buffer[i + 1 + start] = (y + rad * cos(1.0 / 3 * i * M_PI / prez)) * yfact;
        buffer[i + 2 + start] = r;
        buffer[i + 3 + start] = g;
        buffer[i + 4 + start] = b;
        buffer[i + 5 + start] = a;
    }
    return start + indprez;
}
void turtleSquare(double x1, double y1, double x2, double y2, double r, double g, double b, double a, double xfact, double yfact) { // draws a square
    // float square[24] = {x1 * xfact, y1 * yfact, r, g, b, a, x2 * xfact, y1 * yfact, r, g, b, a, x2 * xfact, y2 * yfact, r, g, b, a, x1 * xfact, y2 * yfact, r, g, b, a};
    // glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 24, square, GL_STATIC_DRAW);
    // glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}
void turtleTriangle(double x1, double y1, double x2, double y2, double x3, double y3, double r, double g, double b, double a, double xfact, double yfact) { // draws a triangle
    // float triangle[18] = {x1 * xfact, y1 * yfact, r, g, b, a, x2 * xfact, y2 * yfact, r, g, b, a, x3 * xfact, y3 * yfact, r, g, b, a};
    // glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 18, triangle, GL_STATIC_DRAW);
    // glDrawArrays(GL_TRIANGLES, 0, 3);
}
int turtleQuad(float *buffer, int start, double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4, double r, double g, double b, double a, double xfact, double yfact) { // draws a quadrilateral
    buffer[start] = x1 * xfact;
    buffer[start + 1] = y1 * yfact;
    buffer[start + 2] = r;
    buffer[start + 3] = g;
    buffer[start + 4] = b;
    buffer[start + 5] = a;
    buffer[start + 6] = x2 * xfact;
    buffer[start + 7] = y2 * yfact;
    buffer[start + 8] = r;
    buffer[start + 9] = g;
    buffer[start + 10] = b;
    buffer[start + 11] = a;
    buffer[start + 12] = x3 * xfact;
    buffer[start + 13] = y3 * yfact;
    buffer[start + 14] = r;
    buffer[start + 15] = g;
    buffer[start + 16] = b;
    buffer[start + 17] = a;
    buffer[start + 18] = x4 * xfact;
    buffer[start + 19] = y4 * yfact;
    buffer[start + 20] = r;
    buffer[start + 21] = g;
    buffer[start + 22] = b;
    buffer[start + 23] = a;
    return start + 24;
}
void turtleUpdate() { // draws the turtle's path on the screen
    char changed = 0;
    int len = turtools.penPos -> length;
    unitype *ren = turtools.penPos -> data;
    char *renType = turtools.penPos -> type;
    unitype *lastFrame = turtools.penPosOld -> data;
    if (len != turtools.penPosOld -> length) {
        changed = 1;
    } else {
        for (int i = 0; i < len && changed == 0; i++) {
            switch (renType[i]) {
                case 'd':
                if (ren[i].d != lastFrame[i].d) {changed = 1;}
                break;
                case 'h':
                if (ren[i].h != lastFrame[i].h) {changed = 1;}
                break;
                case 'c':
                if (ren[i].c != lastFrame[i].c) {changed = 1;}
                break;
            }
        }
    }
    glfwGetWindowSize(turtools.window, &turtools.screenbounds[0], &turtools.screenbounds[1]);
    if (turtools.lastscreenbounds[0] != turtools.screenbounds[0] || turtools.lastscreenbounds[1] != turtools.screenbounds[1]) {
        changed = 1;
    }
    turtools.lastscreenbounds[0] = turtools.screenbounds[0];
    turtools.lastscreenbounds[1] = turtools.screenbounds[1];
    list_copy(turtools.penPos, turtools.penPosOld); // unideal
    if (changed == 1) { // only redraw the screen if there have been any changes from last frame
        double xfact = (turtools.bounds[2] - turtools.bounds[0]) >> 1;
        double yfact = (turtools.bounds[3] - turtools.bounds[1]) >> 1;
        xfact = 1 / xfact;
        yfact = 1 / yfact;
        double lastSize = -1;
        double lastPrez = -1;
        double precomputedLog = 5;
        float *masterBuffer = malloc(sizeof(float) * 6 * len);
        int start = 0;
        glClear(GL_COLOR_BUFFER_BIT);
        for (int i = 0; i < len; i += 9) {
            if (renType[i] == 'd') {
                if (ren[i + 7].h == 0) {
                    if (!(lastSize == ren[i + 2].d) || !(lastPrez != ren[i + 8].d))
                        precomputedLog = ren[i + 8].d * log(1 + ren[i + 2].d);
                    lastSize = ren[i + 2].d;
                    lastPrez = ren[i + 8].d;
                    start = turtleCircle(masterBuffer, start, ren[i].d, ren[i + 1].d, ren[i + 2].d, ren[i + 3].d, ren[i + 4].d, ren[i + 5].d, ren[i + 6].d, xfact, yfact, precomputedLog);
                }
                if (ren[i + 7].h == 1)
                    turtleSquare(ren[i].d - ren[i + 2].d, ren[i + 1].d - ren[i + 2].d, ren[i].d + ren[i + 2].d, ren[i + 1].d + ren[i + 2].d, ren[i + 3].d, ren[i + 4].d, ren[i + 5].d, ren[i + 6].d, xfact, yfact);
                if (ren[i + 7].h == 2)
                    turtleTriangle(ren[i].d - ren[i + 2].d, ren[i + 1].d - ren[i + 2].d, ren[i].d + ren[i + 2].d, ren[i + 1].d - ren[i + 2].d, ren[i].d, ren[i + 1].d + ren[i + 2].d, ren[i + 3].d, ren[i + 4].d, ren[i + 5].d, ren[i + 6].d, xfact, yfact);
                if (i + 9 < len && renType[i + 9] == 'd' && (ren[i + 7].h == 4 || (fabs(ren[i].d - ren[i + 9].d) > ren[i + 2].d / 2 || fabs(ren[i + 1].d - ren[i + 10].d) > ren[i + 2].d / 2))) { // tests for next point continuity and also ensures that the next point is at sufficiently different coordinates
                    double dir = atan((ren[i + 9].d - ren[i].d) / (ren[i + 1].d - ren[i + 10].d));
                    double sinn = sin(dir + M_PI / 2);
                    double coss = cos(dir + M_PI / 2);
                    start = turtleQuad(masterBuffer, start, ren[i].d + ren[i + 2].d * sinn, ren[i + 1].d - ren[i + 2].d * coss, ren[i + 9].d + ren[i + 2].d * sinn, ren[i + 10].d - ren[i + 2].d * coss, ren[i + 9].d - ren[i + 2].d * sinn, ren[i + 10].d + ren[i + 2].d * coss, ren[i].d - ren[i + 2].d * sinn, ren[i + 1].d + ren[i + 2].d * coss, ren[i + 3].d, ren[i + 4].d, ren[i + 5].d, ren[i + 6].d, xfact, yfact);
                    if (ren[i + 7].h == 4 && i + 18 < len && renType[i + 18] == 'd') {
                        double dir2 = atan((ren[i + 18].d - ren[i + 9].d) / (ren[i + 10].d - ren[i + 19].d));
                        double sinn2 = sin(dir2 + M_PI / 2);
                        double coss2 = cos(dir2 + M_PI / 2);
                        turtleTriangle(ren[i + 9].d + ren[i + 2].d * sinn, ren[i + 10].d - ren[i + 2].d * coss, ren[i + 9].d - ren[i + 2].d * sinn, ren[i + 10].d + ren[i + 2].d * coss, ren[i + 9].d + ren[i + 11].d * sinn2, ren[i + 10].d - ren[i + 11].d * coss2, ren[i + 3].d, ren[i + 4].d, ren[i + 5].d, ren[i + 6].d, xfact, yfact);
                        turtleTriangle(ren[i + 9].d + ren[i + 2].d * sinn, ren[i + 10].d - ren[i + 2].d * coss, ren[i + 9].d - ren[i + 2].d * sinn, ren[i + 10].d + ren[i + 2].d * coss, ren[i + 9].d - ren[i + 11].d * sinn2, ren[i + 10].d + ren[i + 11].d * coss2, ren[i + 3].d, ren[i + 4].d, ren[i + 5].d, ren[i + 6].d, xfact, yfact);
                    }
                } else {
                    if (ren[i + 7].h == 4 && i > 8 && renType[i - 8] == 'c') {
                        if (!(lastSize == ren[i + 2].d) || !(lastPrez != ren[i + 8].d))
                            precomputedLog = ren[i + 8].d * log(1 + ren[i + 2].d);
                        lastSize = ren[i + 2].d;
                        lastPrez = ren[i + 8].d;
                        start = turtleCircle(masterBuffer, start, ren[i].d, ren[i + 1].d, ren[i + 2].d, ren[i + 3].d, ren[i + 4].d, ren[i + 5].d, ren[i + 6].d, xfact, yfact, precomputedLog);
                    }
                }
                glBufferData(GL_ARRAY_BUFFER, sizeof(float) * start, masterBuffer, GL_DYNAMIC_DRAW);
                glDrawArrays(GL_TRIANGLE_FAN, 0, start / 6);
                start = 0;
            }
        }
        glfwSwapBuffers(turtools.window);
    }
    glfwPollEvents();
    if (glfwWindowShouldClose(turtools.window)) {
        turtools.close = 1;
        glfwTerminate();
    }
}
void turtleMainLoop() { // keeps the window open
    while (turtools.close == 0) {
        turtleUpdate();
    }
}