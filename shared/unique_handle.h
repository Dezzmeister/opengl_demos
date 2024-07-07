#pragma once
#include <functional>

template <typename T>
class unique_handle {
public:
	using deleter = std::function<void(T)>;

	// TODO: Error handling
	unique_handle(T _null_handle, deleter _delete_handle);

	unique_handle(const unique_handle<T> &other) = delete;
	unique_handle(unique_handle<T> &&other);

	unique_handle<T>& operator=(const unique_handle<T> &other) = delete;
	// TODO: Error handling. What if a GL call fails here
	unique_handle<T>& operator=(unique_handle<T> &&other) noexcept;

	operator T() noexcept;
	operator T() const noexcept;

	T * operator&() noexcept;
	const T * operator&() const noexcept;

	// Assigns to the handle held within. If a null handle is provided
	// and the held handle is not null, the held handle will be deleted.
	T& operator=(T other) noexcept;

	friend bool operator==(const unique_handle<T> &a, const unique_handle<T> &b) {
		return a.handle == b.handle;
	}

	~unique_handle();
private:
	T handle;
	T null_handle;
	deleter delete_handle;
};

template <typename T>
unique_handle<T>::unique_handle(T _null_handle, deleter _delete_handle) :
	handle(_null_handle),
	null_handle(_null_handle),
	delete_handle(_delete_handle)
{}

template <typename T>
unique_handle<T>::unique_handle(unique_handle<T> &&other) {
	handle = other.handle;
	null_handle = other.null_handle;
	delete_handle = other.delete_handle;

	other.handle = other.null_handle;
}

template <typename T>
unique_handle<T>& unique_handle<T>::operator=(unique_handle<T> &&other) noexcept {
	if (handle != null_handle) {
		delete_handle(handle);
		handle = null_handle;
	}

	handle = other.handle;
	null_handle = other.null_handle;
	delete_handle = other.delete_handle;

	other.handle = other.null_handle;

	return *this;
}

template <typename T>
unique_handle<T>::~unique_handle() {
	if (handle != null_handle) {
		delete_handle(handle);
	}
}

template <typename T>
unique_handle<T>::operator T() noexcept {
	return handle;
}

template <typename T>
unique_handle<T>::operator T() const noexcept {
	return handle;
}

template <typename T>
T * unique_handle<T>::operator&() noexcept {
	return &handle;
}

template <typename T>
const T * unique_handle<T>::operator&() const noexcept {
	return &handle;
}

template <typename T>
T& unique_handle<T>::operator=(T other) noexcept {
	if (other == null_handle && handle != null_handle) {
		delete_handle(handle);
	}

	handle = other;

	return handle;
}
