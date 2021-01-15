//
// BY ig:@5bub
//

#include "../headers/includes.h"
using namespace std::literals::chrono_literals;
int main()
{
	std::unique_ptr<Instagram> ig =  std::make_unique<Instagram>();
	auto clear = []() { system("cls"); };
	// Register CTRL - C signal and (signal_handler)lambda expression to delete ig for deallocates memory from heap
	signal(SIGINT, [](int Ecode) {
		exit(Ecode);
		});
	std::string username, password;
try_again:
	clear();
	std::cout << "Username: ";
	std::getline(std::cin, username);
	std::cout << "Password: ";
	std::getline(std::cin, password);
	if (!(ig->Login(username, password))) {
		std::cout << "Failed, Try Again" << std::endl;
		std::this_thread::sleep_for(1s);
		goto try_again;
	}
	std::cout << "Logged in Successfully" << std::endl;
	clear();
	while (true) {
		ig->GetUpdate();
		std::cout << "== Loop Complete ==\n";
	}
	return 0;
}
