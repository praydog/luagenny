// Validation test for generated template headers.
// This file includes the generated .hpp files and verifies they compile
// correctly with the expected memory layouts via static_assert and runtime checks.
//
// This file is compiled AFTER the Lua test generates the headers.

#include <cstddef>
#include <iostream>
#include <cstdint>

// The genny schema uses "ushort" which isn't standard C++
using ushort = uint16_t;

// Include generated headers
#include "TemplateBox.hpp"
#include "TemplatePair.hpp"
#include "TemplateMixed.hpp"
#include "TemplateArray.hpp"
#include "TemplateList.hpp"
#include "TemplateBitfield.hpp"
#include "Foo.hpp"
#include "Bar.hpp"
#include "TemplateChild.hpp"
#include "TemplateChildComplex.hpp"
#include "TemplateVirtual.hpp"
#include "TemplateUser.hpp"
#include "VirtualBase.hpp"
#include "TemplatePtrUser.hpp"


// Use typedefs to avoid comma-in-macro issues with offsetof
using BoxInt = TemplateBox<int>;
using BoxFoo = TemplateBox<Foo>;
using PairIntFloat = TemplatePair<int, float>;
using PairFloatInt = TemplatePair<float, int>;
using MixedFloat = TemplateMixed<float>;
using MixedInt = TemplateMixed<int>;
using ArrInt = TemplateArray<int>;
using ListFoo = TemplateList<Foo>;
using BfInt = TemplateBitfield<int>;
using ChildInt = TemplateChild<int>;
using ChildCplxInt = TemplateChildComplex<int>;
using VirtInt = TemplateVirtual<int>;

// Size assertions for template instantiations
static_assert(sizeof(BoxInt) == 8 + sizeof(void*), "TemplateBox<int> size");
static_assert(sizeof(BoxFoo) == 8 + sizeof(void*), "TemplateBox<Foo> size");

static_assert(sizeof(PairIntFloat) == 8, "TemplatePair<int,float> size");
static_assert(sizeof(PairFloatInt) == 8, "TemplatePair<float,int> size");

static_assert(sizeof(MixedFloat) == 12 + sizeof(void*), "TemplateMixed<float> size");
static_assert(sizeof(MixedInt) == 12 + sizeof(void*), "TemplateMixed<int> size");

static_assert(sizeof(ArrInt) == 20, "TemplateArray<int> size");

// Offset assertions for TemplateBox (explicit @ 0x8 padding)
static_assert(offsetof(BoxInt, data) == 0x8, "TemplateBox.data offset");
static_assert(offsetof(BoxFoo, data) == 0x8, "TemplateBox<Foo>.data offset");

// Offset assertions for TemplatePair (sequential layout)
static_assert(offsetof(PairIntFloat, key) == 0, "TemplatePair.key offset");
static_assert(offsetof(PairIntFloat, value) == 4, "TemplatePair.value offset");

// Offset assertions for TemplateMixed (mixed template + concrete fields)
static_assert(offsetof(MixedFloat, header) == 0, "TemplateMixed.header offset");
static_assert(offsetof(MixedFloat, value) == 4, "TemplateMixed.value offset");
static_assert(offsetof(MixedFloat, ptr) == 8, "TemplateMixed.ptr offset");
static_assert(offsetof(MixedFloat, footer) == 8 + sizeof(void*), "TemplateMixed.footer offset");

// Offset assertions for TemplateArray
static_assert(offsetof(ArrInt, items) == 0, "TemplateArray.items offset");
static_assert(offsetof(ArrInt, count) == 16, "TemplateArray.count offset");

// Offset assertions for TemplateList (T** container pattern)
static_assert(offsetof(ListFoo, data) == 0, "TemplateList.data offset");
static_assert(offsetof(ListFoo, capacity) == sizeof(void*), "TemplateList.capacity offset");
static_assert(offsetof(ListFoo, size) == sizeof(void*) + 4, "TemplateList.size offset");
static_assert(sizeof(ListFoo) == sizeof(void*) + 8, "TemplateList<Foo> size");

// TemplateBitfield: bitfields should share storage unit
static_assert(sizeof(BfInt) == 12, "TemplateBitfield<int> size: int(4) + bitfield unit(4) + int(4)");
static_assert(offsetof(BfInt, flags) == 0, "TemplateBitfield.flags offset");
static_assert(offsetof(BfInt, after_bf) == 8, "TemplateBitfield.after_bf offset");

// TemplateChild: inherits from Foo, T field should be after Foo's layout
static_assert(sizeof(ChildInt) == sizeof(Foo) + sizeof(int), "TemplateChild<int> size");
static_assert(offsetof(ChildInt, extra) == sizeof(Foo), "TemplateChild<int>.extra after Foo");

// TemplateChildComplex: inherits from Bar, has T value + T* ptr + int footer
static_assert(offsetof(ChildCplxInt, value) == sizeof(Bar), "ChildComplex.value after Bar");
static_assert(offsetof(ChildCplxInt, ptr) == sizeof(Bar) + 4, "ChildComplex.ptr after value");
static_assert(offsetof(ChildCplxInt, footer) == sizeof(Bar) + 4 + sizeof(void*), "ChildComplex.footer after ptr");

// TemplateVirtual: has vtable pointer + T data @ 0x8
static_assert(offsetof(VirtInt, data) == 0x8, "TemplateVirtual.data @ 0x8");
static_assert(sizeof(VirtInt) >= 0xC, "TemplateVirtual size includes vtable + data");

// Non-template VirtualBase: vtable pointer + data @ 0x8
static_assert(offsetof(VirtualBase, data) == 0x8, "VirtualBase.data @ 0x8");
static_assert(sizeof(VirtualBase) >= 0xC, "VirtualBase size includes vtable + data");

int main() {
    int failures = 0;

    auto check = [&](bool cond, const char* desc) {
        if (!cond) {
            std::cerr << "FAIL: " << desc << std::endl;
            ++failures;
        }
    };

    // Runtime value tests: TemplateBox
    {
        BoxInt box{};
        int val = 42;
        box.data = &val;
        check(box.data != nullptr, "TemplateBox<int>.data set");
        check(*box.data == 42, "TemplateBox<int>.data deref");
    }

    // Runtime value tests: TemplatePair
    {
        PairIntFloat pair{};
        pair.key = 99;
        pair.value = 3.14f;
        check(pair.key == 99, "TemplatePair.key value");
        check(pair.value > 3.13f && pair.value < 3.15f, "TemplatePair.value value");
    }

    // Runtime value tests: TemplateMixed
    {
        MixedFloat mixed{};
        mixed.header = 0xAA;
        mixed.value = 6.28f;
        mixed.ptr = &mixed.value;
        mixed.footer = 0xBB;
        check(mixed.header == 0xAA, "TemplateMixed.header value");
        check(*mixed.ptr == mixed.value, "TemplateMixed.ptr deref");
        check(mixed.footer == 0xBB, "TemplateMixed.footer value");
    }

    // Runtime value tests: TemplateArray
    {
        ArrInt arr{};
        for (int i = 0; i < 4; ++i) arr.items[i] = (i + 1) * 10;
        arr.count = 4;
        check(arr.items[0] == 10, "TemplateArray.items[0]");
        check(arr.items[3] == 40, "TemplateArray.items[3]");
        check(arr.count == 4, "TemplateArray.count");
    }

    // TemplateUser (composite struct using multiple template instantiations)
    {
        TemplateUser user{};
        Foo foo{};
        foo.a = 123;
        user.box.data = &foo;
        user.pair.key = 1;
        user.pair.value = 2.0f;
        user.mixed.header = 0xFF;
        user.arr.count = 3;
        user.box_int.data = &user.pair.key;
        user.pair_swapped.key = 9.9f;
        user.pair_swapped.value = 7;

        check(user.box.data->a == 123, "TemplateUser.box.data->a");
        check(user.pair.key == 1, "TemplateUser.pair.key");
        check(user.mixed.header == 0xFF, "TemplateUser.mixed.header");
        check(user.arr.count == 3, "TemplateUser.arr.count");
        check(*user.box_int.data == 1, "TemplateUser.box_int.data deref");
        check(user.pair_swapped.value == 7, "TemplateUser.pair_swapped.value");
    }

    if (failures == 0) {
        std::cout << "All codegen tests passed!" << std::endl;
    } else {
        std::cerr << failures << " codegen test(s) failed!" << std::endl;
    }

    return failures;
}
