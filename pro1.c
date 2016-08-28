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

static int vertexAmounts[7];
static int provinceCounter = 0;

typedef struct {
  double r;
  double g;
  double b;
} COLOR;

COLOR **buffer;

struct Coordenada {
    double longitud;
    double latitud;
    double w;
};

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

void calculateMinMax(int vertexAmount, struct Coordenada *coordenadas){
    int i;
    Xmin = min(coordenadas[0].longitud, coordenadas[1].longitud);
    Xmax = max(coordenadas[0].longitud, coordenadas[1].longitud);
    Ymin = min(coordenadas[0].latitud, coordenadas[1].latitud);
    Ymax = max(coordenadas[0].latitud, coordenadas[1].latitud);

    for(i = 0; i < vertexAmount; i++){
        Xmin = min(Xmin, coordenadas[i].longitud);
        Xmax = max(Xmax, coordenadas[i].longitud);
        Ymin = min(Ymin, coordenadas[i].latitud);
        Ymax = max(Ymax, coordenadas[i].latitud);
    }
    printf("Minimos: X = ");
    printf("%.14lf ",Xmin);
    printf("  Y = ");
    printf("%.14lf ",Ymin);
    printf("\n");

    printf("Maximos: X = ");
    printf("%.14lf ",Xmax);
    printf("  Y = ");
    printf("%.14lf ",Ymax);
    printf("\n");
}

void paintPolygon(int vertexAmount, struct Coordenada *coordenadas, void (*f)(int,int), int counter){
    int i;
    int Yf[vertexAmount];
    int Xf[vertexAmount];
    for(i = 0; i < vertexAmount; i++){
        Yf[i] = (int) (res * ((coordenadas[counter + i].latitud - Ymin) / (Ymax - Ymin)));
        Xf[i] = (int) (res * ((coordenadas[counter + i].longitud - Xmin) / (Xmax - Xmin)));
    }
    for(i = 0; i < vertexAmount - 1; i++){
        bresenham(Xf[i], Yf[i], Xf[i+1], Yf[i+1],(*f));
    }
    bresenham(Xf[vertexAmount-1], Yf[vertexAmount-1], Xf[0], Yf[0], (*f));
}

void readFiles(){
    char *provinces[7] = {"mapa/Puntarenas.txt",
                          "mapa/Alajuela.txt",
                          "mapa/Cartago.txt",
                          "mapa/Limon.txt", 
                          "mapa/SanJose.txt",
                          "mapa/Heredia.txt",
                          "mapa/Guanacaste.txt"};
    char comma;
    int i, j, k, c, vertexAmount;
    int totalVertexCount = 0;
    int vertexAmounts[7];
    int counter = 0;
    double lon, lat;
    int ptp = 7;
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
    struct Coordenada coordenadas[totalVertexCount];
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
            coordenadas[counter + i].longitud = lon;
            coordenadas[counter + i].latitud = lat;
            coordenadas[counter + i].w = 1;
        }
        counter += vertexAmounts[k];
        fclose(g);
    }
    calculateMinMax(totalVertexCount,coordenadas);
    counter = 0;
    for(i = 0; i < ptp; i++){
        
        if (i==0) {glColor3f (0,1,1);}    
        if (i==1) {glColor3f (1,1,0);}
        if (i==2) {glColor3f (1,0,0);}
        if (i==3) {glColor3f (1,0,1);}
        if (i==4) {glColor3f (1,0,0);}
        if (i==5) {glColor3f (1,1,1);}
        if (i==6) {glColor3f (0,1,0);}
        if (i==7) {glColor3f (0,0,1);}

        paintPolygon(vertexAmounts[i], coordenadas, plot, counter);
        counter += vertexAmounts[i];
    }
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
    glFlush();
    glutMainLoop();
}
