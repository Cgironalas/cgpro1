#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "malloc.h"
#include <time.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

static int res = 800;

static double Xmin;
static double Xmax;
static double Ymin;
static double Ymax;

static int provinceCounter = 0;

struct Coord {
    double longitud;
    double latitud;
    double w;
};

static double geoT[9] = {1,0,0, 0,1,0, 0,0,1};
static int ptp = 7;
static int vertexAmounts[7];
static struct Coord *coords;

typedef struct {
  double r;
  double g;
  double b;
} COLOR;

COLOR **buffer;



double min(double a, double b){
    if(a < b) { return a; }
    else { return b; }
}

double max(double a, double b){
    if(a < b) { return b; }
    else { return a; }
}

void plot (int x, int y){
    COLOR color;

    glBegin (GL_POINTS);
    glVertex2i (x,y);
    glEnd();
}

void bresenham (int x0, int y0, int x1, int y1, void (*plot)(int,int)){
    int d2x,d2y,dx,dy,d,
        Delta_N,Delta_NE,Delta_E,Delta_SE,
        Delta_S,Delta_SW,Delta_W,Delta_NW,
        xp,yp;

    xp = x0; yp = y0;
    (*plot)(xp,yp);

    d2y = 2*(y1-y0);
    d2x = 2*(x1-x0);
    dy  = (y1-y0);
    dx  = (x1-x0);

    if (y1 > y0) { //1,2,3 o 4
        if (x1 > x0) { //1 o 2
            Delta_NE =  d2y-d2x;
            if ( dy <= dx ){ //Octante 1

                Delta_E  =  d2y;
                d = d2y-dx;

                while (xp < x1){ //Avanza en x
                    xp++;
                    if (d<0) {
                        d += Delta_E;
                    }
                    else {
                        yp++;
                        d += Delta_NE;
                    }
                    (*plot)(xp,yp);
                }
            }
            else{ //Octante 2

                Delta_N  = -d2x;
                d = dy -d2x;

                while (yp < y1){ //Avanza en y
                    yp++;
                    if (d<=0) {
                        xp++;
                        d += Delta_NE;
                    }
                    else {
                        d += Delta_N;
                    }
                    (*plot)(xp,yp);
                }
            }
        }
        else { //3 u 4
            Delta_NW = -d2y-d2x;
            if (dx >= -dy){ //Octante 3

                Delta_N  = -d2x;
                d = -dy -d2x;

                while (yp < y1){ //Avanza en y
                    yp++;
                    if (d<=0) {
                        d += Delta_N;
                    }
                    else {
                        xp--;
                        d += Delta_NW;
                    }
                    (*plot)(xp,yp);
                }
            }
            else{ //Octante 4
                Delta_W  = -d2y;
                d = -d2y-dx ;

                while (xp > x1){ //Retrocede en x
                    xp--;
                    if (d<=0) {
                        yp++;
                        d += Delta_NW;
                    }
                    else {
                        d += Delta_W;
                    }
                    (*plot)(xp,yp);
                }
            }
        }
    }
    else { //5,6,7 u 8
        if (x1 < x0) { //5 o 6
            Delta_SW = -d2y+d2x;
            if (dx <= dy){ //Octante 5

                Delta_W  = -d2y   ;
                d = -d2y+dx ;

                while (xp > x1){ //Retrocede en x
                    xp--;
                    if (d<=0) {
                        d += Delta_W;
                    }
                    else {
                        yp--;
                        d += Delta_SW;
                    }
                    (*plot)(xp,yp);

                }
            }
            else{ //Octante 6

                Delta_S  =  d2x;
                d = -dy +d2x;

                while (yp > y1){ //Retrocede en y
                    yp--;
                    if (d<0) {
                        xp--;
                        d += Delta_SW;
                    }
                    else {
                        d += Delta_S;
                    }
                    (*plot)(xp,yp);

                }
            }
        }
        else { //7 u 8
            Delta_SE =  d2y+d2x;
            if (dx <= -dy){ //Octante 7

                Delta_S  =  d2x   ;
                d =  dy +d2x;

                while (yp > y1){ //Retrocede en y
                    yp--;
                    if (d<0) {
                        d += Delta_S;
                    }
                    else {
                        xp++;
                        d += Delta_SE;
                    }
                    (*plot)(xp,yp);

                }
            }
            else{ //Octante 8

                Delta_E  =  d2y;
                d =  d2y+dx;

                while (xp < x1){ //Avanza en x
                    xp++;
                    if (d<0) {
                        yp--;
                        d += Delta_SE;
                    }
                    else {
                        d += Delta_E;
                    }
                    (*plot)(xp,yp);

                }
            }
        }
    }
}

void calculateMinMax(int vertexAmount){
    int i;
    Xmin = min(coords[0].longitud, coords[1].longitud);
    Xmax = max(coords[0].longitud, coords[1].longitud);
    Ymin = min(coords[0].latitud, coords[1].latitud);
    Ymax = max(coords[0].latitud, coords[1].latitud);

    for(i = 0; i < vertexAmount; i++){
        Xmin = min(Xmin, coords[i].longitud);
        Xmax = max(Xmax, coords[i].longitud);
        Ymin = min(Ymin, coords[i].latitud);
        Ymax = max(Ymax, coords[i].latitud);
    }
}

void paintPolygon(int vertexAmount, struct Coord *coords, void (*f)(int,int), int counter){
    int i;
    int Yf[vertexAmount];
    int Xf[vertexAmount];

    for(i = 0; i < vertexAmount; i++){
        Yf[i] = (int) (res * ((coords[counter + i].latitud - Ymin) / (Ymax - Ymin)));
        Xf[i] = (int) (res * ((coords[counter + i].longitud - Xmin) / (Xmax - Xmin)));
    }
    
    for(i = 0; i < vertexAmount - 1; i++){
        bresenham(Xf[i], Yf[i], Xf[i+1], Yf[i+1],(*f));
    }
    
    bresenham(Xf[vertexAmount-1], Yf[vertexAmount-1], Xf[0], Yf[0], (*f));
}

void readFiles(){

    char *provinces[7] = {"mapa/Puntarenas.txt",
                          "mapa/Alajuela.txt",
                          "mapa/Limon.txt", 
                          "mapa/SanJose.txt",
                          "mapa/Heredia.txt",
                          "mapa/Guanacaste.txt",
                          "mapa/Cartago.txt"  };
    
    char comma;
    int i, j, k, c, vertexAmount;
    int totalVertexCount = 0;

    int counter = 0;
    double lon, lat;

    for(i = 0; i < ptp; i++){
        FILE* file = fopen(provinces[i], "r");
        vertexAmount = 0;
        while ((c = getc(file)) != EOF){
            if(c == '\n'){
                vertexAmount++;
                totalVertexCount++;
            }
        }
        vertexAmount++;
        totalVertexCount++;
        fclose(file);
        vertexAmounts[i] = vertexAmount;
    }
    
    //coords = (struct Coord*)calloc(totalVertexCount, sizeof(struct Coord));
    coords = malloc(sizeof(struct Coord)*totalVertexCount);

    for(k = 0; k < ptp; k++){
        FILE* g = fopen(provinces[k], "r");
        for(i = 0; i < vertexAmounts[k]; i++){
            for(j = 0; j < 2; j++){
                if(j == 0){
                    fscanf(g,"%lf", &lon);
                    fscanf(g, "%c", &comma);
                }else{
                    fscanf(g,"%lf", &lat);
                }
            }
            coords[counter + i].longitud = lon;
            coords[counter + i].latitud = lat;
            coords[counter + i].w = 1;
        }
        counter += vertexAmounts[k];
        fclose(g);
    }
    
    calculateMinMax(totalVertexCount);
}

void drawBorders (struct Coord *pParam, int pene) {
    int counter = 0;
    int i;
    for(i = 0; i < ptp; i++){

        glColor3f ( ((double)i*50)/255 , pene , ((double)i+50)/255 ); 
        
        paintPolygon(vertexAmounts[i], pParam, plot, counter);
        counter += vertexAmounts[i];
    }
    //free(coords);
}


int main(int argc, char *argv[]){
    buffer = (COLOR **)malloc(res * sizeof(COLOR*));
    
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(res,res);
    glutCreateWindow("CG Proyecto 0");
    glClear(GL_COLOR_BUFFER_BIT);
    gluOrtho2D(-0.5, res +0.5, -0.5, res + 0.5);

    readFiles();
    drawBorders(coords, 1);
    glFlush();
    glutMainLoop();
}
