#include <iostream>
#include <optional>
#include <windows.h>
#include "Dispatcher.h"

struct MyStruct {};
struct AnotherStruct {};

class MyStore {
public:
	void onAction(const MyStruct& a) {
		std::cout << "Hello world!" << std::endl;
	}

	void onAction(const AnotherStruct& a) {
		std::cout << "Hello world 2!" << std::endl;
	}
};

class SomeStore {
public:
	std::optional<int> reduce(const MyStruct& a,
		                      const int& previousState) {
		return previousState + 1;
	}

	std::optional<int> reduce(const AnotherStruct& a,
		                      const int& previousState) {
		return previousState - 1;
	}
};

int main() {

	unid::Dispatcher dispatcher;
	
	unid::ReductionStore<int, SomeStore, MyStruct, AnotherStruct> f;
	unid::Store<MyStore, MyStruct, AnotherStruct> g;
	
	dispatcher.registerStore(&g);
	dispatcher.dispatch(MyStruct{});
	dispatcher.dispatch(MyStruct{});
	dispatcher.dispatch(MyStruct{});
	dispatcher.dispatch(AnotherStruct{});

	std::cout << f.getState();

	return 0;
}
