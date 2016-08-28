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

static double Xmin =  100;
static double Xmax =  -100;
static double Ymin = 12;
static double Ymax =  1;


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

    //glFlush();
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

void paintPolygon(int vertexAmount, struct Coordenada *coordenadas, void (*f)(int,int)){
    int i;
    int Yf[vertexAmount];
    int Xf[vertexAmount];
    for(i = 0; i < vertexAmount; i++){
        Yf[i] = (int) (res * ((coordenadas[i].latitud - Ymin) / (Ymax - Ymin)));
        Xf[i] = (int) (res * ((coordenadas[i].longitud - Xmin) / (Xmax - Xmin)));
    }
    printf("%d\n", Yf[0]);
    printf("%d\n", Xf[0]);
    for(i = 0; i < vertexAmount - 1; i++){
        bresenham(Xf[i], Yf[i], Xf[i+1], Yf[i+1],(*f));
    }
    bresenham(Xf[vertexAmount-1], Yf[vertexAmount-1], Xf[0], Yf[0], (*f));
}

int main(int argc, char *argv[]){
    buffer = (COLOR **)malloc(res * sizeof(COLOR*));
    
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(res,res);
    glutCreateWindow("CG Proyecto 0");
    glClear(GL_COLOR_BUFFER_BIT);
    gluOrtho2D(-0.5, res +0.5, -0.5, res + 0.5);



    FILE* file = fopen("test.txt", "r");
    int vertexAmount = 0;
    int c;
    while ((c = getc(file)) != EOF){
        if(c == '\n'){
            vertexAmount++;
        }
    }
    vertexAmount++;
    fclose(file);
    printf("%d", vertexAmount);
    printf("\n");


    struct Coordenada coordAlajuela[vertexAmount];       //1
    struct Coordenada coordCartago[vertexAmount];        //2
    struct Coordenada coordGuanacaste[vertexAmount];     //3
    struct Coordenada coordHeredia[vertexAmount];        //4
    struct Coordenada coordLimon[vertexAmount];          //5
    struct Coordenada coordPuntarenas[vertexAmount];     //6
    struct Coordenada coordSanJose[vertexAmount];        //7



    FILE* g = fopen("test.txt", "r");
    char comma;
    double lon, lat;
    int i;
    int j;

    for(i = 0; i < vertexAmount; i++){
        for(j = 0; j < 2; j++){
            if(j == 0){
                fscanf(g,"%lf", &lon);
                fscanf(g, "%c", &comma);
            }else{
                fscanf(g,"%lf", &lat);
                //fscanf(g, "%c", &comma);
            }
        }
        coordAlajuela[i].longitud = lon;
        coordAlajuela[i].latitud = lat;
        coordAlajuela[i].w = 1;
    }
    fclose(g);
    /*
    for(i = 0; i < vertexAmount; i++){
        printf("%d", i);
        printf("\n");
        printf("%.14lf ", coordAlajuela[i].longitud);
        printf("%.14lf ", coordAlajuela[i].latitud);
        printf("\n");
    }*/

    //double minX = 100;
    //double minY = 12;
    //double maxX = -100;
    //double maxY = 1;

    for(i = 0; i < vertexAmount; i++){
        Xmin = min(Xmin, coordAlajuela[i].longitud);
        Xmax = max(Xmax, coordAlajuela[i].longitud);
        Ymin = min(Ymin, coordAlajuela[i].latitud);
        Ymax = max(Ymax, coordAlajuela[i].latitud);
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

    //printf("Jk");
    //printf("\n");

    paintPolygon(vertexAmount, coordAlajuela, plot);
    glFlush();
    glutMainLoop();
}
