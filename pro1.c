#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Coordenada {
    double longitud;
    double latitud;
    double w;
};

int main(){
    FILE* file = fopen("test.txt", "r");
    int counter = 0;
    int c;
    while ((c = getc(file)) != EOF){
        if(c == '\n'){
            counter++;
        }
    }
    counter++;
    fclose(file);
    printf("%d", counter);
    printf("\n");



    struct Coordenada coordAlajuela[counter];       //1
    struct Coordenada coordCartago[counter];        //2
    struct Coordenada coordGuanacaste[counter];     //3
    struct Coordenada coordHeredia[counter];        //4
    struct Coordenada coordLimon[counter];          //5
    struct Coordenada coordPuntarenas[counter];     //6
    struct Coordenada coordSanJose[counter];        //7



    FILE* g = fopen("test.txt", "r");
    char comma;
    double lon, lat;
    int i;
    int j;

    for(i = 0; i < counter; i++){
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

    for(i = 0; i < counter; i++){
        printf("%d", i);
        printf("\n");
        printf("%.14lf ", coordAlajuela[i].longitud);
        printf("%.14lf ", coordAlajuela[i].latitud);
        printf("\n");
    }
}
