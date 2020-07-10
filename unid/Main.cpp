#include <iostream>
#include <optional>
#include <windows.h>
#include "Dispatcher.h"

struct Action1 {};
struct Action2 {};
struct NonDispatchedAction {};

class MyStore {
public:
	void operator()(const Action1& a) {
		std::cout << "Action1 dispatched from MyStore!" << std::endl;
	}

	void operator()(const Action2& a) {
		std::cout << "Action2 dispatched from MyStore!" << std::endl;
	}
};

class MyStaticStore {
public:
	void operator() (const Action1& a) {
		std::cout << "Action1 Dispatched from MyStaticStore!" << std::endl;
	}
};

class MyStaticStore2 {
public:
	MyStaticStore2() = default;
	MyStaticStore2(const MyStaticStore2&) = delete;
	const MyStaticStore2& operator=(const MyStaticStore2&) = delete;

	void operator() (const double& b) {
		std::cout << "Double Dispatched from MyStaticStore2!" << std::endl;
	}
};

int main() {

	unid::DynamicRoute dispatcher;
	
	// unid::ReductionStore<int, SomeStore, MyStruct, AnotherStruct> f;
	unid::EventConsumer<MyStore, Action1, Action2> g;
	unid::EventConsumer<MyStaticStore, Action1> b;

	dispatcher.registerConsumer(g);
	dispatcher.registerConsumer(b);
	// dispatcher(Action1{});
	// dispatcher(Action1{});
	// dispatcher(Action1{});
	// dispatcher(Action2{});

	// std::cout << f.getState();
	MyStaticStore static1{};
	MyStaticStore2 static2{};
	unid::StaticRoute staticDisp(static1, static2);
	staticDisp(Action1{});
	std::cout << std::endl;
	staticDisp(Action1{});
	std::cout << std::endl;
	staticDisp(Action1{});
	std::cout << std::endl;
	staticDisp(NonDispatchedAction{});

	return 0;
}
