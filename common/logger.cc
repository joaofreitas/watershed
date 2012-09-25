/**
 * \file common/logger.cc
 * \author Rodrigo Silva Oliveira
 * \author Thatyene Louise Alves de Souza Ramos
 */

#include "common/logger.h"

const string Logger::ERROR_TEXT = "ERROR";
const string Logger::INFO_TEXT = "INFO";
const string Logger::WARNING_TEXT = "WARNING";
int Logger::number_of_instances_ = 0;
pthread_mutex_t Logger::class_mutex_;

Logger::Logger ( string log_file_name, bool screen_as_output, string screen_separator ) {
	++number_of_instances_;
	if ( number_of_instances_ == 1 ) {
		pthread_mutex_init ( &class_mutex_, NULL );
	}
	screen_as_output_ = screen_as_output;
	screen_separator_ = screen_separator;
	log_file_.open ( log_file_name.c_str (), fstream::app );
}

Logger::~Logger ( void ) {
	--number_of_instances_;
	if ( number_of_instances_ == 0 ) {
		pthread_mutex_destroy ( &class_mutex_ );
	}
	log_file_.close ();
}

void Logger::Lock ( void ) {
	pthread_mutex_lock ( &class_mutex_ );
}

void Logger::PrintError ( string message ) {
	message = ERROR_TEXT + " " + message;
	PrintFile ( message );
	if ( screen_as_output_ ) {
		PrintScreen ( message );
	}
}

void Logger::PrintFile ( string message ) {
	time_t rawtime;
	struct tm* timeinfo;
	int time_size = 50;
	char formatted_time[time_size];

	time ( &rawtime );
	timeinfo = localtime ( &rawtime );
	strftime ( formatted_time, time_size, "[%F %T]", timeinfo );

	log_file_ << formatted_time << " " << message << endl;
}

void Logger::PrintInfo ( string message ) {
	message = INFO_TEXT + " " + message;
	PrintFile ( message );
	if ( screen_as_output_ ) {
		PrintScreen ( message );
	}
}

void Logger::PrintScreen ( string message ) {
	time_t rawtime;
	struct tm* timeinfo;
	int time_size = 50;
	char formatted_time[time_size];

	time ( &rawtime );
	timeinfo = localtime ( &rawtime );
	strftime ( formatted_time, time_size, "[%F %T]", timeinfo );

	cout << formatted_time << " " << message << endl;
}

void Logger::PrintScreenSeparator ( void ) {
	log_file_ << screen_separator_ << endl;
	cout << screen_separator_ << endl;
}

void Logger::PrintWarning ( string message ) {
	message = WARNING_TEXT + " " + message;
	PrintFile ( message );
	if ( screen_as_output_ ) {
		PrintScreen ( message );
	}
}

void Logger::Unlock ( void ) {
	pthread_mutex_unlock ( &class_mutex_ );
}
