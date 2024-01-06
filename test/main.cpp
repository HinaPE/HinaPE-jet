#include "jet/jet.h"

#include "jet/sphere3.h"
#include <iostream>

int main()
{
	using namespace jet;
	auto sphere = Sphere3::builder()
			.withCenter({1, 1, 1})
			.withRadius(1).makeShared();
	std::cout << sphere->closestDistance({0, 0, 0}) << "\n";
	return 0;
}
