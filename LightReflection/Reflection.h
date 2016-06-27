#ifndef _REFLECTION_
#define _REFLECTION_

#include <iostream>
#include <tuple>
#include <unordered_map>
#include "MemberReflection.h"
#include "TypeReflection.h"

class Reflection
{
private:
	static Reflection* instance;
	// 变量/函数名，地址
	typedef std::unordered_map<std::string, unsigned int> AddrMap;
	typedef std::unordered_map<int, unsigned int> CreatorMap;
	
	struct RefData { 
		CreatorMap creator;
		AddrMap memberVar;
		AddrMap memberMethod;
	};
	// 类名，creator地址，成员变量地址，成员函数地址
	typedef std::unordered_map<std::string, RefData> ClassMap;
	ClassMap classMap;

private:
	Reflection() = default;
	~Reflection(){}

public:
	static Reflection* CreateInstance()
	{
		if (!instance)
			instance = new Reflection();
		return instance;
	}
	static void Release()
	{
		if (!instance)
			delete instance;
	}

	void RegisterConstructor(const char* className, unsigned int addr, int paramsCount)
	{
		if (classMap.count(className) == 0)
		{
			CreatorMap cm;
			cm[paramsCount] = addr;
			AddrMap m1, m2;
			RefData data;
			data.creator = std::move(cm);
			data.memberVar = std::move(m1);
			data.memberMethod = std::move(m2);
			classMap[className] = std::move(data);
		}
		else
		{
			RefData& data = classMap[className];
			CreatorMap& cm = data.creator;
			cm[paramsCount] = addr;
		}
	}

	void RegisterMemberVariable(const char* className, const char* memberName, unsigned int addr)
	{
		if (classMap.count(className) != 0)
		{
			classMap[className].memberVar.insert(std::pair<std::string, unsigned int>(memberName, addr));
		}
	}

	void RegisterMemberMethod(const char* className, const char* memberName, unsigned int addr)
	{
		if (classMap.count(className) != 0)
		{
			classMap[className].memberMethod.insert(std::pair<std::string, unsigned int>(memberName, addr));
		}
	}

	unsigned int GetCreatorAddr(const char* className, int paramsCount)
	{
		if (classMap.count(className) == 0)
			return 0;
		CreatorMap& cm = classMap[className].creator;
		if (cm.count(paramsCount) != 0)
			return cm[paramsCount];
		return 0;
	}

	template<class... Args>
	void* CreateClassByAddr(unsigned int addr, Args... args)
	{
		return (*(void*(*)(Args...))addr)(args...);
	}

	template<class... Args>
	void* CreateClass(const char* className, Args... args)
	{
		if (classMap.count(className) == 0)
			return NULL;
		
		unsigned int addr = GetCreatorAddr(className, sizeof...(args));
		return CreateClassByAddr(addr, args...);
	}

	unsigned int GetVarAddr(const char* className, const char* varName)
	{
		if (classMap.count(className) == 0)
			return 0;

		AddrMap& addrMap = classMap[className].memberVar;
		if (addrMap.count(varName))
			return addrMap[varName];

		return 0;
	}

	template<class Type>
	void SetVarByAddr(unsigned int varAddr, unsigned objectAddr, Type value)
	{
		if (objectAddr != 0)
		{
			*((unsigned int*)(objectAddr + varAddr)) = *(unsigned int*)&value;
		}
	}

	template<class Type>
	void SetVar(const char* className, const char* varName, unsigned int objectAddr, Type value)
	{
		unsigned int addr = GetVarAddr(className, varName);
		SetVarByAddr(addr, objectAddr, value);
	}

	template<class RV>
	RV GetVarByAddr(unsigned int varAddr, unsigned int objectAddr)
	{		
		return (RV)(*((unsigned int*)(objectAddr + varAddr)));
	}

	template<class RV>
	RV GetVar(const char* className, const char* varName, unsigned int objectAddr)
	{
		unsigned int addr = GetVarAddr(className, varName);
		return GetVarByAddr<RV>(addr, objectAddr);
	}

	unsigned int GetMethodAddr(const char* className, const char* methodName)
	{
		if (classMap.count(className) == 0)
			return 0;

		AddrMap& addrMap = classMap[className].memberMethod;
		if (addrMap.count(methodName))
			return addrMap[methodName];

		return 0;
	}

	template<class RV, class... Args>
	RV InvokeByAddr(unsigned int methodAddr, unsigned int objectAddr, bool is_stdcall, Args... args)
	{
		if (objectAddr)
		{
			__asm {
				mov ecx, objectAddr
			}
		}

		if (is_stdcall)
			return (*(RV(__stdcall *)(Args...))methodAddr)(args...);
		return (*(RV(*)(Args...))methodAddr)(args...);
	}

	template <class RV, class... Args>
	RV Invoke(const char* className, const char* methodName, unsigned int objectAddr, bool is_stdcall, Args... args)
	{
		unsigned int addr = GetMethodAddr(className, methodName);
		return InvokeByAddr<RV, Args...>(addr, objectAddr, is_stdcall, args...);
	}
};
Reflection* Reflection::instance = NULL;

#endif