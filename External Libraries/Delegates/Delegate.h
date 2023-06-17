#pragma once


#include <functional>
#include "..\Debug\Macros.h"
#include <vector>

#define DELEGATE(DelegateName,ReturnType, ...) \
	using DelegateName = Delegate<ReturnType, __VA_ARGS__>;

#define SECURE_DELEGATE(DelegateName, Class, ...) \
	using DelegateName = SecureDelegate<Class, __VA_ARGS__>;

#define MULTICAST_DELEGATE(DelegateName, ...) \
	using DelegateName = MulticastDelegate<__VA_ARGS__>;

#define SECURE_MULTICAST_DELEGATE(DelegateName, Class, ...) \
	using DelegateName = SecureMulticastDelegate<Class, __VA_ARGS__>;

template<class Class, typename ...Args>
class SecureMulticastDelegate final
{
	class Class;
public:

	SecureMulticastDelegate() = default;

	void Bind(std::function<void(Args...)> Funct)
	{
		Functions.push_back(Funct);
	}

	void Unbind(std::function<void(Args...)> Function)
	{
		const size_t Size = Functions.size();
		for (const size_t i = 0; i < Size; ++i)
			if (Functions[i] == Function)
				Functions.erase(Functions.begin() + i);
	}

private:

	friend class Class;

	void Execute(Args... Arguments) const
	{
		for (auto& Function : Functions)
			Function(Arguments...);
	}

	void UnbindAll()
	{
		Functions.clear();
	}

	SecureMulticastDelegate(const SecureMulticastDelegate&) = delete;
	SecureMulticastDelegate(SecureMulticastDelegate&&) = delete;
	SecureMulticastDelegate& operator=(const SecureMulticastDelegate&) = delete;
	SecureMulticastDelegate& operator=(SecureMulticastDelegate&&) = delete;

	std::vector<std::function<void(Args...)>> Functions;

};

template<typename ...Args>
class MulticastDelegate final
{
public:

	MulticastDelegate() = default;
	
	void Bind(std::function<void(Args...)> Funct)
	{
		Functions.push_back(Funct);
	}

	void Execute(Args... Arguments) const
	{
		for (auto& Function : Functions)
			Function(Arguments...);
	}

	void Unbind(std::function<void(Args...)> Function)
	{
		const size_t Size = Functions.size();
		for (const size_t i = 0; i < Size; ++i)
			if (Functions[i] == Function)
				Functions.erase(Functions.begin() + i);
	}

	void UnbindAll()
	{
		Functions.clear();
	}


private:

	MulticastDelegate(const MulticastDelegate&) = delete;
	MulticastDelegate(MulticastDelegate&&) = delete;
	MulticastDelegate& operator=(const MulticastDelegate&) = delete;
	MulticastDelegate& operator=(MulticastDelegate&&) = delete;

	std::vector<std::function<void(Args...)>> Functions;

};

template<class Class, typename ReturnType, typename ...Args>
class SecureDelegate final
{
	class Class;
public:

	SecureDelegate() = default;
	SecureDelegate(std::function<ReturnType(Args...)> Funct) : Function(Funct) {}

	void Bind(std::function<ReturnType(Args...)> Funct)
	{
		if (Function)
			LOG(Warning, "There is already a function bound to this delegate {}", "");
		this->Function = Funct;
	}

	bool inline IsBound() const { return Function; }


private:

	ReturnType Execute(Args... Arguments) const
	{
		return Function(Arguments...);
	}

	void Unbind() { Function = nullptr; }

	friend class Class;

	std::function<ReturnType(Args...)> Function;

	
	SecureDelegate(const SecureDelegate&) = delete;
	SecureDelegate(SecureDelegate&&) = delete;
	SecureDelegate& operator=(const SecureDelegate&) = delete;
	SecureDelegate& operator=(SecureDelegate&&) = delete;

};



template<typename ReturnType, typename ...Args>
class Delegate final
{
public:

	Delegate(std::function<ReturnType(Args...)> Funct) : Function(Funct) {}

	void Bind(std::function<ReturnType(Args...)> Funct)
	{
		if (Function)
			LOG(Warning, "There is already a function bound to this delegate {}", "");
		this->Function = Funct;
	}

	bool inline IsBound() const { return Function; }

	ReturnType Execute(Args... Arguments) const
	{
		return Function(Arguments...);
	}

	void Unbind() { Function = nullptr; }


private:

	std::function<ReturnType(Args...)> Function;

	Delegate() = delete;
	Delegate(const Delegate&) = delete;
	Delegate(Delegate&&) = delete;
	Delegate& operator=(const Delegate&) = delete;
	Delegate& operator=(Delegate&&) = delete;

};