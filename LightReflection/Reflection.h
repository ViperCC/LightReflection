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
	typedef std::unordered_map<int, unsigned int> NumMap;
	typedef std::unordered_map<std::string, NumMap> MethodMap;
	
	struct RefData { 
		NumMap creator;
		unsigned int destructor;
		AddrMap memberVar;
		MethodMap memberMethod;
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
			NumMap cm;
			cm[paramsCount] = addr;
			AddrMap m1;
			MethodMap m2;
			RefData data;
			data.creator = std::move(cm);
			data.memberVar = std::move(m1);
			data.memberMethod = std::move(m2);
			classMap[className] = std::move(data);
		}
		else
		{
			RefData& data = classMap[className];
			NumMap& cm = data.creator;
			cm[paramsCount] = addr;
		}
	}

	void RegisterDestructor(const char* className, unsigned int addr)
	{
		RefData& data = classMap[className];
		unsigned int dm = addr;
		data.destructor = dm;
	}

	void RegisterMemberVariable(const char* className, const char* memberName, unsigned int addr)
	{
		if (classMap.count(className) != 0)
		{
			classMap[className].memberVar.insert(std::pair<std::string, unsigned int>(memberName, addr));
		}
	}

	void RegisterMemberMethod(const char* className, const char* memberName, unsigned int addr, int paramsCount)
	{
		if (classMap.count(className) != 0)
		{
			MethodMap& mm = classMap[className].memberMethod;
			if (mm.count(memberName) != 0)
			{
				NumMap& nm = mm[memberName];
				nm[paramsCount] = addr;
			}
			else
			{
				NumMap nm;
				nm[paramsCount] = addr;
				mm[memberName] = std::move(nm);
			}
		}
	}

	unsigned int GetCreatorAddr(const char* className, int paramsCount)
	{
		if (classMap.count(className) == 0)
			return 0;
		NumMap& cm = classMap[className].creator;
		if (cm.count(paramsCount) != 0)
			return cm[paramsCount];
		return 0;
	}

	template<class... Args>
	void* InvokeByAddr(unsigned int addr, Args... args)
	{
		return (*(void*(*)(Args...))addr)(args...);
	}

	template<class... Args>
	void* CreateClass(const char* className, Args... args)
	{
		if (classMap.count(className) == 0)
			return NULL;
		
		unsigned int addr = GetCreatorAddr(className, sizeof...(args));
		return InvokeByAddr(addr, args...);
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

	unsigned int GetMethodAddr(const char* className, const char* methodName, unsigned int paramsCount)
	{
		if (classMap.count(className) == 0)
			return 0;

		MethodMap& mm = classMap[className].memberMethod;
		if (mm.count(methodName) != 0)
		{
			NumMap& nm = mm[methodName];
			if (nm.count(paramsCount) != 0)
			{
				return nm[paramsCount];
			}
		}

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
		unsigned int addr = GetMethodAddr(className, methodName, sizeof...(args));
		
		return InvokeByAddr<RV, Args...>(addr, objectAddr, is_stdcall, args...);
	}

	void ReleaseObject(const char* className, unsigned int objectAddr)
	{
		if (classMap.count(className) != 0)
		{
			RefData& data = classMap[className];
			InvokeByAddr(data.destructor, objectAddr);
		}
	}
};
Reflection* Reflection::instance = NULL;

#endif