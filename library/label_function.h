/**
 * \file library/label_function.h
 * \author Rodrigo Silva Oliveira
 * \author Thatyene Louise Alves de Souza Ramos
 */

#ifndef WATERSHED_LIBRARY_LABEL_FUNCTION_H_
#define WATERSHED_LIBRARY_LABEL_FUNCTION_H_

/* C libraries */
#include <dlfcn.h>

/* C++ libraries */
#include <iostream>

/* Project's .h */
#include "comm/message.h"

using namespace std;

/**
 * \class LabelFunction
 * \brief Implementation of a generic label function.
 * \author Rodrigo Silva Oliveira
 * \author Thatyene Louise Alves de Souza Ramos
 * \version 1.0
 * \date 2011
 */
class LabelFunction {

	public:

		/**
		 * \brief Default constructor. Creates a new LabelFunction instance.
		 * \return Not applicable.
		 */
		LabelFunction(void);

		/**
		 * \brief Destructor.
		 * \return Not applicable.
		 */
		virtual ~LabelFunction(void);

		/**
		 * \brief Retrieves the data consumer module rank that will receive the message.
		 * \param message Message to be sent.
		 * \param total_instances Number of data consumer module instances.
		 * \return The rank of the data consumer instance chosen to receive the message.
		 */
		virtual int GetLabel(Message& message, int total_instances) = 0;

	protected:

	private:
};

/** \brief The types of the class factories. */
typedef LabelFunction* create_function();

/** \brief Macro used to create a new instance of a specialized label function. */
#define RegisterFunction( class_name ) extern "C" LabelFunction* GetInstance() { return new class_name; }

#endif /* WATERSHED_LIBRARY_LABEL_FUNCTION_H_ */
