/**
 * \file library/configurator.cc
 * \author Rodrigo Silva Oliveira
 * \author Thatyene Louise Alves de Souza Ramos
 */

/* Project's .h */
#include "library/configurator.h"

ProcessingModuleConfigurator::ProcessingModuleConfigurator(void) {
	number_termination_messages_ = 0;
}

ProcessingModuleConfigurator::ProcessingModuleConfigurator(string parse_file)
		throw (XMLParserException) {
	number_termination_messages_ = 0;
	try {
		processing_module_parser_.Parse(parse_file);
		FillItems();
	} catch (XMLParserException& e) {
		throw(e);
	}
}

ProcessingModuleConfigurator::~ProcessingModuleConfigurator(void) {
	demands_.clear();
}

void ProcessingModuleConfigurator::AddDemand(string demand) {
	demands_.push_back(demand);
}

void ProcessingModuleConfigurator::AddInput(InputFlow input) {
	inputs_.push_back(input);
}

void ProcessingModuleConfigurator::FillItems(void) throw (XMLParserException) {

	/* Global attributes */
	processing_module_parser_.DefineCurrentElementByName(0, "global");
	SetName(processing_module_parser_.GetAttributeByName("name"));
	SetLibraryFile(processing_module_parser_.GetAttributeByName("library"));

	/* Running directory is the module's location. */
	vector < string > path = Util::TokenizeString("/", GetLibraryFile());
	running_directory_ = "";
	for (uint i = 0; i < path.size() - 1; ++i) {
		running_directory_ += "/" + path[i];
	}

	string n_instances = processing_module_parser_.GetAttributeByName(
			"instances");
	if (n_instances.compare("") == 0) {
		SetNumberInstances(
				Constants::PROCESSING_MODULE_AUTOMATIC_NUMBER_INSTANCES);
	} else {
		SetNumberInstances(atoi(n_instances.c_str()));
	}
	SetArguments(processing_module_parser_.GetAttributeByName("arguments"));

	/* Inputs attributes */
	if (processing_module_parser_.DefineCurrentElementByName(0, "inputs") != 0) {
		int num_inputs = 0;
		int ni = 0;
		num_inputs = processing_module_parser_.GetChildByName(ni, "input");
		while (ni < num_inputs) {
			processing_module_parser_.GetChildByName(ni, "input");
			InputFlow flow_in;
			flow_in.SetName(
					processing_module_parser_.GetAttributeByName("name"));
			flow_in.SetQuery(processing_module_parser_.GetAttributeByName(
					"query"));
			flow_in.SetPolicy(processing_module_parser_.GetAttributeByName(
					"policy"));
			flow_in.SetPolicyFunctionFile(
					processing_module_parser_.GetAttributeByName(
							"policy_function_file"));

			if (flow_in.GetPolicy().compare(Constants::POLICY_LABELED) == 0
					&& flow_in.GetPolicyFunctionFile().compare(
							Constants::EMPTY_ATTRIBUTE) == 0) {
				string
						msg =
								"processing module " + GetName()
										+ " has to specify a policy function file for the input "
										+ flow_in.GetName();
				throw XMLParserException(msg);
			}

			AddInput(flow_in);
			++ni;
		}
	}
	processing_module_parser_.CleanChildren();

	/* Output attributes */
	if (processing_module_parser_.DefineCurrentElementByName(0, "output") != 0) {
		SetFlowOut(processing_module_parser_.GetAttributeByName("name"));
		SetFlowOutStructure(processing_module_parser_.GetAttributeByName(
				"structure"));
	} else {
		SetFlowOut(Constants::EMPTY_ATTRIBUTE);
		SetFlowOutStructure(Constants::EMPTY_ATTRIBUTE);
	}

	/* Demands attributes */
	if (processing_module_parser_.DefineCurrentElementByName(0, "demands") != 0) {
		int num_demands = 0;
		int d = 0;
		num_demands = processing_module_parser_.GetChildByName(d, "demand");
		while (d < num_demands) {
			processing_module_parser_.GetChildByName(d, "demand");
			AddDemand(processing_module_parser_.GetAttributeByName("name"));
			++d;
		}
	}
	processing_module_parser_.CleanChildren();
}

string ProcessingModuleConfigurator::GetArguments(void) {
	return arguments_;
}

string ProcessingModuleConfigurator::GetConfiguratorFileName(void) {
	return configurator_file_name_;
}

int ProcessingModuleConfigurator::GetDatabasePeerIdentification(void) {
	return database_peer_identification_;
}

string ProcessingModuleConfigurator::GetDatabasePortName(void) {
	return database_port_name_;
}

vector<string>* ProcessingModuleConfigurator::GetDemands(void) {
	return &demands_;
}

vector<InputFlow>* ProcessingModuleConfigurator::GetInputs(void) {
	return &inputs_;
}

string ProcessingModuleConfigurator::GetFlowOut(void) {
	return flow_out_;
}

string ProcessingModuleConfigurator::GetFlowOutStructure(void) {
	return flow_out_structure_;
}

string ProcessingModuleConfigurator::GetLibraryFile(void) {
	return library_file_;
}

string ProcessingModuleConfigurator::GetName(void) {
	return name_;
}

int ProcessingModuleConfigurator::GetNumberInstances(void) {
	return number_instances_;
}

int ProcessingModuleConfigurator::GetNumberTerminationMessages(void) {
	return number_termination_messages_;
}

string ProcessingModuleConfigurator::GetPortName(void) {
	return port_name_;
}

void ProcessingModuleConfigurator::Print(void) {
	cout
			<< "-------------------------------------------------------------------------------------"
			<< endl;
	cout << "Name      : " << GetName() << endl;
	cout << "Library   : " << GetLibraryFile() << endl;
	cout << "Instances : " << GetNumberInstances() << endl;
	cout << "Arguments : " << GetArguments() << endl;
	cout << "Inputs    : " << endl;
	for (uint i = 0; i < inputs_.size(); ++i) {
		cout << "\tName: " << inputs_[i].GetName() << endl << "\tQuery: "
				<< inputs_[i].GetQuery() << endl << "\tPolicy: "
				<< inputs_[i].GetPolicy() << endl << "\tFunction File: "
				<< inputs_[i].GetPolicyFunctionFile() << endl << endl;
	}
	cout << "Output    : " << GetFlowOut() << endl;
	cout << "Out DTD   : " << GetFlowOutStructure() << endl;
	cout << "Demands   : " << endl;
	for (uint i = 0; i < demands_.size(); ++i) {
		cout << "\tName: " << demands_[i] << endl;
	}
	cout
			<< "-------------------------------------------------------------------------------------"
			<< endl;
}

string ProcessingModuleConfigurator::GetQueryFlowIn(void) {
	return query_flow_in_;
}

string ProcessingModuleConfigurator::GetRunningDirectory(void) {
	return running_directory_;
}

string ProcessingModuleConfigurator::GetStructureFlowOut(void) {
	return structure_flow_out_;
}

void ProcessingModuleConfigurator::SetArguments(string arguments) {
	arguments_ = arguments;
}

void ProcessingModuleConfigurator::SetConfiguratorFileName(
		string configurator_file_name) {
	configurator_file_name_ = configurator_file_name;
}

void ProcessingModuleConfigurator::SetDatabasePeerIdentification(
		int database_peer_identification) {
	database_peer_identification_ = database_peer_identification;
}

void ProcessingModuleConfigurator::SetDatabasePortName(
		string database_port_name) {
	database_port_name_ = database_port_name;
}

void ProcessingModuleConfigurator::SetFlowOut(string flow_out) {
	flow_out_ = flow_out;
}

void ProcessingModuleConfigurator::SetFlowOutStructure(
		string flow_out_structure) {
	flow_out_structure_ = flow_out_structure;
}

void ProcessingModuleConfigurator::SetLibraryFile(string library_file) {
	library_file_ = library_file;
}

void ProcessingModuleConfigurator::SetName(string name) {
	name_ = name;
}

void ProcessingModuleConfigurator::SetNumberInstances(int number_instances) {
	number_instances_ = number_instances;
}

void ProcessingModuleConfigurator::SetNumberTerminationMessages(
		int number_termination_messages) {
	number_termination_messages_ = number_termination_messages;
}

void ProcessingModuleConfigurator::SetPortName(string port_name) {
	port_name_ = port_name;
}
