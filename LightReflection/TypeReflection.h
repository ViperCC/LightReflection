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

template<class T, class... Args>
T* ClassCreateFunc(Args... args)
{
	return new T(args...);
}

template<class T>
void ClassReleaseFunc(void* p)
{
	delete (T*)p;
}

#define REF_CREATOR(params_count, class_type, ...) \
	template<class... Args> \
	class Create##class_type##params_count##ForFuncAddr { \
		public: Create##class_type##params_count##ForFuncAddr() {  \
					Reflection* ref = Reflection::CreateInstance(); \
					ref->RegisterConstructor(#class_type, FuncToAddr<class_type*, Args...>(ClassCreateFunc<class_type, Args...>), params_count); \
					ref->RegisterDestructor(#class_type, FuncToAddr<void, void*>(ClassReleaseFunc<class_type>)); \
		} \
	}; \
	Create##class_type##params_count##ForFuncAddr<__VA_ARGS__>* Create##class_type##params_count##ForFuncAddr##Object = new Create##class_type##params_count##ForFuncAddr<__VA_ARGS__>();

#endif