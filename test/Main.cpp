#include <iostream>
#include <memory>

extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}
#include <sol/sol.hpp>

#if _MSC_VER == 1932 || _MSC_VER == 1933
#if _MSC_VER == 1932
#define _MSC_OLD_VER 1932
#endif

#if _MSC_VER == 1933
#define _MSC_OLD_VER 1933
#endif

#undef _MSC_VER
#define _MSC_VER 1919
#include <tao/pegtl/demangle.hpp>
#undef _MSC_VER
#define _MSC_VER _MSC_OLD_VER
#endif

#include <LuaGenny.h>

#include <sdkgenny_parser.hpp>

#pragma pack(push,1)
enum Place { EARTH = 1, MOON = 2, MARS = 3 };

struct Date {
    unsigned short nWeekDay : 3;
    unsigned short nMonthDay : 6;
    unsigned short nMonth : 5;
    unsigned short nYear : 8;
};

struct Foo {
    int a{};
    int b{};
    float c{};
    Place p{};
    int bf1 : 4;
    Place bf2 : 2;
    int rest : 26;
};

struct Bar {
    int d{};
    Foo* foo{};
    int m[4][3];
    union {
        Date date;
        unsigned int date_int{};
    };
};

struct DeltaTest {
    int first{};
    char pad[4]{};  // 4-byte gap
    int second{};
};

struct Thing {
    int abc{};
};

struct RTTITest {
    virtual ~RTTITest(){};
};

struct A {
    virtual ~A() {}
};

struct B : A {
    virtual ~B() {}
};

struct C : B {
    virtual ~C() {}
};

struct D {
    virtual ~D() {}
};

struct E : C, D {
    virtual ~E() {}
};

struct Person {
    int age{20};
};

struct Student : Person {
    float gpa{3.9};
};

struct Faculty : Person {
    int wage{30000};
};

struct TA : Student, Faculty {
    int hours{40};
};

template<typename T>
struct TemplateBox {
    char pad[8]{};
    T* data{};
};

template<typename K, typename V>
struct TemplatePair {
    K key{};
    V value{};
};

template<typename T>
struct TemplateMixed {
    int header{};
    T value{};
    T* ptr{};
    int footer{};
};

template<typename T>
struct TemplateArray {
    T items[4]{};
    int count{};
};

template<typename T>
struct TemplateList {
    T** data{};
    int capacity{};
    int size{};
};

struct Baz : Bar {
    TA ta{};
    int e{};
    int thing{};
    int* f{};
    Foo g{};
    Thing* things{};
    char* hello{};
    wchar_t* wide_hello{};
    char intrusive_hello[32]{"hello, intrusive world!"};
    bool im_true{true};
    bool im_false{false};
    char im_also_true{7};
    TemplateBox<Foo> tpl_box{};
    TemplatePair<int, float> tpl_pair{};
    TemplateMixed<float> tpl_mixed{};
    TemplateArray<int> tpl_arr{};
    TemplateList<Thing> tpl_list{};
    DeltaTest delta_test{};
    __declspec(align(sizeof(void*))) RTTITest* rtti{};
    E* e_ptr{};
};
#pragma pack(pop)

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

struct DeltaTest {
    int first
    int second + 4
}

struct VirtualBase {
    virtual void first_virtual() @ 0
    virtual int second_virtual() @ 1
    int data @ 0x8
}

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

template <typename T>
struct TemplateBox {
    T* data @ 0x8
}

template <typename K, typename V>
struct TemplatePair {
    K key
    V value
}

template <typename T>
struct TemplateMixed {
    int header
    T value
    T* ptr
    int footer
}

// Test: template with explicit size + T by value (trailing padding test)
template <typename T>
struct TemplateSized 0x20 {
    int header
    T value
}

template <typename T>
struct TemplateArray {
    T[4] items
    int count
}

template <typename T>
struct TemplateList {
    T** data
    int capacity
    int size
}

// Test: + delta in template (Copilot comment 2)
template <typename T>
struct TemplateDelta {
    int header
    T value + 4
}

// Test: + delta after template param (pathological case)
template <typename T>
struct TemplateDeltaAfterT {
    T header
    int value + 4
}

// Test: bitfields in template struct (Copilot comment 1)
template <typename T>
struct TemplateBitfield {
    T flags
    int bf_a : 4
    int bf_b : 4
    int after_bf
}

// Test: template class (Copilot comment 3)
template <typename T>
class TemplateClassBox {
    T* data @ 0x8
}

// Test: template with parent (Copilot comment 4)
template <typename T>
struct TemplateChild : Foo {
    T extra
}

// Test: template with parent + explicit size + multiple fields
template <typename T>
struct TemplateChildComplex : Bar {
    T value
    T* ptr
    int footer
}

// Test: template with virtual function (vtable pointer)
template <typename T>
struct TemplateVirtual {
    virtual void dummy() @ 0
    T data @ 0x8
}

// Test: template with parent AND virtual function (double-count vtable test)
template <typename T>
struct TemplateVirtualChild : VirtualBase {
    T extra
}

// Test: template with parent AND its OWN virtual function (double-count vtable test)
template <typename T>
struct TemplateVirtualChild2 : VirtualBase {
    virtual int* child_virtual() @ 2
    T extra @ 0x10
}


struct TemplateUser {
    TemplateBox<Foo> box
    TemplatePair<int, float> pair
    TemplateMixed<float> mixed
    TemplateArray<int> arr
    TemplateBox<int> box_int
    TemplatePair<float, int> pair_swapped
    TemplateList<Foo> entity_list
}

// Test: instantiated template as pointer (soft dep / forward decl test)
struct TemplatePtrUser {
    TemplateBox<Foo>* box_ptr
    TemplatePair<int, float>* pair_ptr
    int value
}

struct Baz : Bar 0x100 {
	TA ta
    int e
    int thing
    int* f
    Foo g
    Thing* things
    char* hello [[utf8*]]
    wchar_t* wide_hello [[utf16*]]
    char[32] intrusive_hello [[utf8*]]
    bool im_true
    bool im_false
    bool im_also_true
    TemplateBox<Foo> tpl_box
    TemplatePair<int, float> tpl_pair
    TemplateMixed<float> tpl_mixed
    TemplateArray<int> tpl_arr
    TemplateList<Thing> tpl_list
    DeltaTest delta_test
	//RTTITest* test + 5
}

struct RTTITest 0x100 {
	uintptr_t** vtable
}
)";

std::unique_ptr<sdkgenny::Sdk> parse_gennyfile() {
    auto sdk = std::make_unique<sdkgenny::Sdk>();

    sdkgenny::parser::State s{};
    s.parents.push_back(sdk->global_ns());

    tao::pegtl::memory_input in{gennyfile, "text"};

    try {
        if (tao::pegtl::parse<sdkgenny::parser::Grammar, sdkgenny::parser::Action>(in, s)) {
        }
    } catch (const std::exception& e) {
        std::cout << "Error: " << e.what() << std::endl;
    } catch (...) {
        std::cout << "unknown error" << std::endl;
    }
    
    return sdk;
}

int main(int argc, char* argv[]) {
    bool performing_tests = false;
    std::string script_path{};

    if (argc >= 2) {
        performing_tests = std::string{argv[1]} == "--test";
        if (performing_tests) {
            if (argc < 3) {
                std::cerr << "Usage: " << argv[0] << " --test <script.lua>" << std::endl;
                return 1;
            }
            script_path = argv[2];
        }
    }

    // Create a Lua state
    sol::state lua{};

    lua.open_libraries(sol::lib::base, sol::lib::package, sol::lib::string, sol::lib::math, sol::lib::table, sol::lib::bit32,
    sol::lib::utf8, sol::lib::os, sol::lib::coroutine);

    // Add the sdkgenny bindings
    luaopen_luagenny(lua);
    sol::table sdkgenny = sol::stack::pop<sol::table>(lua);
    lua["sdkgenny"] = sdkgenny;

    auto sdk = parse_gennyfile();
    lua["parsed"] = sdk.get();

    auto foo = new Foo{};
    foo->a = 42;
    foo->b = 1337;
    foo->c = 77.7f;
    foo->p = Place::MARS;
    foo->bf1 = Place::MOON;
    foo->bf2 = Place::MARS;
    foo->rest = 12345678;

    auto baz = new Baz{};
    baz->d = 123;
    baz->foo = foo;
    for (auto i = 0; i < 4; ++i) {
        for (auto j = 0; j < 3; ++j) {
            baz->m[i][j] = i + j;
        }
    }
    baz->date.nWeekDay = 1;
    baz->date.nMonthDay = 2;
    baz->date.nMonth = 3;
    baz->date.nYear = 4;
    baz->e = 666;
    baz->f = new int[10];
    for (auto i = 0; i < 10; ++i) {
        baz->f[i] = i;
    }
    baz->g = *foo;
    ++baz->g.a;
    ++baz->g.b;
    ++baz->g.c;
    baz->g.p = Place::MOON;
    baz->things = new Thing[10];
    for (auto i = 0; i < 10; ++i) {
        baz->things[i].abc = i * 2;
    }
    baz->hello = (char*)"Hello, world!";
    baz->wide_hello = (wchar_t*)L"Hello, wide world!";

    auto rtti = new RTTITest{};
    baz->rtti = rtti;
    baz->e_ptr = new E{};
    baz->tpl_box.data = foo;
    baz->tpl_pair.key = 99;
    baz->tpl_pair.value = 2.718f;
    baz->tpl_mixed.header = 0xAA;
    baz->tpl_mixed.value = 6.28f;
    baz->tpl_mixed.ptr = &baz->tpl_mixed.value;
    baz->tpl_mixed.footer = 0xBB;
    for (int i = 0; i < 4; ++i) baz->tpl_arr.items[i] = (i + 1) * 10;
    baz->tpl_arr.count = 4;
    // Populate TemplateList<Thing> with Thing* entries
    static Thing* thing_ptrs[5];
    static Thing thing_instances[5];
    for (int i = 0; i < 5; ++i) {
        thing_instances[i].abc = (i + 1) * 100;
        thing_ptrs[i] = &thing_instances[i];
    }
    baz->tpl_list.data = thing_ptrs;
    baz->tpl_list.capacity = 5;
    baz->tpl_list.size = 5;
    baz->delta_test.first = 111;
    baz->delta_test.second = 222;

    lua["bazaddr"] = (uintptr_t)baz;

    std::cout << "0x" << std::hex << (uintptr_t)baz << std::endl;
    int result = 1;

    if (!performing_tests) {
        std::string input{};

        while (true) {
            std::cout << "> ";
            std::getline(std::cin, input);

            const auto command = input.c_str();

            if (input == "quit") {
                break;
            } else {
                auto result = luaL_loadbuffer(lua, command, strlen(command), nullptr);

                if (result == LUA_OK) {
                    result = lua_pcall(lua, 0, LUA_MULTRET, 0);

                    if (result != LUA_OK) {
                        std::cout << "ERROR: " << luaL_checkstring(lua, -1) << std::endl;
                    }
                } else {
                    std::cout << "ERROR: " << luaL_checkstring(lua, -1) << std::endl;
                }
            }
        }
    } else {
        try {
            result = lua.script_file(script_path).get<bool>() ? 0 : 1;
        } catch (const std::exception& e) {
            std::cout << "Error: " << e.what() << std::endl;
        } catch (...) {
            std::cout << "unknown error" << std::endl;
        }

        if (result == 0) {
            std::cout << "Test passed!" << std::endl;
        } else {
            std::cout << "Test failed!" << std::endl;
        }
    }

    return result;
}