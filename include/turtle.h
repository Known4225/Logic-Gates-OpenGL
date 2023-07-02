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
    double currentColor[4]; // for reducing API color calls
} turtle; // all globals are conSTRUCTed here

turtle turtools;

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
    gladLoadGL();
    glfwMakeContextCurrent(window); // various glfw things
    glEnable(GL_ALPHA);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA);
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
    turtools.circleprez = 9; // default circleprez value
    turtools.pen = 0;
    turtools.penr = 0.0;
    turtools.peng = 0.0;
    turtools.penb = 0.0;
    turtools.pena = 0.0;
    for (int i = 0; i < 3; i++) {
        turtools.currentColor[i] = 0.0;
    }
    turtools.currentColor[3] = 1.0;
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
void turtleCircle(double x, double y, double rad, double r, double g, double b, double a, double xfact, double yfact, double prez) { // draws a circle at the specified x and y (coordinates)
    char colorChange = 0;
    if (r != turtools.currentColor[0]) {colorChange = 1;}
    if (g != turtools.currentColor[1]) {colorChange = 1;}
    if (b != turtools.currentColor[2]) {colorChange = 1;}
    if (a != turtools.currentColor[3]) {colorChange = 1;}
    if (colorChange == 1) {
        glColor4d(r, g, b, a);
        turtools.currentColor[0] = r;
        turtools.currentColor[1] = g;
        turtools.currentColor[2] = b;
        turtools.currentColor[3] = a;
    }
    glBegin(GL_TRIANGLE_FAN);
    for (double i = 0; i < prez; i++) {
        glVertex2d((x + rad * sin(2 * i * M_PI / prez)) * xfact, (y + rad * cos(2 * i * M_PI / prez)) * yfact);
    }
    glEnd();
}
void turtleSquare(double x1, double y1, double x2, double y2, double r, double g, double b, double a, double xfact, double yfact) { // draws a square
    char colorChange = 0;
    if (r != turtools.currentColor[0]) {colorChange = 1;}
    if (g != turtools.currentColor[1]) {colorChange = 1;}
    if (b != turtools.currentColor[2]) {colorChange = 1;}
    if (a != turtools.currentColor[3]) {colorChange = 1;}
    if (colorChange == 1) {
        glColor4d(r, g, b, a);
        turtools.currentColor[0] = r;
        turtools.currentColor[1] = g;
        turtools.currentColor[2] = b;
        turtools.currentColor[3] = a;
    }
    glBegin(GL_TRIANGLE_FAN);
    glVertex2d(x1 * xfact, y1 * yfact);
    glVertex2d(x2 * xfact, y1 * yfact);
    glVertex2d(x2 * xfact, y2 * yfact);
    glVertex2d(x1 * xfact, y2 * yfact);
    glEnd();
}
void turtleTriangle(double x1, double y1, double x2, double y2, double x3, double y3, double r, double g, double b, double a, double xfact, double yfact) { // draws a triangle
    char colorChange = 0;
    if (r != turtools.currentColor[0]) {colorChange = 1;}
    if (g != turtools.currentColor[1]) {colorChange = 1;}
    if (b != turtools.currentColor[2]) {colorChange = 1;}
    if (a != turtools.currentColor[3]) {colorChange = 1;}
    if (colorChange == 1) {
        glColor4d(r, g, b, a);
        turtools.currentColor[0] = r;
        turtools.currentColor[1] = g;
        turtools.currentColor[2] = b;
        turtools.currentColor[3] = a;
    }
    glBegin(GL_TRIANGLE_FAN);
    glVertex2d(x1 * xfact, y1 * yfact);
    glVertex2d(x2 * xfact, y2 * yfact);
    glVertex2d(x3 * xfact, y3 * yfact);
    glEnd();
}
void turtleQuad(double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4, double r, double g, double b, double a, double xfact, double yfact) { // draws a quadrilateral
    char colorChange = 0;
    if (r != turtools.currentColor[0]) {colorChange = 1;}
    if (g != turtools.currentColor[1]) {colorChange = 1;}
    if (b != turtools.currentColor[2]) {colorChange = 1;}
    if (a != turtools.currentColor[3]) {colorChange = 1;}
    if (colorChange == 1) {
        glColor4d(r, g, b, a);
        turtools.currentColor[0] = r;
        turtools.currentColor[1] = g;
        turtools.currentColor[2] = b;
        turtools.currentColor[3] = a;
    }
    glBegin(GL_TRIANGLE_FAN);
    glVertex2d(x1 * xfact, y1 * yfact);
    glVertex2d(x2 * xfact, y2 * yfact);
    glVertex2d(x3 * xfact, y3 * yfact);
    glVertex2d(x4 * xfact, y4 * yfact);
    glEnd();
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
        glClear(GL_COLOR_BUFFER_BIT);
        for (int i = 0; i < len; i += 9) {
            if (renType[i] == 'd') {
                if (ren[i + 7].h == 0) {
                    if (!(lastSize == ren[i + 2].d) || !(lastPrez != ren[i + 8].d))
                        precomputedLog = ren[i + 8].d * log(1 + ren[i + 2].d);
                    lastSize = ren[i + 2].d;
                    lastPrez = ren[i + 8].d;
                    turtleCircle(ren[i].d, ren[i + 1].d, ren[i + 2].d, ren[i + 3].d, ren[i + 4].d, ren[i + 5].d, ren[i + 6].d, xfact, yfact, precomputedLog);
                }
                if (ren[i + 7].h == 1)
                    turtleSquare(ren[i].d - ren[i + 2].d, ren[i + 1].d - ren[i + 2].d, ren[i].d + ren[i + 2].d, ren[i + 1].d + ren[i + 2].d, ren[i + 3].d, ren[i + 4].d, ren[i + 5].d, ren[i + 6].d, xfact, yfact);
                if (ren[i + 7].h == 2)
                    turtleTriangle(ren[i].d - ren[i + 2].d, ren[i + 1].d - ren[i + 2].d, ren[i].d + ren[i + 2].d, ren[i + 1].d - ren[i + 2].d, ren[i].d, ren[i + 1].d + ren[i + 2].d, ren[i + 3].d, ren[i + 4].d, ren[i + 5].d, ren[i + 6].d, xfact, yfact);
                if (i + 9 < len && renType[i + 9] == 'd' && (ren[i + 7].h == 4 || (fabs(ren[i].d - ren[i + 9].d) > ren[i + 2].d / 2 || fabs(ren[i + 1].d - ren[i + 10].d) > ren[i + 2].d / 2))) { // tests for next point continuity and also ensures that the next point is at sufficiently different coordinates
                    double dir = atan((ren[i + 9].d - ren[i].d) / (ren[i + 1].d - ren[i + 10].d));
                    double sinn = sin(dir + M_PI / 2);
                    double coss = cos(dir + M_PI / 2);
                    turtleQuad(ren[i].d + ren[i + 2].d * sinn, ren[i + 1].d - ren[i + 2].d * coss, ren[i + 9].d + ren[i + 2].d * sinn, ren[i + 10].d - ren[i + 2].d * coss, ren[i + 9].d - ren[i + 2].d * sinn, ren[i + 10].d + ren[i + 2].d * coss, ren[i].d - ren[i + 2].d * sinn, ren[i + 1].d + ren[i + 2].d * coss, ren[i + 3].d, ren[i + 4].d, ren[i + 5].d, ren[i + 6].d, xfact, yfact);
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
                        turtleCircle(ren[i].d, ren[i + 1].d, ren[i + 2].d, ren[i + 3].d, ren[i + 4].d, ren[i + 5].d, ren[i + 6].d, xfact, yfact, precomputedLog);
                    }
                }
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
void turtoolsFree() {
    list_free(turtools.keyPressed);
    list_free(turtools.penPos);
    list_free(turtools.penPosOld);
    free(turtools.screenbounds);
    free(turtools.lastscreenbounds);
    free(turtools.initscreenbounds);
    free(turtools.bounds);
}