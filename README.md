# LightReflection
轻量级反射库

可在类定义外部注册反射信息，所以支持第三方类的反射。

当前支持类构造，成员变量反射，成员函数反射，支持overload和override。

REF_CREATOR(params_count, class_type) 注册class的构造器，需要指定构造函数参数个数。

REF_VAR(class_type, VAR_ADDR(class_type, var1), VAR_ADDR(class_type, var2)) 注册成员变量，无需指定变量类型。

REF_METHOD(AA,
RefStruct(method_name, METHOD_ADDR(class_type, func, return_type)),
RefStruct(method_name1, METHOD_ADDR(class_type, func, return_type, args...))
) 注册成员函数，需要手动指定函数名（为了实现overload，所以不自动生成），注册时需要指定返回类型和参数类型，如果没有参数则不用指定。

具体用法可看main.cpp。
