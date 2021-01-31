#include <iostream>

int main() {
    std::cout << "Hello!\n";
    std::string response;
    std::getline(std::cin, response);
    std::cout << "you're name is " << response << std::endl;
    return 0;
}