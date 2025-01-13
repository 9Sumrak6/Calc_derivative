#include "factory.hpp"


int main() {
	Factory factory;
	auto f = factory.Create("polynomial", {0, 3, 1});
	auto g = factory.Create("const", -10);
	auto h = *g + *f;
	cout << h.GetDeriv(10) << endl;
}