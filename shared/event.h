#pragma once
#include <algorithm>
#include <assert.h>
#include <concepts>
#include <functional>
#include <list>

// An "effectful" event is one that does something before and/or after it's fired
template <typename EventType>
concept effectful = requires(EventType e) {
	{ e.before_fire() } -> std::convertible_to<int>;
	{ e.after_fire() } -> std::convertible_to<int>;
};

template <typename EventType>
class event_listener;

template <typename EventType>
class event_channel {
private:
	struct listener_key {
		event_listener<EventType> * listener;
		int pos;

		listener_key(event_listener<EventType> * _listener, int _pos) : listener(_listener), pos(_pos) {}

		friend bool operator<(const listener_key &lhs, const listener_key &rhs) noexcept {
			return std::tie(lhs.pos, lhs.listener) < std::tie(rhs.pos, rhs.listener);
		}

		friend bool operator==(const listener_key &lhs, const listener_key &rhs) noexcept {
			return lhs.listener == rhs.listener;
		}
	};

	std::list<listener_key> listeners;

public:
	event_listener<EventType> * _subscribe(event_listener<EventType> * listener, int pos) {
		listener_key key(listener, pos);

		auto it = std::upper_bound(listeners.begin(), listeners.end(), key);
		listeners.insert(it, key);

		return listener;
	}

	void _unsubscribe(event_listener<EventType> * listener) {
		listener_key key(listener, 0);

		auto it = std::find(listeners.begin(), listeners.end(), key);

		if (it == listeners.end()) {
			// TODO: Errors
			throw "Not subscribed";
		}

		listeners.erase(it, std::next(it));
	}

	void _fire(EventType &event) {
		for (auto listener : listeners) {
			listener.listener->handle(event);
		}
	}

protected:
	~event_channel() {
		assert(("All listeners are cleaned up", listeners.size() == 0));
	}
};

template <typename Item, typename ... Items>
struct exists {
	static constexpr bool value{ (std::is_same<Item, Items>::value || ...) };
};

template <typename ... Events>
class event_bus : public event_channel<Events>... {
public:
	template <typename EventType>
	requires exists<EventType, Events ...>::value && effectful<EventType>
	void fire(EventType &event) {
		event.before_fire();

		event_channel<EventType> * channel = static_cast<event_channel<EventType> *>(this);

		channel->_fire(event);
		event.after_fire();
	}

	template <typename EventType>
	requires exists<EventType, Events ...>::value && !effectful<EventType>
	void fire(EventType &event) {
		event_channel<EventType> * channel = static_cast<event_channel<EventType> *>(this);

		channel->_fire(event);
	}
};

template <typename EventType>
class event_listener {
private:
	event_channel<EventType> * channel;
	event_listener<EventType> * id;
	int priority;

protected:
	void set_channel(event_channel<EventType> * _channel) {
		channel = _channel;
	}

public:
	event_listener(event_channel<EventType> * _channel, int _priority = 0) : channel(_channel), id(nullptr), priority(_priority) {}

	event_listener(const event_listener<EventType> &other) : channel(other.channel), id(nullptr), priority(other.priority) {
		if (other.id != nullptr) {
			id = channel->_subscribe(this, priority);
		}
	}

	event_listener(event_listener<EventType> &&other) : channel(other.channel), id(nullptr), priority(other.priority) {
		if (other.id != nullptr) {
			other.channel->_unsubscribe(other.id);
			other.id = nullptr;

			id = channel->_subscribe(this, priority);
		}
	}

	event_listener<EventType>& operator=(const event_listener<EventType> &other) {
		if (id != nullptr) {
			channel->_unsubscribe();
		}

		channel = other.channel;
		priority = other.priority;
		id = nullptr;

		if (other.id != nullptr) {
			id = channel->_subscribe(this, priority);
		}

		return *this;
	}

	event_listener<EventType>& operator=(event_listener<EventType> &&other) {
		if (id != nullptr) {
			channel->_unsubscribe(id);
		}

		channel = other.channel;
		priority = other.priority;
		id = nullptr;

		if (other.id != nullptr) {
			other.channel->_unsubscribe(other.id);
			other.id = nullptr;

			id = channel->_subscribe(this, priority);
		}

		return *this;
	}

	~event_listener() {
		if (id != nullptr) {
			channel->_unsubscribe(id);
			id = nullptr;
		}
	}

	// TODO: Throw errors
	void subscribe() {
		if (channel == nullptr) {
			printf("No event channel to subscribe to\n");
			return;
		}

		if (id) {
			printf("Already subscribed\n");
			return;
		}

		id = channel->_subscribe(this, priority);
	}

	virtual int handle(EventType &event) = 0;
};
