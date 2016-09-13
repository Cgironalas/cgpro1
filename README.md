Tecnológico de Costa Rica
IC8019 - Introducción A Los Gráficos Por Computadora

Profesor: 
	Dr. Francisco Torres

Estudiantes:
	Carlos Girón Alas
	Julián J. Méndez O.
	Daniel Troyo Garro

Programa que despliega el mapa de Costa Rica generado por archivos de texto con puntos en coordenadas universales de latitud y longitud.

Los archivos con las coordenadas se encuentran en la carpeta map/
Quitar alguno de estos archivos impedirá que el programa corra.

El programa tiene 3 modos: 
	-Bordes: El modo por defecto, se activa con l)
	-Relleno: Se rellenan las provincias con colores, se activa ccon f)
	-Texturas Se reemplaza el relleno con texeles de imágenes cargados con las imágenes en la carpeta textures/. Se activa con t y en caso de que alguna de las imágenes no se encuentre, se reemplaza con relleno estático. 

Para correr el archivo, se abre la carpeta cgpro1 en terminal y se corre ./pro1. En caso de que los archivos .o y ejecutable no estén disponibles puede correrse make en la terminal para generarlos. 

Los comandos disponibles son:
	a - 
		Rotación en sentido del reloj
	d - 
		Rotación en sentido contrario al reloj
	← - 
		Paneo hacia la izquierda (el efecto mueve el mapa a la derecha)
	→ - 
		Paneo hacia la derecha (el efecto mueve el mapa a la izquierda)
	↑ - 
		Paneo hacia arriba (el efecto mueve el mapa hacia abajo)
	↓ - 
		Paneo hacia abajo (el efecto mueve el mapa hacia arriba)
	mouse scroll down - 
		Zoom out
	mouse scroll up - 
		Zoom in 
	shift + (a, d, ←, →, ↑, ↓, mouse scroll down, mouse scroll up) - 
		Versión acelerada de cualquiera de los efectos previamente descritos
	alt + (a, d, ←, →, ↑, ↓) - 
		Versión ralentizada de los efectos rotación y paneo correspondientes. 
	ctrl + (mouse scroll down, mouse scroll up) - 
		Versión ralentizada de los zoom in y zoom out.
	r - 
		"Rave" hace que el fondo cambie constantemente de color. Discreción en caso de sensibilidad a este efecto. 
	spacebar - 
		Resetea el mapa a la posición orignal en que se cargó
	m- 
		Activa/desactiva el movimiento fluido 
	esc - 
		Cierra el programa.

