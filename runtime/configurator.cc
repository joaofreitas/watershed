/**
 * \file runtime/configurator.cc
 * \author Rodrigo Silva Oliveira
 * \author Thatyene Louise Alves de Souza Ramos
 */

/* Project's .h */
#include "runtime/configurator.h"

RuntimeConfigurator::RuntimeConfigurator ( string parse_file ) throw ( XMLParserException ) {
	try {
		configuration_parser_.Parse ( parse_file );
	}
	catch ( XMLParserException& e ) {
		throw e;
	}
}

RuntimeConfigurator::~RuntimeConfigurator ( void ) {

}

void RuntimeConfigurator::FillConfiguratorItems ( void ) throw ( XMLParserException ) {

	try {
		/* OMPI attributes */
		configuration_parser_.DefineCurrentElementByName ( 0, "ompi" );
		ompi_prefix_.assign ( configuration_parser_.GetAttributeByName ( "prefix" ) );

		/* Server attributes */
		configuration_parser_.DefineCurrentElementByName ( 0, "server" );
		server_name_.assign ( configuration_parser_.GetAttributeByName ( "name" ) );
		db_executable_name_.assign ( configuration_parser_.GetAttributeByName ( "db_executable_name" ) );
		server_home_.assign ( configuration_parser_.GetAttributeByName ( "home" ) );
		running_dir_.assign ( configuration_parser_.GetAttributeByName ( "running_dir" ) );
		log_file_ = running_dir_ + "/" + Constants::FILE_LOG;
		lock_file_ = running_dir_ + "/" + Constants::FILE_LOCK;
		info_file_ = running_dir_ + "/" + Constants::FILE_INFO;
		exe_dir_ = server_home_ + "/" + Constants::DIR_BIN;

		/* Database attributes */
		configuration_parser_.DefineCurrentElementByName ( 0, "database" );
		db_executable_name_.assign ( configuration_parser_.GetAttributeByName ( "exe_name" ) );
		database_arguments_.assign ( configuration_parser_.GetAttributeByName ( "arguments" ) );

		/* ProcessingModule attributes */
		configuration_parser_.DefineCurrentElementByName ( 0, "processing_module" );
		processing_module_executable_name_.assign ( configuration_parser_.GetAttributeByName ( "exe_name" ) );

		/* Hosts attributes */
		int num_hosts = 0;
		int h = 0;
		num_hosts = configuration_parser_.DefineCurrentElementByName ( h, "host" );
		while ( h < num_hosts ) {
			configuration_parser_.DefineCurrentElementByName ( h, "host" );
			Host host ( configuration_parser_.GetAttributeByName ( "name" ) );
			string is_database = configuration_parser_.GetAttributeByName ( "database_server" );
			if ( is_database.compare ( "true" ) == 0 ) {
				host.SetDatabaseServer ( true );
			}
			else {
				host.SetDatabaseServer ( false );
			}
			int num_resources = 0;
			int r = 0;
			num_resources = configuration_parser_.GetChildByName ( r, "resource" );
			/* Resources attributes */
			while ( r < num_resources ) {
				configuration_parser_.GetChildByName ( r, "resource" );
				host.AddResource ( configuration_parser_.GetAttributeByName ( "name" ) );
				r++;
			}
			hosts_[host.GetName ()] = host;
			configuration_parser_.CleanChildren ();
			h++;
		}
	}
	catch ( exception& e ) {
		throw e;
	}
}

string RuntimeConfigurator::GetDBArguments ( void ) {
	return database_arguments_;
}

string RuntimeConfigurator::GetDBExeName ( void ) {
	return db_executable_name_;
}

string RuntimeConfigurator::GetExeDir ( void ) {
	return exe_dir_;
}

map < string, Host >* RuntimeConfigurator::GetHosts ( void ) {
	return &hosts_;
}

string RuntimeConfigurator::GetInfoFile ( void ) {
	return info_file_;
}

string RuntimeConfigurator::GetLockFile ( void ) {
	return lock_file_;
}

string RuntimeConfigurator::GetLogFile ( void ) {
	return log_file_;
}

string RuntimeConfigurator::GetOMPIPrefix ( void ) {
	return ompi_prefix_;
}

string RuntimeConfigurator::GetProcessingModuleExeName ( void ) {
	return processing_module_executable_name_;
}

string RuntimeConfigurator::GetRunningDir ( void ) {
	return running_dir_;
}

string RuntimeConfigurator::GetServerHome ( void ) {
	return server_home_;
}

string RuntimeConfigurator::GetServerName ( void ) {
	return server_name_;
}

void RuntimeConfigurator::Print ( void ) {
	cout << "-------------------------------------------------------------------------------------" << endl;
	cout << "OMPI prefix      : " << GetOMPIPrefix () << endl;
	cout << "Server name      : " << GetServerName () << endl;
	cout << "Server home      : " << GetServerHome () << endl;
	cout << "Running dir      : " << GetRunningDir () << endl;
	cout << "DB exe name      : " << GetDBExeName () << endl;
	cout << "DB args          : " << GetDBArguments () << endl;
	cout << "PM exe name      : " << GetProcessingModuleExeName () << endl;

	cout << "Hosts:" << endl;
	for ( map < string, Host >::iterator it = hosts_.begin (); it != hosts_.end (); ++it ) {
		cout << "\tName: " << it->second.GetName () << endl << "\tDatabase Server: " << it->second.IsDatabaseServer () << "\tResources: ";
		for ( uint i = 0; i < it->second.GetResources()->size (); ++i ) {
			cout << it->second.GetResources()->at ( i ) << " ";
		}
		cout << endl << endl;
	}
	cout << "-------------------------------------------------------------------------------------" << endl;
}

void RuntimeConfigurator::SetInfoFile ( string info_file ) {
	info_file_ = info_file;
}

void RuntimeConfigurator::SetLockFile ( string lock_file ) {
	lock_file_ = lock_file;
}

void RuntimeConfigurator::SetLogFile ( string log_file ) {
	log_file_ = log_file;
}
