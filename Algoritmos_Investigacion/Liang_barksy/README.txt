Tecnológico de Costa Rica
 IC8019 - Gráficos Por Computadora, PROYECTO 0: Algoritmos de líneas


 Profesor: Dr. Francisco Torres Rojas
 Estudiantes:
	Carlos Girón Alas
	Julián J. Méndez Oconitrillo
	Daniel Troyo Garro

Archivos dados:
	pro0.c
	pro0.h
	pro0.o
	pro0
	makefile
	README.txt


Librerías utilizadas: GLUT, GL y GLU.


Para ejecutar el programa, debe instalar ciertos paquetes con anterioridad. 
Ejecute los siguientes comandos para instalarlos.


	sudo apt-get install build-essential
	sudo apt-get install  freeglut3-dev

Tras realizar esto extraiga el proyecto y diríjase a la ruta de extracción:

	cd path/to/extracted/folder/pro0/ 

Ejecute el makefile.

	make

Ejecute el programa.

	./pro0 <res> <# lineas> <veces>

<res> : Entero que indica la resolución de la pantalla cuadrada sobre la cual se dibujarán las lineas.
<# lineas> : Entero que indica el número de líneas a ser dibujadas en la pantalla por cada algoritmo.
<# veces> : Entero que indica el número de veces a dibujar una línea por cada algoritmo.

El programa requiere los tres parámetros, de lo contrario mostrará error. 