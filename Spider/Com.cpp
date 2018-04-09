#include "Com.h"

void Com::HandleLogs(void) {
	std::ifstream	file;
	std::ofstream	tmp;
	std::string		str;
	int				i = 0;

	file.open(LOGS_FILE);
	tmp.open(TMP_FILE);
	while (getline(file, str)) {
		if (i == 0) {
			/* Call here send message function */
			std::cout << str << std::endl;
		}
		if (i != 0)
			tmp << str << std::endl;
		++i;
	}
	file.close();
	tmp.close();
	remove(LOGS_FILE);
	rename(TMP_FILE, LOGS_FILE);
}