#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "malloc.h"
#include <time.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>



static int res = 600;
static double Xmin;
static double Xmax;
static double Ymin;
static double Ymax;

static double XminTemp;
static double XmaxTemp;
static double YminTemp;
static double YmaxTemp;

static double xCenter;
static double yCenter; 



static int provinceCounter = 0;
static int totalVertexCount = 0;

static unsigned int inputEnable = 1;

struct Coord {
    double longitud;
    double latitud;
    double w;
};

static double geoT[9] = {1,0,0, 0,1,0, 0,0,1};
static int ptp = 7;
static int vertexAmounts[7]; //Punto en el que pasa de una provincia a otra.
static struct Coord *coords;
static struct Coord *coordsTemp;

static int zoomInLimit = -6;
static int zoomOutLimit = 12;
static double zoomActual = 0; //

static double panningMovement; //Acumuladores
static double actualRotationDegree; //Acumulador de rotación 

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





void disableKeyboardAndMouse(){
    inputEnable = 0;
}

void enableKeyboardAndMouse(){
    inputEnable = 1;
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
        //printf("Mouse click %s en (%d , %d ) \n", (state==GLUT_DOWN)? "Down":"Up", x, y);
    }
}



void resize(int width, int height) {
    // we ignore the params and do:
    glutReshapeWindow(res, res);
}

void copyClippingArray(){

    for (int i=0; i<totalVertexCount;i++){

        coordsTemp[i].longitud = coords[i].longitud;
        coordsTemp[i].latitud = coords[i].latitud;
        coordsTemp[i].w = coords[i].w;
    }
}

void resetValues(){
    
    for (int i=0; i<totalVertexCount;i++){ //Recorro lista de vertices

        coordsTemp[i].longitud  = coords[i].longitud; 
        coordsTemp[i].latitud  = coords[i].latitud; 
        coordsTemp[i].w  = coords[i].w; 
    }

    XminTemp = Xmin;
    XmaxTemp = Xmax;
    YminTemp = Ymin;
    YmaxTemp = Ymax;

    actualRotationDegree=0.0;
    zoomActual=0;
}

void calculateCenterPoint(){
    xCenter = (XmaxTemp-XminTemp)/2;
    yCenter = (YmaxTemp-YminTemp)/2;

    xCenter+=XminTemp;
    yCenter+=YminTemp;
}


/*
     direction {0 = up, 
     1 = down, 
     2= right, 
     3 = left}
*/
     /*
void progressiveMotionPanning(double newXmin, double newXmax, double newYmin, double newYmax, unsigned int direction){
    if (direction== 2 || direction==3){ //Es paneo horizontal
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
}
*/

void panEntireScene(unsigned int direction, double percentage){
    /*mode{0==NORMAL, 1 slow y 2 fast}
    direction {0 = up, 1 = down, 2= right, 3 = left}
    0.0 percentage < 1.0 para evitar división
    */
    if (direction== 2 || direction==3){ //Es paneo horizontal
        double xDelta=XmaxTemp-XminTemp;

        if (direction==2){ //Es para la derecha
            XmaxTemp-= xDelta * percentage;
            XminTemp-= xDelta * percentage;
        }else{ //Es para la izquierda

            XmaxTemp+= xDelta * percentage;
            XminTemp+= xDelta * percentage;
        }
    }else{//Es paneo vertical
        
        double yDelta = YmaxTemp-YminTemp;
        if (direction==0){ //Es para arriba

            YmaxTemp-= yDelta * percentage;
            YminTemp-= yDelta * percentage;
        }else{ //Es para abajo

            YmaxTemp+= yDelta * percentage;
            YminTemp+= yDelta * percentage;
        }
    }

    printf("Ventana de (%lf,%lf) a (%lf, %lf) \n", Xmin, Ymin, Xmax, Ymax);
    enableKeyboardAndMouse();
}



void panning(unsigned int directionPan , int specialMode){
    /*directionPan {0 = up, 1 = down, 2= right, 3 = left}
    specialMode = tecla de modo.
    */
    if (specialMode == GLUT_ACTIVE_SHIFT){
        {}
        panEntireScene(directionPan, 0.3);

        printf("Fast panning \n");
        
    }else if (specialMode == GLUT_ACTIVE_CTRL){

        printf("Slow panning \n");
        panEntireScene(directionPan, 0.05);
    }else{ //Modo normal
        panEntireScene(directionPan, 0.125);
    }
}



void zoomScene(double zoomScale){

    //Cálculo del punto central de la ventana actual
    calculateCenterPoint();

    XminTemp = ((XminTemp-xCenter)*zoomScale)+xCenter;
    YminTemp =((YminTemp-yCenter)*zoomScale)+yCenter;
    XmaxTemp =((XmaxTemp-xCenter)*zoomScale)+xCenter;
    YmaxTemp =((YmaxTemp-yCenter)*zoomScale)+yCenter;

    printf("Centro: (%f, %f) \n", xCenter,yCenter);
    printf("Zoom con escala %f \n", zoomScale);
    printf("Ventana de (%lf,%lf) a (%lf, %lf) \n", Xmin, Ymin, Xmax, Ymax);
    

}






int validateZoom(double zoomScale){
    /*Regresa 0 si la operacion si se puede realizar.
    1 si no se puede realizar.
    Recibe la cantidad de escalas que se pretende realizar.
    Esta escala es negativa si se quiere hacer zoom in y
    positiva si se quiere hacer zoom out.
    REALIZA EL CAMBIO DEL CONTADOR AUTOMÁTICAMENTE.
    */
    double temp = zoomActual;
    printf("Zoom Temp: %lf \n", (temp+zoomScale));
    if(zoomInLimit<=(temp+zoomScale)){ //No se pasa del limite de ZoomIn?

        if(zoomOutLimit>=(temp+zoomScale)){ //No se pasa del limite de ZoomOut?
            zoomActual=temp+zoomScale; //Altera contador
            printf("Valido - zoomActual: %lf \n", zoomActual);
            return 0; //Sí se puede realizar
        }else{
            return 1; //Se pasa del limite de zoomOut
        }
    }else{
        return 1; //Se pasa del limite de zoomIn
    }
}


void zooming(int typeZoom, int specialMode){
    //Zoom out = typeZoom 0
    //Zoom in = typeZoom 1
    double z;

    if (specialMode == GLUT_ACTIVE_SHIFT){

        printf("Fast zooming \n");
        z  = 3;
        if (typeZoom==0){  //Zoom out
            printf("Zoom out \n");
            if(validateZoom(z)==1){ //Fuera de limites
            /*Si es rápido y zoomIn, se le restan 3 al contador de zoom.
            Lo mismo si es rápido y zoomOut, sólo que se le sumaría.
            */
            return;
            }else{
                zoomScene(z);
            }
            
        }else { //Zoom In
            printf("Zoom in \n");
            z=1/z;
            if(validateZoom(-z)==1){ //Fuera de limites
                return;
            }else{
                zoomScene(z);
            }    
        }
    }else if (specialMode == GLUT_ACTIVE_CTRL){

        z=1.5;
        printf("Slow zooming \n");
        if (typeZoom==0){  //Zoom out
            printf("Zoom out \n");
            if(validateZoom(z)==1){ //Fuera de limites
            /*Si es lento y zoomIn, se le restan 1.5 al contador de zoom.
            Lo mismo si es lento y zoomOut, sólo que se le sumaría.
            */
            return;
            }else{
                zoomScene(z);
            }
        }else { //Zoom In
            printf("Zoom in \n");
            if(validateZoom(-z)==1){ //Fuera de limites
                return;
            }else{
                z=1/z;
                zoomScene(z);
            }
        }
    }else{ //Modo normal

        z=2;
        if (typeZoom==0){  //Zoom out
            printf("Zoom out \n");
            if(validateZoom(z)==1){ //Fuera de limites
            /*Si es normal y zoomIn, se le restan 2 al contador de zoom.
            Lo mismo si es normal y zoomOut, sólo que se le sumaría.
            */
                return;
            }else{
                zoomScene(z);
            }
        }else { //Zoom In
            printf("Zoom in \n");
            if(validateZoom(-z)==1){ //Fuera de limites
                return;
            }else{
                z=1/z;
                zoomScene(z);  
            }    
        }
    }
}


//NO USADA POR EL MOMENTO.
void validateRotationAngle(){

    if(actualRotationDegree>360.0){
        actualRotationDegree=actualRotationDegree-360.0;
    }
    if(actualRotationDegree<0.0){
        actualRotationDegree=360.0-actualRotationDegree;
    }
}


void rotateUniverse(double degrees){

    //Se le suma al acumulador
    actualRotationDegree = actualRotationDegree + degrees;
    //validateRotationAngle();
    calculateCenterPoint();


    //Se vuelve a radianes al meterlo a las funciones
    double sinAngle=sin(actualRotationDegree/180.0);
    double cosAngle=cos(actualRotationDegree/180.0);

    printf("Grados acumulados: %lf \n", degrees);
    printf("Centro en (%lf, %lf) \n", xCenter, yCenter);
    printf("Seno de %lf: %lf \n", actualRotationDegree, sinAngle);
    printf("Coseno de %lf: %lf \n", actualRotationDegree, cosAngle);
    printf("Valor de rotación actual %lf \n", actualRotationDegree);
    double matrixRotationFila0[3]={cosAngle, 
        -sinAngle,
        xCenter-xCenter*cosAngle+yCenter*sinAngle};
    double matrixRotationFila1[3]={sinAngle, 
        cosAngle, 
        yCenter-xCenter*sinAngle-yCenter*cosAngle};
    double matrixRotationFila2[3]={0.0,0.0,1.0};


    for (int i=0; i<totalVertexCount;i++){ //Recorro lista de vertices

        double x=coords[i].longitud;
        double y=coords[i].latitud;
        double w=coords[i].w;

        //printf("Punto a rotar (%lf, %lf) \n", coords[i].longitud,coords[i].latitud);

        coordsTemp[i].longitud = matrixRotationFila0[0]*x+matrixRotationFila0[1]*y+matrixRotationFila0[2]*w;
        coordsTemp[i].latitud = matrixRotationFila1[0]*x+matrixRotationFila1[1]*y+matrixRotationFila1[2]*w;
        coordsTemp[i].w=matrixRotationFila2[0]*x+matrixRotationFila2[1]*y+matrixRotationFila2[2]*w;

        //printf("Punto rotado (%lf, %lf) \n", coords[i].longitud,coords[i].latitud);

    }
    //copyClippingArray();
}




//direction = 0 izquierda, 1 derecha
void rotating(int direction, int specialMode){

    double degrees;

    if (specialMode == GLUT_ACTIVE_SHIFT){ //RApida
        degrees  = 30.0;
        if (direction==0){    //Rotación izquierda    
            
            rotateUniverse(degrees); //Rota 30 grados
        }else {   //Rotacion derecha
            degrees= -1.0*degrees;
            rotateUniverse(degrees);
        }
        printf("Rotacion rapida \n");
        
    }else if (specialMode == GLUT_ACTIVE_CTRL){
        degrees  = 5.0;
        printf("Rotacion lenta \n");
        if (direction==0){    //Rotación izquierda    
            rotateUniverse(degrees);
        }else {   //Rotacion derecha
            degrees= -1.0*degrees;
            rotateUniverse(degrees);
        }  
    }else { //Modo normal
        degrees  = 10.0;
        if (direction==0){    //Rotación izquierda    
            rotateUniverse(degrees);
        }else {     //Rotacion derecha
            degrees= -1.0*degrees;
            rotateUniverse(degrees); 
        }
        
        printf("Rotacion normal\n");
    }
}


/*
    mode{0==NORMAL, 1 slow y 2 fast}
    direction {0 = Zoom Out, 1 = Zoom In }
*/
void processKeyPressed(unsigned char key, int x, int y){

    int specialMode = glutGetModifiers();

    if (inputEnable==0){
        printf("Input rechazado \n");
        return; //Se rechaza;
    }else{
        //disableKeyboardAndMouse();
    }

    switch (key){

        case 73: //Se presiona a Zoom In - i mayúscula
        printf("I presionada \n");
            zooming(1,specialMode);
            break;

        case 105:  //Se presiona d Zoom In - i minúscula
        printf("i presionada \n");
            zooming(1,specialMode);
            break;
        case 79: 
            printf("O presionada \n");
            zooming(0,specialMode); //SE presiona Zoom out - o mayúscula
            break;

        case 111:
        printf("o presionada \n");
            zooming(0,specialMode); //SE presiona Zoom out - o minúscula
            break;

        case 82: //R mayúscula- Rotate right
            printf("R presionada \n");
            rotating(1,specialMode);
            break;

        case 114: //r minúscula- Rotate right
            printf("r presionada \n");
            rotating(1,specialMode);
            break;

        case 76: //L mayúscula - Rotate left
            printf("L presionada \n");
            rotating(0,specialMode);
            break;

        case 108: //l minúscula - Rotate left
            printf("l presionada \n");
            rotating(0,specialMode);
            break; 
        case 27:
            printf("Ventana reseteada \n");
            resetValues();
            break;
    }
    //enableKeyboardAndMouse();        
}

void specialKeys(int key, int x, int y){

    int specialMode = glutGetModifiers();
    int directionPan;

    if (inputEnable==0){
        printf("Input rechazado \n");
        return; //Se rechaza;
    }else{
        //disableKeyboardAndMouse(); //lo desactiva al entrar en otra operacion;
    }


    switch (key){

        case GLUT_KEY_UP:
            directionPan = 1;
            printf("Panning up \n");
            panning  (directionPan, specialMode);
            break;

        case GLUT_KEY_DOWN:
            directionPan = 0;
            printf("Panning down \n");
            panning  (directionPan, specialMode);
            break;

        case GLUT_KEY_RIGHT:
            directionPan = 3;
            printf("Panning right \n");
            panning  (directionPan, specialMode);
            break;

        case GLUT_KEY_LEFT:
            directionPan = 2;
            printf("Panning left \n");
            panning (directionPan, specialMode);
            break;


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
                //Trivially rejected. Se debe igualar a bordes rechazados.
                printf("Linea clipeada totalmente. Igualada a bordes\n");

                switch(i){

                    case 0: //Borde izquierdo
                        *x0=edgeLeft;
                        *x1=edgeLeft;
                    case 1: //Borde derecho
                        *x0=edgeRight;
                        *x1=edgeRight;
                    case 2:  //Borde inferior
                        *y0=edgeBottom;
                        *y1=edgeBottom;
                    case 3: //Borde superior
                        *y0=edgeTop;
                        *y1=edgeTop;
                }

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

    XminTemp = Xmin;
    XmaxTemp = Xmax;
    YminTemp = Ymin;
    YmaxTemp = Ymax;
}

void drawHorLine (int x0, int x1, int y){ //Garantizado ser más rápido que Bresenham.
    int i;
    for (i = x0; i < x1; i++){
        plot(i,y);
    }
}

void scanlineFill (int vertexAmount, struct Coord *pCoords, void (*f)(int,int), int counter) {

	int i,j,active,dy,dx,temp;
	int scanline = res;

	int xi[vertexAmount+1];
	int Yf[vertexAmount+1];
    int Xf[vertexAmount+1];
    double slope[vertexAmount+1];
    
   
    //Mapeo a puntos del framebuffer. 
    for(i = 0; i < vertexAmount; i++){
        Yf[i] = (int) (res * ((pCoords[counter + i].latitud - YminTemp) / (YmaxTemp - YminTemp)));
        Xf[i] = (int) (res * ((pCoords[counter + i].longitud - XminTemp) / (XmaxTemp - XminTemp)));
    }

    Xf[vertexAmount] = Xf[0];
	Yf[vertexAmount] = Yf[0];

	//Dibujar los bordes para que no haya imperfecciones. 
	for(i=0;i<vertexAmount;i++){
		bresenham (Xf[i],Yf[i],Xf[i+1],Yf[i+1],plot);
	}

	for(i=0; i<vertexAmount; i++) {
		dy = Yf[i+1] - Yf[i];
		dx = Xf[i+1] - Xf[i];

		if(dy==0) slope[i] = 1.0;
		if(dx==0) slope[i] = 0.0;

		if( (dy!=0) && (dx!=0) ) {
			slope[i]= (double)dx/dy; 
		}
	}

	while (scanline > 0){ //0 ó el menor de las Y. 
		active=0;
		
		//Cálculo de las intersecciones activas ymin < scanline < ymax
		for(i=0;i<vertexAmount;i++){
			if( (( Yf[i]<= scanline ) && (Yf[i+1] > scanline ))  ||
				(( Yf[i] > scanline ) && (Yf[i+1]<= scanline )) )  {
				
				xi[active]=(int)(Xf[i]+slope[i]*(scanline-Yf[i])); //Intersección.
				active++;
			}
		}

		//Ordena las intersecciones por x ascendiente.
		for(j=0;j<active-1;j++){ 
			for(i=0;i<active-1;i++){
				if(xi[i]>xi[i+1]){
					temp=xi[i];
					xi[i]=xi[i+1];
					xi[i+1]=temp;
				}
			}
		}

		//Dibuja las líneas rectas entre las intersecciones. 
		for(i=0;i<active;i+=2){
			drawHorLine(xi[i],xi[i+1]+1,scanline);
		}
		scanline--;
	}			
}

void delineate(int vertexAmount, struct Coord *pCoords, void (*f)(int,int), int counter){//antes paintPolygon
    int i;
    int Yf[vertexAmount+1];
    int Xf[vertexAmount+1];

    for(i = 0; i < vertexAmount; i++){
        Yf[i] = (int) (res * ((pCoords[counter + i].latitud - YminTemp) / (YmaxTemp - YminTemp))); 
        Xf[i] = (int) (res * ((pCoords[counter + i].longitud - XminTemp) / (XmaxTemp - XminTemp))); 
    }

    Yf[vertexAmount] = Yf[0];
    Xf[vertexAmount] = Xf[0];
    
    for(i = 0; i <= vertexAmount - 1; i++){
        bresenham(Xf[i], Yf[i], Xf[i+1], Yf[i+1],(*f));
    }
}


void readFiles(){

    //Actualiza el arreglo global dinámico que almacenará las coordenadas universales actuales. 
    //AL leerlas del archivo establece todo con el mapa completo. 
    char *provinces[7] = {"mapa/Puntarenas.txt",
                          "mapa/Alajuela.txt",
                          "mapa/Limon.txt", 
                          "mapa/SanJose.txt",
                          "mapa/Heredia.txt",
                          "mapa/Guanacaste.txt",
                          "mapa/Cartago.txt" };
    
    char comma;
    int i, j, k, c, vertexAmount;

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
    coordsTemp = malloc(sizeof(struct Coord)*totalVertexCount);

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

void allBorders (struct Coord *pParam, int pColores) {//antes drawBorders
    int counter = 0;
    int i;
    for(i = 0; i < ptp; i++){

        glColor3f ( 1, pColores , 1 ); 
        
        delineate(vertexAmounts[i], pParam, plot, counter); 
        counter += vertexAmounts[i]; 
    }
}

void allScanlines (struct Coord *pParam, int pColores) {
    int counter = 0;
    int i;

    for(i = 0; i < ptp; i++){

        glColor3f ( ((double)i*50)/255 , pColores , ((double)i+50)/255 ); 
        
        scanlineFill(vertexAmounts[i], pParam, plot, counter);
        counter += vertexAmounts[i];
    }
}






void clipPolygons(){

    copyClippingArray();

    int j=0;
    double x0=0;
    double x1=0;
    double y0=0;
    double y1=0;

    for (int i=0; i<totalVertexCount;i++){ //Recorro lista de vertices

        if (i==vertexAmounts[j]){
            j++;
            continue; //SIguiente iteracion;
        }
        if (i!=totalVertexCount-1){



            //printf("Linea a cortar de (%lf,%lf) a (%lf, %lf) \n", coordsTemp[i].longitud, coordsTemp[i].latitud, coordsTemp[i+1].longitud, coordsTemp[i+1].latitud);
            //printf("Ventana de (%lf,%lf) a (%lf, %lf) \n", Xmin, Ymin, Xmax, Ymax);
            x0 = coordsTemp[i].longitud; 
            y0 = coordsTemp[i].latitud;
            x1 = coordsTemp[i+1].longitud; 
            y1 = coordsTemp[i+1].latitud;

            //liangBarsky(Xmin, Xmax, Ymin, Ymax, x0, y0, x1, y1, &x0, &y0, &x1, &y1 );
            cohenSutherland(Xmin, Xmax, Ymin, Ymax, &x0, &y0, &x1, &y1); 
            
            coordsTemp[i].longitud = x0;
            coordsTemp[i].latitud = y0;
            coordsTemp[i+1].longitud =x1; 
            coordsTemp[i+1].latitud = y1;
            //cohenSutherland(Xmin, Xmax, Ymin, Ymax, &coordsTemp[i].longitud, &coordsTemp[i].latitud, &coordsTemp[i+1].longitud, &coordsTemp[i+1].latitud); 
            //printf("Linea cortada de (%lf,%lf) a (%lf, %lf) \n", coordsTemp[i].longitud, coordsTemp[i].latitud, coordsTemp[i+1].longitud, coordsTemp[i+1].latitud);
     
        }
    }
}
void renderScreen(){

    //clipPolygons();
    
    allScanlines(coordsTemp, 1);
    allBorders(coordsTemp, 1);
    //glFlush();
    glutSwapBuffers();
}


void renderScene(void){

    glClearColor(0.0f, 0.0f, 0.0f ,1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //printf("Xmin: %lf \t Ymin: %lf \n Xmax: %lf \t Ymax: %lf", Xmin, Ymin, Xmax, Ymax);
    renderScreen();
    
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
    clipPolygons();
    allScanlines(coordsTemp, 1);
    allBorders(coordsTemp, 1);

    glFlush();

    glutMouseFunc(mouse);
    glutKeyboardFunc(processKeyPressed);
    glutDisplayFunc(renderScene);
    //glutIdleFunc(renderScene); //Llama a renderScene cuando el glut no tiene comandos en buffer;
    glutSpecialFunc(specialKeys);
    glutReshapeFunc(resize);

    glutMainLoop();
}
