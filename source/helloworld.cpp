#include <iostream>

extern int patch_ver_01();

int main(void)
{
	int xxx = patch_ver_01();
	std::cout << "xxx " << xxx;
	return 0;
}
