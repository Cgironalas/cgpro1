#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "malloc.h"
#include <time.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>




double genRandLimNumber(double lim){
    static double tiempo;
    tiempo+=time(NULL);
    struct timespec tim, tim2;
    tim.tv_sec = 0;
    tim.tv_nsec = 50000;
    nanosleep(&tim , &tim2);
    srand(tiempo);
    double a = rand();
    
    int result = (int)( a / lim );
    double mod = a - (double)( result ) * lim;
    // printf("Random : %f \n", mod);
    return mod;
}


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


void liangBarsky (double edgeLeft, double edgeRight, double edgeBottom, double edgeTop,   // Define the x/y clipping values for the border.
                  double x0, double y0, double x1, double y1,                 // Define the start and end points of the line.
                  double *x0clip, double *y0clip, double *x1clip, double *y1clip)         // The output values, so declare these outside.
{
    
    double t0 = 0.0;    double t1 = 1.0;
    double xdelta = x1-x0;
    double ydelta = y1-y0;
    double p,q,r;


    for(int i=0; i<4; i++) {   // Traverse through left, right, bottom, top edges.
        if (i==0) { //Left edge.  
            p = -xdelta;    
            q = edgeLeft-x0;
            q=q*-1;  

        }
        if (i==1) {   //Right edge.
            p = xdelta;     
            q =  (edgeRight-x0); 
        }
        if (i==2) {  //Bottom edge.
            p = -1*ydelta;    
            q = (edgeBottom-y0);
            q=q*-1;
        }
        if (i==3) { //TOp edge
         p = ydelta;     
         q =  (edgeTop-y0);   
        }   
        r = q/p;
        
        if(p==0 && q<0){
            return;    //Trivially rejected, parallel to one edge.
        }
        if(p<0) {
            if(r>t1){
                return;  //Trivially rejected.
            }          
            else if(r>t0){
                t0=r;   
            }           // CLipping has been made.
        } else if(p>0) {
            if(r<t0) {

                return;
            }
                  
            else if(r<t1){
                t1=r;    
            }          // CLipping has been made.
        }
    }

    *x0clip = x0 + t0*xdelta;
    *y0clip = y0 + t0*ydelta;
    *x1clip = x0 + t1*xdelta;
    *y1clip = y0 + t1*ydelta;

    return;       
}







int main(int argc, char *argv[]) {



    double xmin=900.00, ymin = 900.00;
    double xmax=5000.00, ymax = 5000.00;
    //ESquinas de la ventana en (900,900) y (2000, 2000).
    double res = 6000.00; // Universo  cuadrado

    int numLineas = 50; 

    clock_t start[2], end[2];
    double arregloCorX0[numLineas], arregloCorY0[numLineas], arregloCorX1[numLineas], arregloCorY1[numLineas];

    double arreglo1CorX0[numLineas], arreglo1CorY0[numLineas], arreglo1CorX1[numLineas], arreglo1CorY1[numLineas];
    int i,veces;

    for (i=0; i<numLineas; i++){ //Generacion de lineas 
        arregloCorX0[i] = genRandLimNumber(5500.00);
        arregloCorY0[i] = genRandLimNumber(5500.00);
        arregloCorX1[i] = genRandLimNumber(5500.00);
        arregloCorY1[i] = genRandLimNumber(5500.00);

        arreglo1CorX0[i] = arregloCorX0[i];
        arreglo1CorY0[i] = arregloCorY0[i];
        arreglo1CorX1[i] = arregloCorX1[i];
        arreglo1CorY1[i] = arregloCorY1[i];
    }


    printf("happening \n");

    start[0]=clock();
    for (i=0; i<numLineas; i++){
        double x0, x1, y0, y1;
        liangBarsky(xmin,xmax, ymin, ymax, arregloCorX0[i],arregloCorY0[i], arregloCorX1[i], arregloCorY1[i], &x0, &y0, &x1, &y1);
        arregloCorX0[i]=x0;
        arregloCorY0[i]=y0;
        arregloCorX1[i]= x1;
        arregloCorY1[i]= y1;
    }

    end[0]=clock();




    //Algoritmo 2
    start[1]=clock();
    for (i=0; i<numLineas; i++){
        cohenSutherland(xmin,xmax, ymin, ymax, &arreglo1CorX0[i],&arreglo1CorY0[i], &arreglo1CorX1[i], &arreglo1CorY1[i]);

        
    }
    end[1]=clock();
    




    /*
    buffer = (COLOR **)malloc(res * sizeof(COLOR*));
    
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(res,res);
    glutCreateWindow("CG Proyecto 0");
    glClear(GL_COLOR_BUFFER_BIT);
    gluOrtho2D(-0.5, res +0.5, -0.5, res + 0.5);
    */

    printf("Tecnológico de Costa Rica\n IC8019 - Gráficos Por Computadora, PROYECTO 0: Algoritmos de líneas\n");
    printf("\n\n Profesor: Dr. Francisco Torres Rojas\n");
    printf(" Estudiantes:\n\tCarlos Girón Alas\n\tJulián J. Méndez Oconitrillo\n\tDaniel Troyo Garro\n");
    printf("17 agosto 2016\n\n");
     
    printf("Duraciones: \n \t\tAlgoritmo Liang-Barsky \tDuración con %i lineas\n", numLineas);
       
    printf("\t %lf segundos \n",((double) end[0]-start[0])/CLOCKS_PER_SEC);

    printf("Duraciones: \n \t\tAlgoritmo Cohen-Sutherland \tDuración con %i lineas\n", numLineas);
       
    printf("\t %lf segundos \n",((double) end[1]-start[1])/CLOCKS_PER_SEC);
    
    //glutMainLoop();

    return 0;
}
