# sdkgenny-lua
Lua bindings for [sdkgenny](https://github.com/cursey/sdkgenny)

## Projects using this
* [regenny](https://github.com/cursey/regenny) - A reverse engineering tool to interactively reconstruct structures and generate header files 
	* Can REPL or run scripts using the Lua API to perform convenient/automated analysis on structures
	* See the initial [Pull Request](https://github.com/cursey/regenny/pull/10) for examples

## Examples
```
PS F:\Programming\projects\sdkgenny-lua\build\Debug> .\test.exe
0x223aa80d010
> baz = sdkgenny.StructOverlay(0x223aa80d010, parsed:global_ns():struct("Baz")) 
> print(baz.things:address())
2352207548688
> print(baz.things[0])
sol.luagenny::api::StructOverlay: 00000223AA7D8338
> print(baz.things[0].abc)
0
> print(baz.things[1].abc)
2
> print(baz.things[2].abc)
4
> print(baz.hello)
Hello, world!
> print(baz.im_true)
true
> print(baz.im_false)
false
> print(baz.im_also_true)
true
> print(baz.ta.hours)
40
```

gennyfile used
```cpp
constexpr auto gennyfile = R"(
type int 4 [[i32]]
type float 4 [[f32]]
type ushort 2 [[u16]]
type str 8 [[utf8*]]
type wstr 8 [[utf16*]]
type bool 1 [[bool]]
type uintptr_t 8 [[u64]]
type char 1
type wchar_t 2

struct RTTITest{}

enum Place {
    EARTH = 1,
    MOON = 2,
    MARS = 3,
}

struct Date {
    ushort nWeekDay : 3
    ushort nMonthDay : 6
    ushort nMonth : 5
    ushort nYear : 8
}

struct Foo {
    int a
    int b
    float c
    Place p
    Place bf1 : 4
    Place bf2 : 2
}

struct Bar {
    int d
    Foo* foo
    int[4][3] m
    Date date
}

struct Thing {
    int abc
}

struct Person {
    int age
}

struct Student : Person {
    float gpa
}

struct Faculty : Person {
    int wage
}

struct TA : Student, Faculty {
    int hours
}

struct Baz : Bar 0x100 {
	TA ta
    int e
    int thing
    int* f
    Foo g
    Thing* things
    char* hello [[utf8*]]
    //wchar_t* wide_hello [[utf16*]]
    //char[32] intrusive_hello [[utf8*]]
    bool im_true +40
    bool im_false
    bool im_also_true
	//RTTITest* test + 5
}

struct RTTITest 0x100 {
	uintptr_t** vtable
}
)";
```
