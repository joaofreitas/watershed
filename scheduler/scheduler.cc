/**
 * \file scheduler/scheduler.cc
 * \author Rodrigo Silva Oliveira
 * \author Thatyene Louise Alves de Souza Ramos
 */

/* Project's .h */
#include <scheduler/scheduler.h>

Scheduler::Scheduler ( void ) {
	last_assigned_database_identification_ = 0;
}

Scheduler::~Scheduler ( void ) {

}

vector<SchedulerOperation> Scheduler::Balance ( map<string, ProcessingModuleEntry*>* active_processing_modules ) {
	vector<SchedulerOperation> operations;
//	op.SetOperationDescription(Constants::SCHED_OP_MIGRATION);
	return operations;
}

map<string, int> Scheduler::ChooseHostsToAddProcessingModule ( map<string, Host>* cluster, MpiCommunicator* database_communicator, ProcessingModuleConfigurator* processing_module_configurator ) {

	int number_instances = processing_module_configurator->GetNumberInstances ( );
	vector < string > *demands = processing_module_configurator->GetDemands ( );
	map<string, int> chosen_hosts;
	bool able;

	/* Determines the eligible hosts */
	if (number_instances > 0 || number_instances == Constants::PROCESSING_MODULE_AUTOMATIC_NUMBER_INSTANCES) {
		for (map<string, Host>::iterator it = cluster->begin ( ); it != cluster->end ( ); ++it) {
			able = true;
			for (vector<string>::iterator it_d = demands->begin ( ); it_d != demands->end ( ); ++it_d) {
				if (!it->second.HasResource ( *it_d )) {
					able = false;
				}
			}
			if (able) {
				chosen_hosts[it->first] = 0;
			}
		}
	}
	else {
		/** TODO Lançar excecao neste ponto. */
	}

	if (number_instances != Constants::PROCESSING_MODULE_AUTOMATIC_NUMBER_INSTANCES) {
		/* Distributes the instances among the hosts in a round robin fashion */
		if (chosen_hosts.size ( ) > 0) {
			while (number_instances != 0) {
				for (map<string, int>::iterator it = chosen_hosts.begin ( ); it != chosen_hosts.end ( ); ++it) {
					++(it->second);
					--number_instances;
					if (number_instances == 0) {
						break;
					}
				}
			}

			for (map<string, int>::iterator it = chosen_hosts.begin ( ); it != chosen_hosts.end ( ); ++it) {
				if (it->second == 0) {
					chosen_hosts.erase ( it, chosen_hosts.end ( ) );
					break;
				}
			}
		}
	}
	else {
		for (map<string, int>::iterator it = chosen_hosts.begin ( ); it != chosen_hosts.end ( ); ++it) {
			it->second = 1;
		}
	}

	processing_module_configurator->SetDatabasePeerIdentification ( last_assigned_database_identification_ );
	last_assigned_database_identification_ = (++last_assigned_database_identification_) % database_communicator->GetNumberProcesses ( );
	return chosen_hosts;
}

int Scheduler::GetLastAssignedDatabase ( void ) {
	return last_assigned_database_identification_;
}
