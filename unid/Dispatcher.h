#pragma once

#include "TypeIdentifier.h"
#include <vector>
#include <unordered_map>

namespace unid {

class IStore {
public:
	virtual void dispatch(uint64_t id, const void* action) = 0;
	virtual std::vector<uint64_t> getActionIDs() const = 0;
};

template <typename Impl, typename ...Args>
class Store : public IStore, public Impl {
public:

	using Impl::onAction;

	virtual void dispatch(uint64_t id, const void* action) override {
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
			this->onAction(reinterpret_cast<const T&>(action));
		}
		if constexpr (sizeof...(Args) > 0) {
			dispatchImpl<Args...>(id, action);
		}
	}
};

template <typename T, typename Impl>
class ReductionStoreImpl : public Impl {
public:
	using Impl::reduce;

	template <typename U>
	void onAction(const U& action) {
		auto newState = reduce(action, m_state);
		if (newState.has_value()) {
			m_state = *newState;
		}
	}

	const T& getState() const {
		return m_state;
	}

private:
	T m_state{};
};

template <typename State, typename Impl, typename ...Args>
using ReductionStore = Store<ReductionStoreImpl<State, Impl>, Args...>;

class Dispatcher {
public:

	template <typename T>
	void dispatch(const T& action) {
		uint64_t id = details::type_id<T>();
		m_map[id]->dispatch(id, reinterpret_cast<const void*>(&action));
	}

	void registerStore(IStore* store) {
		for (uint64_t id : store->getActionIDs()) {
			m_map[id] = store;
		}
	};

	std::unordered_map<uint64_t, IStore*> m_map;
};

}
