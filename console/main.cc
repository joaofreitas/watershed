/**
 * \file console/main.cc
 * \author Rodrigo Silva Oliveira
 * \author Thatyene Louise Alves de Souza Ramos
 */

/* Project's .h */
#include "console/console.h"

int main ( int argc, char **argv ) {
	Console watershed_console ( argc, argv );
	watershed_console.ConnectToServer ();
	watershed_console.ParseCommand ();
	watershed_console.DisconnectFromServer();
	return 0;
}
