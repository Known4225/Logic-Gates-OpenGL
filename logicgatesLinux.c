/* select OS */
// #define OS_WINDOWS
#define OS_LINUX

#include "include/ribbon.h"
#include "include/popup.h"
#ifdef OS_WINDOWS
#include "include/win32Tools.h"
#endif
#ifdef OS_LINUX
#include "include/zenityFileDialog.h"
#endif
#include <time.h>
#define STB_IMAGE_IMPLEMENTATION
#include "include/stb_image.h" // THANK YOU https://github.com/nothings/stb

typedef struct { // all logicgates variables (shared state) are defined here
    double globalsize; // size multiplier - bigger means zoomed in
    double themeColors[55]; // rgb colour array, there are 9 colours, accross 2 themes each with 3 components for rgb
    char theme; // this is the index to use in themeColors when determining what colour to render an object in
    char sidebar; // whether sidebar is shown, 0 - hidden, 1 - shown
    char selecting; // i dont know
    char indicators; // idk
    char mouseType; // mouseType?
    char wireMode; // there are three wireModes, 0 is classic, 1 is angular (new style), and 2 is no wire render
    double scrollSpeed; // how fast the scroll zooms in, I think it's a 1.15x
    double rotateSpeed; // how fast the arrow keys rotate components
    int rotateCooldown; // what?
    double mx; // mouseX bounded by window
    double my; // mouseY bounded by window
    double mw; // mouseWheel
    double boundXmin; // bound for mx
    double boundYmin; // bound for my
    double boundXmax; // bound for mx
    double boundYmax; // bound for my
    double scaling; // idk
    double GraphPrez; /* how many segments are drawn in the bezier curves used to construct gates with curves, 
    this should scale with the zoom level but actually i don't think it does that. I should add that */
    char *holding; // oh god
    double holdingAng; // idk
    double wxOffE; // some offset var idk
    double wyOffE; // offset for y
    double screenX; // no idea what the screen variables do
    double screenY;
    double sxmax;
    double sxmin;
    double symax;
    double symin;
    int hlgcomp; // bad design decisions
    int hglmove; // bad naming scheme
    double tempX; // wow tempX that's just great
    double tempY; // these were made at a time when this project was small
    double offX; // another offset, but older
    double offY;
    double FocalX; // these have to do with dragging the screen around
    double FocalY; // they keep track of the mouse position
    double FocalCSX;
    double FocalCSY;
    double selectX; // i believe these are the bounds of the selection box
    double selectX2; // these are likely absolute coordinates, but i dont know
    double selectY;
    double selectY2;
    list_t *compSlots; // a lookup table for which components have two inputs vs one
    char movingItems; // to keep track of movement of components so it can be detected and undone
    char wireHold; // whether you are toggled for wiring (hold space or click the wire symbol)
    int wiringStart; // the component ID of the start of a (under construction) wire
    int wiringEnd; // the component ID of the end of a wire at the moment it is constructed
    
    /* these 6 lists make up the data of the circuit */
    list_t *components; // list of components (1 item for each component, a string with "POWER", "AND", etc), a component's "ID" or "Handle" is that component's index in this list
    list_t *groups; // list of group data (1 element per component, integer represents group ID, -1 is no group. IDs start at 1. 0 is not used)
    list_t *positions; // list of component positions (3 items for each component, doubles specifying x, y, and angle)
    list_t *io; // list of binary inputs and outputs of a component (3 items for each component, 2 inputs followed by the output of the component (either a 0 or 1))
    list_t *inpComp; // list of component ID inputs, 3 items per component, format: number of possible inputs (either 1 or 2), input 1 (ID, 0 if none), input 2 (ID, 0 if none)
    list_t *wiring; // list of component connections (3 items per connection, it goes sender (ID), reciever (ID), powered (0 or 1))
    
    char keys[32];
    list_t *groupSelect; // list of component IDs in the hovered/selected group
    list_t *deleteQueue; // when many components are deleted, they are queued here
    list_t *selected; // list of selected component IDs
    list_t *selectOb; // list of selected component IDs (but transferred to selected a tick later?)
    list_t *copyBuffer; // for ctrl+c and ctrl+v
    list_t *undoBuffer; // list of lists containing the state of the program after every undoable action
    int undoIndex; // what position in the undoBuffer are we at
    double sinRot;
    double cosRot;
    char defaultShape; // having to do with the penshape
    double defaultPrez; // having to do with the circle triangle precision
    double specialPrez; // having to do with special cases where more circle precision is necessary
} logicgates;
void init(logicgates *selfp) { // initialises the logicgates variabes (shared state)
    logicgates self = *selfp;
    self.globalsize = 1.5;
    double themes[55] = {0,
    /* light theme */ 
    0, 0, 0, // component color
    195, 195, 195, // selection box color
    255, 0, 0, // powered color
    255, 146, 146, // powered selected
    230, 230, 230, // sidebar color
    95, 95, 95, // selected component (sidebar) color
    255, 234, 0, // receiving power color (POWER component)
    255, 248, 181, // receive power color selected
    255, 255, 255, // background color
    
    /* dark theme */
    0, 0, 0, // component color
    40, 40, 40, // selection box color
    200, 200, 200, // powered color
    190, 190, 190, // powered selected
    50, 50, 50, // sidebar color
    40, 40, 40, // selected component (sidebar) color
    19, 236, 48, // receiving power color (POWER component)
    116, 255, 133, // receive power color selected
    60, 60, 60 // background color
    };
    memcpy(self.themeColors, themes, sizeof(self.themeColors));
    self.theme = 27;
    if (self.theme == 27) // for testing dark mode default
        ribbonDarkTheme();
    turtleBgColor(self.themeColors[25 + self.theme], self.themeColors[26 + self.theme], self.themeColors[27 + self.theme]);
    self.scrollSpeed = 1.15;
    self.rotateSpeed = 1;
    self.rotateCooldown = 1;
    self.mx = 0;
    self.my = 0;
    self.scaling = 2;
    self.sidebar = 1;
    self.GraphPrez = 12;
    self.holding = "a"; // in hindsight this should have been an int
    self.holdingAng = 90;
    self.indicators = 1;
    self.mouseType = 0;
    self.wxOffE = 0;
    self.wyOffE = 0;
    self.wireMode = 1;
    self.screenX = 0;
    self.screenY = 0;
    self.sxmax = 0;
    self.sxmin = 0;
    self.symax = 0;
    self.symin = 0;
    self.selecting = 0;
    self.hlgcomp = 0;
    self.hglmove = 0;
    self.tempX = 0;
    self.tempY = 0;
    self.offX = 0;
    self.offY = 0;
    self.FocalX = 0;
    self.FocalY = 0;
    self.FocalCSX = 0;
    self.FocalCSY = 0;
    self.selectX = 0;
    self.selectX2 = 0;
    self.selectY = 0;
    self.selectY2 = 0;
    self.movingItems = 0;
    self.wireHold = 0;
    self.wiringStart = 0;
    self.wiringEnd = 0;
    self.compSlots = list_init();
    list_append(self.compSlots, (unitype) "null", 's');
    list_append(self.compSlots, (unitype) "POWER", 's');
    list_append(self.compSlots, (unitype) 1, 'i');
    list_append(self.compSlots, (unitype) "NOT", 's');
    list_append(self.compSlots, (unitype) 1, 'i');
    list_append(self.compSlots, (unitype) "AND", 's');
    list_append(self.compSlots, (unitype) 2, 'i');
    list_append(self.compSlots, (unitype) "OR", 's');
    list_append(self.compSlots, (unitype) 2, 'i');
    list_append(self.compSlots, (unitype) "XOR", 's');
    list_append(self.compSlots, (unitype) 2, 'i');
    list_append(self.compSlots, (unitype) "NOR", 's');
    list_append(self.compSlots, (unitype) 2, 'i');
    list_append(self.compSlots, (unitype) "NAND", 's');
    list_append(self.compSlots, (unitype) 2, 'i');
    list_append(self.compSlots, (unitype) "BUFFER", 's');
    list_append(self.compSlots, (unitype) 1, 'i');
    
    self.components = list_init();
    list_append(self.components, (unitype) "null", 's'); // they start with an 'n' char or "null" string so they are 1-indexed not 0-indexed because I'm a bad coder
    self.groups = list_init();
    list_append(self.groups, (unitype) 'n', 'c');
    self.positions = list_init();
    list_append(self.positions, (unitype) 'n', 'c');
    self.io = list_init();
    list_append(self.io, (unitype) 'n', 'c');
    self.inpComp = list_init();
    list_append(self.inpComp, (unitype) 'n', 'c');
    self.wiring = list_init();
    list_append(self.wiring, (unitype) 'n', 'c');
    

    for (int i = 0; i < 32; i++) {
        self.keys[i] = 0;
    }
    self.groupSelect = list_init();
    list_append(self.groupSelect, (unitype) 'n', 'c');
    self.deleteQueue = list_init();
    list_append(self.deleteQueue, (unitype) 'n', 'c');
    self.selected = list_init();
    list_append(self.selected, (unitype) "null", 's');
    self.selectOb = list_init();
    list_append(self.selectOb, (unitype) "null", 's');
    self.copyBuffer = list_init();
    list_append(self.copyBuffer, (unitype) 'n', 'c');
    for (int i = 1; i < 7; i++) {
        list_append(self.copyBuffer, (unitype) list_init(), 'r');
        list_append(self.copyBuffer -> data[i].r, (unitype) 'n', 'c');
    }
    self.undoBuffer = list_init();
    list_append(self.undoBuffer, (unitype) 'n', 'c');
    self.undoIndex = 0;
    self.sinRot = 0;
    self.cosRot = 0;
    self.defaultShape = 0; // 0 for circle (pretty), 3 for none (fastest), basically 0 is prettiest 3 is fastest, everything between is a spectrum
    self.defaultPrez = 5; // normal use doesn't need super precise circles
    self.specialPrez = 9; // in special cases such as the power block and ends of NOT blocks require more precise circles
    *selfp = self;
}
void clearAll(logicgates *selfp) { // clears the stage
    logicgates self = *selfp;
    list_clear(self.components);
    list_append(self.components, (unitype) "null", 's');
    list_clear(self.groups);
    list_append(self.groups, (unitype) 'n', 'c');
    list_clear(self.positions);
    list_append(self.positions, (unitype) 'n', 'c');
    list_clear(self.io);
    list_append(self.io, (unitype) 'n', 'c');
    list_clear(self.inpComp);
    list_append(self.inpComp, (unitype) 'n', 'c');
    list_clear(self.wiring);
    list_append(self.wiring, (unitype) 'n', 'c');
    *selfp = self;
}
void import(logicgates *selfp, const char *filename) { // imports a file
    logicgates self = *selfp;
    printf("Attempting to load %s\n", filename);
    FILE *file = fopen(filename, "r");
    char str1[20] = "null";
    double doub1;
    int int1;
    int end = 0;
    int num = 0;
    int oldCompLen = self.components -> length - 1;
    while (end != EOF) {
        end = fscanf(file, "%s", str1);
        if (str1[0] == '-') {break;}
        if (str1[0] == '0') {break;}
        if (str1[0] == '1') {break;}
        if (str1[0] == '2') {break;}
        if (str1[0] == '3') {break;}
        if (str1[0] == '4') {break;}
        if (str1[0] == '5') {break;}
        if (str1[0] == '6') {break;}
        if (str1[0] == '7') {break;}
        if (str1[0] == '8') {break;}
        if (str1[0] == '9') {break;}
        num += 1;
    }
    if (end == EOF) {
        printf("%s Bad Read\n", filename);
        fclose(file);
    } else {
        rewind(file);
        for (int i = 0; i < num; i++) {
            end = fscanf(file, "%s", str1);
            if (end == EOF) {
                printf("file corrupted at word %d\n", i + 1);
                return;
            }   
            // parse string (component from group)
            int j = 0;
            char str2[12] = "null";
            while (str1[j] != '\0') {
                if (str1[j] > 47 && str1[j] < 58) {
                    strcpy(str2, &str1[j]);
                    str1[j] = '\0';
                    break;
                }
                j++;
            }
            list_append(self.components, (unitype) str1, 's');
            if (strcmp(str2, "null") == 0) {
                list_append(self.groups, (unitype) -1, 'i');
            } else {
                list_append(self.groups, (unitype) atoi(str2), 'i');
            }
        }
        for (int i = 0; i < num * 3; i++) {
            end = fscanf(file, "%lf", &doub1);
            if (end == EOF) {
                printf("file corrupted at word %d\n", i + num + 1);
                return;
            }
            list_append(self.positions, (unitype) doub1, 'd');
        }
        for (int i = 0; i < num * 3; i++) {
            end = fscanf(file, "%d", &int1);
            if (end == EOF) {
                printf("file corrupted at word %d\n", i + num * 4 + 1);
                return;
            }
            list_append(self.io, (unitype) int1, 'i');
        }
        for (int i = 0; i < num * 3; i += 3) {
            end = fscanf(file, "%d", &int1);
            if (end == EOF) {
                printf("file corrupted at word %d\n", i + num * 7 + 1);
                return;
            }
            list_append(self.inpComp, (unitype) int1, 'i');
            for (int j = 0; j < 2; j++) {
                end = fscanf(file, "%d", &int1);
                if (end == EOF) {
                    printf("file corrupted at word %d\n", i + j + num * 10 + 1);
                    return;
                }
                if (int1 == 0) {
                    // special case: 0 means no component attached
                    list_append(self.inpComp, (unitype) 0, 'i');
                } else {
                    list_append(self.inpComp, (unitype) (int1 + oldCompLen), 'i');
                }
            }
        }
        int i = 0;
        while (end != EOF) {
            end = fscanf(file, "%d", &int1);
            if (end == EOF) {
                continue;
                return;
            }
            list_append(self.wiring, (unitype) (int1 + oldCompLen), 'i');
            end = fscanf(file, "%d", &int1);
            if (end == EOF) {
                printf("file corruptedd at word %d\n", i + 1 + num * 13 + 1);
                return;
            }
            list_append(self.wiring, (unitype) (int1 + oldCompLen), 'i');
            end = fscanf(file, "%d", &int1);
            if (end == EOF) {
                printf("file corrupteddd at word %d\n", i + 2 + num * 13 + 1);
                return;
            }
            list_append(self.wiring, (unitype) int1, 'i');
            i += 3;
        }
        self.screenX = -self.positions -> data[1].d;
        self.screenY = -self.positions -> data[2].d;
        printf("%s loaded successfully\n", filename);
        fclose(file);
    }
    *selfp = self;
}
void export(logicgates *selfp, const char *filename) { // exports a file
    logicgates self = *selfp;
    FILE *file = fopen(filename, "w+");
    for (int i = 1; i < self.components -> length; i++) {
        fprintf(file, "%s", self.components -> data[i].s);
        if (self.groups -> data[i].i == -1) {
            fprintf(file, " ");
        } else {
            fprintf(file, "%d ", self.groups -> data[i].i);
        }
    }
    for (int i = 1; i < self.positions -> length; i++) {
        fprintf(file, "%.0lf ", self.positions -> data[i].d);
    }
    for (int i = 1; i < self.io -> length; i++) {
        fprintf(file, "%d ", self.io -> data[i].i);
    }
    for (int i = 1; i < self.inpComp -> length; i++) {
        fprintf(file, "%d ", self.inpComp -> data[i].i);
    }
    for (int i = 1; i < self.wiring -> length; i++) {
        fprintf(file, "%d ", self.wiring -> data[i].i);
    }
    printf("Successfully saved to %s\n", filename);
    fclose(file);
    *selfp = self;
}
void POWER(logicgates *selfp, double x, double y, double size, double rot, char state, char select) { // draws a POWER component
    logicgates self = *selfp;
    rot /= 57.2958; // convert to radians
    turtleGoto(x, y);
    turtlePenShape("circle");
    turtlePenPrez(self.specialPrez);
    turtlePenSize(size * 12.5 * self.scaling);
    turtlePenDown();
    turtlePenUp();
    if (state == 2) {
        turtlePenSize(size * 10 * self.scaling);
        if (select == 1) {
            turtlePenColor(self.themeColors[22 + self.theme], self.themeColors[23 + self.theme], self.themeColors[24 + self.theme]);
        } else {
            turtlePenColor(self.themeColors[19 + self.theme], self.themeColors[20 + self.theme], self.themeColors[21 + self.theme]);
        }
        turtlePenDown();
        turtlePenUp();
        turtlePenColor(self.themeColors[1 + self.theme], self.themeColors[2 + self.theme], self.themeColors[3 + self.theme]);
    }
    if (state == 1) {
        turtlePenSize(size * 10 * self.scaling);
        if (select == 1) {
            turtlePenColor(self.themeColors[10 + self.theme], self.themeColors[11 + self.theme], self.themeColors[12 + self.theme]);
        } else {
            turtlePenColor(self.themeColors[7 + self.theme], self.themeColors[8 + self.theme], self.themeColors[9 + self.theme]);
        }
        turtlePenDown();
        turtlePenUp();
        turtlePenColor(self.themeColors[1 + self.theme], self.themeColors[2 + self.theme], self.themeColors[3 + self.theme]);
    }
    turtle.penshape = self.defaultShape;
    turtlePenPrez(self.defaultPrez);
}
void NOT(logicgates *selfp, double x, double y, double size, double rot) { // draws a NOT component
    logicgates self = *selfp;
    rot /= 57.2958; // convert to radians
    double sinRot = sin(rot);
    double cosRot = cos(rot);
    turtlePenSize(size * self.scaling);
    turtleGoto(x + (-11 * size * sinRot) - (11 * size * cosRot), y + (-11 * size * cosRot) + (11 * size * sinRot));
    turtlePenDown();
    turtleGoto(x + (7 * size * sinRot), y + (7 * size * cosRot));
    turtleGoto(x + (-11 * size * sinRot) - (-11 * size * cosRot), y + (-11 * size * cosRot) + (-11 * size * sinRot));
    turtleGoto(x + (-11 * size * sinRot) - (11 * size * cosRot), y + (-11 * size * cosRot) + (11 * size * sinRot));
    turtlePenUp();
    turtleGoto(x + (10 * size * sinRot), y + (10 * size * cosRot));
    turtlePenShape("circle");
    turtlePenPrez(self.specialPrez);
    turtlePenSize(size * 3.5 * self.scaling);
    turtlePenDown();
    turtlePenUp();
    turtlePenSize(size * 1.5 * self.scaling);
    turtlePenColor(self.themeColors[25 + self.theme], self.themeColors[26 + self.theme], self.themeColors[27 + self.theme]);
    turtlePenDown();
    turtlePenUp();
    turtlePenColor(self.themeColors[1 + self.theme], self.themeColors[2 + self.theme], self.themeColors[3 + self.theme]);
    turtle.penshape = self.defaultShape;
    turtlePenPrez(self.defaultPrez);
}
void AND(logicgates *selfp, double x, double y, double size, double rot) {// draws an AND component
    logicgates self = *selfp;
    rot /= 57.2958; // convert to radians
    double sinRot = sin(rot);
    double cosRot = cos(rot);
    turtlePenSize(size * self.scaling);
    turtleGoto(x + (-12 * size * sinRot) - (-9 * size * cosRot), y + (-12 * size * cosRot) + (-9 * size * sinRot));
    turtlePenDown();
    turtleGoto(x + (4 * size * sinRot) - (-9 * size * cosRot), y + (4 * size * cosRot) + (-9 * size * sinRot));
    double i = 180;
    for (int j = 0; j < self.GraphPrez + 1; j++) {
        double k = i / 57.2958;
        turtleGoto(x + ((4 * size + sin(k) * 8 * size) * sinRot) - (cos(k) * 9 * size * cosRot), y + ((4 * size + sin(k) * 8 * size) * cosRot) + (cos(k) * 9 * size * sinRot));
        i -= (180 / self.GraphPrez);
    }
    turtleGoto(x + (-12 * size * sinRot) - (9 * size * cosRot), y + (-12 * size * cosRot) + (9 * size * sinRot));
    turtleGoto(x + (-12 * size * sinRot) - (-9 * size * cosRot), y + (-12 * size * cosRot) + (-9 * size * sinRot));
    turtlePenUp();
}
void OR(logicgates *selfp, double x, double y, double size, double rot) {// draws an OR component
    logicgates self = *selfp;
    rot /= 57.2958; // convert to radians
    double sinRot = sin(rot);
    double cosRot = cos(rot);
    turtlePenSize(size * self.scaling);
    turtleGoto(x + (-11 * size * sinRot) - (9 * size * cosRot), y + (-11 * size * cosRot) + (9 * size * sinRot));
    turtlePenDown();
    double k;
    double i = 180;
    for (int j = 0; j < self.GraphPrez + 1; j++) {
        k = i / 57.2958;
        double tempX = x + ((-11 * size + sin(k) * 5 * size) * sinRot) - (cos(k) * -9 * size * cosRot);
        double tempY = y + ((-11 * size + sin(k) * 5 * size) * cosRot) + (cos(k) * -9 * size * sinRot);
        turtleGoto(tempX, tempY);
        i -= (180 / self.GraphPrez);
    }
    i += (180 / self.GraphPrez);
    for (int j = 0; j < (self.GraphPrez + 1) / 1.5; j++) {
        k = i / 57.2958;
        turtleGoto(x + ((-11 * size + sin(k) * 25 * size) * sinRot) - ((9 * size - cos(k) * 18 * size) * cosRot), y + ((-11 * size + sin(k) * 25 * size) * cosRot) + ((9 * size - cos(k) * 18 * size) * sinRot));
        i += (90 / self.GraphPrez);
    }
    turtleGoto(x + (10.3 * size * sinRot), y + (10.3 * size * cosRot));
    turtlePenUp();
    turtleGoto(x + (-11 * size * sinRot) - (9 * size * cosRot), y + (-11 * size * cosRot) + (9 * size * sinRot));
    turtlePenDown();
    i = 0;
    for (int j = 0; j < (self.GraphPrez + 1) / 1.5; j++) {
        k = i / 57.2958;
        turtleGoto(x + ((-11 * size + sin(k) * 25 * size) * sinRot) - ((-9 * size + cos(k) * 18 * size) * cosRot), y + ((-11 * size + sin(k) * 25 * size) * cosRot) + ((-9 * size + cos(k) * 18 * size) * sinRot));
        i += (90 / self.GraphPrez);
    }
    turtleGoto(x + (10.3 * size * sinRot), y + (10.3 * size * cosRot));
    turtlePenUp();
}
void XOR(logicgates *selfp, double x, double y, double size, double rot) {// draws an XOR component
    logicgates self = *selfp;
    rot /= 57.2958; // convert to radians
    double sinRot = sin(rot);
    double cosRot = cos(rot);
    turtlePenSize(size * self.scaling);
    double k;
    double i = 180;
    i -= 180 / self.GraphPrez;
    k = i / 57.2958;
    turtleGoto(x + ((-15 * size + sin(k) * 5 * size) * sinRot) - (cos(k) * -9 * size * cosRot), y + ((-15 * size + sin(k) * 5 * size) * cosRot) + (cos(k) * -9 * size * sinRot));
    turtlePenDown();
    for (int j = 0; j < self.GraphPrez - 1; j++) {
        k = i / 57.2958;
        turtleGoto(x + ((-15 * size + sin(k) * 5 * size) * sinRot) - (cos(k) * -9 * size * cosRot), y + ((-15 * size + sin(k) * 5 * size) * cosRot) + (cos(k) * -9 * size * sinRot));
        i -= 180 / self.GraphPrez;
    }
    turtlePenUp();
    i = 180;
    i -= 180 / self.GraphPrez;
    k = i / 57.2958;
    turtleGoto(x + ((-11 * size + sin(k) * 5 * size) * sinRot) - (cos(k) * -9 * size * cosRot), y + ((-11 * size + sin(k) * 5 * size) * cosRot) + (cos(k) * -9 * size * sinRot));
    turtlePenDown();
    for (int j = 0; j < self.GraphPrez - 1; j++) {
        k = i / 57.2958;
        turtleGoto(x + ((-11 * size + sin(k) * 5 * size) * sinRot) - (cos(k) * -9 * size * cosRot), y + ((-11 * size + sin(k) * 5 * size) * cosRot) + (cos(k) * -9 * size * sinRot));
        i -= (180 / self.GraphPrez);
    }
    i += (180 / self.GraphPrez);
    for (int j = 0; j < (self.GraphPrez - 2) / 1.5; j++) {
        k = i / 57.2958;
        turtleGoto(x + ((-11 * size + sin(k) * 25 * size) * sinRot) - ((9 * size - cos(k) * 18 * size) * cosRot), y + ((-11 * size + sin(k) * 25 * size) * cosRot) + ((9 * size - cos(k) * 18 * size) * sinRot));
        i += (90 / self.GraphPrez);
    }
    turtleGoto(x + (10.3 * size * sinRot), y + (10.3 * size * cosRot));
    turtlePenUp();
    i = 180;
    i -= 180 / self.GraphPrez;
    k = i / 57.2958;
    turtleGoto(x + ((-11 * size + sin(k) * 5 * size) * sinRot) - (cos(k) * -9 * size * cosRot), y + ((-11 * size + sin(k) * 5 * size) * cosRot) + (cos(k) * -9 * size * sinRot));
    turtlePenDown();
    i = 0;
    i += 180 / self.GraphPrez;
    for (int j = 0; j < (self.GraphPrez - 2) / 1.5; j++) {
        k = i / 57.2958;
        turtleGoto(x + ((-11 * size + sin(k) * 25 * size) * sinRot) - ((-9 * size + cos(k) * 18 * size) * cosRot), y + ((-11 * size + sin(k) * 25 * size) * cosRot) + ((-9 * size + cos(k) * 18 * size) * sinRot));
        i += (90 / self.GraphPrez);
    }
    turtleGoto(x + (10.3 * size * sinRot), y + (10.3 * size * cosRot));
    turtlePenUp();
}
void NOR(logicgates *selfp, double x, double y, double size, double rot) { // draws a NOR component
    logicgates self = *selfp;
    rot /= 57.2958; // convert to radians
    double sinRot = sin(rot);
    double cosRot = cos(rot);
    turtlePenSize(size * self.scaling);
    turtleGoto(x + (-13 * size * sinRot) - (9 * size * cosRot), y + (-13 * size * cosRot) + (9 * size * sinRot));
    turtlePenDown();
    double k;
    double i = 180;
    for (int j = 0; j < self.GraphPrez + 1; j++) {
        k = i / 57.2958;
        turtleGoto(x + ((-13 * size + sin(k) * 5 * size) * sinRot) - (cos(k) * -9 * size * cosRot), y + ((-13 * size + sin(k) * 5 * size) * cosRot) + (cos(k) * -9 * size * sinRot));
        i -= (180 / self.GraphPrez);
    }
    i += (180 / self.GraphPrez);
    for (int j = 0; j < (self.GraphPrez + 1) / 1.5; j++) {
        k = i / 57.2958;
        turtleGoto(x + ((-13 * size + sin(k) * 25 * size) * sinRot) - ((9 * size - cos(k) * 18 * size) * cosRot), y + ((-13 * size + sin(k) * 25 * size) * cosRot) + ((9 * size - cos(k) * 18 * size) * sinRot));
        i += (90 / self.GraphPrez);
    }
    turtleGoto(x + (8.3 * size * sinRot), y + (8.3 * size * cosRot));
    turtlePenUp();
    turtleGoto(x + (-13 * size * sinRot) - (9 * size * cosRot), y + (-13 * size * cosRot) + (9 * size * sinRot));
    turtlePenDown();
    i = 0;
    for (int j = 0; j < (self.GraphPrez + 1) / 1.5; j++) {
        k = i / 57.2958;
        turtleGoto(x + ((-13 * size + sin(k) * 25 * size) * sinRot) - ((-9 * size + cos(k) * 18 * size) * cosRot), y + ((-13 * size + sin(k) * 25 * size) * cosRot) + ((-9 * size + cos(k) * 18 * size) * sinRot));
        i += (90 / self.GraphPrez);
    }
    turtleGoto(x + (8.3 * size * sinRot), y + (8.3 * size * cosRot));
    turtlePenUp();
    turtleGoto(x + (11.5 * size * sinRot), y + (11.5 * size * cosRot));
    turtlePenShape("circle");
    turtlePenPrez(self.specialPrez);
    turtlePenSize(size * 3.5 * self.scaling);
    turtlePenDown();
    turtlePenUp();
    turtlePenSize(size * 1.5 * self.scaling);
    turtlePenColor(self.themeColors[25 + self.theme], self.themeColors[26 + self.theme], self.themeColors[27 + self.theme]);
    turtlePenDown();
    turtlePenUp();
    turtlePenColor(self.themeColors[1 + self.theme], self.themeColors[2 + self.theme], self.themeColors[3 + self.theme]);
    turtle.penshape = self.defaultShape;
    turtlePenPrez(self.defaultPrez);
}
void NAND(logicgates *selfp, double x, double y, double size, double rot) { // draws a NAND component
    logicgates self = *selfp;
    rot /= 57.2958; // convert to radians
    double sinRot = sin(rot);
    double cosRot = cos(rot);
    turtlePenSize(size * self.scaling);
    turtleGoto(x + (-12 * size * sinRot) - (-9 * size * cosRot), y + (-12 * size * cosRot) + (-9 * size * sinRot));
    turtlePenDown();
    turtleGoto(x + (4 * size * sinRot) - (-9 * size * cosRot), y + (4 * size * cosRot) + (-9 * size * sinRot));
    double k;
    double i = 180;
    for (int j = 0; j < self.GraphPrez + 1; j++) {
        k = i / 57.2958;
        turtleGoto(x + ((4 * size + sin(k) * 8 * size) * sinRot) - (cos(k) * 9 * size * cosRot), y + ((4 * size + sin(k) * 8 * size) * cosRot) + (cos(k) * 9 * size * sinRot));
        i -= (180 / self.GraphPrez);
    }
    turtleGoto(x + (-12 * size * sinRot) - (9 * size * cosRot), y + (-12 * size * cosRot) + (9 * size * sinRot));
    turtleGoto(x + (-12 * size * sinRot) - (-9 * size * cosRot), y + (-12 * size * cosRot) + (-9 * size * sinRot));
    turtlePenUp();
    turtleGoto(x + (15 * size * sinRot), y + (15 * size * cosRot));
    turtlePenShape("circle");
    turtlePenPrez(self.specialPrez);
    turtlePenSize(size * 3.5 * self.scaling);
    turtlePenDown();
    turtlePenUp();
    turtlePenSize(size * 1.5 * self.scaling);
    turtlePenColor(self.themeColors[25 + self.theme], self.themeColors[26 + self.theme], self.themeColors[27 + self.theme]);
    turtlePenDown();
    turtlePenUp();
    turtlePenColor(self.themeColors[1 + self.theme], self.themeColors[2 + self.theme], self.themeColors[3 + self.theme]);
    turtle.penshape = self.defaultShape;
    turtlePenPrez(self.defaultPrez);
}
void BUFFER(logicgates *selfp, double x, double y, double size, double rot) { // draws a BUFFER component
    logicgates self = *selfp;
    rot /= 57.2958; // convert to radians
    double sinRot = sin(rot);
    double cosRot = cos(rot);
    turtlePenSize(size * self.scaling);
    turtleGoto(x + (-8 * size * sinRot) - (11 * size * cosRot), y + (-8 * size * cosRot) + (11 * size * sinRot));
    turtlePenDown();
    turtleGoto(x + (10 * size * sinRot), y + (10 * size * cosRot));
    turtleGoto(x + (-8 * size * sinRot) - (-11 * size * cosRot), y + (-8 * size * cosRot) + (-11 * size * sinRot));
    turtleGoto(x + (-8 * size * sinRot) - (11 * size * cosRot), y + (-8 * size * cosRot) + (11 * size * sinRot));
    turtlePenUp();
}
void wireSymbol(logicgates *selfp, double x, double y, double size, double rot) { // draws the wireSymbol on the sidebar
    logicgates self = *selfp;
    rot /= 57.2958; // convert to radians
    // double sinRot = sin(rot);
    // double cosRot = cos(rot);
    turtlePenSize(size * self.scaling);
    turtleGoto(x + -12 * size, y + -9 * size);
    turtlePenDown();
    turtleGoto(x + -6 * size, y + -9 * size);
    turtleGoto(x + 6 * size, y + 9 * size);
    turtleGoto(x + 12 * size, y + 9 * size);
    turtlePenUp();
}
void deleteComp(logicgates *selfp, int index, int replace) { // deletes a component
    /* bad foresight compelled me to identify a component's ID
    based on the index of it's position in the self.components array
    
    This means that when a component is deleted all components with an ID greater than that one has
    themselves reassigned a new ID. This means that every reference to that component (in the other data lists)
    must be updated to the new ID.

    This allows fast access to a component's data by just given its ID, because it's an array lookup
    but this is hell for deleting, and if there are any references to a component via its ID anywhere in
    the program it must be updated.

    yknow having these "components" with associated "data" is something that perhaps, just maybe,
    could've had its own type.

    We unfortunately live with our decisions, somehow this seemed like the best way to do this at the time, 
    and to be fair it was written in scratch

    The replace feature:
    This is delete and replace. This only works with POWER, NOT, and BUFFER
    It takes the input (if any) of the gate and connects it to all outputs while deleting the gate
    */
    logicgates self = *selfp;
    
    // identify input to deleted component
    int inputCon = -1;
    if (replace && self.inpComp -> data[index * 3].i == 0) { // any component with one input
        // identify input (if any)
        for (int j = 1; j < self.wiring -> length; j += 3) {
            if (self.wiring -> data[j + 1].i == index) {
                inputCon = self.wiring -> data[j].i;
                if (inputCon > index) {
                    inputCon--;
                }
                break;
            }
        }
    }
    // gather numInputs and numInputsHolding
    char doDelete = 1;
    int numInputs = self.inpComp -> data[index * 3 - 2].i; // numInputs is how many actual wire inputs are connected to this component
    if (numInputs == 2 && self.inpComp -> data[index * 3].i == 0) {
        // if it can support two but only has one
        numInputs--;
    }
    if (self.inpComp -> data[index * 3 - 1].i == 0) {
        // this is equivalent to 0 inputs
        numInputs--;
    }
    int numInputsHolding = 0;
    if (strcmp(self.holding, "a") != 0 && strcmp(self.holding, "b") != 0) {
        numInputsHolding = self.compSlots -> data[list_find(self.compSlots, (unitype) self.holding, 's') + 1].i; // this is how many inputs can be taken by self.holding
        if (replace && (numInputsHolding >= numInputs || numInputs == 0)) {
            doDelete = 0;
            inputCon = 0; // in the case where you replace a 2 input with another 2 input
            // printf("replaced %s with %s\n", self.components -> data[index].s, self.holding);
            // replace component with self.holding
            free(self.components -> data[index].s);
            self.components -> data[index].s = strdup(self.holding);
            // group does not change
            // position does not change
            // io does not change
            self.inpComp -> data[index * 3 - 2].i = numInputsHolding;
            // other inpComp does not change
            // wiring does not change
        }
    }
    // reform selected components
    if (inputCon == -1 || numInputsHolding < numInputs) {
        int len = self.selected -> length;
        for (int i = 1; i < len; i++) {
            if (self.selected -> data[i].i > index) {
                self.selected -> data[i] = (unitype) (self.selected -> data[i].i - 1);
            }
        }
    }
    // printf("%s %d %d %d\n", self.holding, numInputs, numInputsHolding, inputCon);
    int i = 1;
    int k = (int) round((self.wiring -> length - 1) / 3);
    for (int j = 0; j < k; j++) {
        if (self.wiring -> data[i].i == index || self.wiring -> data[i + 1].i == index) {
            if (inputCon == -1) {
                list_delete(self.wiring, i);
                list_delete(self.wiring, i);
                list_delete(self.wiring, i);
            } else {
                if (numInputsHolding >= numInputs) {
                    // case: replace
                    // no change in wiring
                    i += 3;
                } else {
                    if (self.wiring -> data[i + 1].i == index) {
                        // case: normal delete wire
                        list_delete(self.wiring, i);
                        list_delete(self.wiring, i);
                        list_delete(self.wiring, i);
                    } else if (self.wiring -> data[i].i == index) {
                        // case: replace wire
                        self.wiring -> data[i].i = inputCon;
                        if (self.wiring -> data[i + 1].i > index) {
                            self.wiring -> data[i + 1].i--;
                        }
                        i += 3;
                    }
                }
            }
        } else {
            if (inputCon == -1 || numInputsHolding < numInputs) {
                // assume a component is getting deleted
                if (self.wiring -> data[i].i > index) {
                    self.wiring -> data[i].i--;
                }
                if (self.wiring -> data[i + 1].i > index) {
                    self.wiring -> data[i + 1].i--;
                }
            }
            i += 3;
        }
    }
    i = 2;
    k = (int) round((self.inpComp -> length - 1) / 3);
    for (int j = 0; j < k; j++) {
        if (self.inpComp -> data[i].i == index || self.inpComp -> data[i + 1].i == index) {
            if (self.inpComp -> data[i].i == index) {
                if (self.inpComp -> data[i + 1].i == 0) {
                    if (inputCon == -1) {
                        // normal no replace wire
                        self.inpComp -> data[i] = (unitype) 0;
                        self.io -> data[i - 1] = (unitype) 0;
                    } else {
                        if (numInputsHolding >= numInputs) {
                            // case: replace
                            // no change in inpComp
                        } else {
                            // one input component replace case
                            self.inpComp -> data[i] = (unitype) inputCon;
                        }
                    }
                } else {
                    if (inputCon == -1) {
                        // normal no replace wire (shift input2 to input1)
                        if (self.inpComp -> data[i + 1].i > index) {
                            self.inpComp -> data[i] = (unitype) (self.inpComp -> data[i + 1].i - 1);
                        } else {
                            self.inpComp -> data[i] = self.inpComp -> data[i + 1];
                        }
                        self.inpComp -> data[i + 1] = (unitype) 0;
                        self.io -> data[i] = (unitype) 0;
                    } else {
                        if (numInputsHolding >= numInputs) {
                            // case: replace
                            // no change in inpComp
                        } else {
                            // case: replace (still shift)
                            self.inpComp -> data[i] = (unitype) inputCon;
                        }
                    }
                }
            } else {
                if (inputCon == -1) {
                    // normal no replace wire
                    if (self.inpComp -> data[i].i > index) {
                        self.inpComp -> data[i].i--;
                    }
                    self.inpComp -> data[i + 1] = (unitype) 0;
                    self.io -> data[i] = (unitype) 0;
                } else {
                    if (numInputsHolding >= numInputs) {
                        // case: replace
                        // no change in inpComp
                    } else {
                        // case: replace second input
                        if (self.inpComp -> data[i].i > index) {
                            self.inpComp -> data[i].i--;
                        }
                        self.inpComp -> data[i + 1] = (unitype) inputCon;
                    }
                }
            }
        } else {
            if (inputCon == -1 || numInputsHolding < numInputs) {
                // assume a component is getting deleted
                if (self.inpComp -> data[i].i > index) {
                    self.inpComp -> data[i].i--;
                }
                if (self.inpComp -> data[i + 1].i > index) {
                    self.inpComp -> data[i + 1].i--;
                }
            }
        }
        i += 3;
    }
    if (doDelete) {
        list_delete(self.components, index);
        list_delete(self.groups, index);
        list_delete(self.positions, index * 3 - 2);
        list_delete(self.positions, index * 3 - 2);
        list_delete(self.positions, index * 3 - 2);
        list_delete(self.io, index * 3 - 2);
        list_delete(self.io, index * 3 - 2);
        list_delete(self.io, index * 3 - 2);
        list_delete(self.inpComp, index * 3 - 2);
        list_delete(self.inpComp, index * 3 - 2);
        list_delete(self.inpComp, index * 3 - 2);
    }
    *selfp = self;
}
void groupSelected(logicgates *selfp, int ungroup) { // creates a group for selected components
    /* principle of groups:
    in file: append groupID to the end of a component name
    POWER1 POWER1 AND1 OR2 POWER2 POWER2

    in code: have a "groups" list
    we need
    - fast access to tell what group a given component is in
    - fast access to get all the components in a given group (potentially compromisable)

    on the screen: 
    - moving one element of a group should move all the elements
    - selecting one element of a group should select all the elements
    - each component can only be in one group
    - selecting components and hitting 'G' will put those components in a new group. Any components already in a group will lose their relation to that group
      - if all components are in the same group it will ungroup them
    - selecting components and hitting "shift + G" will put those components in no group
    - bonus: groups will have a square around them when hovered over (like a selection square but more minimal)
    - copying components that are exclusively in a group will create a new group for the copied components

    Technical invariant:
    You cannot select a subset of components from a group (for now)
     - Do not make this assumption in the code though, because I'm not sure if there will be a way later
    */

    logicgates self = *selfp;
    self.sxmax = 0;
    self.sxmin = 0;
    self.symax = 0;
    self.symin = 0;
    self.selecting = 3;
    if (self.hlgcomp > 0 && self.selected -> length == 1) {
        // case: pressing g on a single component
        // this puts that component in no group
        self.groups -> data[self.hlgcomp].i = -1;
    } else {
        if (ungroup) {
            // case: shift + G (ungroup)
            for (int i = 1; i < self.selected -> length; i++) {
                self.groups -> data[self.selected -> data[i].i].i = -1;
            }
        } else {
            int groupCheck = self.groups -> data[self.selected -> data[1].i].i;
            if (groupCheck > 0) {
                for (int i = 2; i < self.selected -> length; i++) {
                    if (self.groups -> data[self.selected -> data[i].i].i != groupCheck) {
                        groupCheck = 0;
                        break;
                    }
                }
            }
            if (groupCheck > 0) {
                // case: all selected items are part of the same (non -1) group
                // ungroup items
                for (int i = 1; i < self.selected -> length; i++) {
                    self.groups -> data[self.selected -> data[i].i].i = -1;
                }
            } else {
                // case: group items
                // find next available group ID
                int groupID = 1;
                for (int i = 1; i < self.groups -> length; i++) {
                    if (self.groups -> data[i].i >= groupID) {
                        groupID = self.groups -> data[i].i + 1;
                    }
                }
                // put all selected items in that group
                for (int i = 1; i < self.selected -> length; i++) {
                    self.groups -> data[self.selected -> data[i].i].i = groupID;
                }
            }
        }
    }
    list_clear(self.selected);
    list_append(self.selected, (unitype) "null", 's');
    *selfp = self;
}
void copySelected(logicgates *selfp) { // copies and pastes selected components
    logicgates self = *selfp;
    self.sxmax = 0;
    self.sxmin = 0;
    self.symax = 0;
    self.symin = 0;
    self.selecting = 3;
    double j = 0;
    double k = 0;
    int l = self.components -> length;
    int m1 = self.selected -> length;
    for (int i = 1; i < m1; i++) {
        j += self.positions -> data[self.selected -> data[i].i * 3 - 2].d;
        k += self.positions -> data[self.selected -> data[i].i * 3 - 1].d;
    }
    j /= m1 - 1;
    k /= m1 - 1;
    for (int i = 1; i < m1; i++) {
        list_append(self.components, self.components -> data[self.selected -> data[i].i], 's');
        list_append(self.groups, (unitype) -1, 'i');
        list_append(self.positions, (unitype) (self.positions -> data[self.selected -> data[i].i * 3 - 2].d + self.mx / (self.globalsize * 0.75) - self.screenX - j), 'd');
        list_append(self.positions, (unitype) (self.positions -> data[self.selected -> data[i].i * 3 - 1].d + self.my / (self.globalsize * 0.75) - self.screenY - k), 'd');
        list_append(self.positions, self.positions -> data[self.selected -> data[i].i * 3], 'd');
        list_append(self.io, (unitype) 0, 'i');
        if (strcmp(self.components -> data[i].s, "POWER") == 0) { // preserve toggle data
            list_append(self.io, (unitype) self.io -> data[self.selected -> data[i].i * 3 - 1], 'i');
        } else {
            list_append(self.io, (unitype) 0, 'i');
        }
        list_append(self.io, (unitype) 0, 'i');
        list_append(self.inpComp, self.inpComp -> data[self.selected -> data[i].i * 3 - 2], 'i');
        if (list_count(self.selected, self.inpComp -> data[self.selected -> data[i].i * 3 - 1], 'i') > 0) {
            list_append(self.inpComp, (unitype) (l + list_find(self.selected, self.inpComp -> data[self.selected -> data[i].i * 3 - 1], 'i') - 1), 'i');
            if (list_count(self.selected, self.inpComp -> data[self.selected -> data[i].i * 3], 'i') > 0) {
                list_append(self.inpComp, (unitype) (l + list_find(self.selected, self.inpComp -> data[self.selected -> data[i].i * 3], 'i') - 1), 'i');
            } else {
                list_append(self.inpComp, (unitype) 0, 'i');
            }
        } else {
            if (list_count(self.selected, self.inpComp -> data[self.selected -> data[i].i * 3], 'i') > 0) {
                list_append(self.inpComp, (unitype) (l + list_find(self.selected, self.inpComp -> data[self.selected -> data[i].i * 3], 'i') - 1), 'i');
            } else {
                list_append(self.inpComp, (unitype) 0, 'i');
            }
            list_append(self.inpComp, (unitype) 0, 'i');
        }
    }
    int n = self.components -> length - self.selected -> length;
    list_t *wireTemp = list_init();
    for (int i = 1; i < m1; i++) {
        list_append(wireTemp, (unitype) (n + i), 'i');
    }
    int len = self.wiring -> length;
    for (int i = 1; i < len; i += 3) {
        if (list_count(self.selected, self.wiring -> data[i], 'i') > 0 && list_count(self.selected, self.wiring -> data[i + 1], 'i') > 0) {
            list_append(self.wiring, wireTemp -> data[list_find(self.selected, self.wiring -> data[i], 'i') - 1], 'i');
            list_append(self.wiring, wireTemp -> data[list_find(self.selected, self.wiring -> data[i + 1], 'i') - 1], 'i');
            list_append(self.wiring, (unitype) 0, 'i');
        }
    }
    list_free(wireTemp);
    int groupCheck = self.groups -> data[self.selected -> data[1].i].i;
    if (groupCheck > 0) {
        for (int i = 2; i < self.selected -> length; i++) {
            if (self.groups -> data[self.selected -> data[i].i].i != groupCheck) {
                groupCheck = 0;
                break;
            }
        }
    }
    int i = self.components -> length - self.selected -> length + 1;
    list_clear(self.selected);
    list_append(self.selected, (unitype) "null", 's');
    for (int o = 1; o < m1; o++) {
        list_append(self.selected, (unitype) i, 'i');
        i += 1;
    }
    if (groupCheck > 0) {
        groupSelected(&self, 0);
    }
    *selfp = self;
}
void copyToBuffer(logicgates *selfp, char cut) {
    logicgates self = *selfp;

    // clear the copy buffer
    for (int i = 1; i < 7; i++) {
        list_clear(self.copyBuffer -> data[i].r);
        list_append(self.copyBuffer -> data[i].r, (unitype) 'n', 'c');
    }

    self.sxmax = 0;
    self.sxmin = 0;
    self.symax = 0;
    self.symin = 0;
    self.selecting = 3;
    double j = 0;
    double k = 0;
    int l = self.components -> length;
    int m1 = self.selected -> length;
    for (int i = 1; i < m1; i++) {
        j += self.positions -> data[self.selected -> data[i].i * 3 - 2].d;
        k += self.positions -> data[self.selected -> data[i].i * 3 - 1].d;
    }
    j /= m1 - 1;
    k /= m1 - 1;
    for (int i = 1; i < m1; i++) {
        list_append(self.copyBuffer -> data[1].r, self.components -> data[self.selected -> data[i].i], 's');
        list_append(self.copyBuffer -> data[2].r, self.groups -> data[self.selected -> data[i].i], 'i');
        list_append(self.copyBuffer -> data[3].r, (unitype) (self.positions -> data[self.selected -> data[i].i * 3 - 2].d - j), 'd');
        list_append(self.copyBuffer -> data[3].r, (unitype) (self.positions -> data[self.selected -> data[i].i * 3 - 1].d - k), 'd');
        list_append(self.copyBuffer -> data[3].r, self.positions -> data[self.selected -> data[i].i * 3], 'd');
        list_append(self.copyBuffer -> data[4].r, self.io -> data[self.selected -> data[i].i * 3 - 2], 'i');
        list_append(self.copyBuffer -> data[4].r, self.io -> data[self.selected -> data[i].i * 3 - 1], 'i');
        list_append(self.copyBuffer -> data[4].r, self.io -> data[self.selected -> data[i].i * 3], 'i');
        list_append(self.copyBuffer -> data[5].r, self.inpComp -> data[self.selected -> data[i].i * 3 - 2], 'i');
        // do not add l or 1, 1 will be subtracted later, as will the later l be added because it may change between copying and pasting
        if (list_count(self.selected, self.inpComp -> data[self.selected -> data[i].i * 3 - 1], 'i') > 0) {
            list_append(self.copyBuffer -> data[5].r, (unitype) (list_find(self.selected, self.inpComp -> data[self.selected -> data[i].i * 3 - 1], 'i')), 'i');
            if (list_count(self.selected, self.inpComp -> data[self.selected -> data[i].i * 3], 'i') > 0) {
                list_append(self.copyBuffer -> data[5].r, (unitype) (list_find(self.selected, self.inpComp -> data[self.selected -> data[i].i * 3], 'i')), 'i');
            } else {
                list_append(self.copyBuffer -> data[5].r, (unitype) 0, 'i');
            }
        } else {
            if (list_count(self.selected, self.inpComp -> data[self.selected -> data[i].i * 3], 'i') > 0) {
                list_append(self.copyBuffer -> data[5].r, (unitype) (list_find(self.selected, self.inpComp -> data[self.selected -> data[i].i * 3], 'i')), 'i');
            } else {
                list_append(self.copyBuffer -> data[5].r, (unitype) 0, 'i');
            }
            list_append(self.copyBuffer -> data[5].r, (unitype) 0, 'i');
        }
    }
    /*
    adds appropriate wires but with the expectation that these values
    will be added to the current number of components when it is pasted,
    so remember to do that
    */
    int n = 0;
    list_t *wireTemp = list_init();
    for (int i = 1; i < m1; i++) {
        list_append(wireTemp, (unitype) (n + i), 'i');
    }
    int len = self.wiring -> length;
    for (int i = 1; i < len; i += 3) {
        if (list_count(self.selected, self.wiring -> data[i], 'i') > 0 && list_count(self.selected, self.wiring -> data[i + 1], 'i') > 0) {
            list_append(self.copyBuffer -> data[6].r, wireTemp -> data[list_find(self.selected, self.wiring -> data[i], 'i') - 1], 'i');
            list_append(self.copyBuffer -> data[6].r, wireTemp -> data[list_find(self.selected, self.wiring -> data[i + 1], 'i') - 1], 'i');
            list_append(self.copyBuffer -> data[6].r, (unitype) 0, 'i');
        }
    }
    list_free(wireTemp);
    if (cut) {
        len = self.selected -> length - 1;
        for (int i = 0; i < len; i++) {
            deleteComp(&self, self.selected -> data[1].i, 0);
            list_delete(self.selected, 1);
        }
        self.selecting = 0;
        list_clear(self.selectOb),
        list_append(self.selectOb, (unitype) "null", 's');
    }
    *selfp = self;
}
void pasteFromBuffer(logicgates *selfp, char toMouse) {
    logicgates self = *selfp;
    self.sxmax = 0;
    self.sxmin = 0;
    self.symax = 0;
    self.symin = 0;
    self.selecting = 3;

    int groupCheck = -1; // are the pasted components grouped?
    groupCheck = self.copyBuffer -> data[2].r -> data[1].i;
    if (groupCheck > 0) {
        for (int i = 2; i < self.copyBuffer -> data[2].r -> length; i++) {
            if (self.copyBuffer -> data[2].r -> data[i].i != groupCheck) {
                groupCheck = 0;
                break;
            }
        }
    }
    
    double j = 0;
    double k = 0;
    int l = self.components -> length;
    int m1 = self.copyBuffer -> data[1].r -> length; // length of copied data
    for (int i = 1; i < m1; i++) {
        // component straight copied from copyBuffer
        list_append(self.components, self.copyBuffer -> data[1].r -> data[i], 's');
        // groups. If every component is in the same group then pasted components are grouped
        list_append(self.groups, (unitype) -1, 'i');
        // positions must be translated by mouse and screen position to reflect their new location of pastement, center of mass has been precalculated
        if (toMouse) {
            list_append(self.positions, (unitype) (self.copyBuffer -> data[3].r -> data[i * 3 - 2].d + self.mx / (self.globalsize * 0.75) - self.screenX), 'd');
            list_append(self.positions, (unitype) (self.copyBuffer -> data[3].r -> data[i * 3 - 1].d + self.my / (self.globalsize * 0.75) - self.screenY), 'd');
        } else {
            // to center of screen
            list_append(self.positions, (unitype) (self.copyBuffer -> data[3].r -> data[i * 3 - 2].d - self.screenX), 'd');
            list_append(self.positions, (unitype) (self.copyBuffer -> data[3].r -> data[i * 3 - 1].d - self.screenY), 'd');
        }
        
        list_append(self.positions, self.copyBuffer -> data[3].r -> data[i * 3], 'd');
        list_append(self.io, (unitype) 0, 'i');
        if (strcmp(self.components -> data[i].s, "POWER") == 0) { // preserve togged data
            list_append(self.io, (unitype) self.copyBuffer -> data[4].r -> data[i * 3 - 1], 'i');
        } else {
            list_append(self.io, (unitype) 0, 'i');
        }
        list_append(self.io, (unitype) 0, 'i');
        // inpComp is more complicated
        list_append(self.inpComp, (unitype) (l + self.copyBuffer -> data[5].r -> data[i * 3 - 2].i), 'i');
        // the next two could be 0, if they are, keep them at 0
        if (self.copyBuffer -> data[5].r -> data[i * 3 - 1].i == 0)
            list_append(self.inpComp, (unitype) 0, 'i');
        else
            list_append(self.inpComp, (unitype) (l + self.copyBuffer -> data[5].r -> data[i * 3 - 1].i - 1), 'i');
        if (self.copyBuffer -> data[5].r -> data[i * 3].i == 0)
            list_append(self.inpComp, (unitype) 0, 'i');
        else
            list_append(self.inpComp, (unitype) (l + self.copyBuffer -> data[5].r -> data[i * 3].i - 1), 'i');
    }
    int len = self.copyBuffer -> data[6].r -> length;
    for (int i = 1; i < len; i += 3) {
        list_append(self.wiring, (unitype) (l + self.copyBuffer -> data[6].r -> data[i].i - 1), 'i');
        list_append(self.wiring, (unitype) (l + self.copyBuffer -> data[6].r -> data[i + 1].i - 1), 'i');
        list_append(self.wiring, (unitype) 0, 'i');
    }
    int i = self.components -> length - m1 + 1;
    list_clear(self.selected);
    list_append(self.selected, (unitype) "null", 's');
    for (int o = 1; o < m1; o++) {
        list_append(self.selected, (unitype) i, 'i');
        i += 1;
    }
    if (groupCheck > 0) {
        groupSelected(&self, 0);
    }
    *selfp = self;
}
void addUndo(logicgates *selfp) { // adds current state of program to undo list
    logicgates self = *selfp;
    self.undoIndex++;
    while (self.undoBuffer -> length > self.undoIndex) {
        list_delete(self.undoBuffer, self.undoBuffer -> length - 1);
    }
    if (self.undoBuffer -> length == self.undoIndex) {
        // printf("this is good\n");
    }
    list_append(self.undoBuffer, (unitype) list_init(), 'r');
    list_append(self.undoBuffer -> data[self.undoIndex].r, (unitype) (int) self.components -> length, 'i');
    // packs data
    for (int i = 1; i < self.components -> length; i++) {
        list_append(self.undoBuffer -> data[self.undoIndex].r, self.components -> data[i], 's');
    }
    for (int i = 1; i < self.groups -> length; i++) {
        list_append(self.undoBuffer -> data[self.undoIndex].r, self.groups -> data[i], 'i');
    }
    for (int i = 1; i < self.positions -> length; i++) {
        list_append(self.undoBuffer -> data[self.undoIndex].r, self.positions -> data[i], 'd');
    }
    for (int i = 1; i < self.io -> length; i++) {
        list_append(self.undoBuffer -> data[self.undoIndex].r, self.io -> data[i], 'i');
    }
    for (int i = 1; i < self.inpComp -> length; i++) {
        list_append(self.undoBuffer -> data[self.undoIndex].r, self.inpComp -> data[i], 'i');
    }
    for (int i = 1; i < self.wiring -> length; i++) {
        list_append(self.undoBuffer -> data[self.undoIndex].r, self.wiring -> data[i], 'i');
    }
    *selfp = self;
}
void transferUndoBuffer(logicgates *selfp) { // transfers undoBuffer data to the working lists
    logicgates self = *selfp;
    // no selecting when undoing
    self.wireHold = 0; // this literally does nothing. Like it doesn't even set wireHold to 0 i dont know why
    self.selecting = 0;
    list_clear(self.selectOb);
    list_append(self.selectOb, (unitype) "null", 's');
    list_clear(self.selected);
    list_append(self.selected, (unitype) "null", 's');

    int numComp = self.undoBuffer -> data[self.undoIndex].r -> data[0].i;
    int globIndex = 1;
    // components
    list_clear(self.components);
    list_append(self.components, (unitype) "null", 's');
    for (int i = 1; i < numComp; i++) {
        list_append(self.components, self.undoBuffer -> data[self.undoIndex].r -> data[globIndex], 's');
        globIndex++;
    }
    // groups
    list_clear(self.groups);
    list_append(self.groups, (unitype) 'n', 'c');
    for (int i = 1; i < numComp; i++) {
        list_append(self.groups, self.undoBuffer -> data[self.undoIndex].r -> data[globIndex], 'i');
        globIndex++;
    }
    // positions
    list_clear(self.positions);
    list_append(self.positions, (unitype) 'n', 'c');
    for (int i = 1; i < numComp; i++) {
        list_append(self.positions, self.undoBuffer -> data[self.undoIndex].r -> data[globIndex], 'd');
        list_append(self.positions, self.undoBuffer -> data[self.undoIndex].r -> data[globIndex + 1], 'd');
        list_append(self.positions, self.undoBuffer -> data[self.undoIndex].r -> data[globIndex + 2], 'd');
        globIndex += 3;
    }
    // io
    list_clear(self.io);
    list_append(self.io, (unitype) 'n', 'c');
    for (int i = 1; i < numComp; i++) {
        list_append(self.io, self.undoBuffer -> data[self.undoIndex].r -> data[globIndex], 'i');
        list_append(self.io, self.undoBuffer -> data[self.undoIndex].r -> data[globIndex + 1], 'i');
        list_append(self.io, self.undoBuffer -> data[self.undoIndex].r -> data[globIndex + 2], 'i');
        globIndex += 3;
    }
    // inpComp
    list_clear(self.inpComp);
    list_append(self.inpComp, (unitype) 'n', 'c');
    for (int i = 1; i < numComp; i++) {
        list_append(self.inpComp, self.undoBuffer -> data[self.undoIndex].r -> data[globIndex], 'i');
        list_append(self.inpComp, self.undoBuffer -> data[self.undoIndex].r -> data[globIndex + 1], 'i');
        list_append(self.inpComp, self.undoBuffer -> data[self.undoIndex].r -> data[globIndex + 2], 'i');
        globIndex += 3;
    }
    // wiring
    list_clear(self.wiring);
    list_append(self.wiring, (unitype) 'n', 'c');
    while (globIndex < self.undoBuffer -> data[self.undoIndex].r -> length) {
        list_append(self.wiring, self.undoBuffer -> data[self.undoIndex].r -> data[globIndex], 'i');
        list_append(self.wiring, self.undoBuffer -> data[self.undoIndex].r -> data[globIndex + 1], 'i');
        list_append(self.wiring, self.undoBuffer -> data[self.undoIndex].r -> data[globIndex + 2], 'i');
        globIndex += 3;
    }
}
void undo(logicgates *selfp) { // undo
    if (selfp -> undoIndex > 1) {
        selfp -> undoIndex--;
    }
    transferUndoBuffer(selfp);
}
void redo(logicgates *selfp) {
    if (selfp -> undoIndex + 1 < selfp -> undoBuffer -> length) {
        selfp -> undoIndex++;
    }
    transferUndoBuffer(selfp);
}
double dmod(double input, double modulus) { // fmod that always returns a positive number
    double out = fmod(input, modulus);
    if (out < 0) {
        return modulus + out;
    }
    return out;
}
void snapToGrid(logicgates *selfp, double gridsize) { // snaps components to a grid
    // printf("%lf\n", dmod(-1, 5));
    logicgates self = *selfp;
    double j = 0;
    double k = 0;
    int m1 = self.positions -> length;
    for (int i = 1; i < m1; i += 3) {
        j += self.positions -> data[i].d;
        k += self.positions -> data[i + 1].d;
    }
    j /= m1 / 3; // average x and y positions
    k /= m1 / 3;
    for (int i = 1; i < m1; i += 3) {
        self.positions -> data[i] = (unitype) (self.positions -> data[i].d - j); // normalise average x and y positions to 0
        self.positions -> data[i + 1] = (unitype) (self.positions -> data[i + 1].d - k);
    }
    double pivotX; // set the pivot point for the algorithm
    double pivotY; // center the pivot on each component position and test for distance
    double currentSnapX;
    double currentSnapY;
    double bestSnapX = 1 / 0.0;
    double bestSnapY = 1 / 0.0;
    double bestSnapModX = 0;
    double bestSnapModY = 0;
    for (int i = 1; i < m1; i += 3) {
        pivotX = dmod(self.positions -> data[i].d, gridsize);
        pivotY = dmod(self.positions -> data[i + 1].d, gridsize);
        currentSnapX = 0;
        currentSnapY = 0;
        for (int l = 1; l < m1; l += 3) {
            if (dmod(self.positions -> data[l].d - pivotX, gridsize) > gridsize * 0.5)
                currentSnapX += gridsize - dmod(self.positions -> data[l].d - pivotX, gridsize);
            else
                currentSnapX += dmod(self.positions -> data[l].d - pivotX, gridsize);
            if (dmod(self.positions -> data[l + 1].d - pivotX, gridsize) > gridsize * 0.5)
                currentSnapY += gridsize - dmod(self.positions -> data[l + 1].d - pivotY, gridsize);
            else
                currentSnapY += dmod(self.positions -> data[l + 1].d - pivotY, gridsize);
        }
        if (currentSnapX < bestSnapX) {
            bestSnapX = currentSnapX;
            bestSnapModX = pivotX;
        }
        if (currentSnapY < bestSnapY) {
            bestSnapY = currentSnapY;
            bestSnapModY = pivotY;
        }
    }
    for (int i = 1; i < m1; i += 3) {
        self.positions -> data[i] = (unitype) (round((self.positions -> data[i].d - bestSnapModX) / gridsize) * gridsize + bestSnapModX); // snap to grid
        self.positions -> data[i + 1] = (unitype) (round((self.positions -> data[i + 1].d - bestSnapModY) / gridsize) * gridsize + bestSnapModY);
    }
    self.screenX += j;
    self.screenY += k;
    j = 0;
    k = 0;
    for (int i = 1; i < m1; i += 3) {
        j += self.positions -> data[i].d;
        k += self.positions -> data[i + 1].d;
    }
    j /= m1 / 3; // average x and y positions
    k /= m1 / 3;
    for (int i = 1; i < m1; i += 3) {
        self.positions -> data[i] = (unitype) (self.positions -> data[i].d - j); // normalise to 0 again
        self.positions -> data[i + 1] = (unitype) (self.positions -> data[i + 1].d - k);
    }
    /* straight up genius algorithm, it will snap to closest but then also renormalise to 0,
    which ensures that files don't have wacky massive decimal number for the coordinates. It also puts the
    center of the mass at 0, 0 but the user does not notice the translation since their screen is translated
    by the same amount.
    */
    self.screenX += j;
    self.screenY += k;
    *selfp = self;
}
void selectionBox(logicgates *selfp, double x1, double y1, double x2, double y2) { // draws the selection box
    logicgates self = *selfp;
    turtlePenColor(self.themeColors[4 + self.theme], self.themeColors[5 + self.theme], self.themeColors[6 + self.theme]);
    turtlePenSize(self.globalsize * self.scaling);
    turtleGoto(x1, y1);
    turtlePenDown();
    turtleGoto(x1, y2);
    turtleGoto(x2, y2);
    turtleGoto(x2, y1);
    turtleGoto(x1, y1);
    turtlePenUp();
    if (x1 > x2) {
        self.sxmax = x1;
        self.sxmin = x2;
    } else {
        self.sxmax = x2;
        self.sxmin = x1;
    }
    if (y1 > y2) {
        self.symax = y1;
        self.symin = y2;
    } else {
        self.symax = y2;
        self.symin = y1;
    }
    *selfp = self;
}
void groupBox(logicgates *selfp, int groupID) {
    logicgates self = *selfp;
    // gather dimensions of box
    double minX = 100000000;
    double maxX = -100000000;
    double minY = 100000000;
    double maxY = -100000000;
    for (int i = 1; i < self.components -> length; i++) {
        if (self.groups -> data[i].i == groupID) {
            if (self.positions -> data[i * 3 - 2].d > maxX) {
                maxX = self.positions -> data[i * 3 - 2].d;
            }
            if (self.positions -> data[i * 3 - 2].d < minX) {
                minX = self.positions -> data[i * 3 - 2].d;
            }
            if (self.positions -> data[i * 3 - 1].d > maxY) {
                maxY = self.positions -> data[i * 3 - 1].d;
            }
            if (self.positions -> data[i * 3 - 1].d < minY) {
                minY = self.positions -> data[i * 3 - 1].d;
            }
            list_append(self.groupSelect, (unitype) i, 'i');
        }
    }
    double compWidthFactor = 16;
    minX = (minX + self.screenX - compWidthFactor) * self.globalsize;
    maxX = (maxX + self.screenX + compWidthFactor) * self.globalsize;
    minY = (minY + self.screenY - compWidthFactor) * self.globalsize;
    maxY = (maxY + self.screenY + compWidthFactor) * self.globalsize;
    turtlePenColor(self.themeColors[1 + self.theme], self.themeColors[2 + self.theme], self.themeColors[3 + self.theme]);
    turtlePenSize(self.globalsize * self.scaling * 0.25);
    turtleGoto(minX, minY);
    turtlePenDown();
    turtleGoto(minX, maxY);
    turtleGoto(maxX, maxY);
    turtleGoto(maxX, minY);
    turtleGoto(minX, minY);
    turtlePenUp();
    *selfp = self;
}
void hlgcompset(logicgates *selfp) { // sets hlgcomp to whatever component the mouse is hovering over
    logicgates self = *selfp;
    self.globalsize *= 0.75; // resizing
    self.hlgcomp = 0;
    int len = self.components -> length;
    for (int i = 1; i < len; i++) {
        if ((self.mx / self.globalsize - self.screenX + 18) > self.positions -> data[i * 3 - 2].d && (self.mx / self.globalsize - self.screenX - 18) < self.positions -> data[i * 3 - 2].d && (self.my / self.globalsize - self.screenY + 18) > self.positions -> data[i * 3 - 1].d && (self.my / self.globalsize - self.screenY - 18) < self.positions -> data[i * 3 - 1].d) {
            self.hlgcomp = i;
        }
    }
    list_clear(self.groupSelect);
    list_append(self.groupSelect, (unitype) 'n', 'c');
    if (self.hglmove == 0) {
        if (self.hlgcomp > 0 && self.groups -> data[self.hlgcomp].i > 0) {
            groupBox(&self, self.groups -> data[self.hlgcomp].i);
        }
    } else {
        if (self.groups -> data[self.hglmove].i > 0) {
            groupBox(&self, self.groups -> data[self.hglmove].i);
        }
    }
    self.globalsize /= 0.75;
    *selfp = self;
}
void rotateSelected(logicgates *selfp, double degrees) { // rotates selected components by degrees
    logicgates self = *selfp;
    double j = 0;
    double k = 0;
    int len = self.selected -> length;
    for (int i = 1; i < len; i++) {
        j += self.positions -> data[self.selected -> data[i].i * 3 - 2].d;
        k += self.positions -> data[self.selected -> data[i].i * 3 - 1].d;
    }
    j /= self.selected -> length - 1;
    k /= self.selected -> length - 1;
    double radians = degrees / 57.2958;
    len = self.selected -> length;
    for (int i = 1; i < len; i++) {
        double n = j + (self.positions -> data[self.selected -> data[i].i * 3 - 2].d - j) * (cos(radians)) - (self.positions -> data[self.selected -> data[i].i * 3 - 1].d - k) * (sin(radians));
        self.positions -> data[self.selected -> data[i].i * 3 - 1] = (unitype) (k + (self.positions -> data[self.selected -> data[i].i * 3 - 2].d - j) * (sin(radians)) + (self.positions -> data[self.selected -> data[i].i * 3 - 1].d - k) * (cos(radians)));
        self.positions -> data[self.selected -> data[i].i * 3 - 2] = (unitype) n;
        self.positions -> data[self.selected -> data[i].i * 3] = (unitype) (self.positions -> data[self.selected -> data[i].i * 3].d - degrees);
        if (self.positions -> data[self.selected -> data[i].i * 3].d < 0)
            self.positions -> data[self.selected -> data[i].i * 3] = (unitype) (self.positions -> data[self.selected -> data[i].i * 3].d + 360);
        if (self.positions -> data[self.selected -> data[i].i * 3].d > 360)
            self.positions -> data[self.selected -> data[i].i * 3] = (unitype) (self.positions -> data[self.selected -> data[i].i * 3].d - 360);
    }
    *selfp = self;
}
extern inline void bothLeft(logicgates *selfp, double sinRot, double cosRot, char isComp1, char comp1Above2, char comp1Upper, char comp2Upper) { // case 1: both input components are to the "left" of the destination (generalised left)
    logicgates self = *selfp;
    if (comp1Above2) { // if comp1 component is on top
        if (comp1Upper && !comp2Upper) {
            if (isComp1) {
                self.wxOffE = (cosRot * 4) * self.globalsize;
                self.wyOffE = (sinRot * -4) * self.globalsize;
            } else {
                self.wxOffE = (cosRot * -4) * self.globalsize;
                self.wyOffE = (sinRot * 4) * self.globalsize;
            }
        } else {
            if (isComp1) {
                self.wxOffE = (cosRot * 4) * self.globalsize;
                self.wyOffE = (sinRot * -4) * self.globalsize;
            } else {
                self.wxOffE = (cosRot * -4) * self.globalsize;
                self.wyOffE = (sinRot * 4) * self.globalsize;
            }
            if (comp1Upper) { // this necessarily means that y1 and y2 are above y3
                if (!isComp1) {
                    self.wxOffE += sinRot * 5 * self.globalsize;
                    self.wyOffE += cosRot * 5 * self.globalsize;
                }
            } else {
                if (isComp1) {
                    self.wxOffE += sinRot * 5 * self.globalsize;
                    self.wyOffE += cosRot * 5 * self.globalsize;
                }
            }
        }
    } else {
        if (comp2Upper && !comp1Upper) {
            if (isComp1) {
                self.wxOffE = (cosRot * -4) * self.globalsize;
                self.wyOffE = (sinRot * 4) * self.globalsize;
            } else {
                self.wxOffE = (cosRot * 4) * self.globalsize;
                self.wyOffE = (sinRot * -4) * self.globalsize;
            }
        } else {
            if (isComp1) {
                self.wxOffE = (cosRot * -4) * self.globalsize;
                self.wyOffE = (sinRot * 4) * self.globalsize;
            } else {
                self.wxOffE = (cosRot * 4) * self.globalsize;
                self.wyOffE = (sinRot * -4) * self.globalsize;
            }
            if (comp2Upper) {
                if (isComp1) {
                    self.wxOffE += sinRot * 5 * self.globalsize;
                    self.wyOffE += cosRot * 5 * self.globalsize;
                }
            } else {
                if (!isComp1) {
                    self.wxOffE += sinRot * 5 * self.globalsize;
                    self.wyOffE += cosRot * 5 * self.globalsize;
                }
            }
        }
    }
    *selfp = self;
}
extern inline void oneRight(logicgates *selfp, double sinRot, double cosRot, char isComp1, char comp1Right, char comp1Upper, char comp2Upper) { // case 2: one of the components is on the right side of the destination
    logicgates self = *selfp;
    if (comp1Right) { // right side always gets its preference
        if (isComp1) {
            if (comp1Upper) {
                self.wxOffE = (cosRot * 4) * self.globalsize;
                self.wyOffE = (sinRot * -4) * self.globalsize;
            } else {
                self.wxOffE = (cosRot * -4) * self.globalsize;
                self.wyOffE = (sinRot * 4) * self.globalsize;
            }
        } else {
            if (comp1Upper) {
                self.wxOffE = (cosRot * -4) * self.globalsize;
                self.wyOffE = (sinRot * 4) * self.globalsize;
            } else {
                self.wxOffE = (cosRot * 4) * self.globalsize;
                self.wyOffE = (sinRot * -4) * self.globalsize;
            }
            if ((comp1Upper && comp2Upper) || (!comp1Upper && !comp2Upper)) {
                self.wxOffE += sinRot * 5 * self.globalsize;
                self.wyOffE += cosRot * 5 * self.globalsize;
            }
        }
    } else { // left side does whichever the right doesn't
        if (isComp1) {
            if (comp2Upper) {
                self.wxOffE = (cosRot * -4) * self.globalsize;
                self.wyOffE = (sinRot * 4) * self.globalsize;
            } else {
                self.wxOffE = (cosRot * 4) * self.globalsize;
                self.wyOffE = (sinRot * -4) * self.globalsize;
            }
            if ((comp1Upper && comp2Upper) || (!comp1Upper && !comp2Upper)) {
                self.wxOffE += sinRot * 5 * self.globalsize;
                self.wyOffE += cosRot * 5 * self.globalsize;
            }
        } else {
            if (comp2Upper) {
                self.wxOffE = (cosRot * 4) * self.globalsize;
                self.wyOffE = (sinRot * -4) * self.globalsize;
            } else {
                self.wxOffE = (cosRot * -4) * self.globalsize;
                self.wyOffE = (sinRot * 4) * self.globalsize;
            }
        }
    }
    *selfp = self;
}
extern inline void bothRight(logicgates *selfp, double sinRot, double cosRot, char isComp1, char comp1Above2, char comp1Upper, char comp2Upper) { // case 3: both components are on the right side of the destination
    logicgates self = *selfp;
    if (comp1Above2) { // if comp1 component is on top
        if (comp1Upper && !comp2Upper) {
            if (isComp1) {
                self.wxOffE = (cosRot * 4) * self.globalsize;
                self.wyOffE = (sinRot * -4) * self.globalsize;
            } else {
                self.wxOffE = (cosRot * -4) * self.globalsize;
                self.wyOffE = (sinRot * 4) * self.globalsize;
            }
        } else {
            if (isComp1) {
                self.wxOffE = (cosRot * -4) * self.globalsize;
                self.wyOffE = (sinRot * 4) * self.globalsize;
            } else {
                self.wxOffE = (cosRot * 4) * self.globalsize;
                self.wyOffE = (sinRot * -4) * self.globalsize;
            }
            if (comp1Upper) {
                if (isComp1) {
                    self.wxOffE += sinRot * 5 * self.globalsize;
                    self.wyOffE += cosRot * 5 * self.globalsize;
                }
            } else {
                if (!isComp1) {
                    self.wxOffE += sinRot * 5 * self.globalsize;
                    self.wyOffE += cosRot * 5 * self.globalsize;
                }
            }
        }
    } else {
        if (comp2Upper && !comp1Upper) {
            if (isComp1) {
                self.wxOffE = (cosRot * -4) * self.globalsize;
                self.wyOffE = (sinRot * 4) * self.globalsize;
            } else {
                self.wxOffE = (cosRot * 4) * self.globalsize;
                self.wyOffE = (sinRot * -4) * self.globalsize;
            }
        } else {
            if (isComp1) {
                self.wxOffE = (cosRot * 4) * self.globalsize;
                self.wyOffE = (sinRot * -4) * self.globalsize;
            } else {
                self.wxOffE = (cosRot * -4) * self.globalsize;
                self.wyOffE = (sinRot * 4) * self.globalsize;
            }
            if (comp2Upper) {
                if (!isComp1) {
                    self.wxOffE += sinRot * 5 * self.globalsize;
                    self.wyOffE += cosRot * 5 * self.globalsize;
                }
            } else {
                if (isComp1) {
                    self.wxOffE += sinRot * 5 * self.globalsize;
                    self.wyOffE += cosRot * 5 * self.globalsize;
                }
            }
        }
    }
    *selfp = self;
}
void compareAng(logicgates *selfp, int index1, int index2, int comp1, int comp2, double x1, double y1, double x2, double y2, double x3, double y3, double rot) { // for comparisons between two components wired to one, to make sure their wires don't cross (works 95% of the time)
    logicgates self = *selfp;
    /*
    x1, y1: first component position
    x2, y2: second component position
    x3, y3: destination wire position
    */
    // turtlePenColor(255, 0, 0);
    // turtlePenSize(5);
    // turtleGoto(x1, y1);
    // turtlePenDown();
    // turtlePenUp();
    // turtlePenColor(0, 0, 255);
    // turtleGoto(x2, y2);
    // turtlePenDown();
    // turtlePenUp();
    // turtlePenColor(0, 255, 0);
    // turtleGoto(x3, y3);
    // turtlePenDown();
    // turtleGoto(x3 + sin(rot / 57.2958) * 10, y3 + cos(rot / 57.2958) * 10);
    // turtlePenUp();
    // turtlePenSize(2);
    double sinRot = sin(rot / 57.2958);
    double cosRot = cos(rot / 57.2958);
    double morphX1 = ((x3 - x1) * cosRot) - ((y3 - y1) * sinRot);
    double morphY1 = ((x3 - x1) * sinRot) - ((y3 - y1) * cosRot);
    double morphX2 = ((x3 - x2) * cosRot) - ((y3 - y2) * sinRot);
    double morphY2 = ((x3 - x2) * sinRot) - ((y3 - y2) * cosRot);
    // printf("x1: %.2lf x2: %.2lf x3: %.2lf\n", x1, x2, x3);
    char comp1Quad = 1;
    double comp1Ang = atan((x3 - x1) / (y3 - y1)) - rot / 57.2958;
    if (y1 > y3) {
        comp1Ang += 3.1415926;
    }
    if (sin(comp1Ang) < 0) {
        comp1Quad += 2;
        if (cos(comp1Ang) < 0) {
            comp1Quad += 1;
        }
    } else {
        if (cos(comp1Ang) > 0) {
            comp1Quad += 1;
        }
    }
    char comp2Quad = 1;
    double comp2Ang = atan((x3 - x2) / (y3 - y2)) - rot / 57.2958;
    if (y2 > y3) {
        comp2Ang += 3.1415926;
    }
    if (sin(comp2Ang) < 0) {
        comp2Quad += 2;
        if (cos(comp2Ang) < 0) {
            comp2Quad += 1;
        }
    } else {
        if (cos(comp2Ang) > 0) {
            comp2Quad += 1;
        }
    }
    double compareAngle = atan((x2 - x1) / (y2 - y1)) - rot / 57.2958;
    if (y1 > y2) {
        compareAngle += 3.1415926;
    }
    if ((comp1Quad == 2 || comp1Quad == 3) && (comp2Quad == 2 || comp2Quad == 3)) {
        bothLeft(&self, sinRot, cosRot, comp1 == self.wiring -> data[index1].i, sin(compareAngle) > 0, comp1Quad < 3, comp2Quad < 3);
    } else {
        if ((comp1Quad == 1 || comp1Quad == 4) && (comp2Quad == 1 || comp2Quad == 4)) {
            bothRight(&self, sinRot, cosRot, comp1 == self.wiring -> data[index1].i, sin(compareAngle) > 0, comp1Quad < 3, comp2Quad < 3);
        } else {
            oneRight(&self, sinRot, cosRot, comp1 == self.wiring -> data[index1].i, comp1Quad == 1 || comp1Quad == 4, comp1Quad < 3, comp2Quad < 3);
            // if (asin(morphX2 / (sqrt(morphY2 * morphY2 + morphX2 * morphX2) + 0.001)) > asin(morphX1 / (sqrt(morphY1 * morphY1 + morphX1 * morphX1) + 0.001))) {
            //     if (comp1 == self.wiring -> data[index1].i) { // check if this function is run on comp1 or comp2
            //         self.wxOffE = ((cosRot * -4) + 5) * self.globalsize;
            //         self.wyOffE = (sinRot * 4) * self.globalsize;
            //     } else {
            //         self.wxOffE = (cosRot * 4) * self.globalsize;
            //         self.wyOffE = (sinRot * -4) * self.globalsize;
            //     }
            // } else {
            //     if (comp1 == self.wiring -> data[index1].i) {
            //         self.wxOffE = ((cosRot * 4)) * self.globalsize;
            //         self.wyOffE = (sinRot * -4) * self.globalsize;
            //     } else {
            //         self.wxOffE = ((cosRot * -4) + 5) * self.globalsize;
            //         self.wyOffE = (sinRot * 4) * self.globalsize;
            //     }
            // }
        }
    }
    /* notes:
    x1, y1 - first (index1) position of wire origin
    x2, y2 - second (index2) position of wire origin
    x3, y3 - destination position of wire connection

    if both of the components are to the left of the destination, then we do comparison on the components y positions, and the one with the lower y gets the bottom connection, and the higher y gets the top
    extra criteria: if the y position of the lower y is below the destination y AND the y position of the upper y is above the destination y, then we remove the extra poke distance of the bottom connection (since wires will not be in danger of crossing)

    if one of the components is to the right of the destination, then it gets the connection closest to it (so if it's above y3, it gets the top one and below it gets the bottom)
    extra criteria: if the y position of the right side component compares to the destination y does not match the left side components y position when compared to the destination y, then we remove the extra poke distance of the bottom connection

    if both components are to the right of the destination, then we do comparison on the components y positions, and the one with the lower y gets the top connection, and the higher y gets the bottom connection
    extra criteria: if the y position of the lower y is below the destination y AND the y position of the upper y is above the destination y, then we remove the extra poke distance of the bottom connection AND we reverse the destination (the lower gets the lower, and upper gets the upper)

    generalised:
    a generalised "to the right of the destination" would be to compare the vectors
    */
    *selfp = self;
}
void wireIO(logicgates *selfp, int index1, int index2) { // this script actually performs the logic of the logic gates, this will update the output of a gate given its two inputs
    logicgates self = *selfp;
    if (strcmp(self.components -> data[self.wiring -> data[index1].i].s, "POWER") == 0) // if I didn't use strings this could be a switch statement, in fact not using strings would have lots of performance benefits but I also don't care
        self.io -> data[self.wiring -> data[index1].i * 3] = (unitype) (self.io -> data[self.wiring -> data[index1].i * 3 - 2].i || self.io -> data[self.wiring -> data[index1].i * 3 - 1].i);
    if (strcmp(self.components -> data[self.wiring -> data[index1].i].s, "BUFFER") == 0) {
        self.io -> data[self.wiring -> data[index1].i * 3] = self.io -> data[self.wiring -> data[index1].i * 3 - 1];
        self.io -> data[self.wiring -> data[index1].i * 3 - 1] = self.io -> data[self.wiring -> data[index1].i * 3 - 2];
    }
    if (strcmp(self.components -> data[self.wiring -> data[index1].i].s, "NOT") == 0)
        self.io -> data[self.wiring -> data[index1].i * 3] = (unitype) (!self.io -> data[self.wiring -> data[index1].i * 3 - 2].i);
    if (strcmp(self.components -> data[self.wiring -> data[index1].i].s, "AND") == 0)
        self.io -> data[self.wiring -> data[index1].i * 3] = (unitype) (self.io -> data[self.wiring -> data[index1].i * 3 - 2].i && self.io -> data[self.wiring -> data[index1].i * 3 - 1].i);
    if (strcmp(self.components -> data[self.wiring -> data[index1].i].s, "OR") == 0)
        self.io -> data[self.wiring -> data[index1].i * 3] = (unitype) (self.io -> data[self.wiring -> data[index1].i * 3 - 2].i || self.io -> data[self.wiring -> data[index1].i * 3 - 1].i);
    if (strcmp(self.components -> data[self.wiring -> data[index1].i].s, "XOR") == 0)
        self.io -> data[self.wiring -> data[index1].i * 3] = (unitype) abs(self.io -> data[self.wiring -> data[index1].i * 3 - 2].i - self.io -> data[self.wiring -> data[index1].i * 3 - 1].i);
    if (strcmp(self.components -> data[self.wiring -> data[index1].i].s, "NOR") == 0)
        self.io -> data[self.wiring -> data[index1].i * 3] = (unitype) (!(self.io -> data[self.wiring -> data[index1].i * 3 - 2].i || self.io -> data[self.wiring -> data[index1].i * 3 - 1].i));
    if (strcmp(self.components -> data[self.wiring -> data[index1].i].s, "NAND") == 0)
        self.io -> data[self.wiring -> data[index1].i * 3] = (unitype) (!(self.io -> data[self.wiring -> data[index1].i * 3 - 2].i && self.io -> data[self.wiring -> data[index1].i * 3 - 1].i));
    self.wiring -> data[index1 + 2] = self.io -> data[self.wiring -> data[index1].i * 3];
    if (self.inpComp -> data[self.wiring -> data[index2].i * 3 - 1].i == self.wiring -> data[index1].i)
        self.io -> data[self.wiring -> data[index1 + 1].i * 3 - 2] = self.io -> data[self.wiring -> data[index1].i * 3];
    else
        self.io -> data[self.wiring -> data[index1 + 1].i * 3 - 1] = self.io -> data[self.wiring -> data[index1].i * 3];
    if (self.compSlots -> data[list_find(self.compSlots, self.components -> data[self.wiring -> data[index2].i], 's') + 1].i == 2) {
        if (self.inpComp -> data[self.wiring -> data[index2].i * 3].i == 0) {
            self.wxOffE = 0;
            self.wyOffE = 0;
        } else {
            int tempAng = self.inpComp -> data[self.wiring -> data[index2].i * 3 - 1].i * 3;
            int tempAng2 = self.inpComp -> data[self.wiring -> data[index2].i * 3].i * 3;
            int tempAng3 = self.wiring -> data[index2].i * 3;
            compareAng(&self, index1, index2, self.inpComp -> data[self.wiring -> data[index2].i * 3 - 1].i, 
            self.inpComp -> data[self.wiring -> data[index2].i * 3 - 2].i, 
            (self.positions -> data[tempAng - 2].d + self.screenX + sin(self.positions -> data[tempAng].d / 57.2958) * 22.5) * self.globalsize, 
            (self.positions -> data[tempAng - 1].d + self.screenY + cos(self.positions -> data[tempAng].d / 57.2958) * 22.5) * self.globalsize, 
            (self.positions -> data[tempAng2 - 2].d + self.screenX + sin(self.positions -> data[tempAng2].d / 57.2958) * 22.5) * self.globalsize, 
            (self.positions -> data[tempAng2 - 1].d + self.screenY + cos(self.positions -> data[tempAng2].d / 57.2958) * 22.5) * self.globalsize, 
            (self.positions -> data[tempAng3 - 2].d + self.screenX - sin(self.positions -> data[tempAng3].d / 57.2958) * 22.5) * self.globalsize, 
            (self.positions -> data[tempAng3 - 1].d + self.screenY - cos(self.positions -> data[tempAng3].d / 57.2958) * 22.5) * self.globalsize, 
            self.positions -> data[tempAng3].d);
        }
    } else {
        self.wxOffE = 0;
        self.wyOffE = 0;
    }
    *selfp = self;
}
void renderComp(logicgates *selfp) { // this function renders all the components in the window
    logicgates self = *selfp;
    list_clear(self.selectOb);
    list_append(self.selectOb, (unitype) "null", 's');
    list_t *memo = list_init();
    list_t *selectedGroups = list_init();
    int len = self.components -> length;
    for (int i = 1; i < len; i++) {
        double renderX = (self.positions -> data[i * 3 - 2].d + self.screenX) * self.globalsize * 0.75;
        double renderY = (self.positions -> data[i * 3 - 1].d + self.screenY) * self.globalsize * 0.75;
        if (list_count(self.selected, (unitype) i, 'i') > 0 || (renderX + 12 * self.globalsize > self.sxmin && renderX + -12 * self.globalsize < self.sxmax && renderY + 12 * self.globalsize > self.symin && renderY + -12 * self.globalsize < self.symax && self.selecting == 1)) {
            if (self.groups -> data[i].i != -1 && list_count(selectedGroups, self.groups -> data[i], 'i') == 0) {
                // printf("add %d\n", self.groups -> data[i].i);
                list_append(selectedGroups, self.groups -> data[i], 'i');
            }
            list_append(memo, (unitype) 1, 'i');
        } else {
            list_append(memo, (unitype) 0, 'i');
        }
    }
    for (int i = 1; i < len; i++) {
        double renderX = (self.positions -> data[i * 3 - 2].d + self.screenX) * self.globalsize * 0.75;
        double renderY = (self.positions -> data[i * 3 - 1].d + self.screenY) * self.globalsize * 0.75;
        char inGroup = self.groups -> data[i].i != -1 && list_count(selectedGroups, self.groups -> data[i], 'i') > 0;
        if (inGroup || memo -> data[i - 1].i) {
            list_append(self.selectOb, (unitype) i, 'i');
            turtlePenColor(self.themeColors[4 + self.theme], self.themeColors[5 + self.theme], self.themeColors[6 + self.theme]);
        } else {
            turtlePenColor(self.themeColors[1 + self.theme], self.themeColors[2 + self.theme], self.themeColors[3 + self.theme]);
        }
        if (renderX + 15 * self.globalsize > -240 && renderX + -15 * self.globalsize < 240 && renderY + 15 * self.globalsize > -180 && renderY + -15 * self.globalsize < 180) {
            if (strcmp(self.components -> data[i].s, "POWER") == 0) {
                if (self.io -> data[i * 3 - 1].i == 1) {
                    if (list_count(self.selectOb, (unitype) i, 'i') > 0)
                        POWER(&self, renderX, renderY, self.globalsize, self.positions -> data[i * 3].d, 1, 1);
                    else
                        POWER(&self, renderX, renderY, self.globalsize, self.positions -> data[i * 3].d, 1, 0);
                } else {
                    if (self.io -> data[i * 3 - 2].i == 1) {
                        if (list_count(self.selectOb, (unitype) i, 'i') > 0)
                            POWER(&self, renderX, renderY, self.globalsize, self.positions -> data[i * 3].d, 2, 1);
                        else
                            POWER(&self, renderX, renderY, self.globalsize, self.positions -> data[i * 3].d, 2, 0);
                    } else {
                        if (list_count(self.selectOb, (unitype) i, 'i') > 0)
                            POWER(&self, renderX, renderY, self.globalsize, self.positions -> data[i * 3].d, 0, 1);
                        else
                            POWER(&self, renderX, renderY, self.globalsize, self.positions -> data[i * 3].d, 0, 0);
                    }
                }
            }
            if (strcmp(self.components -> data[i].s, "AND") == 0)
                AND(&self, renderX, renderY, self.globalsize, self.positions -> data[i * 3].d);
            if (strcmp(self.components -> data[i].s, "OR") == 0)
                OR(&self, renderX, renderY, self.globalsize, self.positions -> data[i * 3].d);
            if (strcmp(self.components -> data[i].s, "NOT") == 0)
                NOT(&self, renderX, renderY, self.globalsize, self.positions -> data[i * 3].d);
            if (strcmp(self.components -> data[i].s, "XOR") == 0)
                XOR(&self, renderX, renderY, self.globalsize, self.positions -> data[i * 3].d);
            if (strcmp(self.components -> data[i].s, "NOR") == 0)
                NOR(&self, renderX, renderY, self.globalsize, self.positions -> data[i * 3].d);
            if (strcmp(self.components -> data[i].s, "NAND") == 0)
                NAND(&self, renderX, renderY, self.globalsize, self.positions -> data[i * 3].d);
            if (strcmp(self.components -> data[i].s, "BUFFER") == 0)
                BUFFER(&self, renderX, renderY, self.globalsize, self.positions -> data[i * 3].d);
        }
    }
    list_free(selectedGroups);
    *selfp = self;
}
void renderWire(logicgates *selfp, double size) { // this function renders all the wiring in the window (a bit buggy if the components are outside the window, it doesn't do line intercepts and is likely bounded by total screen size but if I were to do bound intercepts I would do it in the turtle abstration)
    logicgates self = *selfp;
    self.globalsize *= 0.75; // um just resizing no big deal
    turtlePenSize(size * self.scaling);
    int len = self.wiring -> length - 1;
    for (int i = 1; i < len; i += 3) {
        wireIO(&self, i, i + 1); // it was, in hindsight, a bad idea to do logic in the render function, but i guess it's fine
        if (self.wireMode != 2) {
            double wireTXS = (self.positions -> data[self.wiring -> data[i].i * 3 - 2].d + self.screenX) * self.globalsize;
            double wireTYS = (self.positions -> data[self.wiring -> data[i].i * 3 - 1].d + self.screenY) * self.globalsize;
            turtleGoto(wireTXS, wireTYS);
            if (self.wiring -> data[i + 2].i == 1) {
                if (list_count(self.selectOb, self.wiring -> data[i], 'i') > 0 || list_count(self.selectOb, self.wiring -> data[i + 1], 'i') > 0 || list_count(self.selected, self.wiring -> data[i], 'i') > 0 || list_count(self.selected, self.wiring -> data[i + 1], 'i') > 0)
                    turtlePenColor(self.themeColors[10 + self.theme], self.themeColors[11 + self.theme], self.themeColors[12 + self.theme]);
                else
                    turtlePenColor(self.themeColors[7 + self.theme], self.themeColors[8 + self.theme], self.themeColors[9 + self.theme]);
            } else {
                if (list_count(self.selectOb, self.wiring -> data[i], 'i') > 0 || list_count(self.selectOb, self.wiring -> data[i + 1], 'i') > 0 || list_count(self.selected, self.wiring -> data[i], 'i') > 0 || list_count(self.selected, self.wiring -> data[i + 1], 'i') > 0)
                    turtlePenColor(self.themeColors[4 + self.theme], self.themeColors[5 + self.theme], self.themeColors[6 + self.theme]);
                else
                    turtlePenColor(self.themeColors[1 + self.theme], self.themeColors[2 + self.theme], self.themeColors[3 + self.theme]);
            }
            turtlePenDown();
            wireTXS += sin((self.positions -> data[self.wiring -> data[i].i * 3].d) / 57.2958) * 22.5 * self.globalsize;
            wireTYS += cos((self.positions -> data[self.wiring -> data[i].i * 3].d) / 57.2958) * 22.5 * self.globalsize;
            turtleGoto(wireTXS, wireTYS);
            double wireRot = (self.positions -> data[self.wiring -> data[i + 1].i * 3].d) / 57.2958; // direction of destination component (radians)
            double wireTXE = (self.positions -> data[self.wiring -> data[i + 1].i * 3 - 2].d + self.screenX) * self.globalsize - (sin(wireRot) * 22.5 * self.globalsize + self.wxOffE); // x position of destination component
            double wireTYE = (self.positions -> data[self.wiring -> data[i + 1].i * 3 - 1].d + self.screenY) * self.globalsize - (cos(wireRot) * 22.5 * self.globalsize + self.wyOffE); // y position of destination component
            double distance = (wireTXE - wireTXS) * sin(wireRot) + (wireTYE - wireTYS) * cos(wireRot);
            if (self.wireMode == 0)
                turtleGoto(wireTXS + distance * sin(wireRot), wireTYS + distance * cos(wireRot));
            turtleGoto(wireTXE, wireTYE);
            distance = (sin(wireRot) * 22.5 * self.globalsize + self.wxOffE) * sin(wireRot) + (cos(wireRot) * 22.5 * self.globalsize + self.wyOffE) * cos(wireRot);
            if (self.wireMode == 0)
                turtleGoto(wireTXE + distance * sin(wireRot), wireTYE + distance * cos(wireRot));
            turtleGoto(wireTXE + (sin(wireRot) * 22.5 * self.globalsize + self.wxOffE), wireTYE + (cos(wireRot) * 22.5 * self.globalsize + self.wyOffE));
            turtlePenUp();
            turtlePenColor(self.themeColors[1 + self.theme], self.themeColors[2 + self.theme], self.themeColors[3 + self.theme]);
        }
    }
    self.globalsize /= 0.75;
    *selfp = self;
}
void renderSidebar(logicgates *selfp, char side) { // this function draws the sidebar, but really its never on the side it's a bottom or top bar
    logicgates self = *selfp;
    turtlePenColorAlpha(self.themeColors[13 + self.theme], self.themeColors[14 + self.theme], self.themeColors[15 + self.theme], 55);
    turtlePenSize(30 * self.scaling);
    self.boundXmin = -241;
    self.boundXmax = 241;
    self.boundYmin = -181;
    self.boundYmax = 169; // cut off ribbon
    if (side == 1 || side == 2) {
        double i = 155 - (side % 2) * 305;
        if (i > 0) {
            self.boundYmax = 120 - (side % 2) * 240;
        } else {
            self.boundYmin = 120 - (side % 2) * 240;
        }
        turtleGoto(-280, i);
        turtlePenDown();
        turtleGoto(280, i);
        turtlePenUp();
        double j = -200;
        if (strcmp(self.holding,"POWER") == 0 && self.indicators == 1) {
            turtlePenColor(self.themeColors[16 + self.theme], self.themeColors[17 + self.theme], self.themeColors[18 + self.theme]);
        } else {
            turtlePenColor(self.themeColors[1 + self.theme], self.themeColors[2 + self.theme], self.themeColors[3 + self.theme]);
        }
        POWER(&self, j, i, 1.5, 90, 0, 1);
        j += 50;
        if (strcmp(self.holding,"NOT") == 0 && self.indicators == 1) {
            turtlePenColor(self.themeColors[16 + self.theme], self.themeColors[17 + self.theme], self.themeColors[18 + self.theme]);
        } else {
            turtlePenColor(self.themeColors[1 + self.theme], self.themeColors[2 + self.theme], self.themeColors[3 + self.theme]);
        }
        NOT(&self, j, i, 1.5, 90);
        j += 50;
        if (strcmp(self.holding,"AND") == 0 && self.indicators == 1) {
            turtlePenColor(self.themeColors[16 + self.theme], self.themeColors[17 + self.theme], self.themeColors[18 + self.theme]);
        } else {
            turtlePenColor(self.themeColors[1 + self.theme], self.themeColors[2 + self.theme], self.themeColors[3 + self.theme]);
        }
        AND(&self, j, i, 1.5, 90);
        j += 50;
        if (strcmp(self.holding,"OR") == 0 && self.indicators == 1) {
            turtlePenColor(self.themeColors[16 + self.theme], self.themeColors[17 + self.theme], self.themeColors[18 + self.theme]);
        } else {
            turtlePenColor(self.themeColors[1 + self.theme], self.themeColors[2 + self.theme], self.themeColors[3 + self.theme]);
        }
        OR(&self, j, i, 1.5, 90);
        j += 50;
        if (strcmp(self.holding,"XOR") == 0 && self.indicators == 1) {
            turtlePenColor(self.themeColors[16 + self.theme], self.themeColors[17 + self.theme], self.themeColors[18 + self.theme]);
        } else {
            turtlePenColor(self.themeColors[1 + self.theme], self.themeColors[2 + self.theme], self.themeColors[3 + self.theme]);
        }
        XOR(&self, j, i, 1.5, 90);
        j += 50;
        if (strcmp(self.holding,"NOR") == 0 && self.indicators == 1) {
            turtlePenColor(self.themeColors[16 + self.theme], self.themeColors[17 + self.theme], self.themeColors[18 + self.theme]);
        } else {
            turtlePenColor(self.themeColors[1 + self.theme], self.themeColors[2 + self.theme], self.themeColors[3 + self.theme]);
        }
        NOR(&self, j, i, 1.5, 90);
        j += 50;
        if (strcmp(self.holding,"NAND") == 0 && self.indicators == 1) {
            turtlePenColor(self.themeColors[16 + self.theme], self.themeColors[17 + self.theme], self.themeColors[18 + self.theme]);
        } else {
            turtlePenColor(self.themeColors[1 + self.theme], self.themeColors[2 + self.theme], self.themeColors[3 + self.theme]);
        }
        NAND(&self, j, i, 1.5, 90);
        j += 50;
        if (strcmp(self.holding,"BUFFER") == 0 && self.indicators == 1) {
            turtlePenColor(self.themeColors[16 + self.theme], self.themeColors[17 + self.theme], self.themeColors[18 + self.theme]);
        } else {
            turtlePenColor(self.themeColors[1 + self.theme], self.themeColors[2 + self.theme], self.themeColors[3 + self.theme]);
        }
        BUFFER(&self, j, i, 1.5, 90);
        j += 45;
        if ((self.keys[1] || self.wireHold == 1) && self.indicators == 1) {
            turtlePenColor(self.themeColors[16 + self.theme], self.themeColors[17 + self.theme], self.themeColors[18 + self.theme]);
        } else {
            turtlePenColor(self.themeColors[1 + self.theme], self.themeColors[2 + self.theme], self.themeColors[3 + self.theme]);
        }
        wireSymbol(&self, j, i, 1.5, 90);
    }
    turtlePenColor(self.themeColors[1 + self.theme], self.themeColors[2 + self.theme], self.themeColors[3 + self.theme]);
    *selfp = self;
}
void mouseTick(logicgates *selfp) { // all the functionality for the mouse is handled in this beast of a function, it's an absolute mess
    logicgates self = *selfp;
    self.globalsize *= 0.75; // resizing
    char updateQue = 0; // que for update undo
    if (turtleMouseDown()) {
        if (!(self.keys[0])) {
            self.keys[0] = 1;
            if (self.mx > self.boundXmin && self.mx < self.boundXmax && self.my > self.boundYmin && self.my < self.boundYmax) {
                self.mouseType = 0;
                if (!(self.selecting == 2) && !(list_count(self.selected, (unitype) self.hlgcomp, 'i') > 0) && !((self.keys[1] || self.wireHold == 1) && !(self.hlgcomp == 0))) { // deselect
                    self.wireHold = 0;
                    self.selecting = 0;
                    list_clear(self.selectOb);
                    list_append(self.selectOb, (unitype) "null", 's');
                    list_clear(self.selected);
                    list_append(self.selected, (unitype) "null", 's');
                }
                if (self.keys[2]) { // left shift key or s key
                    if (strcmp(self.holding, "a") == 0 || strcmp(self.holding, "b") == 0) {
                        self.selecting = 1;
                        list_clear(self.selectOb);
                        list_append(self.selectOb, (unitype) "null", 's');
                        self.selectX = self.mx;
                        self.selectY = self.my;
                    } else {
                        list_append(self.components, (unitype) self.holding, 's');
                        list_append(self.groups, (unitype) -1, 'i');
                        list_append(self.positions, (unitype) (self.mx / self.globalsize - self.screenX), 'd');
                        list_append(self.positions, (unitype) (self.my / self.globalsize - self.screenY), 'd');
                        list_append(self.positions, (unitype) self.holdingAng, 'd');
                        list_append(self.io, (unitype) 0, 'i');
                        list_append(self.io, (unitype) 0, 'i');
                        list_append(self.io, (unitype) 0, 'i');
                        list_append(self.inpComp, self.compSlots -> data[list_find(self.compSlots, (unitype) self.holding, 's') + 1], 'i');
                        list_append(self.inpComp, (unitype) 0, 'i');
                        list_append(self.inpComp, (unitype) 0, 'i');
                        self.holding = "b";
                        // update undo
                        addUndo(&self);
                    }
                } else {
                    if (strcmp(self.holding, "a") == 0 || strcmp(self.holding, "b") == 0) {
                        if (!(self.hlgcomp == 0)) {
                            if (self.keys[1] || self.wireHold == 1) {
                                self.wiringStart = self.hlgcomp;
                            } else {
                                self.hglmove = self.hlgcomp;
                                self.tempX = self.positions -> data[self.hglmove * 3 - 2].d;
                                self.tempY = self.positions -> data[self.hglmove * 3 - 1].d;
                                self.offX = self.positions -> data[self.hglmove * 3 - 2].d - (self.mx / self.globalsize - self.screenX);
                                self.offY = self.positions -> data[self.hglmove * 3 - 1].d - (self.my / self.globalsize - self.screenY);
                            }
                            if (list_count(self.selectOb, (unitype) self.hlgcomp, 'i') > 0) {
                                if (self.selecting == 2) {
                                    self.selecting = 3;
                                    list_clear(self.selected);
                                    list_append(self.selected, (unitype) "null", 's');
                                    int len = self.selectOb -> length;
                                    for (int i = 1; i < len; i++) {
                                        list_append(self.selected, self.selectOb -> data[i], 'i');
                                    }
                                }
                            } else {
                                if (!(self.selecting == 3) && !((self.keys[1] || self.wireHold == 1) && !(self.hlgcomp == 0))) {
                                    self.wireHold = 0;
                                    self.selecting = 0;
                                    list_clear(self.selectOb);
                                    list_append(self.selectOb, (unitype) "null", 's');
                                    list_clear(self.selected);
                                    list_append(self.selected, (unitype) "null", 's');
                                }
                            }
                        }
                        if (strcmp(self.holding, "b") == 0) {
                            self.holding = "a";
                        }
                    } else {
                        list_append(self.components, (unitype) self.holding, 's');
                        list_append(self.groups, (unitype) -1, 'i');
                        list_append(self.positions, (unitype) (self.mx / self.globalsize - self.screenX), 'd');
                        list_append(self.positions, (unitype) (self.my / self.globalsize - self.screenY), 'd');
                        list_append(self.positions, (unitype) self.holdingAng, 'd');
                        list_append(self.io, (unitype) 0, 'i');
                        list_append(self.io, (unitype) 0, 'i');
                        list_append(self.io, (unitype) 0, 'i');
                        list_append(self.inpComp, self.compSlots -> data[list_find(self.compSlots, (unitype) self.holding, 's') + 1], 'i');
                        list_append(self.inpComp, (unitype) 0, 'i');
                        list_append(self.inpComp, (unitype) 0, 'i');
                        self.holding = "b";
                        // update undo
                        addUndo(&self);
                    }
                    self.FocalX = self.mx;
                    self.FocalY = self.my;
                    self.FocalCSX = self.screenX;
                    self.FocalCSY = self.screenY;
                    if (!(self.selecting == 3) && !((self.keys[1] || self.wireHold == 1) && !(self.hlgcomp == 0))) {
                        self.wireHold = 0;
                        self.selecting = 0;
                        list_clear(self.selectOb);
                        list_append(self.selectOb, (unitype) "null", 's');
                        list_clear(self.selected);
                        list_append(self.selected, (unitype) "null", 's');
                    }
                }
            } else {
                if (self.my > 169) { // on ribbon
                    // printf("on ribbon\n");
                    self.mouseType = 3;
                    self.FocalX = self.mx;
                    self.FocalY = self.my;
                    self.FocalCSX = self.screenX;
                    self.FocalCSY = self.screenY;
                    self.selecting = 0;
                    self.sxmax = 0;
                    self.symax = 0;
                    self.sxmin = 0;
                    self.symin = 0;
                } else { // on sidebar
                    self.mouseType = 1;
                    self.FocalX = self.mx;
                    self.FocalY = self.my;
                    self.FocalCSX = self.screenX;
                    self.FocalCSY = self.screenY;
                    self.selecting = 0;
                    self.sxmax = 0;
                    self.symax = 0;
                    self.sxmin = 0;
                    self.symin = 0;
                    list_clear(self.selectOb);
                    list_append(self.selectOb, (unitype) "null", 's');
                    list_clear(self.selected);
                    list_append(self.selected, (unitype) "null", 's');
                    if (self.mx > 168) {
                        if (self.wireHold == 1)
                            self.wireHold = 0;
                        else
                            self.wireHold = 1;
                    } else {
                        if (self.mx > -220) {
                            char *holdingTemp = self.compSlots -> data[(int) round((self.mx + 245) / 48) * 2 - 1].s;
                            if (strcmp(self.holding, holdingTemp) == 0) {
                                self.holding = "a";
                            } else {
                                self.holding = holdingTemp;
                            }
                        }
                    }
                }
            }
        }
        if (self.mouseType == 1 && self.mx > self.boundXmin && self.mx < self.boundXmax && self.my > self.boundYmin && self.my < self.boundYmax)
            self.mouseType = 2;
        if (self.keys[2] && self.selecting == 1) { // left shift key or s key
            self.selectX2 = self.mx;
            self.selectY2 = self.my;
            selectionBox(&self, self.selectX, self.selectY, self.selectX2, self.selectY2);
            if (fabs(self.selectX - self.mx) > 4 || fabs(self.selectY - self.my) > 4) {
                list_clear(self.selectOb);
                list_append(self.selectOb, (unitype) "null", 's');
                list_clear(self.selected);
                list_append(self.selected, (unitype) "null", 's');
            } else {
                if (list_count(self.selected, (unitype) self.hlgcomp, 'i') > 0) {
                    self.sxmax = 0;
                    self.symax = 0;
                    self.sxmin = 0;
                    self.symin = 0;
                    if (!(list_count(self.deleteQueue, (unitype) self.hlgcomp, 'i') > 0))
                        list_append(self.deleteQueue, (unitype) self.hlgcomp, 'i');
                }
            }
        } else {
            if (self.selecting == 1) {
                self.FocalX = self.mx;
                self.FocalY = self.my;
                self.FocalCSX = self.screenX;
                self.FocalCSY = self.screenY;
                self.selecting = 0;
                self.sxmax = 0;
                self.symax = 0;
                self.sxmin = 0;
                self.symin = 0;
            }
            if ((self.keys[1] || self.wireHold == 1) && !(self.wiringStart == 0)) {
                if (list_count(self.selected, (unitype) self.wiringStart, 'i') > 0 || list_count(self.selected, (unitype) self.hlgcomp, 'i') > 0)
                    turtlePenColor(self.themeColors[4 + self.theme], self.themeColors[5 + self.theme], self.themeColors[6 + self.theme]);
                else
                    turtlePenColor(self.themeColors[1 + self.theme], self.themeColors[2 + self.theme], self.themeColors[3 + self.theme]);
                turtlePenSize(self.globalsize * self.scaling / 0.75);
                if (list_count(self.selected, (unitype) self.wiringStart, 'i') > 0 && self.selecting > 2) {
                    int len = self.selected -> length;
                    for (int i = 1; i < len; i++) {
                        turtleGoto((self.positions -> data[self.selected -> data[i].i * 3 - 2].d + self.screenX) * self.globalsize, (self.positions -> data[self.selected -> data[i].i * 3 - 1].d + self.screenY) * self.globalsize);
                        turtlePenDown();
                        if ((!(self.hlgcomp == 0 && !(self.hlgcomp == self.wiringStart))) && (self.inpComp -> data[self.wiringEnd * 3 - 1].i == 0 || (self.inpComp -> data[self.wiringEnd * 3].i == 0 && !(self.inpComp -> data[self.wiringEnd * 3 - 1].i == self.wiringStart && self.inpComp -> data[self.wiringEnd * 3 - 2].i > 1))))
                            turtleGoto((self.positions -> data[self.hlgcomp * 3 - 2].d + self.screenX) * self.globalsize, (self.positions -> data[self.hlgcomp * 3 - 1].d + self.screenY) * self.globalsize);
                        else
                            turtleGoto(self.mx, self.my);
                        turtlePenUp();
                    }
                } else {
                    if (list_count(self.selected, (unitype) self.hlgcomp, 'i') > 0 && self.selecting > 1) {
                        int len = self.selected -> length;
                        for (int i = 1; i < len; i++) {
                            turtleGoto((self.positions -> data[self.wiringStart * 3 - 2].d + self.screenX) * self.globalsize, (self.positions -> data[self.wiringStart * 3 - 1].d + self.screenY) * self.globalsize);
                            turtlePenDown();
                            turtleGoto((self.positions -> data[self.selected -> data[i].i * 3 - 2].d + self.screenX) * self.globalsize, (self.positions -> data[self.selected -> data[i].i * 3 - 1].d + self.screenY) * self.globalsize);
                            turtlePenUp();
                        }
                    } else {
                        turtleGoto((self.positions -> data[self.wiringStart * 3 - 2].d + self.screenX) * self.globalsize, (self.positions -> data[self.wiringStart * 3 - 1].d + self.screenY) * self.globalsize);
                        turtlePenDown();
                        if ((self.hlgcomp != 0) && (self.wiringEnd > 0) && (self.hlgcomp != self.wiringStart) && (self.inpComp -> data[self.wiringEnd * 3 - 1].i == 0 || (self.inpComp -> data[self.wiringEnd * 3].i == 0 && (self.inpComp -> data[self.wiringEnd * 3 - 1].i != self.wiringStart) && self.inpComp -> data[self.wiringEnd * 3 - 2].i > 1)))
                            turtleGoto((self.positions -> data[self.hlgcomp * 3 - 2].d + self.screenX) * self.globalsize, (self.positions -> data[self.hlgcomp * 3 - 1].d + self.screenY) * self.globalsize);
                        else
                            turtleGoto(self.mx, self.my);
                        turtlePenUp();
                    }
                }
            }
            if (self.mouseType != 3) {
                if (self.hglmove == 0) {
                    if (self.keys[1] || self.wireHold == 1) {
                        self.FocalX = self.mx;
                        self.FocalY = self.my;
                        self.FocalCSX = self.screenX;
                        self.FocalCSY = self.screenY;
                        self.wiringEnd = self.hlgcomp;
                    } else {
                        if (strcmp(self.holding, "a") == 0) {
                            self.screenX = (self.mx - self.FocalX) / self.globalsize + self.FocalCSX;
                            self.screenY = (self.my - self.FocalY) / self.globalsize + self.FocalCSY;
                        }
                    }
                } else {
                    if (self.selecting == 3) {
                        double anchorX = self.positions -> data[self.hglmove * 3 - 2].d;
                        double anchorY = self.positions -> data[self.hglmove * 3 - 1].d;
                        int len = self.selected -> length;
                        for (int i = 1; i < len; i++) {
                            self.positions -> data[self.selected -> data[i].i * 3 - 2] = (unitype) (self.positions -> data[self.selected -> data[i].i * 3 - 2].d + self.mx / self.globalsize - self.screenX + self.offX - anchorX);
                            self.positions -> data[self.selected -> data[i].i * 3 - 1] = (unitype) (self.positions -> data[self.selected -> data[i].i * 3 - 1].d + self.my / self.globalsize - self.screenY + self.offY - anchorY);
                        }
                    } else {
                        if (self.groupSelect -> length > 1) {
                            double anchorX = self.positions -> data[self.hglmove * 3 - 2].d;
                            double anchorY = self.positions -> data[self.hglmove * 3 - 1].d;
                            for (int i = 1; i < self.groupSelect -> length; i++) {
                                self.positions -> data[self.groupSelect -> data[i].i * 3 - 2] = (unitype) (self.positions -> data[self.groupSelect -> data[i].i * 3 - 2].d + self.mx / self.globalsize - self.screenX + self.offX - anchorX);
                                self.positions -> data[self.groupSelect -> data[i].i * 3 - 1] = (unitype) (self.positions -> data[self.groupSelect -> data[i].i * 3 - 1].d + self.my / self.globalsize - self.screenY + self.offY - anchorY);
                            }
                        } else {
                            self.positions -> data[self.hglmove * 3 - 2] = (unitype) (self.mx / self.globalsize - self.screenX + self.offX);
                            self.positions -> data[self.hglmove * 3 - 1] = (unitype) (self.my / self.globalsize - self.screenY + self.offY);
                        }
                    }
                    self.movingItems = 1;
                }
            }
        }
    } else {
        if (self.movingItems == 1) {
            self.movingItems = 0;
            // que for update
            updateQue = 1;
        }
        if (!(self.mx > self.boundXmin && self.mx < self.boundXmax && self.my > self.boundYmin && self.my < self.boundYmax) && self.hglmove > 0) {
            if (self.selecting > 1 && self.selected -> length > 1 && (strcmp(self.holding, "a") == 0 || strcmp(self.holding, "b") == 0)) {
                int len = self.selected -> length - 1;
                for (int i = 0; i < len; i++) {
                    deleteComp(&self, self.selected -> data[1].i, 0);
                }
                self.selecting = 0;
                list_clear(self.selectOb);
                list_append(self.selectOb, (unitype) "null", 's');
            } else {
                deleteComp(&self, self.hglmove, 0);
            }
        }
        if (self.mouseType == 2 && !(strcmp(self.holding, "a") == 0) && !(strcmp(self.holding, "b") == 0)) {
            self.mouseType = 0;
            if (self.mx > self.boundXmin && self.mx < self.boundXmax && self.my > self.boundYmin && self.my < self.boundYmax) {
                list_append(self.components, (unitype) self.holding, 's');
                list_append(self.groups, (unitype) -1, 'i');
                list_append(self.positions, (unitype) (self.mx / self.globalsize - self.screenX), 'd');
                list_append(self.positions, (unitype) (self.my / self.globalsize - self.screenY), 'd');
                list_append(self.positions, (unitype) self.holdingAng, 'd');
                list_append(self.io, (unitype) 0, 'i');
                list_append(self.io, (unitype) 0, 'i');
                list_append(self.io, (unitype) 0, 'i');
                list_append(self.inpComp, self.compSlots -> data[list_find(self.compSlots, (unitype) self.holding, 's') + 1], 'i');
                list_append(self.inpComp, (unitype) 0, 'i');
                list_append(self.inpComp, (unitype) 0, 'i');
                self.holding = "b";
                // update undo
                addUndo(&self);
            } else {
                self.holding = "a";
            }
        }
        int len = self.deleteQueue -> length - 1;
        for (int i = 0; i < len; i++) {
            if (list_count(self.selected, self.deleteQueue -> data[1], 'i') > 0 && list_count(self.selectOb, self.deleteQueue -> data[1], 'i') > 0) {
                list_remove(self.selected, self.deleteQueue -> data[1], 'i');
                list_remove(self.selectOb, self.deleteQueue -> data[1], 'i');
                list_delete(self.deleteQueue, 1);
            }
        }
        if (self.selecting == 1) {
            self.selecting = 2;
            list_clear(self.selected);
            list_append(self.selected, (unitype) "null", 's');
            len = self.selectOb -> length;
            for (int i = 1; i < len; i++) {
                list_append(self.selected, self.selectOb -> data[i], 'i');
            }
            if (!(self.selectX == self.mx) || !(self.selectY == self.my)) {
                list_clear(self.selectOb);
                list_append(self.selectOb, (unitype) "null", 's');
            }
            if (self.selectX > self.selectX2) {
                self.selectX2 = self.selectX;
                self.selectX = self.mx;
            }
            if (self.selectY > self.selectY2) {
                self.selectY2 = self.selectY;
                self.selectY = self.my;
            }
        } else {
            if ((self.keys[1] || self.wireHold == 1) && !(self.wiringStart == 0) && !(self.wiringEnd == 0) && !(self.wiringStart == self.wiringEnd)) {
                list_t *wireSQueue = list_init();
                list_t *wireEQueue = list_init();
                list_append(wireSQueue, (unitype) 'n', 'c');
                list_append(wireEQueue, (unitype) 'n', 'c');
                if (list_count(self.selected, (unitype) self.wiringStart, 'i') > 0 && self.selecting > 1) {
                    list_append(wireSQueue, (unitype) self.wiringStart, 'i');
                    list_append(wireEQueue, (unitype) self.wiringEnd, 'i');
                    int len = self.selected -> length;
                    for (int i = 1; i < len; i++) {
                        if (!(self.wiringStart == self.selected -> data[i].i) && !(self.wiringEnd == self.selected -> data[i].i))
                            list_append(wireSQueue, self.selected -> data[i], 'i');
                    }
                } else {
                    if (list_count(self.selected, (unitype) self.hlgcomp, 'i') > 0 && self.selecting > 1) {
                        list_append(wireSQueue, (unitype) self.wiringStart, 'i');
                        int len = self.selected -> length;
                        for (int i = 1; i < len; i++) {
                            list_append(wireEQueue, self.selected -> data[i], 'i');
                        }
                    } else {
                        list_append(wireSQueue, (unitype) self.wiringStart, 'i');
                        list_append(wireEQueue, (unitype) self.wiringEnd, 'i');
                        list_clear(self.selectOb);
                        list_append(self.selectOb, (unitype) "null", 's');
                        list_clear(self.selected);
                        list_append(self.selected, (unitype) "null", 's');
                        self.selecting = 0;
                        self.sxmax = 0;
                        self.symax = 0;
                        self.sxmin = 0;
                        self.symin = 0;
                    }
                }
                for (int k = 1; k < wireEQueue -> length; k++) {
                    for (int j = 1; j < wireSQueue -> length; j++) {
                        if (self.inpComp -> data[wireEQueue -> data[k].i * 3].i == wireSQueue -> data[j].i || self.inpComp -> data[wireEQueue -> data[k].i * 3 - 1].i == wireSQueue -> data[j].i) {
                            int i = 1;
                            for (int n = 0; n < (int) round((self.wiring -> length - 1) / 3); n++) {
                                if (self.wiring -> data[i].i == wireSQueue -> data[j].i && self.wiring -> data[i + 1].i == wireEQueue -> data[k].i) {
                                    list_delete(self.wiring, i);
                                    list_delete(self.wiring, i);
                                    list_delete(self.wiring, i);
                                } else {
                                    i += 3;
                                }
                            }
                            if (self.inpComp -> data[wireEQueue -> data[k].i * 3 - 1].i == wireSQueue -> data[j].i) {
                                if (self.inpComp -> data[wireEQueue -> data[k].i * 3].i == 0) {
                                    self.inpComp -> data[wireEQueue -> data[k].i * 3 - 1] = (unitype) 0;
                                    self.io -> data[wireEQueue -> data[k].i * 3 - 2] = (unitype) 0;
                                } else {
                                    self.inpComp -> data[wireEQueue -> data[k].i * 3 - 1] = self.inpComp -> data[wireEQueue -> data[k].i * 3];
                                    self.inpComp -> data[wireEQueue -> data[k].i * 3] = (unitype) 0;
                                }
                            } else {
                                self.inpComp -> data[wireEQueue -> data[k].i * 3] = (unitype) 0;
                            }
                            self.io -> data[wireEQueue -> data[k].i * 3 - 1] = (unitype) 0;
                        } else {
                            if (self.inpComp -> data[wireEQueue -> data[k].i * 3 - 1].i == 0) {
                                self.inpComp -> data[wireEQueue -> data[k].i * 3 - 1] = wireSQueue -> data[j];
                                list_append(self.wiring, wireSQueue -> data[j], 'i');
                                list_append(self.wiring, wireEQueue -> data[k], 'i');
                                list_append(self.wiring, (unitype) 0, 'i');
                            } else {
                                if (self.inpComp -> data[wireEQueue -> data[k].i * 3].i == 0 && !(self.inpComp -> data[wireEQueue -> data[k].i * 3 - 1].i == wireSQueue -> data[j].i) && self.inpComp -> data[wireEQueue -> data[k].i * 3 - 2].i > 1) {
                                    self.inpComp -> data[wireEQueue -> data[k].i * 3] = wireSQueue -> data[j];
                                    list_append(self.wiring, wireSQueue -> data[j], 'i');
                                    list_append(self.wiring, wireEQueue -> data[k], 'i');
                                    list_append(self.wiring, (unitype) 0, 'i');
                                }
                            }
                        }
                    }
                }
                list_free(wireSQueue);
                list_free(wireEQueue);
                // update undo
                addUndo(&self);
            }
            if (self.positions -> length > self.hglmove * 3 && strcmp(self.components -> data[self.hglmove].s, "POWER") == 0 && self.positions -> data[self.hglmove * 3 - 2].d == self.tempX && self.positions -> data[self.hglmove * 3 - 1].d == self.tempY) { // questionable (double check for equality)
                if (self.io -> data[self.hglmove * 3 - 1].i == 0) {
                    self.io -> data[self.hglmove * 3 - 1] = (unitype) 1;
                } else {
                    self.io -> data[self.hglmove * 3 - 1] = (unitype) 0;
                }
            }
            self.hglmove = 0;
            self.wiringStart = 0;
            self.wiringEnd = 0;
            if (self.keys[0])
                self.keys[0] = 0;
        }
    }
    if (updateQue) {
        // update undo
        addUndo(&self);
    }
    self.globalsize /= 0.75;
    *selfp = self;
}
void hotkeyTick(logicgates *selfp) { // most of the keybind functionality is handled here
    logicgates self = *selfp;
    if (turtleKeyPressed(GLFW_KEY_SPACE)) { // space
        self.keys[1] = 1;
    } else {
        self.keys[1] = 0;
    }
    if (turtleKeyPressed(GLFW_KEY_S) || turtleKeyPressed(GLFW_KEY_LEFT_SHIFT)) { // s key or left shift
        if (turtleKeyPressed(GLFW_KEY_S) && self.keys[2] == 0 && self.keys[21] == 1) {
            // this activates when you hit ctrl+s on the first frame of the s button press
            // do save routine
            #ifdef OS_WINDOWS
            if (strcmp(win32FileDialog.selectedFilename, "null") == 0) {
                if (win32FileDialogPrompt(1, "") != -1) {
                    printf("Saved to: %s\n", win32FileDialog.selectedFilename);
                    export(&self, win32FileDialog.selectedFilename);
                }
            } else {
                printf("Saved to: %s\n", win32FileDialog.selectedFilename);
                export(&self, win32FileDialog.selectedFilename);
            }
            #endif
            #ifdef OS_LINUX
            if (strcmp(zenityFileDialog.selectedFilename, "null") == 0) {
                if (zenityFileDialogPrompt(1, "") != -1) {
                    printf("Saved to: %s\n", zenityFileDialog.selectedFilename);
                    export(&self, zenityFileDialog.selectedFilename);
                }
            } else {
                printf("Saved to: %s\n", zenityFileDialog.selectedFilename);
                export(&self, zenityFileDialog.selectedFilename);
            }
            #endif
        }
        self.keys[2] = 1;
    } else {
        self.keys[2] = 0;
    }
    if (turtleKeyPressed(GLFW_KEY_UP)) { // up key
        self.keys[3] = 1;
    } else {
        self.keys[3] = 0;
    }
    if (turtleKeyPressed(GLFW_KEY_DOWN)) { // down key
        self.keys[4] = 1;
    } else {
        self.keys[4] = 0;
    }
    if (turtleKeyPressed(GLFW_KEY_K)) { // k key
        if (!self.keys[5]) {
            unsigned long unixTime = (unsigned long) time(NULL);
            char preset[25];
            sprintf(preset, "Untitled%lu.lg", unixTime);
            export(&self, preset);
        }
        self.keys[5] = 1;
    } else {
        self.keys[5] = 0;
    }
    if (turtleKeyPressed(GLFW_KEY_P) || turtleKeyPressed(GLFW_KEY_E) || turtleKeyPressed(GLFW_KEY_1)) { // p, e, and 1
        if (!self.keys[6]) {
            if (strcmp(self.holding, "POWER") == 0)
                self.holding = "a";
            else
                self.holding = "POWER";
        }
        self.keys[6] = 1;
    } else {
        self.keys[6] = 0;
    }
    if (turtleKeyPressed(GLFW_KEY_X) || turtleKeyPressed(GLFW_KEY_BACKSPACE) || turtleKeyPressed(GLFW_KEY_DELETE)) { // x key
        if (!self.keys[7]) {
            if (self.keys[21] && turtleKeyPressed(GLFW_KEY_X)) {
                // cut
                copyToBuffer(&self, 1);
            } else if (self.selecting > 1 && self.selected -> length > 1) {
                int len = self.selected -> length - 1;
                int j = 1;
                for (int i = 0; i < len; i++) {
                    deleteComp(&self, self.selected -> data[j].i, turtleKeyPressed(GLFW_KEY_LEFT_SHIFT)); // really should've given shift a slot on keys[]
                    if (strcmp(self.holding, "a") == 0 || strcmp(self.holding, "b") == 0) {
                        list_delete(self.selected, 1);
                    } else {
                        j++;
                    }
                }
                // update undo
                addUndo(&self);
                if (strcmp(self.holding, "a") == 0 || strcmp(self.holding, "b") == 0) {
                    self.selecting = 0;
                    list_clear(self.selectOb),
                    list_append(self.selectOb, (unitype) "null", 's');
                }
            } else {
                if (!(self.hlgcomp == 0)) {
                    deleteComp(&self, self.hlgcomp, turtleKeyPressed(GLFW_KEY_LEFT_SHIFT));
                    // update undo
                    addUndo(&self);
                }
            }
        }
        self.keys[7] = 1;
    } else {
        self.keys[7] = 0;
    }
    if (turtleKeyPressed(GLFW_KEY_A) || turtleKeyPressed(GLFW_KEY_3)) { // a and 3
        if (!self.keys[8]) {
            if (self.keys[21] || turtleKeyPressed(GLFW_KEY_A)) {
                // select all
                self.selecting = 2;
                list_clear(self.selected);
                list_append(self.selected, (unitype) "null", 's');
                for (int i = 1; i < self.components -> length; i++) {
                    list_append(self.selected, (unitype) i, 'i');
                }
            } else {
                if (strcmp(self.holding, "AND") == 0)
                    self.holding = "a";
                else
                    self.holding = "AND";
            }
        }
        self.keys[8] = 1;
    } else {
        self.keys[8] = 0;
    }
    if (turtleKeyPressed(GLFW_KEY_O) || turtleKeyPressed(GLFW_KEY_Q) || turtleKeyPressed(GLFW_KEY_4)) { // o, q, and 4
        if (!self.keys[9]) {
            if (strcmp(self.holding, "OR") == 0)
                self.holding = "a";
            else
                self.holding = "OR";
        }
        self.keys[9] = 1;
    } else {
        self.keys[9] = 0;
    }
    if (turtleKeyPressed(GLFW_KEY_N) || turtleKeyPressed(GLFW_KEY_2)) { // n, w, or 2
        if (!self.keys[10]) {
            if (strcmp(self.holding, "NOT") == 0)
                self.holding = "a";
            else
                self.holding = "NOT";
        }
        self.keys[10] = 1;
    } else {
        self.keys[10] = 0;
    }
    if (turtleKeyPressed(GLFW_KEY_T)) { // t key
        if (!self.keys[11]) {
            if (self.theme == 0) {
                self.theme = 27;
                ribbonDarkTheme();
            } else {
                self.theme = 0;
                ribbonLightTheme();
            }
            turtleBgColor(self.themeColors[25 + self.theme], self.themeColors[26 + self.theme], self.themeColors[27 + self.theme]);
        }
        self.keys[11] = 1;
    } else {
        self.keys[11] = 0;
    }
    if (turtleKeyPressed(GLFW_KEY_5)) { // 5 key
        if (!self.keys[12]) {
            if (strcmp(self.holding, "XOR") == 0)
                self.holding = "a";
            else
                self.holding = "XOR";
        }
        self.keys[12] = 1;
    } else {
        self.keys[12] = 0;
    }
    if (turtleKeyPressed(GLFW_KEY_6)) { // 6 key
        if (!self.keys[13]) {
            if (strcmp(self.holding, "NOR") == 0)
                self.holding = "a";
            else
                self.holding = "NOR";
        }
        self.keys[13] = 1;
    } else {
        self.keys[13] = 0;
    }
    if (turtleKeyPressed(GLFW_KEY_7)) { // 7 key
        if (!self.keys[14]) {
            if (strcmp(self.holding, "NAND") == 0)
                self.holding = "a";
            else
                self.holding = "NAND";
        }
        self.keys[14] = 1;
    } else {
        self.keys[14] = 0;
    }
    if (turtleKeyPressed(GLFW_KEY_8)) { // 8 key
        if (!self.keys[15]) {
            if (strcmp(self.holding, "BUFFER") == 0)
                self.holding = "a";
            else
                self.holding = "BUFFER";
        }
        self.keys[15] = 1;
    } else {
        self.keys[15] = 0;
    }
    if (turtleKeyPressed(GLFW_KEY_9)) { // 9 key
        if (!self.keys[16]) {
            if (self.wireHold == 1)
                self.wireHold = 0;
            else
                self.wireHold = 1;
        }
        self.keys[16] = 1;
    } else {
        self.keys[16] = 0;
    }
    if (turtleKeyPressed(GLFW_KEY_C)) { // c key
        if (!self.keys[17]) {
            if (self.keys[21] == 1) {
                if (self.selecting > 1 && self.selected -> length > 1 && (strcmp(self.holding, "a") == 0 || strcmp(self.holding, "b") == 0)) {
                    copyToBuffer(&self, 0); // copy for later (ctrl+c)
                }
            } else {
                if (self.selecting > 1 && self.selected -> length > 1 && (strcmp(self.holding, "a") == 0 || strcmp(self.holding, "b") == 0)) {
                    copySelected(&self); // copy directly
                    // update undo
                    addUndo(&self);
                }
            }    
        }
        self.keys[17] = 1;
    } else {
        self.keys[17] = 0;
    }
    if (turtleKeyPressed(GLFW_KEY_H)) { // h key
        if (!self.keys[18]) {
            if (self.sidebar == 1)
                self.sidebar = 0;
            else
                self.sidebar = 1;
        }
        self.keys[18] = 1;
    } else {
        self.keys[18] = 0;
    }
    if (turtleKeyPressed(GLFW_KEY_Z)) { // z key
        if (!self.keys[19]) {
            if (self.keys[21]) {
                // ctrl+z
                undo(&self);
            } else {
                snapToGrid(&self, 8);
                // update undo
                addUndo(&self);
            }
        }
        self.keys[19] = 1;
    } else {
        self.keys[19] = 0;
    }
    if (turtleKeyPressed(GLFW_KEY_W)) { // w key
        if (!self.keys[20]) {
            if (self.wireMode == 0) {
                self.wireMode = 2;
            } else {
                if (self.wireMode == 1) {
                    self.wireMode = 0;
                } else {
                    self.wireMode = 1;
                }
            }
        }
        self.keys[20] = 1;
    } else {
        self.keys[20] = 0;
    }
    if (turtleKeyPressed(GLFW_KEY_LEFT_CONTROL)) { // left ctrl
        // a combo key
        self.keys[21] = 1;
    } else {
        self.keys[21] = 0;
    }
    if (turtleKeyPressed(GLFW_KEY_V)) { // for ctrl+v
        if (self.keys[22] == 0 && self.keys[21] == 1) {
            // ctrl+v
            pasteFromBuffer(&self, 1);
            // update undo
            addUndo(&self);
        }
        self.keys[22] = 1;
    } else {
        self.keys[22] = 0;
    }
    if (turtleKeyPressed(GLFW_KEY_Y)) {
        if (self.keys[23] == 0 && self.keys[21] == 1) {
            // ctrl+y
            redo(&self);
        }
        self.keys[23] = 1;
    } else {
        self.keys[23] = 0;
    }
    // rotation using sideways arrows
    if (turtleKeyPressed(GLFW_KEY_RIGHT)) {
        self.keys[24] = 1;
        if (strcmp(self.holding, "a") != 0 && strcmp(self.holding, "b") != 0) {
            self.holdingAng += 0.5 * self.rotateSpeed;
        } else {
            if (self.selecting > 1) {
                // if space key pressed
                if (self.keys[1] == 1) {
                    rotateSelected(&self, -0.5 * self.rotateSpeed);
                } else {
                    int i = 1;
                    for (int j = 0; j < self.selected -> length - 1; j++) {
                        self.positions -> data[self.selected -> data[i].i * 3] = (unitype) (self.positions -> data[self.selected -> data[i].i * 3].d + 0.5 * self.rotateSpeed);
                        if (self.positions -> data[self.selected -> data[i].i * 3].d > 360)
                            self.positions -> data[self.selected -> data[i].i * 3] = (unitype) (self.positions -> data[self.selected -> data[i].i * 3].d - 360);
                        i += 1;
                    }
                }
            } else {
                if (self.hlgcomp != 0) {
                    self.positions -> data[self.hlgcomp * 3] = (unitype) (self.positions -> data[self.hlgcomp * 3].d + 0.5 * self.rotateSpeed);
                    if (self.positions -> data[self.hlgcomp * 3].d > 360)
                        self.positions -> data[self.hlgcomp * 3] = (unitype) (self.positions -> data[self.hlgcomp * 3].d - 360);
                }
            }
        }
    } else {
        if (self.keys[24]) {
            // update undo
            addUndo(&self);
            self.keys[24] = 0;
        }
    }
    if (turtleKeyPressed(GLFW_KEY_LEFT)) {
        self.keys[25] = 1;
        if (strcmp(self.holding, "a") != 0 && strcmp(self.holding, "b") != 0) {
            self.holdingAng -= 0.5 * self.rotateSpeed;
        } else {
            if (self.selecting > 1) {
                // if space key pressed
                if (self.keys[1] == 1) {
                    rotateSelected(&self, 0.5 * self.rotateSpeed);
                } else {
                    int i = 1;
                    for (int j = 0; j < self.selected -> length - 1; j++) {
                        self.positions -> data[self.selected -> data[i].i * 3] = (unitype) (self.positions -> data[self.selected -> data[i].i * 3].d - 0.5 * self.rotateSpeed);
                        if (self.positions -> data[self.selected -> data[i].i * 3].d < 0)
                            self.positions -> data[self.selected -> data[i].i * 3] = (unitype) (self.positions -> data[self.selected -> data[i].i * 3].d + 360);
                        i += 1;
                    }
                }
            } else {
                if (self.hlgcomp != 0) {
                    self.positions -> data[self.hlgcomp * 3] = (unitype) (self.positions -> data[self.hlgcomp * 3].d - 0.5 * self.rotateSpeed);
                    if (self.positions -> data[self.hlgcomp * 3].d < 0)
                        self.positions -> data[self.hlgcomp * 3] = (unitype) (self.positions -> data[self.hlgcomp * 3].d + 360);
                }
            }
        }
    } else {
        if (self.keys[25]) {
            // update undo
            addUndo(&self);
            self.keys[25] = 0;
        }
    }
    if (turtleKeyPressed(GLFW_KEY_G)) {
        if (!self.keys[26]) {
            // group selected
            if (turtleKeyPressed(GLFW_KEY_LEFT_SHIFT)) {
                groupSelected(&self, 1);
            } else {
                groupSelected(&self, 0);
            }
        }
        self.keys[26] = 1;
    } else {
        self.keys[26] = 0;
    }
    if (turtleKeyPressed(GLFW_KEY_ESCAPE)) {
        if (!self.keys[27]) {
            self.holding = "a";
        }
        self.keys[27] = 1;
    } else {
        self.keys[27] = 0;
    }
    *selfp = self;
}
void scrollTick(logicgates *selfp) { // all the scroll wheel functionality is handled here
    logicgates self = *selfp;
    if (self.mw > 0) {
        if (self.keys[1] || turtleKeyPressed(GLFW_KEY_LEFT_SHIFT)) {
            if (self.rotateCooldown == 1) {
                if (self.selecting > 1) {
                    rotateSelected(&self, 90);
                    // update undo
                    addUndo(&self);
                } else {
                    if (!(strcmp(self.holding, "a") == 0) && !(strcmp(self.holding, "b") == 0)) {
                        self.holdingAng -= 90;
                    } else {
                        if (!(self.hlgcomp == 0)) {
                            self.positions -> data[self.hlgcomp * 3] = (unitype) (self.positions -> data[self.hlgcomp * 3].d - 90);
                            if (self.positions -> data[self.hlgcomp * 3].d < 0)
                                self.positions -> data[self.hlgcomp * 3] = (unitype) (self.positions -> data[self.hlgcomp * 3].d + 360);
                            // update undo
                            addUndo(&self);
                        }
                    }
                }
                self.rotateCooldown = 0;
            }
        } else {
            self.screenX -= (turtle.mouseX * (-1 / self.scrollSpeed + 1)) / (self.globalsize * 0.75);
            self.screenY -= (turtle.mouseY * (-1 / self.scrollSpeed + 1)) / (self.globalsize * 0.75);
            self.globalsize *= self.scrollSpeed;
        }
    }
    if (self.mw < 0) {
        if (self.keys[1] || turtleKeyPressed(GLFW_KEY_LEFT_SHIFT)) {
            if (self.rotateCooldown == 1) {
                if (self.selecting > 1) {
                    rotateSelected(&self, -90);
                    // update undo
                    addUndo(&self);
                } else {
                    if (!(strcmp(self.holding, "a") == 0) && !(strcmp(self.holding, "b") == 0)) {
                        self.holdingAng += 90;
                    } else {
                        if (!(self.hlgcomp == 0)) {
                            self.positions -> data[self.hlgcomp * 3] = (unitype) (self.positions -> data[self.hlgcomp * 3].d + 90);
                            if (self.positions -> data[self.hlgcomp * 3].d > 360)
                                self.positions -> data[self.hlgcomp * 3] = (unitype) (self.positions -> data[self.hlgcomp * 3].d - 360);
                            // update undo
                            addUndo(&self);
                        }
                    }
                }
                self.rotateCooldown = 0;
            }
        } else {
            self.globalsize /= self.scrollSpeed;
            self.screenX += (turtle.mouseX * (-1 / self.scrollSpeed + 1)) / (self.globalsize * 0.75);
            self.screenY += (turtle.mouseY * (-1 / self.scrollSpeed + 1)) / (self.globalsize * 0.75);
        }
    }
    if (self.mw == 0) {
        self.rotateCooldown = 1;
    }
    *selfp = self;
}

/* ribbon functionality */

void parseRibbonOutput(logicgates *selfp) {
    #ifdef OS_WINDOWS
    logicgates self = *selfp;
    if (ribbonRender.output[0] == 1) {
        ribbonRender.output[0] = 0; // untoggle
        if (ribbonRender.output[1] == 0) { // file
            if (ribbonRender.output[2] == 1) { // new
                printf("New file created\n");
                clearAll(&self);
                strcpy(win32FileDialog.selectedFilename, "null");
            }
            if (ribbonRender.output[2] == 2) { // save
                if (strcmp(win32FileDialog.selectedFilename, "null") == 0) {
                    if (win32FileDialogPrompt(1, "") != -1) {
                        printf("Saved to: %s\n", win32FileDialog.selectedFilename);
                        export(&self, win32FileDialog.selectedFilename);
                    }
                } else {
                    printf("Saved to: %s\n", win32FileDialog.selectedFilename);
                    export(&self, win32FileDialog.selectedFilename);
                }
            }
            if (ribbonRender.output[2] == 3) { // save as
                if (win32FileDialogPrompt(1, "") != -1) {
                    printf("Saved to: %s\n", win32FileDialog.selectedFilename);
                    export(&self, win32FileDialog.selectedFilename);
                }
            }
            if (ribbonRender.output[2] == 4) { // load
                if (win32FileDialogPrompt(0, "") != -1) {
                    // printf("Loaded data from: %s\n", win32FileDialog.selectedFilename);
                    clearAll(&self);
                    import(&self, win32FileDialog.selectedFilename);
                    // update undo
                    addUndo(&self);
                }
            }
        }
        if (ribbonRender.output[1] == 1) { // edit
            if (ribbonRender.output[2] == 1) { // undo
                undo(&self);
            }
            if (ribbonRender.output[2] == 2) { // redo
                redo(&self);
            }
            if (ribbonRender.output[2] == 3) { // cut
                copyToBuffer(&self, 1);
                // update undo
                addUndo(&self);
            }
            if (ribbonRender.output[2] == 4) { // copy
                copyToBuffer(&self, 0);
                // update undo
                addUndo(&self);
            }
            if (ribbonRender.output[2] == 5) { // paste
                pasteFromBuffer(&self, 0);
                // update undo
                addUndo(&self);
            }
            if (ribbonRender.output[2] == 6) { // add file
                if (win32FileDialogPrompt(0, "") != -1) {
                    // printf("Added data from: %s\n", win32FileDialog.selectedFilename);
                    import(&self, win32FileDialog.selectedFilename);
                    strcpy(win32FileDialog.selectedFilename, "null");
                    // update undo
                    addUndo(&self);
                }
            }
        }
        if (ribbonRender.output[1] == 2) { // view
            if (ribbonRender.output[2] == 1) { // appearance
                printf("appearance settings\n");
            } 
            if (ribbonRender.output[2] == 2) { // GLFW
                printf("GLFW settings\n");
            } 
        }
    }
    *selfp = self;
    #endif

    #ifdef OS_LINUX
    logicgates self = *selfp;
    if (ribbonRender.output[0] == 1) {
        ribbonRender.output[0] = 0; // untoggle
        if (ribbonRender.output[1] == 0) { // file
            if (ribbonRender.output[2] == 1) { // new
                printf("New file created\n");
                clearAll(&self);
                strcpy(zenityFileDialog.selectedFilename, "null");
            }
            if (ribbonRender.output[2] == 2) { // save
                if (strcmp(zenityFileDialog.selectedFilename, "null") == 0) {
                    if (zenityFileDialogPrompt(1, "") != -1) {
                        printf("Saved to: %s\n", zenityFileDialog.selectedFilename);
                        export(&self, zenityFileDialog.selectedFilename);
                    }
                } else {
                    printf("Saved to: %s\n", zenityFileDialog.selectedFilename);
                    export(&self, zenityFileDialog.selectedFilename);
                }
            }
            if (ribbonRender.output[2] == 3) { // save as
                if (zenityFileDialogPrompt(1, "") != -1) {
                    printf("Saved to: %s\n", zenityFileDialog.selectedFilename);
                    export(&self, zenityFileDialog.selectedFilename);
                }
            }
            if (ribbonRender.output[2] == 4) { // load
                if (zenityFileDialogPrompt(0, "") != -1) {
                    // printf("Loaded data from: %s\n", zenityFileDialog.selectedFilename);
                    clearAll(&self);
                    import(&self, zenityFileDialog.selectedFilename);
                    // update undo
                    addUndo(&self);
                }
            }
        }
        if (ribbonRender.output[1] == 1) { // edit
            if (ribbonRender.output[2] == 1) { // undo
                undo(&self);
            }
            if (ribbonRender.output[2] == 2) { // redo
                redo(&self);
            }
            if (ribbonRender.output[2] == 3) { // cut
                copyToBuffer(&self, 1);
                // update undo
                addUndo(&self);
            }
            if (ribbonRender.output[2] == 4) { // copy
                copyToBuffer(&self, 0);
                // update undo
                addUndo(&self);
            }
            if (ribbonRender.output[2] == 5) { // paste
                pasteFromBuffer(&self, 0);
                // update undo
                addUndo(&self);
            }
            if (ribbonRender.output[2] == 6) { // add file
                if (zenityFileDialogPrompt(0, "") != -1) {
                    // printf("Added data from: %s\n", zenityFileDialog.selectedFilename);
                    import(&self, zenityFileDialog.selectedFilename);
                    strcpy(zenityFileDialog.selectedFilename, "null");
                    // update undo
                    addUndo(&self);
                }
            }
        }
        if (ribbonRender.output[1] == 2) { // view
            if (ribbonRender.output[2] == 1) { // appearance
                printf("appearance settings\n");
            } 
            if (ribbonRender.output[2] == 2) { // GLFW
                printf("GLFW settings\n");
            } 
        }
    }
    *selfp = self;
    #endif
}

int main(int argc, char *argv[]) {
    GLFWwindow* window;
    /* Initialize glfw */
    if (!glfwInit()) {
        return -1;
    }
    glfwWindowHint(GLFW_SAMPLES, 4); // MSAA (Anti-Aliasing) with 4 samples (must be done before window is created (?))

    /* Create a windowed mode window and its OpenGL context */
    const GLFWvidmode *monitorSize = glfwGetVideoMode(glfwGetPrimaryMonitor());
    int windowHeight = monitorSize -> height * 0.85;
    window = glfwCreateWindow(windowHeight * 4 / 3, windowHeight, "Logic Gates", NULL, NULL);
    if (!window) {
        glfwTerminate();
    }
    glfwMakeContextCurrent(window);
    glfwSetWindowSizeLimits(window, GLFW_DONT_CARE, GLFW_DONT_CARE, windowHeight * 4 / 3, windowHeight);
    gladLoadGL();

    /* initialise FileDialog */
    #ifdef OS_WINDOWS
    win32ToolsInit();
    win32FileDialogAddExtension("txt"); // add txt to extension restrictions
    win32FileDialogAddExtension("lg"); // add lg to extension restrictions
    char constructedPath[MAX_PATH + 1];
    #endif
    #ifdef OS_LINUX
    zenityFileDialogInit(argv[0]); // must include argv[0] to get executableFilepath
    zenityFileDialogAddExtension("txt"); // add txt to extension restrictions
    zenityFileDialogAddExtension("lg"); // add lg to extension restrictions
    char constructedPath[4097];
    #endif

    /* load logo */
    GLFWimage icon;
    int iconChannels;
    #ifdef OS_WINDOWS
    strcpy(constructedPath, win32FileDialog.executableFilepath);
    #endif
    #ifdef OS_LINUX
    strcpy(constructedPath, zenityFileDialog.executableFilepath);
    #endif
    strcat(constructedPath, "include/LogicGatesIcon.jpg");
    unsigned char *iconPixels = stbi_load(constructedPath, &icon.width, &icon.height, &iconChannels, 4); // 4 color channels for RGBA
    icon.pixels = iconPixels;
    glfwSetWindowIcon(window, 1, &icon);

    /* initialize turtle */
    turtleInit(window, -240, -180, 240, 180);
    /* initialise textGL */
    #ifdef OS_WINDOWS
    strcpy(constructedPath, win32FileDialog.executableFilepath);
    #endif
    #ifdef OS_LINUX
    strcpy(constructedPath, zenityFileDialog.executableFilepath);
    #endif
    strcat(constructedPath, "include/fontBez.tgl");
    textGLInit(window, constructedPath);
    /* initialise ribbon */
    #ifdef OS_WINDOWS
    strcpy(constructedPath, win32FileDialog.executableFilepath);
    #endif
    #ifdef OS_LINUX
    strcpy(constructedPath, zenityFileDialog.executableFilepath);
    #endif
    strcat(constructedPath, "include/ribbonConfig.txt");
    ribbonInit(window, constructedPath);
    /* initialise popup */
    #ifdef OS_WINDOWS
    strcpy(constructedPath, win32FileDialog.executableFilepath);
    #endif
    #ifdef OS_LINUX
    strcpy(constructedPath, zenityFileDialog.executableFilepath);
    #endif
    strcat(constructedPath, "include/popupConfig.txt");
    popupInit(constructedPath, -50, -50, 50, 50);
    
    int tps = 60; // ticks per second (locked to fps in this case)
    clock_t start, end;
    logicgates self;
    init(&self); // initialise the logicgates
    turtle.penshape = self.defaultShape; // set the shape
    turtlePenPrez(self.defaultPrez); // set the prez

    if (argc > 1) {
        import(&self, argv[1]);
        #ifdef OS_WINDOWS
        strcpy(win32FileDialog.selectedFilename, argv[1]);
        #endif
        #ifdef OS_LINUX
        strcpy(zenityFileDialog.selectedFilename, argv[1]);
        #endif
    }
    // update undo
    addUndo(&self);
    int frame = 0;
    while (turtle.close == 0) {
        start = clock(); // for frame syncing
        turtleGetMouseCoords(); // get the mouse coordinates
        if (turtle.mouseX > 240) { // bound mouse coordinates to window coordinates
            self.mx = 240;
        } else {
            if (turtle.mouseX < -240) {
                self.mx = -240;
            } else {
                self.mx = turtle.mouseX;
            }
        }
        if (turtle.mouseY > 180) {
            self.my = 180;
        } else {
            if (turtle.mouseY < -180) {
                self.my = -180;
            } else {
                self.my = turtle.mouseY;
            }
        }
        self.mw = turtleMouseWheel();
        if (self.keys[3])
            self.mw += 1;
        if (self.keys[4])
            self.mw -= 1;
        turtleClear();
        renderComp(&self);
        renderWire(&self, self.globalsize);
        renderSidebar(&self, self.sidebar);
        hlgcompset(&self);
        // render holding component
        turtlePenColor(self.themeColors[1 + self.theme], self.themeColors[2 + self.theme], self.themeColors[3 + self.theme]);
        if (strcmp(self.holding, "POWER") == 0)
            POWER(&self, self.mx, self.my, self.globalsize, self.holdingAng, 0, 0);
        if (strcmp(self.holding, "AND") == 0)
            AND(&self, self.mx, self.my, self.globalsize, self.holdingAng);
        if (strcmp(self.holding, "OR") == 0)
            OR(&self, self.mx, self.my, self.globalsize, self.holdingAng);
        if (strcmp(self.holding, "NOT") == 0)
            NOT(&self, self.mx, self.my, self.globalsize, self.holdingAng);
        if (strcmp(self.holding, "XOR") == 0)
            XOR(&self, self.mx, self.my, self.globalsize, self.holdingAng);
        if (strcmp(self.holding, "NOR") == 0)
            NOR(&self, self.mx, self.my, self.globalsize, self.holdingAng);
        if (strcmp(self.holding, "NAND") == 0)
            NAND(&self, self.mx, self.my, self.globalsize, self.holdingAng);
        if (strcmp(self.holding, "BUFFER") == 0)
            BUFFER(&self, self.mx, self.my, self.globalsize, self.holdingAng);
        ribbonDraw(); // do ribbon before mouseTick
        parseRibbonOutput(&self);
        mouseTick(&self);
        hotkeyTick(&self);
        scrollTick(&self);
        turtleUpdate(); // update the screen
        end = clock();
        if (frame % 60 == 0) {
            frame = 0;
        }
        frame += 1;
        while ((double) (end - start) / CLOCKS_PER_SEC < (1 / (double) tps)) {
            end = clock();
        }
    }
    frame = 0;
    printf("end %d\n", sizeof(logicgates));
    while (turtle.shouldClose == 0) {
        start = clock();
        if (frame > -1)
            turtle.shouldClose = 1;
        turtleUpdate();
        frame += 1;
        while ((double) (end - start) / CLOCKS_PER_SEC < (1 / (double) tps)) {
            end = clock();
        }
    }
}