/**
 * \file stream/main.cc
 * \author Rodrigo Silva Oliveira
 * \author Thatyene Louise Alves de Souza Ramos
 */

/* Project's .h */
#include "stream/stream_manager.h"

using namespace std;

int main ( int argc, char** argv ) {

	StreamManager* manager = new StreamManager ( argc, argv );
	manager->ExchangeInitialInformation ();
	manager->Run ();
	delete ( manager );
	return 0;
}
