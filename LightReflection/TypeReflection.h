#ifndef _TYPE_REFLECTION_
#define _TYPE_REFLECTION_

template<class... Args>
using CREATOR_FUNC = void*(*)(Args...);

template<class T, class... Args>
union FuncAddrUnion
{
	unsigned int addr;
	T(*func)(Args...);
};

template<class T, class... Args>
unsigned int FuncToAddr(T(*func)(Args...))
{
	FuncAddrUnion<T, Args...> fau;
	fau.func = func;
	return fau.addr;
}

template<class T>
void ClassReleaseFunc(void* p)
{
	delete (T*)p;
}

#define REF_CREATOR(params_count, class_type, ...) \
	template<class... Args> \
	void* Create##class_type##params_count(Args... args) \
	{ \
		return new class_type(args...); \
	} \
	class Create##class_type##params_count##ForFuncAddr { \
		public: Create##class_type##params_count##ForFuncAddr() {  \
					Reflection* ref = Reflection::CreateInstance(); \
					ref->RegisterConstructor(#class_type, FuncToAddr<void*, __VA_ARGS__>(Create##class_type##params_count), params_count); \
					ref->RegisterDestructor(#class_type, FuncToAddr<void, void*>(ClassReleaseFunc<class_type>)); \
		} \
	}; \
	Create##class_type##params_count##ForFuncAddr* Create##class_type##params_count##ForFuncAddr##Object = new Create##class_type##params_count##ForFuncAddr();

#endif