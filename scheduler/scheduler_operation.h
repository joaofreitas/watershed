/**
 * \file common/scheduler_operation.h
 * \author Rodrigo Silva Oliveira
 * \author Thatyene Louise Alves de Souza Ramos
 */

#ifndef WATERSHED_SCHEDULER_SCHEDULER_OPERATION_H_
#define WATERSHED_SCHEDULER_SCHEDULER_OPERATION_H_

#include <string>

using namespace std;

/**
 * todo
 */
class SchedulerOperation {

	public:

		/**
		 * todo
		 */
		SchedulerOperation ( void );

		/**
		 * todo
		 */
		virtual ~SchedulerOperation ( void );

		/**
		 * todo
		 */
		int GetInstanceRank ( void );

		/**
		 * todo
		 */
		int GetOperationDescription ( void );

		/**
		 * todo
		 */
		string GetModuleName ( void );

		/**
		 * todo
		 */
		string GetTargetHost ( void );

		/**
		 * todo
		 */
		void SetInstanceRank ( int instance_rank_ );

		/**
		 * todo
		 */
		void SetModuleName ( string module_name_ );

		/**
		 * todo
		 */
		void SetOperationDescription ( int operation_description_ );

		/**
		 * todo
		 */
		void SetTargetHost ( string target_host_ );

	protected:

	private:

		/** todo */
		int instance_rank_;
		/** todo */
		int operation_description_;
		/** todo */
		string module_name_;
		/** todo */
		string target_host_;
};

#endif /* WATERSHED_SCHEDULER_SCHEDULER_OPERATION_H_ */
