#include "include/turtle.h"
#include "include/ribbon.h"
#include "include/zenityFileDialog.h"
#include <time.h>
#define STB_IMAGE_IMPLEMENTATION
#include "include/stb_image.h" // THANK YOU https://github.com/nothings/stb

typedef struct { // all logicgates variables (shared state) are defined here
    double globalsize;
    double themeColors[55];
    char theme;
    char sidebar;
    char selecting;
    char indicators;
    char mouseType;
    char wireMode;
    double scrollSpeed;
    double rotateSpeed;
    int rotateCooldown;
    double mx;
    double my;
    double mw;
    double boundXmin;
    double boundYmin;
    double boundXmax;
    double boundYmax;
    double scaling;
    double GraphPrez;
    char *holding;
    double holdingAng;
    double wxOffE;
    double wyOffE;
    double screenX;
    double screenY;
    double sxmax;
    double sxmin;
    double symax;
    double symin;
    int hlgcomp;
    int hglmove;
    double tempX;
    double tempY;
    double offX;
    double offY;
    double FocalX;
    double FocalY;
    double FocalCSX;
    double FocalCSY;
    double selectX;
    double selectX2;
    double selectY;
    double selectY2;
    char wireHold;
    int wiringStart;
    int wiringEnd;
    list_t *components; // list of components (1 item for each component, a string with "POWER", "AND", etc), a component's "ID" or "Handle" is that component's index in this list
    list_t *compSlots; // a lookup table for which components have two inputs vs one
    list_t *deleteQueue; // when many components are deleted, they are queued here
    list_t *inpComp; // list of component ID inputs, 3 items per component, format: number of possible connections (either 1 or 2), connection 1 (ID, 0 if none), connection 2 (ID, 0 if none)
    list_t *io; // list of binary inputs and outputs of a component (3 items for each component, 2 inputs followed by the output of the component (either a 0 or 1))
    char keys[21];
    list_t *positions; // list of component positions (3 items for each component, doubles specifying x, y, and angle)
    list_t *selected; // list of selected component IDs
    list_t *selectOb; // list of selected component IDs (but different?)
    list_t *wiring; // list of component connections (3 items per connection, it goes sender (ID), reciever (ID), powered (0 or 1))
    list_t *wireTemp;
    double sinRot;
    double cosRot;
    char defaultShape;
    double defaultPrez;
    double specialPrez;
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
    self.wireMode = 0;
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
    self.wireHold = 0;
    self.wiringStart = 0;
    self.wiringEnd = 0;
    self.components = list_init();
    list_append(self.components, (unitype) "null", 's'); // they start with an 'n' char or "null" string so they are 1-indexed not 0-indexed because I'm a bad coder
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
    self.deleteQueue = list_init();
    list_append(self.deleteQueue, (unitype) 'n', 'c');
    self.inpComp = list_init();
    list_append(self.inpComp, (unitype) 'n', 'c');
    self.io = list_init();
    list_append(self.io, (unitype) 'n', 'c');
    for (int i = 0; i < 20; i++)
        self.keys[i] = 0;
    self.positions = list_init();
    list_append(self.positions, (unitype) 'n', 'c');
    self.selected = list_init();
    list_append(self.selected, (unitype) "null", 's');
    self.selectOb = list_init();
    list_append(self.selectOb, (unitype) "null", 's');
    self.wiring = list_init();
    list_append(self.wiring, (unitype) 'n', 'c');
    self.wireTemp = list_init();
    list_append(self.wireTemp, (unitype) 'n', 'c');
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
    list_clear(self.inpComp);
    list_append(self.inpComp, (unitype) 'n', 'c');
    list_clear(self.positions);
    list_append(self.positions, (unitype) 'n', 'c');
    list_clear(self.io);
    list_append(self.io, (unitype) 'n', 'c');
    list_clear(self.wiring);
    list_append(self.wiring, (unitype) 'n', 'c');
    *selfp = self;
}
void import(logicgates *selfp, const char *filename) { // imports a file
    logicgates self = *selfp;
    printf("Attempting to load %s\n", filename);
    FILE *file = fopen(filename, "r");
    char str1[10] = "null";
    double doub1;
    int int1;
    int end = 0;
    int num = 0;
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
            list_append(self.components, (unitype) str1, 's');
        }
        for (int i = 0; i < num * 3; i++) {
            end = fscanf(file, "%lf", &doub1);
            list_append(self.positions, (unitype) doub1, 'd');
        }
        for (int i = 0; i < num * 3; i++) {
            end = fscanf(file, "%d", &int1);
            list_append(self.io, (unitype) int1, 'i');
        }
        for (int i = 0; i < num * 3; i++) {
            end = fscanf(file, "%d", &int1);
            list_append(self.inpComp, (unitype) int1, 'i');
        }
        while (end != EOF) {
            end = fscanf(file, "%d", &int1);
            if (end != EOF)
                list_append(self.wiring, (unitype) int1, 'i');
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
        for (int i = 1; i < self.components -> length; i++)
            fprintf(file, "%s ", self.components -> data[i].s);
        for (int i = 1; i < self.positions -> length; i++)
            fprintf(file, "%.0lf ", self.positions -> data[i].d);
        for (int i = 1; i < self.io -> length; i++)
            fprintf(file, "%d ", self.io -> data[i].i);
        for (int i = 1; i < self.inpComp -> length; i++)
            fprintf(file, "%d ", self.inpComp -> data[i].i);
        for (int i = 1; i < self.wiring -> length; i++)
            fprintf(file, "%d ", self.wiring -> data[i].i);
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
void copySelected(logicgates *selfp) { // copies and pastes selected components
    logicgates self = *selfp;
    self.sxmax = 0;
    self.sxmin = 0;
    self.symax = 0;
    self.symin = 0;
    self.selecting = 3;
    double j = 0;
    double k = 0;
    int l = self.components -> length - self.selected -> data[1].i;
    int m1 = self.selected -> length;
    for (int i = 1; i < m1; i++) {
        j += self.positions -> data[self.selected -> data[i].i * 3 - 2].d;
        k += self.positions -> data[self.selected -> data[i].i * 3 - 1].d;
    }
    j /= m1 - 1;
    k /= m1 - 1;
    for (int i = 1; i < m1; i++) {
        list_append(self.components, self.components -> data[self.selected -> data[i].i], 's');
        list_append(self.positions, (unitype) (self.positions -> data[self.selected -> data[i].i * 3 - 2].d + self.mx / self.globalsize - self.screenX - j), 'd');
        list_append(self.positions, (unitype) (self.positions -> data[self.selected -> data[i].i * 3 - 1].d + self.my / self.globalsize - self.screenY - k), 'd');
        list_append(self.positions, self.positions -> data[self.selected -> data[i].i * 3], 'd');
        list_append(self.io, (unitype) 0, 'i');
        list_append(self.io, (unitype) 0, 'i');
        list_append(self.io, (unitype) 0, 'i');
        list_append(self.inpComp, self.inpComp -> data[self.selected -> data[i].i * 3 - 2], 'i');
        if (list_count(self.selected, self.inpComp -> data[self.selected -> data[i].i * 3 - 1], 'i') > 0) {
            list_append(self.inpComp, (unitype) (l + self.inpComp -> data[self.selected -> data[i].i * 3 - 1].i), 'i');
            if (list_count(self.inpComp, self.inpComp -> data[self.selected -> data[i].i * 3], 'i') > 0)
                list_append(self.inpComp, (unitype) (l + self.inpComp -> data[self.selected -> data[i].i * 3].i), 'i');
            else
                list_append(self.inpComp, (unitype) 0, 'i');
        } else {
            if (list_count(self.selected, self.inpComp -> data[self.selected -> data[i].i * 3], 'i') > 0)
                list_append(self.inpComp, (unitype) (l + self.inpComp -> data[self.selected -> data[i].i * 3].i), 'i');
            else
                list_append(self.inpComp, (unitype) 0, 'i');
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
    int i = self.components -> length - self.selected -> length + 1;
    list_clear(self.selected);
    list_append(self.selected, (unitype) "null", 's');
    for (int o = 1; o < m1; o++) {
        list_append(self.selected, (unitype) i, 'i');
        i += 1;
    }
    *selfp = self;
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
void deleteComp(logicgates *selfp, int index) { // deletes a component
    logicgates self = *selfp;
    int len = self.selected -> length;
    for (int i = 1; i < len; i++) {
        if (self.selected -> data[i].i > index)
            self.selected -> data[i] = (unitype) (self.selected -> data[i].i - 1);
    }
    int i = 1;
    int k = (int) round((self.wiring -> length - 1) / 3);
    for (int j = 0; j < k; j++) {
        if (self.wiring -> data[i].i == index || self.wiring -> data[i + 1].i == index) {
            list_delete(self.wiring, i);
            list_delete(self.wiring, i);
            list_delete(self.wiring, i);
        } else {
            if (self.wiring -> data[i].i > index)
                self.wiring -> data[i] = (unitype) (self.wiring -> data[i].i - 1);
            if (self.wiring -> data[i + 1].i > index)
                self.wiring -> data[i + 1] = (unitype) (self.wiring -> data[i + 1].i - 1);
            i += 3;
        }
    }
    i = 2;
    k = (int) round((self.inpComp -> length - 1) / 3);
    for (int j = 0; j < k; j++) {
        if (self.inpComp -> data[i].i == index || self.inpComp -> data[i + 1].i == index) {
            if (self.inpComp -> data[i].i == index) {
                if (!(self.inpComp -> data[i + 1].i == 0)) {
                    if (self.inpComp -> data[i + 1].i > index)
                        self.inpComp -> data[i] = (unitype) (self.inpComp -> data[i + 1].i - 1);
                    else
                        self.inpComp -> data[i] = self.inpComp -> data[i + 1];
                    self.inpComp -> data[i + 1] = (unitype) 0;
                    self.io -> data[i] = (unitype) 0;
                } else {    
                    self.inpComp -> data[i] = (unitype) 0;
                    self.inpComp -> data[i + 1] = (unitype) 0;
                }
            } else {
                if (self.inpComp -> data[i].i > index)
                    self.inpComp -> data[i] = (unitype) (self.inpComp -> data[i].i - 1);
                self.inpComp -> data[i + 1] = (unitype) 0;
                self.io -> data[i] = (unitype) 0;
            }
        } else {
            if (self.inpComp -> data[i].i > index)
                self.inpComp -> data[i] = (unitype) (self.inpComp -> data[i].i - 1);
            if (self.inpComp -> data[i + 1].i > index)
                self.inpComp -> data[i + 1] = (unitype) (self.inpComp -> data[i + 1].i - 1);
        }
        i += 3;
    }
    list_delete(self.components, index);
    list_delete(self.positions, index * 3 - 2);
    list_delete(self.positions, index * 3 - 2);
    list_delete(self.positions, index * 3 - 2);
    list_delete(self.io, index * 3 - 2);
    list_delete(self.io, index * 3 - 2);
    list_delete(self.io, index * 3 - 2);
    list_delete(self.inpComp, index * 3 - 2);
    list_delete(self.inpComp, index * 3 - 2);
    list_delete(self.inpComp, index * 3 - 2);
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
    int len = self.components -> length;
    for (int i = 1; i < len; i++) {
        double renderX = (self.positions -> data[i * 3 - 2].d + self.screenX) * self.globalsize * 0.75;
        double renderY = (self.positions -> data[i * 3 - 1].d + self.screenY) * self.globalsize * 0.75;
        if (renderX + 15 * self.globalsize > -240 && renderX + -15 * self.globalsize < 240 && renderY + 15 * self.globalsize > -180 && renderY + -15 * self.globalsize < 180) {
            if (list_count(self.selected, (unitype) i, 'i') > 0 || (renderX + 12 * self.globalsize > self.sxmin && renderX + -12 * self.globalsize < self.sxmax && renderY + 12 * self.globalsize > self.symin && renderY + -12 * self.globalsize < self.symax && self.selecting == 1)) {
                    list_append(self.selectOb, (unitype) i, 'i');
                    turtlePenColor(self.themeColors[4 + self.theme], self.themeColors[5 + self.theme], self.themeColors[6 + self.theme]);
                } else {
                    turtlePenColor(self.themeColors[1 + self.theme], self.themeColors[2 + self.theme], self.themeColors[3 + self.theme]);
                }
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
    *selfp = self;
}
void renderWire(logicgates *selfp, double size) { // this function renders all the wiring in the window (a bit buggy if the components are outside the window, it doesn't do line intercepts and is likely bounded by total screen size but if I were to do bound intercepts I would do it in the turtle abstration)
    logicgates self = *selfp;
    self.globalsize *= 0.75; // um just resizing no big deal
    turtlePenSize(size * self.scaling);
    int len = self.wiring -> length - 1;
    for (int i = 1; i < len; i += 3) {
        wireIO(&self, i, i + 1);
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
    self.boundYmax = 181;
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
void mouseTick(logicgates *selfp) { // all the functionality for the mouse is handled in this beast of a function, it's really messy and super hard to understand
    logicgates self = *selfp;
    self.globalsize *= 0.75; // resizing
    if (turtleMouseDown()) {
        if (!(self.keys[0])) {
            self.keys[0] = 1;
            if (self.mx > self.boundXmin && self.mx < self.boundXmax && self.my > self.boundYmin && self.my < self.boundYmax) {
                self.mouseType = 0;
                if (!(self.selecting == 2) && !(list_count(self.selected, (unitype) self.hlgcomp, 'i') > 0) && !((self.keys[1] || self.wireHold == 1) && !(self.hlgcomp == 0))) {
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
                        if (!(self.hlgcomp == 0) && !(self.hlgcomp == self.wiringStart) && (self.inpComp -> data[self.wiringEnd * 3 - 1].i == 0 || (self.inpComp -> data[self.wiringEnd * 3].i == 0 && !(self.inpComp -> data[self.wiringEnd * 3 - 1].i == self.wiringStart) && self.inpComp -> data[self.wiringEnd * 3 - 2].i > 1)))
                            turtleGoto((self.positions -> data[self.hlgcomp * 3 - 2].d + self.screenX) * self.globalsize, (self.positions -> data[self.hlgcomp * 3 - 1].d + self.screenY) * self.globalsize);
                        else
                            turtleGoto(self.mx, self.my);
                        turtlePenUp();
                    }
                }
            }
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
                    self.positions -> data[self.hglmove * 3 - 2] = (unitype) (self.mx / self.globalsize - self.screenX + self.offX);
                    self.positions -> data[self.hglmove * 3 - 1] = (unitype) (self.my / self.globalsize - self.screenY + self.offY);
                }
            }
        }
    } else {
        if (!(self.mx > self.boundXmin && self.mx < self.boundXmax && self.my > self.boundYmin && self.my < self.boundYmax) && self.hglmove > 0) {
            if (self.selecting > 1 && self.selected -> length > 1 && (strcmp(self.holding, "a") == 0 || strcmp(self.holding, "b") == 0)) {
                int len = self.selected -> length - 1;
                for (int i = 0; i < len; i++) {
                    deleteComp(&self, self.selected -> data[1].i);
                }
                self.selecting = 0;
                list_clear(self.selectOb);
                list_append(self.selectOb, (unitype) "null", 's');
            } else {
                deleteComp(&self, self.hglmove);
            }
        }
        if (self.mouseType == 2 && !(strcmp(self.holding, "a") == 0) && !(strcmp(self.holding, "b") == 0)) {
            self.mouseType = 0;
            if (self.mx > self.boundXmin && self.mx < self.boundXmax && self.my > self.boundYmin && self.my < self.boundYmax) {
                list_append(self.components, (unitype) self.holding, 's');
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
            sprintf(preset, "Untitled%lu.txt", unixTime);
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
    if (turtleKeyPressed(GLFW_KEY_X)) { // x key
        if (!self.keys[7]) {
            if (self.selecting > 1 && self.selected -> length > 1 && (strcmp(self.holding, "a") == 0 || strcmp(self.holding, "b") == 0)) {
                int len = self.selected -> length - 1;
                for (int i = 0; i < len; i++) {
                    deleteComp(&self, self.selected -> data[1].i);
                    list_delete(self.selected, 1);
                }
                self.selecting = 0;
                list_clear(self.selectOb),
                list_append(self.selectOb, (unitype) "null", 's');
            } else {
                if (!(self.hlgcomp == 0))
                    deleteComp(&self, self.hlgcomp);
            }
        }
        self.keys[7] = 1;
    } else {
        self.keys[7] = 0;
    }
    if (turtleKeyPressed(GLFW_KEY_A) || turtleKeyPressed(GLFW_KEY_3)) { // a and 3
        if (!self.keys[8]) {
            if (strcmp(self.holding, "AND") == 0)
                self.holding = "a";
            else
                self.holding = "AND";
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
    if (turtleKeyPressed(GLFW_KEY_W)) { // w key
        if (!self.keys[20]) {
            if (self.wireMode == 0) {
                self.wireMode = 1;
            } else {
                self.wireMode = 0;
            }
        }
        self.keys[20] = 1;
    } else {
        self.keys[20] = 0;
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
            if (self.selecting > 1 && self.selected -> length > 1 && (strcmp(self.holding, "a") == 0 || strcmp(self.holding, "b") == 0))
                copySelected(&self);
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
            snapToGrid(&self, 8);
        }
        self.keys[19] = 1;
    } else {
        self.keys[19] = 0;
    }
    *selfp = self;
}
void scrollTick(logicgates *selfp) { // all the scroll wheel functionality is handled here
    logicgates self = *selfp;
    if (self.mw > 0) {
        if (self.keys[1]) {
            if (self.rotateCooldown == 1) {
                if (self.selecting > 1) {
                    rotateSelected(&self, 90);
                } else {
                    if (!(strcmp(self.holding, "a") == 0) && !(strcmp(self.holding, "b") == 0)) {
                        self.holdingAng -= 90;
                    } else {
                        if (!(self.hlgcomp == 0)) {
                            self.positions -> data[self.hlgcomp * 3] = (unitype) (self.positions -> data[self.hlgcomp * 3].d - 90);
                            if (self.positions -> data[self.hlgcomp * 3].d < 0)
                                self.positions -> data[self.hlgcomp * 3] = (unitype) (self.positions -> data[self.hlgcomp * 3].d + 360);
                        }
                    }
                }
                self.rotateCooldown = 0;
            }
        } else {
            self.screenX -= (turtle.mouseX * (-1 / self.scrollSpeed + 1)) / self.globalsize;
            self.screenY -= (turtle.mouseY * (-1 / self.scrollSpeed + 1)) / self.globalsize;
            self.globalsize *= self.scrollSpeed;
        }
    }
    if (self.mw < 0) {
        if (self.keys[1]) {
            if (self.rotateCooldown == 1) {
                if (self.selecting > 1) {
                    rotateSelected(&self, -90);
                } else {
                    if (!(strcmp(self.holding, "a") == 0) && !(strcmp(self.holding, "b") == 0)) {
                        self.holdingAng += 90;
                    } else {
                        if (!(self.hlgcomp == 0)) {
                            self.positions -> data[self.hlgcomp * 3] = (unitype) (self.positions -> data[self.hlgcomp * 3].d + 90);
                            if (self.positions -> data[self.hlgcomp * 3].d > 360)
                                self.positions -> data[self.hlgcomp * 3] = (unitype) (self.positions -> data[self.hlgcomp * 3].d - 360);
                        }
                    }
                }
                self.rotateCooldown = 0;
            }
        } else {
            self.globalsize /= self.scrollSpeed;
            self.screenX += (turtle.mouseX * (-1 / self.scrollSpeed + 1)) / self.globalsize;
            self.screenY += (turtle.mouseY * (-1 / self.scrollSpeed + 1)) / self.globalsize;
        }
    }
    if (self.mw == 0) {
        self.rotateCooldown = 1;
    }
    *selfp = self;
}

/* ribbon functionality */

void parseRibbonOutput(logicgates *selfp) {
    logicgates self = *selfp;
    if (ribbonRender.output[0] == 1) {
        ribbonRender.output[0] = 0; // untoggle
        if (ribbonRender.output[1] == 0) { // file
            if (ribbonRender.output[2] == 1) { // new
                printf("New file created\n");
                clearAll(&self);
                strcpy(zenityFileDialog.filename, "null");
            }
            if (ribbonRender.output[2] == 2) { // save
                if (strcmp(zenityFileDialog.filename, "null") == 0) {
                    if (zenityFileDialogPrompt(1, "") != -1) {
                        printf("Saved to: %s\n", zenityFileDialog.filename);
                        export(&self, zenityFileDialog.filename);
                    }
                } else {
                    printf("Saved to: %s\n", zenityFileDialog.filename);
                    export(&self, zenityFileDialog.filename);
                }
            }
            if (ribbonRender.output[2] == 3) { // save as
                if (zenityFileDialogPrompt(1, "") != -1) {
                    printf("Saved to: %s\n", zenityFileDialog.filename);
                    export(&self, zenityFileDialog.filename);
                }
            }
            if (ribbonRender.output[2] == 4) { // load
                if (zenityFileDialogPrompt(0, "") != -1) {
                    // printf("Loaded data from: %s\n", zenityFileDialog.filename);
                    clearAll(&self);
                    import(&self, zenityFileDialog.filename);
                }
            }
        }
        if (ribbonRender.output[1] == 1) { // edit
            if (ribbonRender.output[2] == 1) { // undo
                printf("undo\n");
            }
            if (ribbonRender.output[2] == 2) { // redo
                printf("redo\n");
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
}

int main(int argc, char *argv[]) {
    GLFWwindow* window;
    /* Initialize glfw */
    if (!glfwInit()) {
        return -1;
    }
    glfwWindowHint(GLFW_SAMPLES, 4); // MSAA (Anti-Aliasing) with 4 samples (must be done before window is created (?))

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(960, 720, "Logic Gates", NULL, NULL);
    if (!window) {
        glfwTerminate();
    }
    glfwMakeContextCurrent(window);
    glfwSetWindowSizeLimits(window, GLFW_DONT_CARE, GLFW_DONT_CARE, 960, 720);
    gladLoadGL();
    /* load logo */
    GLFWimage icon;
    int iconChannels;
    unsigned char *iconPixels = stbi_load("include/LogicGatesIcon.jpg", &icon.width, &icon.height, &iconChannels, 4); // 4 color channels for RGBA
    icon.pixels = iconPixels;
    glfwSetWindowIcon(window, 1, &icon);

    /* initialize turtle */
    turtleInit(window, -240, -180, 240, 180);
    /* initialise textGL */
    textGLInit(window, "include/fontBez.tgl");
    /* initialise ribbon */
    ribbonInit(window, "include/ribbonConfig.txt");

    /* initialise zenitytools */
    zenityFileDialogInit();
    zenityFileDialogAddExtension("txt"); // add txt to extension restrictions

    int tps = 60; // ticks per second (locked to fps in this case)
    clock_t start, end;
    logicgates self;
    init(&self); // initialise the logicgates
    turtle.penshape = self.defaultShape; // set the shape
    turtlePenPrez(self.defaultPrez); // set the prez

    if (argc > 1) {
        import(&self, argv[1]);
    }
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
        // rotation using sideways arrows
        if (turtleKeyPressed(GLFW_KEY_RIGHT)) {
            if (!self.holding == 0 && !self.holding == 1) {
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
                    if (!self.hlgcomp == 0) {
                        self.positions -> data[self.hlgcomp * 3] = (unitype) (self.positions -> data[self.hlgcomp * 3].d + 0.5 * self.rotateSpeed);
                        if (self.positions -> data[self.hlgcomp * 3].d > 360)
                            self.positions -> data[self.hlgcomp * 3] = (unitype) (self.positions -> data[self.hlgcomp * 3].d - 360);
                    }
                }
            }
        }
        if (turtleKeyPressed(GLFW_KEY_LEFT)) {
            if (!self.holding == 0 && !self.holding == 1) {
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
                    if (!self.hlgcomp == 0) {
                        self.positions -> data[self.hlgcomp * 3] = (unitype) (self.positions -> data[self.hlgcomp * 3].d - 0.5 * self.rotateSpeed);
                        if (self.positions -> data[self.hlgcomp * 3].d < 0)
                            self.positions -> data[self.hlgcomp * 3] = (unitype) (self.positions -> data[self.hlgcomp * 3].d + 360);
                    }
                }
            }
        }
        mouseTick(&self);
        hotkeyTick(&self);
        scrollTick(&self);
        ribbonDraw();
        parseRibbonOutput(&self);
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
}