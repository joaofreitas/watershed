/**
 * \file library/main.cc
 * \author Rodrigo Silva Oliveira
 * \author Thatyene Louise Alves de Souza Ramos
 */

/* Project's .h */
#include "library/processing_module.h"

int argc_;
char** argv_;

int main(int argc, char** argv) {

	argc_ = argc;
	argv_ = argv;

	MpiCommunicator* runtime_communicator; /* Communicator with the runtime. */
	ProcessingModule* processing_module_instance = NULL;
	ProcessingModuleConfigurator* processing_module_configurator;
	void* processing_module_lib;

	runtime_communicator = new MpiCommunicator(argc, argv);

	Message message_from_runtime;
	message_from_runtime.SetOperationCode(Constants::MESSAGE_OP_INIT_PROCESSING_MODULE);
	int source = runtime_communicator->Poll(Constants::COMM_ANY_SOURCE, Constants::MESSAGE_OP_INIT_PROCESSING_MODULE);
	runtime_communicator->Receive(source, &message_from_runtime);
	string message_data = (char*) message_from_runtime.GetData();
	string configurator_file_name = Util::TokenizeString("\t", message_data)[0];
	string database_port_name = Util::TokenizeString("\t", message_data)[1];
	int database_peer = atoi((Util::TokenizeString("\t", message_data)[2]).c_str());
	try {
		processing_module_configurator = new ProcessingModuleConfigurator(configurator_file_name);
		processing_module_configurator->SetDatabasePortName(database_port_name);
		processing_module_configurator->SetDatabasePeerIdentification(database_peer);
		processing_module_configurator->SetConfiguratorFileName(configurator_file_name);
	} catch (XMLParserException& e) {
		throw(e);
	}

	/* Loads the processing module library */
	processing_module_lib = dlopen(processing_module_configurator->GetLibraryFile().c_str(), RTLD_NOW);
	if (!processing_module_lib) {
		string error = dlerror();
		string message = "cannot load processing module library at host " + runtime_communicator->GetHostName() + ". " + error;
		Util::Error(runtime_communicator, message);
		delete (runtime_communicator);
		return 0;
	}

	/* Loads the symbols from the processing module library*/
	create_t* create_processing_module = (create_t*) dlsym(processing_module_lib, "GetInstance");
	if (!create_processing_module) {
		string error = dlerror();
		string message = "cannot load symbols from processing module at host " + runtime_communicator->GetHostName() + ". " + error;
		Util::Error(runtime_communicator, message);
		delete (runtime_communicator);
		dlclose(processing_module_lib);
		return 0;
	}

	/* changes the running directory to the module's location. */
	chdir(processing_module_configurator->GetRunningDirectory().c_str());

	/* Create an instance of the processing module class */
	processing_module_instance = create_processing_module();
	processing_module_instance->SetRuntimeCommunicator(runtime_communicator);
	processing_module_instance->SetConfigurator(processing_module_configurator);

	if (!processing_module_instance->ErrorOnInit()) {
		processing_module_instance->Run();
	} else {
		Util::Error(runtime_communicator, processing_module_instance->GetErrorMessageOnInit() + " in " + processing_module_configurator->GetName());
	}

	delete (processing_module_instance);
	dlclose(processing_module_lib);
	return 0;
}
