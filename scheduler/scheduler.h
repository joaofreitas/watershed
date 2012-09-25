/**
 * \file scheduler/scheduler.h
 * \author Rodrigo Silva Oliveira
 * \author Thatyene Louise Alves de Souza Ramos
 */

#ifndef WATERSHED_SCHEDULER_SCHEDULER_H_
#define WATERSHED_SCHEDULER_SCHEDULER_H_

/* C++ libraries */
#include <map>

/* Project's .h */
#include <library/configurator.h>
#include <library/processing_module_entry.h>
#include <runtime/host.h>
#include <scheduler/scheduler_operation.h>

using namespace std;

/**
 * todo
 */
class Scheduler {

	public:

		/**
		 * \brief Scheduler constructor.
		 * \return Not applicable.
		 */
		Scheduler(void);

		/**
		 * \brief Destructor.
		 * \return Not applicable.
		 */
		virtual ~Scheduler(void);

		/**
		 * \brief Retrieves a database instance identification to be used by the added PM.
		 * \return A database identification chosen in a round robin fashion.
		 */
		int GetLastAssignedDatabase (void);

		/**
		 * \brief Chooses the hosts to receive processing module instances.
		 * \param cluster Cluster information.
		 * \param database_communicator the database communicator.
		 * \param processing_module_configurator The processing module configurator.
		 * \return The hosts names will be used and the number of process' instances for each one.
		 */
		map < string, int > ChooseHostsToAddProcessingModule(map<string, Host>* cluster, MpiCommunicator* database_communicator, ProcessingModuleConfigurator* processing_module_configurator);

		/**
		 * todo
		 */
		vector<SchedulerOperation> Balance (map<string, ProcessingModuleEntry*>* active_processing_modules);

	protected:

	private:

		/** \brief The last database daemon assigned to one processing module. */
		int last_assigned_database_identification_;
};

#endif  // WATERSHED_SCHEDULER_SCHEDULER_H_
