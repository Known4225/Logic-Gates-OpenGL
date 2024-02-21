/* A generic popup box rendered with openGL */

#ifndef POPUPSET
#define POPUPSET 1 // include guard
#include "textGL.h"

typedef struct { // popup variables
    char *message; // message displayed on the popup
    double minX; // left edge of box
    double minY; // bottom of box
    double maxX; // right edge of box
    double maxY; // top of box
    list_t *options; // list of popup box options
    /*
    style:
    0 - default
    1 - 
    */
    char style;
} popup_t;

popup_t popup;

void popupInit(const char *filename, double minX, double minY, double maxX, double maxY) {
    popup.minX = minX;
    popup.minY = minY;
    popup.maxX = maxX;
    popup.maxY = maxY;
    // read information from config file
    FILE *configFile = fopen(filename, "r");
    if (configFile == NULL) {
        printf("Error: file %s not found\n", filename);
        return -1;
    }
    char throw[256]; // maximum size of message or option
    char checksum = fscanf(configFile, "%s", throw); // read message
    popup.message = strdup(throw);
    popup.options = list_init();
    while (checksum != EOF) {
        checksum = fscanf(configFile, "%s", throw);
        if (checksum != EOF) {
            list_append(popup.options, (unitype) strdup(throw), 's');
        }
    }
}

void popupUpdate() {
    turtleQuad(popup.minX, popup.minY, popup.minX, popup.maxY, popup.maxX, popup.maxY, popup.maxX, popup.minY, 130, 130, 130, 0);
    double textSize = 12;
    double textX = popup.minX + (popup.maxX - popup.minX) / 2;
    double textY = popup.maxY - textSize;
    textGLWriteString(popup.message, textX, textY, textSize, 50);
    textY -= textSize * 2;
    textX = popup.minX + textSize;
    for (int i = 0; i < popup.options -> length; i++) {
        textGLWriteString(popup.message, textX, textY, textSize, 0);
        textX += textGLGetStringLength(popup.options -> data[i].s, textSize) + textSize;
    }
}

void popupFree() {
    free(popup.message);
}
#endif