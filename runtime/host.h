/**
 * \file runtime/host.h
 * \author Rodrigo Silva Oliveira
 * \author Thatyene Louise Alves de Souza Ramos
 */

#ifndef WATERSHED_RUNTIME_HOST_H_
#define WATERSHED_RUNTIME_HOST_H_

/* C libraries */
#include <string.h>

/* C++ libraries */
#include <vector>

/* Project's .h */
#include "common/constants.h"

using namespace std;

/**
 * \class Host
 * \brief Implementation of a host information class.
 * \author Rodrigo Silva Oliveira
 * \author Thatyene Louise Alves de Souza Ramos
 * \version 1.0
 * \date 2011
 */
class Host {

	public:

		/**
		 * \brief Default constructor. Creates a new Host instance.
		 * \return Not applicable.
		 */
		Host(void);

		/**
		 * \brief Creates a new Host instance using the values passed as parameters.
		 * \param name Host name.
		 * \return Not applicable.
		 */
		Host(string name);

		/**
		 * \brief Destructor.
		 * \return Not applicable.
		 */
		virtual ~Host(void);

		/**
		 * \brief Checks if a host has a specific resource.
		 * \param resource Resource name to be checked.
		 * \return True if the host has that resource. False otherwise.
		 */
		bool HasResource(string resource);

		/**
		 * \brief Checks if a host has a database daemon.
		 * \return True if the host has that resource. False otherwise.
		 */
		bool IsDatabaseServer(void);

		/**
		 * \brief Retrieves the database daemon identification.
		 * \return The database daemon identification.
		 */
		int GetDatabaseDaemonId(void);

		/**
		 * \brief Retrieves the runtime process identification.
		 * \return The runtime daemon identification value.
		 */
		int GetRuntimeDaemonId(void);

		/**
		 * \brief Retrieves the host name.
		 * \return Name of the host.
		 */
		string GetName(void);

		/**
		 * \brief Retrieves all the resources offered by the host.
		 * \return A list containing the resources' names.
		 */
		vector<string>* GetResources(void);

		/**
		 * \brief Adds a resource to a host.
		 * \param resource_name Resource name.
		 * \return Not applicable.
		 */
		void AddResource(string resource_name);

		/**
		 * \brief Sets the database daemon identification.
		 * \param id The database daemon identification.
		 * \return Not applicable.
		 */
		void SetDatabaseDaemonId(int id);

		/**
		 * \brief Sets a host as a database server or not.
		 * \param value True or false.
		 * \return Not applicable.
		 */
		void SetDatabaseServer(bool value);

		/**
		 * \brief Assign an identification to a runtime daemon in this host.
		 * \param id New host identification.
		 * \return Not applicable.
		 */
		void SetRuntimeDaemonId(int id);

	protected:

	private:

		/** \brief Flag which says if the host has a database daemon. */
		bool database_server_;

		/** Database daemon identification. */
		int database_daemon_id_;

		/** \brief Host identification. Identifies the process id running in this host. */
		int runtime_daemon_id_;

		/** \brief Resources available in the host. */
		vector<string> resources_;

		/** \brief Name of the host. */
		string name_;
};

#endif  // WATERSHED_RUNTIME_HOST_H_
