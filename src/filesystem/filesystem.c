#include <stdio.h>

#include "../../include/structs.h"
#include "../../include/defs.h"

void filesystem_init() {
	
	/*
	 * TODO: 
	 * 
	 * - Dentro de la carpeta cvs busca la carpeta config con el archivo
	 * 	 con el arbol guardado y lo levanta a memoria, si no esta lo 
	 *   crea.
	 * - 
	 * 
	 * */
	 
	 FILE * f;
	 
	 if (f = fopen(CVS_TREE_PATH, "r")) {
		 printf("Existe arbol\n");
	 } else {
		 printf("No existe arbol\n");
	 }
	
}
