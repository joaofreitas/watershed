/**
 * \file runtime/host.cc
 * \author Rodrigo Silva Oliveira
 * \author Thatyene Louise Alves de Souza Ramos
 */

/* Project's .h */
#include "runtime/host.h"

Host::Host(void) {

}

Host::Host(string name) {
	name_ = name;
	database_server_ = false;
	runtime_daemon_id_ = database_daemon_id_ = -1;
}

Host::~Host(void) {

}

void Host::AddResource(string resource_name){
	resources_.push_back(resource_name);
}

int Host::GetDatabaseDaemonId(void) {
	return database_daemon_id_;
}

string Host::GetName(void) {
	return name_;
}

vector<string>* Host::GetResources(void) {
	return &resources_;
}

int Host::GetRuntimeDaemonId(void) {
	return runtime_daemon_id_;
}

bool Host::HasResource(string resource) {
	for (vector<string>::iterator it = resources_.begin(); it!=resources_.end();++it) {
		if (it->compare(resource) == 0) {
			return true;
		}
	}
	return false;
}

bool Host::IsDatabaseServer(void) {
	return database_server_;
}

void Host::SetDatabaseDaemonId(int id) {
	database_daemon_id_ = id;
}

void Host::SetDatabaseServer(bool value) {
	database_server_ = value;
}

void Host::SetRuntimeDaemonId(int id) {
	runtime_daemon_id_ = id;
}
