/**
 * \file comm/message.h
 * \author Rodrigo Silva Oliveira
 * \author Thatyene Louise Alves de Souza Ramos
 */

#ifndef WATERSHED_COMM_MESSAGE_H_
#define WATERSHED_COMM_MESSAGE_H_

/* Other libraries */

/* Project's .h */
#include "comm/message_data.h"

using namespace std;

/**
 * \class Message
 * \brief Implementation of a generic message.
 * \author Rodrigo Silva Oliveira
 * \author Thatyene Louise Alves de Souza Ramos
 * \version 1.0
 * \date 2011
 */
class Message {

	public:

		/**
		 * \brief Default constructor. Creates a new Message instance.
		 * \return Not applicable.
		 */
		Message ( void );

		/**
		 * \brief Creates a new Message instance with user defined fields.
		 * \param data Message data.
		 * \param operation_code Message operation code.
		 * \param size Data size in bytes.
		 * \return Not applicable.
		 */
		Message ( void* data, int operation_code, int size );

		/**
		 * \brief Creates a new Message instance based on the data passed as parameter.
		 * \param data The message data.
		 * \param size The message size in bytes.
		 * \return Not applicable.
		 */
		Message ( void* data, int size );

		/**
		 * \brief Destructor.
		 * \return Not applicable.
		 */
		~Message ( void );

		/**
		 * \brief Retrieves the size of internal data.
		 * \return Size of the data.
		 */
		int GetDataSize ( void );

		/**
		 * \brief Retrieves the message operation code.
		 * \return Message operation code.
		 */
		int GetOperationCode ( void );

		/**
		 * \brief Retrieves the message sequence number.
		 * \return The message sequence number.
		 */
		int GetSequenceNumber ( void );

		/**
		 * \brief Retrieves the real size of message data.
		 * \return Size of the data.
		 */
		int GetSize ( void );

		/**
		 * \brief Retrieves the message sender.
		 * \return The message sender.
		 */
		int GetSource ( void );

		/**
		 * \brief Returns the time when the message was created.
		 * \return The message timnestamp.
		 */
		int GetTimestamp ( void );

		string GetSourceStream(void);

		/**
		 * \brief Retrieves the data from a message.
		 * \return Pointer to the data.
		 */
		void* GetData ( void );

		/**
		 * \brief Sets the message data.
		 * \param data Message data.
		 * \param size Data size.
		 * \return Not applicable.
		 */
		void SetData ( void* data, int size );

		/**
		 * \brief Assign an operation code to a message.
		 * \param operation_code Message operation code.
		 * \return Not applicable.
		 */
		void SetOperationCode ( int operation_code );

		/**
		 * \brief Sets the message sequence number.
		 * \param sequence_number The message sequence number.
		 * \return Not applicable.
		 */
		void SetSequenceNumber ( int sequence_number );

		/**
		 * \brief Assign a source to a message.
		 * \param source Message source.
		 * \return Not applicable.
		 */
		void SetSource ( int source );

		/**
		 *
		 */
		void SetSourceStream(string source_stream);

		/**
		 * \brief Sets the creation time for this message.
		 * \param timestamp The message timestamp.
		 * \return Not applicable.
		 */
		void SetTimestamp ( int timestamp );

	protected:

	private:

		/** \brief Data size in bytes. */
		int data_size_;

		/** \brief Message operation code. */
		int operation_code_;

		/** \brief Sequence number of the message. */
		int sequence_number_;

		/** \brief The message source. */
		int source_;

		/** \brief The message creation timestamp. */
		int timestamp_;

		/** The name of the source stream. */
		char source_stream_[Constants::MAX_LINE_SIZE];

		/** \brief Data. */
		/* Keep this field as the last one because it is necessary to move only the effective data. */
		char data_[Constants::MAX_DATA_SIZE];
};

#endif /*  WATERSHED_COMM_MESSAGE_H_ */
