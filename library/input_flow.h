/**
 * \file library/input_flow.h
 * \author Rodrigo Silva Oliveira
 * \author Thatyene Louise Alves de Souza Ramos
 */

#ifndef WATERSHED_LIBRARY_INPUT_FLOW_H_
#define WATERSHED_LIBRARY_INPUT_FLOW_H_

/* C++ libraries */
#include <string>

using namespace std;

/**
 * \class InputFlow
 * \brief Implementation of the input flow.
 * \author Rodrigo Silva Oliveira
 * \author Thatyene Louise Alves de Souza Ramos
 * \version 1.0
 * \date 2011
 */
class InputFlow {
	public:

		/**
		 * \brief Creates a new instance of InputFlow.
		 * \return Not applicable.
		 */
		InputFlow ( void );

		/**
		 * \brief Destructor. Destroys the instance of InputFlow.
		 * \return Not applicable.
		 */
		virtual ~InputFlow ( void );

		/**
		 * \brief Retrieves the name of the input flow.
		 * \return The name of the input flow.
		 */
		string GetName ( void );

		/**
		 * \brief Retrieves the name of the input flow distribution policy.
		 * \return The name of the input flow policy.
		 */
		string GetPolicy ( void );

		/**
		 * \brief Retrieves the name of the policy function file when it is a labeled stream.
		 * \return The name of the policy function file.
		 */
		string GetPolicyFunctionFile ( void );

		/**
		 * \brief Retrieves the input flow query.
		 * \return The input flow query.
		 */
		string GetQuery ( void );

		/**
		 * \brief Sets the name of the input flow.
		 * \param name Input flow name.
		 * \return Not applicable.
		 */
		void SetName ( string name );

		/**
		 * \brief Sets the name of the input flow policy.
		 * \param policy Input flow policy name.
		 * \return Not applicable.
		 */
		void SetPolicy ( string policy );

		/**
		 * \brief Sets the name of the file containing the policy function when it is a labeled stream.
		 * \param policy_function_file Policy function file name.
		 * \return Not applicable.
		 */
		void SetPolicyFunctionFile ( string policy_function_file );

		/**
		 * \brief Sets the query used in the input stream.
		 * \param query The query to be used.
		 * \return Not applicable.
		 */
		void SetQuery ( string query );

	protected:

	private:

		/** \brief The input flow name. */
		string name_;

		/** \brief The distribution policy. */
		string policy_;

		/** \brief The policy function file name. */
		string policy_function_file_;

		/** \brief The query to be applied on the input data. */
		string query_;
};

#endif /* WATERSHED_LIBRARY_INPUT_FLOW_H_ */
