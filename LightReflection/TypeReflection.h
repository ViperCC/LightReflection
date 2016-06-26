#ifndef _TYPE_REFLECTION_
#define _TYPE_REFLECTION_

template<class... Args>
using CREATOR_FUNC = void*(*)(Args...);

template<class... Args>
union CreatorUnion
{
	unsigned int addr;
	void*(*func)(Args...);
};

#define REF_CREATOR(params_count, class_type, ...) \
	template<class... Args> \
	void* Create##class_type##params_count(Args... args) \
	{ \
		return new class_type(args...); \
	} \
	CreatorUnion<__VA_ARGS__> class_type##params_count##union; \
	class Create##class_type##params_count##ForFuncAddr { \
		public: Create##class_type##params_count##ForFuncAddr() {  \
					class_type##params_count##union.func = Create##class_type##params_count; \
					Reflection::CreateInstance()->RegisterConstructor(#class_type, class_type##params_count##union.addr, params_count); \
		} \
	}; \
	Create##class_type##params_count##ForFuncAddr* Create##class_type##params_count##ForFuncAddr##Object = new Create##class_type##params_count##ForFuncAddr();

#endif