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


void renderScene(void){

    glClearColor(0.0f, 0.0f, 0.0f ,1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    printf("Xmin: %lf \t Ymin: %lf \n Xmax: %lf \t Ymax: %lf", Xmin, Ymin, Xmax, Ymax);
    drawBorders(coords, 0);
    glFlush();
    glutSwapBuffers();
}

void processKeyPressed(unsigned char key, int x, int y){



    int modoTecla = glutGetModifiers();

    switch (key){

        case 43: //Se presiona + Zoom In

            break;

        case 45:  //Se presiona - Zoom Out


            break;
    }
    if (modoTecla==GLUT_ACTIVE_SHIFT){
            printf("Shift pressed\n");
        }else{
            printf("Good bye\n");
            exit(0);    
        }
        
}


void mouse(int button, int state, int x, int y){

    if ((button==3) || (button==4)){  //3 es scroll up y 4 scroll down

        if (state== GLUT_UP){
            return;
        }
        int shiftPressed = glutGetModifiers();
        if (shiftPressed==GLUT_ACTIVE_SHIFT){
            
            printf("Fast Scroll %s en (%d , %d ) \n", (button==3)? "Up":"Down", x, y);
        }

        printf("Scroll %s en (%d , %d ) \n", (button==3)? "Up":"Down", x, y);
    }else{
        printf("Mouse click %s en (%d , %d ) \n", (state==GLUT_DOWN)? "Down":"Up", x, y);
    }
}

void specialKeys(int key, int x, int y){

    int specialMode = glutGetModifiers();
    int directionPan;
    switch (key){

        case GLUT_KEY_UP:
            directionPan = 0;
            printf("Panning up \n");
            panning  (directionPan, specialMode);
            break;

        case GLUT_KEY_DOWN:
            directionPan = 1;
            printf("Panning down \n");
            panning  (directionPan, specialMode);
            break;

        case GLUT_KEY_RIGHT:
            directionPan = 2;
            printf("Panning right \n");
            panning  (directionPan, specialMode);
            break;

        case GLUT_KEY_LEFT:
            directionPan = 3;
            printf("Panning left \n");
            panning (directionPan, specialMode);
            break;
    }
}

void panning(unsigned int directionPan , int specialMode){

    if (specialMode == GLUT_ACTIVE_SHIFT){
        panEntireScene(2, directionPan, 0.3);

        printf("Fast panning \n");
        
    }else if (specialMode == GLUT_ACTIVE_CTRL){

        printf("Slow panning \n");
        panEntireScene(1, directionPan, 0.05);
    }else{ //Modo normal
        panEntireScene(0, directionPan, 0.125);
    }
}

//Trazo de la línea entre dos puntos.
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


//Algoritmo de clipeo con los vértices de la línea pasados POR VALOR.
void cohenSutherland(double edgeLeft, double edgeRight, double edgeBottom, double edgeTop, double *x0, double *y0, double *x1, double *y1){

    unsigned int p0Pos[4] = {0, 0 , 0, 0};
    unsigned int p1Pos[4] = {0, 0 , 0, 0};
    
    for (int i = 0; i<4; i++){
        switch(i){
            case 0: //Left edge
                if (*x0<edgeLeft){
                    p0Pos[3]=1;
                }

                if (*x1<edgeLeft){
                    p1Pos[3]=1;
                }

            case 1: //Right edge

                if (*x0>edgeRight){
                    p0Pos[2]=1;
                }
                if (*x1>edgeRight){
                    p1Pos[2]=1;
                }

            case 2: //Bottom edge

                if (*y0<edgeBottom){
                    p0Pos[1]=1;
                }
                if (*y1<edgeBottom){

                    p1Pos[1]=1;
                }

            case 3: //Top edge

                if (*y0>edgeTop){
                    p0Pos[0]=1;
                }
                if (*y0>edgeTop){

                    p1Pos[0]=1;
                }
        }

        //AND Operation of the vertexes of the line

        unsigned int orResult[4] = {0, 0 , 0, 0};

        for (i=0;i<4;i++){
            if(p0Pos[i]+p1Pos[i]==2){
                return; //Trivially rejected.
            }else if (p0Pos[i]+p1Pos[i]==1){
                orResult[i]=1; //Two shots at once, let's make OR operation.
            }
        }
        
        if (orResult[0]==0 && orResult[1]==0 && orResult[2]==0 && orResult[3]==0 ){
            return; //Trivially accepted.
        }else{
            double m = ((*y1 - *y0)/(*x1- *x0));
            double b = *y0 -(m)*(*x0);
            //We calculate these values right now.

            if (orResult[0]==1){ //Top Edge intersection.
                if (p0Pos[0]==1){ //If the source point was outside
                    *x0 = (edgeTop - b)/m;
                    *y1=edgeTop;
                }else{ //Or the destiny point
                    *x1 = (edgeTop - b)/m;
                    *y1=edgeTop;
                }
            }
            if(orResult[1]==1){ //Bottom Edge intersection.
                if (p0Pos[1]==1){ ///If the source point was outside
                   *x0 = (edgeBottom - b)/m;
                   *y0=edgeBottom;
                }else{ //Or the destiny point
                    *x1 = (edgeBottom - b)/m;
                    *y1=edgeBottom;
                }
            }
            if(orResult[2]==1){ //Right Edge intersection.
                if (p0Pos[2]==1){ //If the source point was outside
                    *y0 = m*(edgeRight) + b;
                    *x0=edgeRight;
                }else{ //Or the destiny point
                    *y1 = m*(edgeRight) + b;
                    *x1=edgeRight;
                }
            }
            if(orResult[3]==1){ //Left Edge intersection.
                if (p0Pos[2]==1){ //If the source point was outside
                    *y0 = m*(edgeLeft) + b;
                    *x0=edgeLeft;
                }else{ //Or the destiny point
                    *y1 = m*(edgeLeft) + b;
                    *x1=edgeLeft;
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


/*
    mode{0==NORMAL, 1 slow y 2 fast}
    direction {0 = up, 1 = down, 2= right, 3 = left}
    0.0 percentage < 1.0 para evitar división
*/
void panEntireScene(unsigned int mode, unsigned int direction, double percentage){

    if (direction== 2 || direction==3){ //ES paneo horizontal
        
        double xDelta=Xmax-Xmin;

        if (direction==2){ //Es para la derecha

            Xmax-= xDelta * percentage;
            Xmin-= xDelta * percentage;

        }else{ //Es para la izquierda

            Xmax+= xDelta * percentage;
            Xmin+= xDelta * percentage;
        }

    }else{//Es paneo vertical
        
        double yDelta = Ymax-Ymin;
        if (direction==0){ //Es para arriba

            Ymax-= yDelta * percentage;
            Ymin-= yDelta * percentage;
        }else{ //Es para abajo

            Ymax+= yDelta * percentage;
            Ymin+= yDelta * percentage;
        }
    } 
     
    renderScene();
}

/*
    mode{0==NORMAL, 1 slow y 2 fast}
    direction {0 = Zoom Out, 1 = Zoom In }
*/
void zoomScene(unsigned int mode, unsigned int direction){

    


    glutDisplayFunc(renderScene);
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




//Actualiza el arreglo global dinámico que almacenará las coordenadas universales actuales. AL leerlas del archivo establece todo con el mapa completo. 
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

void drawBorders (struct Coord *pParam, int pColores) {
    int counter = 0;
    int i;
    for(i = 0; i < ptp; i++){

        glColor3f ( ((double)i*50)/255 , pColores , ((double)i+50)/255 ); 
        
        paintPolygon(vertexAmounts[i], pParam, plot, counter);
        counter += vertexAmounts[i];
    }
    //free(coords);
}




int main(int argc, char *argv[]){
    buffer = (COLOR **)malloc(res * sizeof(COLOR*));
    
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(res,res);
    glutCreateWindow("CG Proyecto 1");
    glClear(GL_COLOR_BUFFER_BIT);
    gluOrtho2D(-0.5, res +0.5, -0.5, res + 0.5);

    readFiles();
    drawBorders(coords, 1);
    glFlush();

    glutMouseFunc(mouse);
    glutKeyboardFunc(processKeyPressed);
    glutDisplayFunc(renderScene);
    glutIdleFunc(renderScene);
    glutSpecialFunc(specialKeys);

    glutMainLoop();
}
