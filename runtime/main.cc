/**
 * \file runtime/main.cc
 * \author Rodrigo Silva Oliveira
 * \author Thatyene Louise Alves de Souza Ramos
 */

/* Project's .h */
#include "runtime/runtime.h"

int main ( int argc, char** argv ) {

	Runtime* watershed;

	try {
		watershed = new Runtime ( argc, argv );
		watershed->Run ( );
	}
	catch ( MemoryOperationException& e ) {
		cout << endl << Constants::SYSTEM_NAME << ": an exception was caught at main.cc: " << e.ToString ( );
		cout << endl << Constants::SYSTEM_NAME << ": stopping...";
	}
	catch ( ProcessSpawnningException& e ) {
		cout << endl << Constants::SYSTEM_NAME << ": an exception was caught at main.cc: " << e.ToString ( );
		cout << endl << Constants::SYSTEM_NAME << ": stopping...";
	}
	catch ( XMLParserException& e ) {
		cout << endl << Constants::SYSTEM_NAME << ": an exception was caught at main.cc: " << e.ToString ( );
		cout << endl << Constants::SYSTEM_NAME << ": stopping...";
	}
	catch ( exception e ) {
		cout << endl << Constants::SYSTEM_NAME << ": an exception was caught at main.cc: " << e.what ( );
		cout << endl << Constants::SYSTEM_NAME << ": stopping...";
	}

	delete ( watershed );
	return EXIT_SUCCESS;
}
