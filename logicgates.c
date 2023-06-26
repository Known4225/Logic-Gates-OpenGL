#include "include/turtle.h"
#include <time.h>

typedef struct { // all class variables (shared state) are defined here
    double globalsize;
    double themeColors[55];
    char theme;
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
    char sidebar;
    double GraphPrez;
    char *holding;
    double holdingAng;
    char indicators;
    char mouseType;
    double wxOffE;
    double wyOffE;
    double CSX;
    double CSY;
    double sxmax;
    double sxmin;
    double symax;
    double symin;
    char selecting;
    double Cscl;
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
    list_t *components;
    list_t *compSlots;
    list_t *deleteQueue;
    list_t *inpComp;
    list_t *io;
    char keys[20];
    list_t *positions;
    list_t *selected;
    list_t *selectOb;
    list_t *wiring;
    list_t *wireTemp;
    double sinRot;
    double cosRot;
    char defaultShape;
    double defaultPrez;
    double specialPrez;
} class;

void init(class *selfp) { // initialises the class variabes (shared state)
    class self = *selfp;
    self.globalsize = 1.5;
    double themes[55] = {0, 
    0, 0, 0, 
    195, 195, 195, 
    255, 0, 0, 
    255, 146, 146, 
    230, 230, 230, 
    95, 95, 95, 
    255, 234, 0, 
    255, 248, 181, 
    255, 255, 255, 
    
    240, 240, 240, 
    60, 60, 60, 
    74, 198, 174, 
    155 ,199, 190, 
    50, 50, 50, 
    200, 200, 200, 
    136, 203, 213, 
    145, 207, 214, 
    30, 30, 30};
    memcpy(self.themeColors, themes, sizeof(self.themeColors));
    self.theme = 0;
    self.scrollSpeed = 1;
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
    self.CSX = 0;
    self.CSY = 0;
    self.sxmax = 0;
    self.sxmin = 0;
    self.symax = 0;
    self.symin = 0;
    self.selecting = 0;
    self.Cscl = 1;
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
void export(class *selfp) { // exports whatever you've made to a file called Untitled{unix_timestamp}
    class self = *selfp;
    unsigned long unixTime = (unsigned long) time(NULL);
    char preset[25];
    sprintf(preset, "Untitled%lu.txt", unixTime);
    FILE *file = fopen(preset, "w+");
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
        printf("Successfully saved to %s\n", preset);
        fclose(file);
    *selfp = self;
}
void POWER(class *selfp, double x, double y, double size, double rot, char state, char select) { // draws a POWER component
    class self = *selfp;
    rot /= 57.2958; // convert to radians
    turtleGoto(x, y);
    turtlePenShape("circle");
    turtlePenPrez(self.specialPrez);
    turtlePenSize(size * 25 * self.scaling);
    turtlePenDown();
    turtlePenUp();
    if (state == 2) {
        turtlePenSize(size * 20 * self.scaling);
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
        turtlePenSize(size * 20 * self.scaling);
        if (select == 1) {
            turtlePenColor(self.themeColors[10 + self.theme], self.themeColors[11 + self.theme], self.themeColors[12 + self.theme]);
        } else {
            turtlePenColor(self.themeColors[7 + self.theme], self.themeColors[8 + self.theme], self.themeColors[9 + self.theme]);
        }
        turtlePenDown();
        turtlePenUp();
        turtlePenColor(self.themeColors[1 + self.theme], self.themeColors[2 + self.theme], self.themeColors[3 + self.theme]);
    }
    turtools.penshape = self.defaultShape;
    turtlePenPrez(self.defaultPrez);
}
void NOT(class *selfp, double x, double y, double size, double rot) { // draws a NOT component
    class self = *selfp;
    rot /= 57.2958; // convert to radians
    double sinRot = sin(rot);
    double cosRot = cos(rot);
    turtlePenSize(size * 2 * self.scaling);
    turtleGoto(x + (-11 * size * sinRot) - (11 * size * cosRot), y + (-11 * size * cosRot) + (11 * size * sinRot));
    turtlePenDown();
    turtleGoto(x + (7 * size * sinRot), y + (7 * size * cosRot));
    turtleGoto(x + (-11 * size * sinRot) - (-11 * size * cosRot), y + (-11 * size * cosRot) + (-11 * size * sinRot));
    turtleGoto(x + (-11 * size * sinRot) - (11 * size * cosRot), y + (-11 * size * cosRot) + (11 * size * sinRot));
    turtlePenUp();
    turtleGoto(x + (10 * size * sinRot), y + (10 * size * cosRot));
    turtlePenShape("circle");
    turtlePenPrez(self.specialPrez);
    turtlePenSize(size * 7 * self.scaling);
    turtlePenDown();
    turtlePenUp();
    turtlePenSize(size * 3 * self.scaling);
    turtlePenColor(self.themeColors[25 + self.theme], self.themeColors[26 + self.theme], self.themeColors[27 + self.theme]);
    turtlePenDown();
    turtlePenUp();
    turtlePenColor(self.themeColors[1 + self.theme], self.themeColors[2 + self.theme], self.themeColors[3 + self.theme]);
    turtools.penshape = self.defaultShape;
    turtlePenPrez(self.defaultPrez);
}
void AND(class *selfp, double x, double y, double size, double rot) {// draws an AND component
    class self = *selfp;
    rot /= 57.2958; // convert to radians
    double sinRot = sin(rot);
    double cosRot = cos(rot);
    turtlePenSize(size * 2 * self.scaling);
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
void OR(class *selfp, double x, double y, double size, double rot) {// draws an OR component
    class self = *selfp;
    rot /= 57.2958; // convert to radians
    double sinRot = sin(rot);
    double cosRot = cos(rot);
    turtlePenSize(size * 2 * self.scaling);
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
void XOR(class *selfp, double x, double y, double size, double rot) {// draws an XOR component
    class self = *selfp;
    rot /= 57.2958; // convert to radians
    double sinRot = sin(rot);
    double cosRot = cos(rot);
    turtlePenSize(size * 2 * self.scaling);
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
void NOR(class *selfp, double x, double y, double size, double rot) { // draws a NOR component
    class self = *selfp;
    rot /= 57.2958; // convert to radians
    double sinRot = sin(rot);
    double cosRot = cos(rot);
    turtlePenSize(size * 2 * self.scaling);
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
    turtlePenSize(size * 7 * self.scaling);
    turtlePenDown();
    turtlePenUp();
    turtlePenSize(size * 3 * self.scaling);
    turtlePenColor(self.themeColors[25 + self.theme], self.themeColors[26 + self.theme], self.themeColors[27 + self.theme]);
    turtlePenDown();
    turtlePenUp();
    turtlePenColor(self.themeColors[1 + self.theme], self.themeColors[2 + self.theme], self.themeColors[3 + self.theme]);
    turtools.penshape = self.defaultShape;
    turtlePenPrez(self.defaultPrez);
}
void NAND(class *selfp, double x, double y, double size, double rot) { // draws a NAND component
    class self = *selfp;
    rot /= 57.2958; // convert to radians
    double sinRot = sin(rot);
    double cosRot = cos(rot);
    turtlePenSize(size * 2 * self.scaling);
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
    turtlePenSize(size * 7 * self.scaling);
    turtlePenDown();
    turtlePenUp();
    turtlePenSize(size * 3 * self.scaling);
    turtlePenColor(self.themeColors[25 + self.theme], self.themeColors[26 + self.theme], self.themeColors[27 + self.theme]);
    turtlePenDown();
    turtlePenUp();
    turtlePenColor(self.themeColors[1 + self.theme], self.themeColors[2 + self.theme], self.themeColors[3 + self.theme]);
    turtools.penshape = self.defaultShape;
    turtlePenPrez(self.defaultPrez);
}
void BUFFER(class *selfp, double x, double y, double size, double rot) { // draws a BUFFER component
    class self = *selfp;
    rot /= 57.2958; // convert to radians
    double sinRot = sin(rot);
    double cosRot = cos(rot);
    turtlePenSize(size * 2 * self.scaling);
    turtleGoto(x + (-8 * size * sinRot) - (11 * size * cosRot), y + (-8 * size * cosRot) + (11 * size * sinRot));
    turtlePenDown();
    turtleGoto(x + (10 * size * sinRot), y + (10 * size * cosRot));
    turtleGoto(x + (-8 * size * sinRot) - (-11 * size * cosRot), y + (-8 * size * cosRot) + (-11 * size * sinRot));
    turtleGoto(x + (-8 * size * sinRot) - (11 * size * cosRot), y + (-8 * size * cosRot) + (11 * size * sinRot));
    turtlePenUp();
}
void wireSymbol(class *selfp, double x, double y, double size, double rot) { // draws the wireSymbol on the sidebar
    class self = *selfp;
    rot /= 57.2958; // convert to radians
    // double sinRot = sin(rot);
    // double cosRot = cos(rot);
    turtlePenSize(size * 2 * self.scaling);
    turtleGoto(x + -12 * size, y + -9 * size);
    turtlePenDown();
    turtleGoto(x + -6 * size, y + -9 * size);
    turtleGoto(x + 6 * size, y + 9 * size);
    turtleGoto(x + 12 * size, y + 9 * size);
    turtlePenUp();
}
void copySelected(class *selfp) { // copies and pastes selected components
    class self = *selfp;
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
        list_append(self.positions, (unitype) (self.positions -> data[self.selected -> data[i].i * 3 - 2].d + self.mx * self.Cscl + self.CSX - j), 'd');
        list_append(self.positions, (unitype) (self.positions -> data[self.selected -> data[i].i * 3 - 1].d + self.my * self.Cscl + self.CSY - k), 'd');
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
void snapToGrid(class *selfp, double gridsize) { // snaps components to a grid
    // printf("%lf\n", dmod(-1, 5));
    class self = *selfp;
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
    self.CSX -= j;
    self.CSY -= k;
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
    self.CSX -= j;
    self.CSY -= k;
    *selfp = self;
}
void selectionBox(class *selfp, double x1, double y1, double x2, double y2) { // draws the selection box
    class self = *selfp;
    turtlePenColor(self.themeColors[4 + self.theme], self.themeColors[5 + self.theme], self.themeColors[6 + self.theme]);
    turtlePenSize(self.globalsize * 2 * self.scaling);
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
void deleteComp(class *selfp, int index) { // deletes a component
    class self = *selfp;
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
void hlgcompset(class *selfp) { // sets hlgcomp to whatever component the mouse is hovering over
    class self = *selfp;
    self.hlgcomp = 0;
    int len = self.components -> length;
    for (int i = 1; i < len; i++) {
        if ((self.mx * self.Cscl + self.CSX + 18) > self.positions -> data[i * 3 - 2].d && (self.mx * self.Cscl + self.CSX - 18) < self.positions -> data[i * 3 - 2].d && (self.my * self.Cscl + self.CSY + 18) > self.positions -> data[i * 3 - 1].d && (self.my * self.Cscl + self.CSY - 18) < self.positions -> data[i * 3 - 1].d) {
            self.hlgcomp = i;
        }
    }
    *selfp = self;
}
void rotateSelected(class *selfp, double degrees) { // rotates selected components by degrees
    class self = *selfp;
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
void compareAng(class *selfp, int index1, int index2, int comp1, int comp2, double x1, double y1, double x2, double y2, double x3, double y3, double rot) { // for comparisons between two components wired to one, to make sure their wires don't cross (works 95% of the time)
    class self = *selfp;
    double sinRot = sin(rot / 57.2958);
    double cosRot = cos(rot / 57.2958);
    double morphX1 = ((x3 - x1) * cosRot) - ((y3 - y1) * sinRot);
    double morphY1 = ((x3 - x1) * sinRot) - ((y3 - y1) * cosRot);
    double morphX2 = ((x3 - x2) * cosRot) - ((y3 - y2) * sinRot);
    double morphY2 = ((x3 - x2) * sinRot) - ((y3 - y2) * cosRot);
    if (asin(morphX2 / (sqrt(morphY2 * morphY2 + morphX2 * morphX2) + 0.001)) > asin(morphX1 / (sqrt(morphY1 * morphY1 + morphX1 * morphX1) + 0.001))) {
        if (comp1 == self.wiring -> data[index1].i) {
            self.wxOffE = (cosRot * -5) / self.Cscl;
            self.wyOffE = (sinRot * 5) / self.Cscl;
        } else {
            self.wxOffE = (cosRot * 5) / self.Cscl;
            self.wyOffE = (sinRot * -5) / self.Cscl;
        }
    } else {
        if (comp1 == self.wiring -> data[index1].i) {
            self.wxOffE = (cosRot * 5) / self.Cscl;
            self.wyOffE = (sinRot * -5) / self.Cscl;
        } else {
            self.wxOffE = (cosRot * -5) / self.Cscl;
            self.wyOffE = (sinRot * 5) / self.Cscl;
        }
    }
    *selfp = self;
}
void wireIO(class *selfp, int index1, int index2) { // this script actually performs the logic of the logic gates, this will update the output of a gate given it's two inputs
    class self = *selfp;
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
            self.positions -> data[tempAng - 2].d + sin(self.positions -> data[tempAng].d / 57.2958) * 22.5 / self.Cscl, 
            self.positions -> data[tempAng - 1].d + cos(self.positions -> data[tempAng].d / 57.2958) * 22.5 / self.Cscl, 
            self.positions -> data[tempAng2 - 2].d + sin(self.positions -> data[tempAng2].d / 57.2958) * 22.5 / self.Cscl, 
            self.positions -> data[tempAng2 - 1].d + cos(self.positions -> data[tempAng2].d / 57.2958) * 22.5 / self.Cscl, 
            self.positions -> data[tempAng3 - 2].d + sin(self.positions -> data[tempAng3].d / 57.2958) * 22.5 / self.Cscl, 
            self.positions -> data[tempAng3 - 1].d + cos(self.positions -> data[tempAng3].d / 57.2958) * 22.5 / self.Cscl, 
            self.positions -> data[tempAng3].d);
        }
    } else {
        self.wxOffE = 0;
        self.wyOffE = 0;
    }
    *selfp = self;
}
void renderComp(class *selfp) { // this function renders all the components in the window
    class self = *selfp;
    list_clear(self.selectOb);
    list_append(self.selectOb, (unitype) "null", 's');
    int len = self.components -> length;
    for (int i = 1; i < len; i++) {
        double renderX = (self.positions -> data[i * 3 - 2].d - self.CSX) / self.Cscl;
        double renderY = (self.positions -> data[i * 3 - 1].d - self.CSY) / self.Cscl;
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
void renderWire(class *selfp, double size) { // this function renders all the wiring in the window (a bit buggy if the components are outside the window, it doesn't do line intercepts and is likely bounded by total screen size but if I were to do bound intercepts I would do it in the turtle abstration)
    class self = *selfp;
    turtlePenSize(size * 2 * self.scaling);
    int len = self.wiring -> length - 1;
    for (int i = 1; i < len; i += 3) {
        wireIO(&self, i, i + 1);
        double wireTXS = (self.positions -> data[self.wiring -> data[i].i * 3 - 2].d - self.CSX) / self.Cscl;
        double wireTYS = (self.positions -> data[self.wiring -> data[i].i * 3 - 1].d - self.CSY) / self.Cscl;
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
        turtleGoto(wireTXS + sin((self.positions -> data[self.wiring -> data[i].i * 3].d) / 57.2958) * 22.5 / self.Cscl, wireTYS + cos((self.positions -> data[self.wiring -> data[i].i * 3].d) / 57.2958) * 22.5 / self.Cscl);
        double wireTXE = (self.positions -> data[self.wiring -> data[i + 1].i * 3 - 2].d - self.CSX) / self.Cscl;
        double wireTYE = (self.positions -> data[self.wiring -> data[i + 1].i * 3 - 1].d - self.CSY) / self.Cscl;
        // wireTXE = 0;
        // wireTYE = 0;
        turtleGoto(wireTXE - (sin((self.positions -> data[self.wiring -> data[i + 1].i * 3].d) / 57.2958) * 22.5 / self.Cscl + self.wxOffE), wireTYE - (cos((self.positions -> data[self.wiring -> data[i + 1].i * 3].d) / 57.2958) * 22.5 / self.Cscl + self.wyOffE));
        // turtleGoto(0, 0);
        turtleGoto(wireTXE, wireTYE);
        turtlePenUp();
        turtlePenColor(self.themeColors[1 + self.theme], self.themeColors[2 + self.theme], self.themeColors[3 + self.theme]);
    }
    *selfp = self;
}
void renderSidebar(class *selfp, char side) { // this function draws the sidebar, but really its never on the side it's a bottom or top bar
    class self = *selfp;
    turtlePenColorAlpha(self.themeColors[13 + self.theme], self.themeColors[14 + self.theme], self.themeColors[15 + self.theme], 55);
    turtlePenSize(60 * self.scaling);
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
void mouseTick(class *selfp) { // all the functionality for the mouse is handled in this beast of a function, it's really messy and super hard to understand
    class self = *selfp;
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
                        list_append(self.positions, (unitype) (self.mx * self.Cscl + self.CSX), 'd');
                        list_append(self.positions, (unitype) (self.my * self.Cscl + self.CSY), 'd');
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
                                self.offX = self.positions -> data[self.hglmove * 3 - 2].d - (self.mx * self.Cscl + self.CSX);
                                self.offY = self.positions -> data[self.hglmove * 3 - 1].d - (self.my * self.Cscl + self.CSY);
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
                        list_append(self.positions, (unitype) (self.mx * self.Cscl + self.CSX), 'd');
                        list_append(self.positions, (unitype) (self.my * self.Cscl + self.CSY), 'd');
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
                    self.FocalCSX = self.CSX;
                    self.FocalCSY = self.CSY;
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
                self.FocalCSX = self.CSX;
                self.FocalCSY = self.CSY;
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
                self.FocalCSX = self.CSX;
                self.FocalCSY = self.CSY;
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
                turtlePenSize(self.globalsize * 2 * self.scaling);
                if (list_count(self.selected, (unitype) self.wiringStart, 'i') > 0 && self.selecting > 2) {
                    int len = self.selected -> length;
                    for (int i = 1; i < len; i++) {
                        turtleGoto((self.positions -> data[self.selected -> data[i].i * 3 - 2].d - self.CSX) / self.Cscl, (self.positions -> data[self.selected -> data[i].i * 3 - 1].d - self.CSY) / self.Cscl);
                        turtlePenDown();
                        if ((!(self.hlgcomp == 0 && !(self.hlgcomp == self.wiringStart))) && (self.inpComp -> data[self.wiringEnd * 3 - 1].i == 0 || (self.inpComp -> data[self.wiringEnd * 3].i == 0 && !(self.inpComp -> data[self.wiringEnd * 3 - 1].i == self.wiringStart && self.inpComp -> data[self.wiringEnd * 3 - 2].i > 1))))
                            turtleGoto((self.positions -> data[self.hlgcomp * 3 - 2].d - self.CSX) / self.Cscl, (self.positions -> data[self.hlgcomp * 3 - 1].d - self.CSY) / self.Cscl);
                        else
                            turtleGoto(self.mx, self.my);
                        turtlePenUp();
                    }
                } else {
                    if (list_count(self.selected, (unitype) self.hlgcomp, 'i') > 0 && self.selecting > 1) {
                        int len = self.selected -> length;
                        for (int i = 1; i < len; i++) {
                            turtleGoto((self.positions -> data[self.wiringStart * 3 - 2].d - self.CSX) / self.Cscl, (self.positions -> data[self.wiringStart * 3 - 1].d - self.CSY) / self.Cscl);
                            turtlePenDown();
                            turtleGoto((self.positions -> data[self.selected -> data[i].i * 3 - 2].d - self.CSX) / self.Cscl, (self.positions -> data[self.selected -> data[i].i * 3 - 1].d - self.CSY) / self.Cscl);
                            turtlePenUp();
                        }
                    } else {
                        turtleGoto((self.positions -> data[self.wiringStart * 3 - 2].d - self.CSX) / self.Cscl, (self.positions -> data[self.wiringStart * 3 - 1].d - self.CSY) / self.Cscl);
                        turtlePenDown();
                        if (!(self.hlgcomp == 0) && !(self.hlgcomp == self.wiringStart) && (self.inpComp -> data[self.wiringEnd * 3 - 1].i == 0 || (self.inpComp -> data[self.wiringEnd * 3].i == 0 && !(self.inpComp -> data[self.wiringEnd * 3 - 1].i == self.wiringStart) && self.inpComp -> data[self.wiringEnd * 3 - 2].i > 1)))
                            turtleGoto((self.positions -> data[self.hlgcomp * 3 - 2].d - self.CSX) / self.Cscl, (self.positions -> data[self.hlgcomp * 3 - 1].d - self.CSY) / self.Cscl);
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
                    self.FocalCSX = self.CSX;
                    self.FocalCSY = self.CSY;
                    self.wiringEnd = self.hlgcomp;
                } else {
                    if (strcmp(self.holding, "a") == 0) {
                        self.CSX = (self.FocalX - self.mx) * self.Cscl + self.FocalCSX;
                        self.CSY = (self.FocalY - self.my) * self.Cscl + self.FocalCSY;
                    }
                }
            } else {
                if (self.selecting == 3) {
                    double anchorX = self.positions -> data[self.hglmove * 3 - 2].d;
                    double anchorY = self.positions -> data[self.hglmove * 3 - 1].d;
                    int len = self.selected -> length;
                    for (int i = 1; i < len; i++) {
                        self.positions -> data[self.selected -> data[i].i * 3 - 2] = (unitype) (self.positions -> data[self.selected -> data[i].i * 3 - 2].d + self.mx * self.Cscl + self.CSX + self.offX - anchorX);
                        self.positions -> data[self.selected -> data[i].i * 3 - 1] = (unitype) (self.positions -> data[self.selected -> data[i].i * 3 - 1].d + self.my * self.Cscl + self.CSY + self.offY - anchorY);
                    }
                } else {
                    self.positions -> data[self.hglmove * 3 - 2] = (unitype) (self.mx * self.Cscl + self.CSX + self.offX);
                    self.positions -> data[self.hglmove * 3 - 1] = (unitype) (self.my * self.Cscl + self.CSY + self.offY);
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
                list_append(self.positions, (unitype) (self.mx * self.Cscl + self.CSX), 'd');
                list_append(self.positions, (unitype) (self.my * self.Cscl + self.CSY), 'd');
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
    *selfp = self;
}
void hotkeyTick(class *selfp) { // most of the keybind functionality is handled here
    class self = *selfp;
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
        if (!self.keys[5])
            export(&self);
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
    if (turtleKeyPressed(GLFW_KEY_N) || turtleKeyPressed(GLFW_KEY_W) || turtleKeyPressed(GLFW_KEY_2)) { // n, w, or 2
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
            if (self.theme == 0)
                self.theme = 27;
            else
                self.theme = 0;
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
            self.sidebar += 1;
            if (self.sidebar > 2)
                self.sidebar = 0;
        }
        self.keys[18] = 1;
    } else {
        self.keys[18] = 0;
    }
    if (turtleKeyPressed(GLFW_KEY_Z)) { // z key
        if (!self.keys[19]) {
            snapToGrid(&self, 48);
        }
        self.keys[19] = 1;
    } else {
        self.keys[19] = 0;
    }
    // printf("[");
    // for (int i = 0; i < 20; i++) {
    //     printf("%d", self.keys[i]);
    // }
    // printf("]\n");
    *selfp = self;
}
void scrollTick(class *selfp) { // all the scroll wheel functionality is handled here
    class self = *selfp;
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
            if (self.Cscl > 0.15) {
                self.CSX += self.mx * 0.1 * self.scrollSpeed;
                self.CSY += self.my * 0.1 * self.scrollSpeed;
                self.Cscl -= 0.1 * self.scrollSpeed;
                self.globalsize = 1.5 / self.Cscl;
            }
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
            self.CSX -= self.mx * 0.1 * self.scrollSpeed;
            self.CSY -= self.my * 0.1 * self.scrollSpeed;
            self.Cscl += 0.1 * self.scrollSpeed;
            self.globalsize = 1.5 / self.Cscl;
        }
    }
    if (self.mw == 0) {
        self.rotateCooldown = 1;
    }
    *selfp = self;
}

int main(int argc, char *argv[]) {
    GLFWwindow* window;
    /* Initialize glfw */
    if (!glfwInit()) {
        return -1;
    }

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(960, 720, "Logic Gates", NULL, NULL);
    if (!window) {
        glfwTerminate();
    }
    glfwMakeContextCurrent(window);
    glfwSetWindowSizeLimits(window, GLFW_DONT_CARE, GLFW_DONT_CARE, 960, 720);
    gladLoadGL();

    /* initialize turtools */
    turtoolsInit(window, -240, -180, 240, 180);
    int tps = 60; // ticks per second (locked to fps in this case)
    clock_t start, end;
    class self;
    init(&self); // initialise the class
    turtools.penshape = self.defaultShape; // set the shape
    turtlePenPrez(self.defaultPrez); // set the prez

    if (argc > 1) {
        printf("Attempting to load %s\n", argv[1]);
        FILE *file = fopen(argv[1], "r");
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
            printf("%s Bad Read\n", argv[1]);
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
                list_append(self.io, (unitype) int1,'i');
            }
            for (int i = 0; i < num * 3; i++) {
                end = fscanf(file, "%d", &int1);
                list_append(self.inpComp, (unitype) int1,'i');
            }
            while (end != EOF) {
                end = fscanf(file, "%d", &int1);
                if (end != EOF)
                    list_append(self.wiring, (unitype) int1,'i');
            }
            self.CSX = self.positions -> data[1].d;
            self.CSY = self.positions -> data[2].d;
            self.Cscl = 1;
            printf("%s loaded successfully\n", argv[1]);
            fclose(file);
        }
    }
    int frame = 0;
    while (turtools.close == 0) {
        start = clock(); // for frame syncing
        turtleGetMouseCoords(); // get the mouse coordinates
        if (turtools.mouseX > 240) { // bound mouse coordinates to window coordinates
            self.mx = 240;
        } else {
            if (turtools.mouseX < -240) {
                self.mx = -240;
            } else {
                self.mx = turtools.mouseX;
            }
        }
        if (turtools.mouseY > 180) {
            self.my = 180;
        } else {
            if (turtools.mouseY < -180) {
                self.my = -180;
            } else {
                self.my = turtools.mouseY;
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
        mouseTick(&self);
        hotkeyTick(&self);
        scrollTick(&self);
        turtleUpdate(); // update the screen
        end = clock();
        if (frame % 60 == 0) {
            frame = 0;
            // printf("ms: %ld\n", end - start);
            // printf("components: %d\n", self.components -> length);
        }
        frame += 1;
        while ((double) (end - start) / CLOCKS_PER_SEC < (1 / (double) tps)) {
            end = clock();
        }
        list_print(turtools.keyPressed);
    }
}