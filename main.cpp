#include "Reflection.h"

class AA
{
public:
	AA(){ printf("AA\n"); }
	AA(int i){ printf("AA %d\n", i); }
	~AA() { printf("delete %d %d\n", x, y); }

	int x = 0;
	int y = 0;
	void func() { printf("AA1 %d %d\n", x, y); }
	void func(int i){ printf("AA2 %d %d %d\n", x, y, i); }
};

REF_CREATOR(0, AA)
REF_CREATOR(1, AA, int)
REF_VAR(AA, VAR_ADDR(AA, x), VAR_ADDR(AA, y))
REF_METHOD(AA,
RefStruct("func", METHOD_ADDR(AA, func, void)),
RefStruct("func1", METHOD_ADDR(AA, func, void, int))
)

int main()
{
	Reflection* ref = Reflection::CreateInstance();
	void* a1 = ref->CreateClass("AA");
	void* a2 = ref->CreateClass("AA", 100);

	ref->SetVar("AA", "x", unsigned int(a1), 123);
	ref->SetVar("AA", "y", unsigned int(a1), 456);
	ref->Invoke<void>("AA", "func", unsigned int(a1), true);
	ref->Invoke<void, int>("AA", "func1", unsigned int(a1), true, 200);
	int y = ref->GetVar<int>("AA", "y", unsigned int(a1));
	printf("y %d\n", y);

	ref->ReleaseObject("AA", unsigned int(a1));
	ref->ReleaseObject("AA", unsigned int(a2));

	Reflection::Release();

	return 0;
}



/*#include <iostream>
using namespace std;
template <typename T>
static void* Destruct()//�õ�T���������ĵ�ַ������
{
	T *p;
	goto getDesAddr;
desAddr:
	p->~T();
#ifdef _WIN32 //_MSC_VER //intel��ʽ���,windows ƽ̨
#ifdef _MSC_VER
	__asm{
		ret
		getDesAddr :
		push eax
			mov eax, desAddr //save the address of T::~T()
			mov p, eax
			pop eax
	}
#endif
#endif
	return (p);
}

typedef void(*Fndes)();
static void executeDestruct(void *addr)//ִ��addrָ�����������
{
	Fndes exe = reinterpret_cast<Fndes>(addr);
	exe();
}


class A{
public:
	~A(){
		cout << "~A" << endl;
	}
};
void main()
{
	void*p = Destruct<A>();
	executeDestruct(p);
}*/