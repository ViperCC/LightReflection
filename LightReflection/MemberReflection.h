#ifndef _MEMBER_REFLECTION_
#define _MEMBER_REFLECTION_

#include <string.h>

struct RefStruct
{
	const char* name;
	unsigned int addr;
	RefStruct(const char* _name, unsigned int _addr) : name(_name), addr(_addr){}
};

#define VAR_ADDR(class_type, var) \
	RefStruct(#var, unsigned int(&((class_type*)0)->var))	

#define REF_VAR(class_type, ...) \
	RefStruct class_type##memberVarArray[] = { __VA_ARGS__, RefStruct("\0", 0) }; \
	class RefVar##class_type##ForVarAddr {  \
		public: RefVar##class_type##ForVarAddr() { \
			RefStruct* p = class_type##memberVarArray; \
			for (p; strcmp(p->name, "\0"); p++) {\
				Reflection::CreateInstance()->RegisterMemberVariable(#class_type, p->name, p->addr); }\
		}\
	}; \
	RefVar##class_type##ForVarAddr* RefVar##class_type##ForVarAddrObject = new RefVar##class_type##ForVarAddr();


template<class FUNC>
union FUNC_ADDR
{
	unsigned int addr;
	FUNC func;
};

template<class ClassType, class RV, class... Args>
unsigned int GetFuncAddr(RV(ClassType::* f)(Args...))
{
	FUNC_ADDR<RV(ClassType::*)(Args...)> addr;
	addr.func = f;
	return addr.addr;
}

template<class ClassType, class RV>
unsigned int GetFuncAddr(RV(ClassType::* f)())
{
	FUNC_ADDR<RV(ClassType::*)()> addr;
	addr.func = f;
	return addr.addr;
}

template<class RV, class... Args>
unsigned int GetFuncAddr(RV(* f)(Args...))
{
	FUNC_ADDR<RV(*)(Args...)> addr;
	addr.func = f;
	return addr.addr;
}

template<class RV>
unsigned int GetFuncAddr(RV(* f)())
{
	FUNC_ADDR<RV(*)()> addr;
	addr.func = f;
	return addr.addr;
}

#define METHOD_ADDR(class_type, method, rv, ...) \
	GetFuncAddr<class_type, rv, __VA_ARGS__>(&class_type::method)

#define REF_METHOD(class_type, ...) \
	RefStruct class_type##memberMethodArray[] = { __VA_ARGS__, RefStruct("\0", 0) }; \
	class RefMethod##class_type##ForMethodAddr {  \
		public: RefMethod##class_type##ForMethodAddr() { \
			RefStruct* p = class_type##memberMethodArray; \
			for (p; strcmp(p->name, "\0"); p++) {\
				Reflection::CreateInstance()->RegisterMemberMethod(#class_type, p->name, p->addr); }\
		}\
	}; \
	RefMethod##class_type##ForMethodAddr* RefMethod##class_type##ForMethodAddrObject = new RefMethod##class_type##ForMethodAddr();

#endif