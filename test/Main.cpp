#include <iostream>
#include <memory>

extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}
#include <sol/sol.hpp>

#include <LuaGenny.hpp>

#include "GennyParser.hpp"

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

std::unique_ptr<genny::Sdk> parse_gennyfile() {
    auto sdk = std::make_unique<genny::Sdk>();

    genny::parser::State s{};
    s.parents.push_back(sdk->global_ns());

    tao::pegtl::memory_input in{gennyfile, "text"};

    try {
        if (tao::pegtl::parse<genny::parser::Grammar, genny::parser::Action>(in, s)) {
        }
    } catch (const std::exception& e) {
        std::cout << "Error: " << e.what() << std::endl;
    } catch (...) {
        std::cout << "unknown error" << std::endl;
    }
    
    return sdk;
}

int main() {
    // Create a Lua state
    sol::state lua{};

    lua.open_libraries(sol::lib::base, sol::lib::package, sol::lib::string, sol::lib::math, sol::lib::table, sol::lib::bit32,
    sol::lib::utf8, sol::lib::os, sol::lib::coroutine);

    // Add the sdkgenny bindings
    sdkgennylua::open(lua);
    sol::table sdkgenny = sol::stack::pop<sol::table>(lua);
    lua["sdkgenny"] = sdkgenny;

    auto sdk = parse_gennyfile();

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

    lua["parsed"] = sdk.get();

    std::cout << "0x" << std::hex << (uintptr_t)baz << std::endl;
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

				if (result != LUA_OK)
				{
					std::cout << "ERROR: " << luaL_checkstring(lua, -1) << std::endl;
				}
			} else {
				std::cout << "ERROR: " << luaL_checkstring(lua, -1) << std::endl;
			}
		}
	}

    std::cout << "Press ENTER to exit.";
    std::cin.get();

    return 0;
}