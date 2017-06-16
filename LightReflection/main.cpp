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
	METHOD_ADDR(0, AA, func, void),
	METHOD_ADDR(1, AA, func, void, int)
)

int main()
{
	Reflection* ref = Reflection::CreateInstance();
	void* a1 = ref->CreateClass("AA");
	void* a2 = ref->CreateClass("AA", 100);

	ref->SetVar("AA", "x", unsigned int(a1), 123);
	ref->SetVar("AA", "y", unsigned int(a1), 456);
	ref->Invoke<void>("AA", "func", unsigned int(a1), true);
	ref->Invoke<void>("AA", "func", unsigned int(a1), true, 200);
	int y = ref->GetVar<int>("AA", "y", unsigned int(a1));
	printf("y %d\n", y);

	ref->ReleaseObject("AA", unsigned int(a1));
	ref->ReleaseObject("AA", unsigned int(a2));

	Reflection::Release();

	return 0;
}