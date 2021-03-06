#pragma once

#include "TypeIdentifier.h"
#include <vector>
#include <unordered_map>
#include <variant>


namespace unid {

class IEventConsumer {
public:
	virtual void operator()(uint64_t id, const void* action) = 0;
	virtual std::vector<uint64_t> getActionIDs() const = 0;
	virtual ~IEventConsumer() = default;
};

template <typename Impl, typename ...Args>
class EventConsumer : public IEventConsumer, public Impl {
public:
	virtual void operator()(uint64_t id, const void* action) override {
		dispatchImpl<Args...>(id, action);
	}

	virtual std::vector<uint64_t> getActionIDs() const override {
		std::vector<uint64_t> vec;
		getActionIDsImpl<Args...>(vec);
		return vec;
	}

private:
	
	template <typename T, typename ...Args>
	void getActionIDsImpl(std::vector<uint64_t>& vec) const {
		vec.push_back(details::type_id<T>());
		if constexpr (sizeof...(Args) > 0) {
			getActionIDsImpl<Args...>(vec);
		}
	}

	template <typename T, typename ...Args>
	void dispatchImpl(uint64_t id, const void* action) {

		constexpr auto clsId = details::type_id<T>();
		if (clsId == id) {
			Impl::operator()(reinterpret_cast<const T&>(action));
		}
		if constexpr (sizeof...(Args) > 0) {
			dispatchImpl<Args...>(id, action);
		}
	}
};

class DynamicRoute final {
public:
	template <typename T>
	void operator()(const T& event) {
		uint64_t id = details::type_id<T>();
		auto eventConsumersMapIter = m_eventConsumers.find(id);
		if (eventConsumersMapIter == m_eventConsumers.end()) {
			throw std::exception("No consumers found for event");
		}
		for (auto& eventConsumer : eventConsumersMapIter->second) {
			eventConsumer(id, reinterpret_cast<const void*>(&event));
		}
	}

	void registerConsumer(IEventConsumer& consumer) {
		for (uint64_t id : consumer.getActionIDs()) {
			m_eventConsumers[id].push_back(consumer);
		}
	};

	void unregisterConsumer(IEventConsumer& consumer) {
		for (uint64_t id : consumer.getActionIDs()) {
			auto& eventConsumersVec = m_eventConsumers[id];
			eventConsumersVec.erase(std::remove_if(
				eventConsumersVec.begin(),
				eventConsumersVec.end(),
				[&consumer](const auto& a) {
				return &a.get() == &consumer;
			}), eventConsumersVec.end());
		}
	}

private:

	using EventConsumerReferenceType = std::reference_wrapper<IEventConsumer>;
	using EventConsumersVectorType = std::vector<EventConsumerReferenceType>;

	std::unordered_map<uint64_t, EventConsumersVectorType> m_eventConsumers;
};

template <typename... EventConsumerTypes>
class StaticRoute final {
public:
	StaticRoute(EventConsumerTypes&... consumers) : m_eventConsumers(consumers...) {}

	template <typename T, size_t I = 0, bool anyConsumer = false>
	void operator()(const T& action) {
		auto& eventConsumer = std::get<I>(m_eventConsumers);
		constexpr bool anyConsumerOnThisLevel = std::is_invocable_v<std::decay_t<decltype(eventConsumer)>, const T&>;
		if constexpr (anyConsumerOnThisLevel) {
			eventConsumer(action);
		}

		if constexpr (I + 1 < sizeof...(EventConsumerTypes)) {
			operator()<T, I + 1, anyConsumer || anyConsumerOnThisLevel>(action);
		} else {
			static_assert(anyConsumer || anyConsumerOnThisLevel, "No consumers found for type");
		}
	}

private:
	std::tuple<EventConsumerTypes&...> m_eventConsumers;
};

}
