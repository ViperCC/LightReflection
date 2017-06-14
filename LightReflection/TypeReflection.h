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

template<class T>
union DestructorUnion
{
	unsigned int addr;
	void(*func)(void*);
};

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
	CreatorUnion<__VA_ARGS__> class_type##params_count##union; \
	class Create##class_type##params_count##ForFuncAddr { \
		public: Create##class_type##params_count##ForFuncAddr() {  \
					class_type##params_count##union.func = Create##class_type##params_count; \
					Reflection* ref = Reflection::CreateInstance(); \
					ref->RegisterConstructor(#class_type, class_type##params_count##union.addr, params_count); \
					DestructorUnion<class_type> du; \
					du.func = ClassReleaseFunc<class_type>; \
					ref->RegisterDestructor(#class_type, du.addr); \
		} \
	}; \
	Create##class_type##params_count##ForFuncAddr* Create##class_type##params_count##ForFuncAddr##Object = new Create##class_type##params_count##ForFuncAddr();

#endif