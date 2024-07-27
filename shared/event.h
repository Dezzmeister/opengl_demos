// This file defines the core data structures for event passing. In this program, an event
// can be anything. There is no "Event" base class, and there is no need for a discriminator
// to distinguish between event types, because the event passing structures are defined in a
// way that allows the compiler to sort out event types for us.
//
// A consumer who wants to handle a `draw_event` needs to do three things:
//		1. Define a class that extends `event_listener<draw_event>` and implements
//			`int handle(draw_event &event)`.
//		2. Get an `event_channel<draw_event>` in the constructor and pass it to the base class.
//		3. Invoke the base class's `subscribe` method to subscribe to `draw_event`s on the
//			previously obtained event channel.
// Once the instance of the derived class is subscribed to an event channel, it will receive all
// events on that channel - the event channel will call the derived class's `handle()` method with
// a mutable reference to the event. Because C++ allows multiple inheritance, a class can extend
// `event_listener` for several different events. As long as the class implements the necessary
// `handle()` methods and obtains `event_channel`s for each event it cares about, it can subscribe to
// each channel and each event type will be dispatched to a different handler, determined at
// compile-time.
//
// Some notes:
//  - Event channels can and should be grouped into event buses. An `event_bus` is just a class that
//		extends multiple `event_channel`s, so it can be used wherever a base `event_channel` is
//		expected.
//	- Event channels allow listeners to subscribe to them with an integer priority. The priority is
//		used to order the event listeners in the channel. Listeners with lower integer priorities are
//		called first, and listeners with higher priorities are called last. The default priority is 0.
//		TODO: Make this a little nicer and easier to understand; it's hard to keep track of various
//		event listener priorities in practice.
//	- Event listeners have copy and move semantics. When an event listener is destroyed, it will unsubscribe
//		from its event channel if it needs to. Subscribed event listeners can also be copied and moved. A
//		subscribed event listener that is copied will cause the copy to subscribe as well, and a subscribed
//		event listener that is moved will unsubscribe, and cause the destination listener to subscribe.
//		It's necessary to deal with copies and moves explicitly because event channels keep non-owning 
//		pointers to event listeners.
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

		if (id != nullptr) {
			printf("Already subscribed\n");
			return;
		}

		id = channel->_subscribe(this, priority);
	}

	void unsubscribe() {
		if (channel == nullptr) {
			printf("No channel to unsubscribe from\n");
			return;
		}

		if (id == nullptr) {
			printf("Not subscribed\n");
			return;
		}

		channel->_unsubscribe(this);
		id = nullptr;
	}

	virtual int handle(EventType &event) = 0;
};
