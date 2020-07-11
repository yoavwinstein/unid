#include <iostream>
#include <optional>
#include <windows.h>
#include "Dispatcher.h"
#include "MainRoute.h"

struct Action1 {};
struct Action2 {};
struct NonDispatchedAction {};

class EventConsumer1 {
public:
	void operator()(const Action1& a) {
		std::cout << "Action1 dispatched from MyStore!" << std::endl;
	}

	void operator()(const Action2& a) {
		std::cout << "Action2 dispatched from MyStore!" << std::endl;
	}
};

class EventConsumer2 {
public:
	void operator() (const Action1& a) {
		std::cout << "Action1 Dispatched from MyStaticStore!" << std::endl;
	}
};

class EventConsumer3 {
public:
	EventConsumer3() = default;
	EventConsumer3(const EventConsumer3&) = delete;
	const EventConsumer3& operator=(const EventConsumer3&) = delete;

	void operator() (const double& b) {
		std::cout << "Double Dispatched from MyStaticStore2!" << std::endl;
	}
};

static unid::RegisterConsumer<unid::EventConsumer<EventConsumer1, Action1, Action2>> x;
static unid::RegisterConsumer<unid::EventConsumer<EventConsumer2, Action1>> y;

int main() {

	unid::DynamicRoute dispatcher;

	auto consumers = unid::ConsumerRegistry::constructAll();
	for (auto& consumer : consumers) {
		dispatcher.registerConsumer(*consumer);
	}

	dispatcher(Action1{});

	return 0;
}
