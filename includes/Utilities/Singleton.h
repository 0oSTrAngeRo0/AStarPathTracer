#pragma once

template <typename T>
class Singleton {
public:
	static T& GetInstance() {
		static T instance = T();
		return instance;
	}

protected:
	Singleton() {}
	virtual ~Singleton() = default;

private:
	Singleton(const Singleton&) = delete;
	Singleton& operator=(const Singleton&) = delete;
};