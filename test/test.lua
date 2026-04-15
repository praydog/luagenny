local test_start = os.clock()

baz = sdkgenny.StructOverlay(bazaddr, parsed:global_ns():find_struct("Baz"))
print(baz)
print(baz:type())
print(string.format("%x", baz:address()))

local bazstruct = baz:type()
print(bazstruct:name())

local function testcompare(x, name)
    for k, v in pairs(
    {
        "typename",
        "type",
        "generic_type",
        "struct",
        "class",
        "enum",
        "enum_class",
        "namespace",
        "reference",
        "pointer",
        "variable",
        "function",
        "virtual_function",
        "static_function",
        "array",
        "parameter",
        "constant"
    }
    ) do
        print(tostring(name or x) .. " == " .. tostring(v) .. ": " .. tostring(x["is_"..v](x)))
        --print(tostring(name or x) .. " == " .. tostring(v) .. ": " .. tostring(x:is_a(v)))
    end
end

testcompare(bazstruct, "bazstruct")

local test_count = 0

local function value_expect(x, expected, metadata)
    test_count = test_count + 1

    if not metadata then
        metadata = "test " .. tostring(test_count)
    end

    if x ~= expected then
        print("test " .. tostring(test_count) .. " failed " .. "[ " .. metadata .. " ]" .. ", got " .. tostring(x) .. ", expected " .. tostring(expected))
        return false
    end

    print("test " .. tostring(test_count) .. " passed " .. "[ " .. metadata .. " ] == " .. tostring(expected))
    return true
end

function do_tests()
    print("testing...")

    local round = function(val, dec)
        dec = dec or 1
        return math.floor(val * 10 ^ dec + 0.5) / 10 ^ dec
    end

    local results = {
        value_expect(bazstruct:name(), "Baz", "bazstruct:name()"),
        value_expect(bazstruct:is_a("struct"), true, "bazstruct:is_a(\"struct\")"),
        value_expect(bazstruct:is_a("type"), true, "bazstruct:is_a(\"type\")"),
        value_expect(bazstruct:is_struct(), true, "bazstruct:is_struct()"),
        value_expect(bazstruct:is_type(), true, "bazstruct:is_type()"),
        value_expect(baz.d, 123, "baz.d"),
        value_expect(baz.foo.a, 42, "baz.foo.a"),
        value_expect(baz.foo.b, 1337, "baz.foo.b"),
        value_expect(round(baz.foo.c, 1), 77.7, "round(baz.foo.c)"),
        value_expect(baz.g.a, 43, "baz.g.a"),
        value_expect(baz.g.b, 1338, "baz.g.b"),
        value_expect(round(baz.g.c, 1), 78.7, "round(baz.g.c)"),
        value_expect(baz.hello, "Hello, world!", "baz.hello"),
        value_expect(baz.e, 666, "baz.e"),
        value_expect(baz.ta.age, 20, "baz.ta.age"),
        value_expect(round(baz.ta.gpa, 1), 3.9, "baz.ta.gpa"),
        value_expect(baz.ta.wage, 30000, "baz.ta.wage"),
        value_expect(baz.ta.hours, 40, "baz.ta.hours"),
        value_expect(bazstruct:find_variable_in_parents("foo") ~= nil, true, "bazstruct:find_variable(\"foo\") ~= nil"),
        value_expect(bazstruct:find_variable_in_parents("foo"):name(), "foo", "bazstruct:find_variable(\"foo\"):name()"),
        value_expect(bazstruct:find_variable_in_parents("foo"):type():is_pointer(), true, "bazstruct:find_variable(\"foo\"):type():is_pointer()"),
        value_expect(bazstruct:find("variable", "e") ~= nil, true, "bazstruct:find(\"variable\", \"e\") ~= nil"),
        value_expect(bazstruct:find_variable("e") == bazstruct:find("variable", "e"), true, "bazstruct:find_variable(\"e\") == bazstruct:find(\"variable\", \"e\")"),
        value_expect(bazstruct:find_variable("e"):is_a("variable"), true, "bazstruct:find(\"variable\", \"e\"):is_a(\"variable\")"),
        value_expect(bazstruct:find_variable("e"):is_variable(), true, "bazstruct:find(\"variable\", \"e\"):is_variable()"),
        value_expect(bazstruct:find_variable("e"):as("variable") ~= nil, true, "bazstruct:find(\"variable\", \"e\"):as(\"variable\") ~= nil"),
        value_expect(bazstruct:find_variable("e"):as_variable() ~= nil, true, "bazstruct:find(\"variable\", \"e\"):as_variable() ~= nil"),
        value_expect(bazstruct:find_variable("e"):is_a("class"), false, "bazstruct:find(\"variable\", \"e\"):is_a(\"class\")"),
        value_expect(bazstruct:find_variable("e"):is_class(), false, "bazstruct:find(\"variable\", \"e\"):is_class()"),
        value_expect(bazstruct:find_variable("e"):as("class") == nil, true, "bazstruct:find(\"variable\", \"e\"):as(\"class\") == nil"),
        value_expect(bazstruct:find_variable("e"):as_class() == nil, true, "bazstruct:find(\"variable\", \"e\"):as_class() == nil"),
        value_expect(bazstruct:find_variable("e"):is_a("struct"), false, "bazstruct:find(\"variable\", \"e\"):is_a(\"struct\")"),
        value_expect(bazstruct:find_variable("e"):is_struct(), false, "bazstruct:find(\"variable\", \"e\"):is_struct()"),
        value_expect(bazstruct:find_variable("e"):as("struct") == nil, true, "bazstruct:find(\"variable\", \"e\"):as(\"struct\") == nil"),
        value_expect(bazstruct:find_variable("e"):as_struct() == nil, true, "bazstruct:find(\"variable\", \"e\"):as_struct() == nil"),
        value_expect(bazstruct:find_variable("e"):is_a("type"), false, "bazstruct:find(\"variable\", \"e\"):is_a(\"type\")"),
        value_expect(bazstruct:find_variable("e"):is_type(), false, "bazstruct:find(\"variable\", \"e\"):is_type()"),
        value_expect(bazstruct:find_variable("e"):as("type") == nil, true, "bazstruct:find(\"variable\", \"e\"):as(\"type\") == nil"),
        value_expect(bazstruct:find_variable("e"):as_type() == nil, true, "bazstruct:find(\"variable\", \"e\"):as_type() == nil"),
        value_expect(bazstruct:find_variable("e"):type() ~= nil, true, "bazstruct:find(\"variable\", \"e\"):type() ~= nil"),
        value_expect(bazstruct:find_variable("e"):type():is_a("type"), true, "bazstruct:find(\"variable\", \"e\"):type():is_a(\"type\")"),
        value_expect(bazstruct:find_variable("e"):type():is_type(), true, "bazstruct:find(\"variable\", \"e\"):type():is_type()"),
        value_expect(bazstruct:find_variable("e"):type():name() == "int", true, "bazstruct:find(\"variable\", \"e\"):type():name() == \"int\""),
        value_expect(bazstruct:find_variable("e"):type():metadata()[1] == "i32", true, "bazstruct:find(\"variable\", \"e\"):type():metadata()[0] == \"i32\""),
        value_expect(bazstruct:find_variable("not_real_var") == nil, true, "bazstruct:find(\"variable\", \"not_real_var\") == nil"),

        value_expect(bazstruct:find_variable("ta"):type():name() == "TA", true, "bazstruct:find(\"variable\", \"ta\"):type():name() == \"TA\""),
        value_expect(bazstruct:find_variable("ta"):type():as_struct() ~= nil, true, "bazstruct:find(\"variable\", \"ta\"):type():as_struct() ~= nil"),
        value_expect(bazstruct:find_variable("ta"):type():as_struct():parents()[1]:name() == "Student", true, "bazstruct:find(\"variable\", \"ta\"):type():as_struct():parents()[1]:name() == \"Student\""),
        value_expect(bazstruct:find_variable("ta"):type():as_struct():parents()[2]:name() == "Faculty", true, "bazstruct:find(\"variable\", \"ta\"):type():parents()[2]:name() == \"Faculty\""),
        value_expect(#bazstruct:find_variable("ta"):type():as_struct():parents()[1]:parents() == 1, true, "#bazstruct:find(\"variable\", \"ta\"):type():as_struct():parents()[1]:parents() == 1"),
        value_expect(bazstruct:find_variable("ta"):type():as_struct():parents()[1]:parents()[1]:name() == "Person", true, "bazstruct:find(\"variable\", \"ta\"):type():as_struct():parents()[1]:parents()[1]:name() == \"Person\""),
        value_expect(#bazstruct:find_variable("ta"):type():as_struct():parents()[1]:parents()[1]:parents(), 0, "#bazstruct:find(\"variable\", \"ta\"):type():as_struct():parents()[1]:parents()[1]:parents() == 0"),
        value_expect(bazstruct:find_variable("ta"):type():as_struct():parents()[1]:find_variable_in_parents("age") ~= nil, true, "bazstruct:find(\"variable\", \"ta\"):type():as_struct():parents()[1]:find_variable_in_parents(\"age\") ~= nil"),
        value_expect(bazstruct:find_variable("ta"):type():as_struct():parents()[1]:find_variable_in_parents("age"):name() == "age", true, "bazstruct:find(\"variable\", \"ta\"):type():as_struct():parents()[1]:find_variable_in_parents(\"age\"):name() == \"age\""),
        
        value_expect(baz.things:type():is_a("pointer"), true, "baz.things:type():is_a(\"pointer\")"),
        value_expect(baz.things:type():to():is_a("struct"), true, "baz.things:type():to():is_a(\"struct\")"),
        value_expect(baz.things:type():to():is_struct(), true, "baz.things:type():to():is_struct()"),
        value_expect(baz.things:type():name(), "Thing*", "baz.things:type():name()"),
    }

    --[[for i=0, 10000 do
        bazstruct:find_variable("e"):type():is_type()
    end]]

    for i=0, 10-1 do
        table.insert(results, value_expect(baz.things[i].abc, i * 2, "baz.things[" .. tostring(i) .. "].abc"))
    end

    table.insert(results, value_expect(baz.f:type():name(), "int*", "baz.f:type():name()"))
    table.insert(results, value_expect(baz.f:type():to():name(), "int", "baz.f:type():to():name()"))

    for i=0, 10-1 do
        table.insert(results, value_expect(baz.f[i], i, "baz.f[" .. tostring(i) .. "]"))
    end

    table.insert(results, value_expect(baz.f:deref() == baz.f[0], true, "baz.f:deref() == baz.f[0]"))

    local known_variables = bazstruct:get_all("variable")
    local known_variables2 = bazstruct:get_all_variable()

    table.insert(results, value_expect(#known_variables == #known_variables2, true, "#known_variables == #known_variables2"))

    table.insert(results, value_expect(known_variables ~= nil, true, "known_variables ~= nil"))
    table.insert(results, value_expect(#known_variables, 18, "#known_variables"))

    for k, v in pairs(known_variables) do
        table.insert(results, value_expect(known_variables[k] == known_variables2[k], true, "known_variables[" .. tostring(k) .. "] == known_variables2[" .. tostring(k) .. "]"))

        local mt = getmetatable(baz)
        local ok, val = pcall(mt.__index, baz, v:name())
        table.insert(results, value_expect(ok, true, "pcall baz." .. v:name()))
    end


    ----------------------------
    ------- write tests --------
    ----------------------------
    -- baz.g.a modification
    local old_baz_g_a = baz.g.a
    baz.g.a = baz.g.a + 1

    table.insert(results, value_expect(baz.g.a, old_baz_g_a + 1, "baz.g.a = baz.g.a + 1"))

    baz.g.a = old_baz_g_a

    table.insert(results, value_expect(baz.g.a, old_baz_g_a, "baz.g.a = old_baz_g_a"))

    -- baz.g.c modification (float/number)
    local old_baz_g_c = baz.g.c

    baz.g.c = baz.g.c + 13.337

    table.insert(results, value_expect(round(baz.g.c, 3), round(old_baz_g_c + 13.337, 3), "baz.g.c = baz.g.c + 13.337"))

    baz.g.c = old_baz_g_c

    table.insert(results, value_expect(round(baz.g.c, 3), round(old_baz_g_c, 3), "baz.g.c = old_baz_g_c"))

    -- TA age modification
    local old_ta_age = baz.ta.age
    baz.ta.age = baz.ta.age + 1

    table.insert(results, value_expect(baz.ta.age, old_ta_age + 1, "baz.ta.age = baz.ta.age + 1"))

    baz.ta.age = old_ta_age

    table.insert(results, value_expect(baz.ta.age, old_ta_age, "baz.ta.age = old_ta_age"))

    -- Testing writing to baz.f[i] (pointer to int array)
    for i=0, 10-1 do
        local old_baz_f_i = baz.f[i]
        baz.f[i] = baz.f[i] * 1337

        table.insert(results, value_expect(baz.f[i], old_baz_f_i * 1337, "baz.f[" .. tostring(i) .. "] = baz.f[" .. tostring(i) .. "] * 1337"))

        baz.f[i] = old_baz_f_i

        table.insert(results, value_expect(baz.f[i], old_baz_f_i, "baz.f[" .. tostring(i) .. "] = old_baz_f_i"))
    end

    -- Testing writing to baz.foo (pointer to structure)
    local old_foo_addr = baz.foo:ptr()
    baz.foo = 123456789

    table.insert(results, value_expect(baz.foo:ptr(), 123456789, "baz.foo = 123456789"))

    baz.foo = nil

    table.insert(results, value_expect(baz.foo:ptr(), 0, "baz.foo = nil (0)"))

    baz.foo = old_foo_addr

    table.insert(results, value_expect(baz.foo:ptr(), old_foo_addr, "baz.foo = old_foo_addr"))

    ----------------------------
    --- boolean overlay reads --
    ----------------------------
    table.insert(results, value_expect(baz.im_true, true, "baz.im_true"))
    table.insert(results, value_expect(baz.im_false, false, "baz.im_false"))
    table.insert(results, value_expect(baz.im_also_true, true, "baz.im_also_true (char 7 as bool)"))

    ----------------------------
    -- PointerOverlay aliases --
    ----------------------------
    table.insert(results, value_expect(baz.f:d() == baz.f[0], true, "baz.f:d() == baz.f[0]"))
    table.insert(results, value_expect(baz.f:dereference() == baz.f[0], true, "baz.f:dereference() == baz.f[0]"))
    table.insert(results, value_expect(baz.f:p() == baz.f:ptr(), true, "baz.f:p() == baz.f:ptr()"))
    table.insert(results, value_expect(baz.f:address() ~= baz.f:ptr(), true, "pointer address() ~= ptr()"))
    table.insert(results, value_expect(baz:address(), bazaddr, "overlay:address() == constructor address"))

    ----------------------------
    --- sdkgenny.parse() test --
    ----------------------------
    local fresh_sdk = sdkgenny.parse([=[
type int 4 [[i32]]
type float 4 [[f32]]

struct ParseTestStruct {
    int x
    float y
}
    ]=])
    table.insert(results, value_expect(fresh_sdk ~= nil, true, "sdkgenny.parse() returns non-nil"))
    local fresh_ns = fresh_sdk:global_ns()
    table.insert(results, value_expect(fresh_ns ~= nil, true, "parsed sdk has global_ns()"))
    local pts = fresh_ns:find_struct("ParseTestStruct")
    table.insert(results, value_expect(pts ~= nil, true, "find parsed struct"))
    table.insert(results, value_expect(pts:name(), "ParseTestStruct", "parsed struct name"))
    table.insert(results, value_expect(pts:find_variable("x") ~= nil, true, "parsed struct has variable x"))
    table.insert(results, value_expect(pts:find_variable("y") ~= nil, true, "parsed struct has variable y"))
    table.insert(results, value_expect(pts:find_variable("x"):type():name(), "int", "parsed x type is int"))
    table.insert(results, value_expect(pts:find_variable("y"):type():name(), "float", "parsed y type is float"))

    ----------------------------
    ----- Sdk method tests -----
    ----------------------------
    local old_hdr_ext = parsed:header_extension()
    parsed:header_extension(".h")
    table.insert(results, value_expect(parsed:header_extension(), ".h", "Sdk:header_extension set/get"))
    parsed:header_extension(old_hdr_ext)
    table.insert(results, value_expect(parsed:header_extension(), old_hdr_ext, "Sdk:header_extension restore"))

    local old_src_ext = parsed:source_extension()
    parsed:source_extension(".c")
    table.insert(results, value_expect(parsed:source_extension(), ".c", "Sdk:source_extension set/get"))
    parsed:source_extension(old_src_ext)

    local old_gen_ns = parsed:generate_namespaces()
    parsed:generate_namespaces(not old_gen_ns)
    table.insert(results, value_expect(parsed:generate_namespaces(), not old_gen_ns, "Sdk:generate_namespaces set/get"))
    parsed:generate_namespaces(old_gen_ns)

    ----------------------------
    -- Object traversal tests --
    ----------------------------
    local ns = parsed:global_ns()

    -- has_any / has_any_*
    table.insert(results, value_expect(bazstruct:has_any("variable"), true, "bazstruct:has_any(variable)"))
    table.insert(results, value_expect(bazstruct:has_any_variable(), true, "bazstruct:has_any_variable()"))
    table.insert(results, value_expect(bazstruct:has_any("function"), false, "bazstruct:has_any(function)"))
    table.insert(results, value_expect(bazstruct:has_any_function(), false, "bazstruct:has_any_function()"))

    -- has_any_in_children
    table.insert(results, value_expect(ns:has_any_in_children("struct"), true, "ns:has_any_in_children(struct)"))
    table.insert(results, value_expect(ns:has_any_struct_in_children(), true, "ns:has_any_struct_in_children()"))
    table.insert(results, value_expect(ns:has_any_in_children("variable"), true, "ns:has_any_in_children(variable)"))

    -- owner / owner_*
    local e_var = bazstruct:find_variable("e")
    table.insert(results, value_expect(e_var:owner("struct"):name(), "Baz", "e_var:owner(struct) == Baz"))
    table.insert(results, value_expect(e_var:owner_struct():name(), "Baz", "e_var:owner_struct() == Baz"))

    -- topmost_owner
    table.insert(results, value_expect(e_var:topmost_owner("namespace") ~= nil, true, "e_var:topmost_owner(namespace) exists"))
    table.insert(results, value_expect(e_var:topmost_owner_namespace() ~= nil, true, "e_var:topmost_owner_namespace() exists"))

    -- find_in_owners (include_self = true vs false)
    table.insert(results, value_expect(bazstruct:find_in_owners("struct", "Baz", true):name(), "Baz", "find_in_owners include_self=true finds self"))
    table.insert(results, value_expect(e_var:find_in_owners("struct", "Baz", false):name(), "Baz", "find_in_owners from variable finds owning struct"))
    table.insert(results, value_expect(bazstruct:find_struct_in_owners("Baz", true):name(), "Baz", "find_struct_in_owners include_self=true"))

    ----------------------------
    ----- Type method tests ----
    ----------------------------
    local int_type = ns:find_type("int")
    table.insert(results, value_expect(int_type ~= nil, true, "find_type(int)"))
    table.insert(results, value_expect(int_type:size(), 4, "int:size() == 4"))
    table.insert(results, value_expect(int_type:name(), "int", "int:name()"))

    local float_type = ns:find_type("float")
    table.insert(results, value_expect(float_type ~= nil, true, "find_type(float)"))
    table.insert(results, value_expect(float_type:size(), 4, "float:size() == 4"))

    -- metadata
    table.insert(results, value_expect(#int_type:metadata() > 0, true, "int has metadata"))
    table.insert(results, value_expect(int_type:metadata()[1], "i32", "int metadata is i32"))
    table.insert(results, value_expect(float_type:metadata()[1], "f32", "float metadata is f32"))

    -- ptr() factory
    local int_ptr_type = int_type:ptr()
    table.insert(results, value_expect(int_ptr_type ~= nil, true, "int:ptr() creates pointer"))
    table.insert(results, value_expect(int_ptr_type:is_pointer(), true, "int:ptr():is_pointer()"))
    table.insert(results, value_expect(int_ptr_type:to():name(), "int", "int:ptr():to() == int"))

    -- ref() factory
    local int_ref_type = int_type:ref()
    table.insert(results, value_expect(int_ref_type ~= nil, true, "int:ref() creates reference"))
    table.insert(results, value_expect(int_ref_type:is_reference(), true, "int:ref():is_reference()"))
    table.insert(results, value_expect(int_ref_type:to():name(), "int", "int:ref():to() == int"))

    -- array() factory
    local int_arr_type = int_type:array(5)
    table.insert(results, value_expect(int_arr_type ~= nil, true, "int:array() creates array"))
    table.insert(results, value_expect(int_arr_type:is_array(), true, "int:array():is_array()"))

    ----------------------------
    --- Variable detail tests --
    ----------------------------
    table.insert(results, value_expect(e_var:offset() > 0, true, "e_var:offset() > 0"))
    table.insert(results, value_expect(e_var:is_bitfield(), false, "e_var:is_bitfield() == false"))
    table.insert(results, value_expect(e_var:type():name(), "int", "e_var:type():name() == int"))
    table.insert(results, value_expect(e_var["end"](e_var), e_var:offset() + e_var:type():size(), "e_var:end() == offset + size"))

    -- bitfield variables via reflection
    local date_struct = ns:find_struct("Date")
    table.insert(results, value_expect(date_struct ~= nil, true, "find Date struct"))
    local nWeekDay = date_struct:find_variable("nWeekDay")
    table.insert(results, value_expect(nWeekDay ~= nil, true, "Date has nWeekDay"))
    table.insert(results, value_expect(nWeekDay:is_bitfield(), true, "nWeekDay:is_bitfield()"))
    table.insert(results, value_expect(nWeekDay:bit_size(), 3, "nWeekDay:bit_size() == 3"))

    local nMonthDay = date_struct:find_variable("nMonthDay")
    table.insert(results, value_expect(nMonthDay ~= nil, true, "Date has nMonthDay"))
    table.insert(results, value_expect(nMonthDay:is_bitfield(), true, "nMonthDay:is_bitfield()"))
    table.insert(results, value_expect(nMonthDay:bit_size(), 6, "nMonthDay:bit_size() == 6"))

    local nMonth = date_struct:find_variable("nMonth")
    table.insert(results, value_expect(nMonth:bit_size(), 5, "nMonth:bit_size() == 5"))

    local nYear = date_struct:find_variable("nYear")
    table.insert(results, value_expect(nYear:bit_size(), 8, "nYear:bit_size() == 8"))

    -- Non-template bitfield bit_offset tests (Date: nWeekDay:3, nMonthDay:6, nMonth:5, nYear:8)
    table.insert(results, value_expect(nWeekDay:bit_offset(), 0, "nWeekDay:bit_offset() == 0"))
    table.insert(results, value_expect(nMonthDay:bit_offset(), 3, "nMonthDay:bit_offset() == 3"))
    table.insert(results, value_expect(nMonth:bit_offset(), 9, "nMonth:bit_offset() == 9"))
    -- nYear spans past 16 bits so it starts in the next storage unit
    table.insert(results, value_expect(nYear:offset(), 2, "nYear:offset() == 2 (next ushort)"))
    table.insert(results, value_expect(nYear:bit_offset(), 0, "nYear:bit_offset() == 0 (new storage unit)"))

    -- Non-template bitfield: field after bitfields has correct byte offset
    -- Foo has: int a(0), int b(4), float c(8), Place p(12), Place bf1:4(16), Place bf2:2(16)
    -- Foo total = 0x14 (20 bytes). Bar has: int d(0), Foo* foo(4), int[4][3] m, Date date
    local foo_struct = ns:find_struct("Foo")
    table.insert(results, value_expect(foo_struct:find_variable("bf1"):bit_offset(), 0, "Foo.bf1:bit_offset() == 0"))
    table.insert(results, value_expect(foo_struct:find_variable("bf2"):bit_offset(), 4, "Foo.bf2:bit_offset() == 4"))
    table.insert(results, value_expect(foo_struct:find_variable("bf1"):offset(), foo_struct:find_variable("bf2"):offset(), "Foo.bf1 and bf2 share storage unit"))

    -- Variable offset setter
    local tv = fresh_ns:find_struct("ParseTestStruct"):find_variable("x")
    tv:offset(16)
    table.insert(results, value_expect(tv:offset(), 16, "variable:offset() setter"))
    tv:offset(0)

    ----------------------------
    -- Struct bitfield() method -
    ----------------------------
    local bf_table = date_struct:bitfield(nWeekDay:offset())
    table.insert(results, value_expect(bf_table ~= nil, true, "struct:bitfield() returns table"))

    ----------------------------
    -- Namespace method tests --
    ----------------------------
    table.insert(results, value_expect(ns:find_struct("Foo") ~= nil, true, "ns:find_struct(Foo)"))
    table.insert(results, value_expect(ns:find_struct("Bar") ~= nil, true, "ns:find_struct(Bar)"))
    table.insert(results, value_expect(ns:find_enum("Place") ~= nil, true, "ns:find_enum(Place)"))
    table.insert(results, value_expect(ns:find_type("int") ~= nil, true, "ns:find_type(int)"))
    table.insert(results, value_expect(ns:find_type("float") ~= nil, true, "ns:find_type(float)"))

    -- namespace creation methods
    local build_int = fresh_ns:find_type("int")
    local build_float = fresh_ns:find_type("float")

    local test_ns = fresh_ns:namespace("TestNamespace")
    table.insert(results, value_expect(test_ns ~= nil, true, "ns:namespace() creates namespace"))
    table.insert(results, value_expect(test_ns:name(), "TestNamespace", "created namespace name"))
    table.insert(results, value_expect(test_ns:is_namespace(), true, "created namespace is_namespace()"))

    local test_struct = test_ns:struct("TestStruct")
    table.insert(results, value_expect(test_struct ~= nil, true, "ns:struct() creates struct"))
    table.insert(results, value_expect(test_struct:name(), "TestStruct", "created struct name"))

    local test_enum = test_ns:enum("TestEnum")
    table.insert(results, value_expect(test_enum ~= nil, true, "ns:enum() creates enum"))
    table.insert(results, value_expect(test_enum:is_enum(), true, "created enum is_enum()"))

    local test_ec = test_ns:enum_class("TestEnumClass")
    table.insert(results, value_expect(test_ec ~= nil, true, "ns:enum_class() creates enum_class"))
    table.insert(results, value_expect(test_ec:is_enum_class(), true, "created enum_class is_enum_class()"))

    local test_gt = test_ns:generic_type("TestGenericType")
    table.insert(results, value_expect(test_gt ~= nil, true, "ns:generic_type() creates generic type"))
    table.insert(results, value_expect(test_gt:is_generic_type(), true, "created generic_type is_generic_type()"))

    -- nested namespace
    local nested_ns = test_ns:namespace("Inner")
    table.insert(results, value_expect(nested_ns ~= nil, true, "nested namespace created"))
    table.insert(results, value_expect(nested_ns:name(), "Inner", "nested namespace name"))

    ----------------------------
    ----- Enum method tests ----
    ----------------------------
    local place_enum = ns:find_enum("Place")
    table.insert(results, value_expect(place_enum ~= nil, true, "find Place enum"))
    local place_vals = place_enum:values()
    table.insert(results, value_expect(#place_vals, 3, "#Place:values() == 3"))

    -- enum value() creation + type()
    test_enum:value("A", 0)
    test_enum:value("B", 1)
    test_enum:value("C", 2)
    local te_vals = test_enum:values()
    table.insert(results, value_expect(#te_vals, 3, "TestEnum:values() count"))
    test_enum:type(fresh_ns:find_type("int"))
    table.insert(results, value_expect(test_enum:type():name(), "int", "Enum:type() set/get"))

    ----------------------------
    --- Struct creation tests --
    ----------------------------
    -- variable creation and chaining
    local sv1 = test_struct:variable("x")
    table.insert(results, value_expect(sv1 ~= nil, true, "struct:variable() creates variable"))
    sv1:type(build_int):offset(0)
    table.insert(results, value_expect(sv1:type():name(), "int", "variable:type() set/get"))
    table.insert(results, value_expect(sv1:offset(), 0, "variable:offset() set to 0"))

    local sv2 = test_struct:variable("y")
    sv2:type(build_float):append()
    table.insert(results, value_expect(sv2:offset(), 4, "variable:append() auto-offsets"))

    -- variable type by string name
    local sv3 = test_struct:variable("z")
    sv3:type("int")
    table.insert(results, value_expect(sv3:type():name(), "int", "variable:type(string) setter"))

    -- struct parents
    table.insert(results, value_expect(#test_struct:parents(), 0, "new struct has 0 parents"))

    -- struct size setter/getter
    test_struct:size(64)
    table.insert(results, value_expect(test_struct:size(), 64, "struct:size() set/get"))
    test_struct:size(0)

    -- struct parent chaining
    local child = test_ns:struct("Child")
    child:parent(test_struct)
    table.insert(results, value_expect(#child:parents(), 1, "struct:parent() adds parent"))
    table.insert(results, value_expect(child:parents()[1]:name(), "TestStruct", "parent name matches"))

    -- nested struct / class / enum creation
    local ns_inner = test_struct:struct("InnerStruct")
    table.insert(results, value_expect(ns_inner ~= nil, true, "struct:struct() creates nested struct"))
    local ns_class = test_struct:class("InnerClass")
    table.insert(results, value_expect(ns_class ~= nil, true, "struct:class() creates nested class"))
    table.insert(results, value_expect(ns_class:is_class(), true, "nested class is_class()"))
    local ns_enum = test_struct:enum("InnerEnum")
    table.insert(results, value_expect(ns_enum ~= nil, true, "struct:enum() creates nested enum"))
    local ns_ec = test_struct:enum_class("InnerEnumClass")
    table.insert(results, value_expect(ns_ec ~= nil, true, "struct:enum_class() creates nested enum_class"))

    -- constant creation
    local test_const = test_struct:constant("MAX_SIZE")
    table.insert(results, value_expect(test_const ~= nil, true, "struct:constant() creates constant"))

    ----------------------------
    -- Function method tests ---
    ----------------------------
    local test_fn = test_struct["function"](test_struct, "doSomething")
    table.insert(results, value_expect(test_fn ~= nil, true, "struct:function() creates function"))
    table.insert(results, value_expect(test_fn:name(), "doSomething", "function name"))
    table.insert(results, value_expect(test_fn:is_function(), true, "function is_function()"))

    test_fn:returns(build_int)
    table.insert(results, value_expect(test_fn:returns():name(), "int", "function:returns() set/get"))

    test_fn:procedure("return 42;")
    table.insert(results, value_expect(test_fn:procedure(), "return 42;", "function:procedure() set/get"))

    test_fn:defined(true)
    table.insert(results, value_expect(test_fn:defined(), true, "function:defined() set true"))
    test_fn:defined(false)
    table.insert(results, value_expect(test_fn:defined(), false, "function:defined() set false"))

    test_fn:depends_on(build_float)
    local deps = test_fn:dependencies()
    table.insert(results, value_expect(#deps, 1, "function:dependencies() has 1 dep"))

    ----------------------------
    -- VirtualFunction tests ---
    ----------------------------
    local test_vfn = test_struct:virtual_function("virtualMethod")
    table.insert(results, value_expect(test_vfn ~= nil, true, "struct:virtual_function() creates vfunc"))
    table.insert(results, value_expect(test_vfn:is_virtual_function(), true, "vfunc is_virtual_function()"))

    test_vfn:vtable_index(5)
    table.insert(results, value_expect(test_vfn:vtable_index(), 5, "vfunc:vtable_index() set/get"))
    test_vfn:vtable_index(0)
    table.insert(results, value_expect(test_vfn:vtable_index(), 0, "vfunc:vtable_index() reset to 0"))

    ----------------------------
    -- StaticFunction tests ----
    ----------------------------
    local test_sfn = test_struct:static_function("staticMethod")
    table.insert(results, value_expect(test_sfn ~= nil, true, "struct:static_function() creates sfunc"))
    table.insert(results, value_expect(test_sfn:is_static_function(), true, "sfunc is_static_function()"))

    ----------------------------
    ---- Constant tests --------
    ----------------------------
    test_const:value("1024")
    table.insert(results, value_expect(test_const:value(), "1024", "constant:value() set/get"))
    table.insert(results, value_expect(test_const:is_constant(), true, "constant is_constant()"))

    ----------------------------
    ----- Array method tests ---
    ----------------------------
    local arr = int_type:array(10)
    arr:count(10)
    table.insert(results, value_expect(arr:count(), 10, "array:count() set/get"))
    table.insert(results, value_expect(arr:of():name(), "int", "array:of() returns element type"))
    -- set element type
    arr:of(float_type)
    table.insert(results, value_expect(arr:of():name(), "float", "array:of(type) setter"))
    arr:of(int_type)

    ----------------------------
    ---- Reference tests -------
    ----------------------------
    local ref = int_type:ref()
    table.insert(results, value_expect(ref:to():name(), "int", "reference:to() getter"))
    ref:to(float_type)
    table.insert(results, value_expect(ref:to():name(), "float", "reference:to(type) setter"))
    ref:to(int_type)

    ----------------------------
    --- Typename tests ---------
    ----------------------------
    local old_stg = test_struct:simple_typename_generation()
    test_struct:simple_typename_generation(not old_stg)
    table.insert(results, value_expect(test_struct:simple_typename_generation(), not old_stg, "typename:simple_typename_generation set/get"))
    test_struct:simple_typename_generation(old_stg)

    ----------------------------
    --- Object name tests ------
    ----------------------------
    local tmp = test_ns:struct("TempName")
    table.insert(results, value_expect(tmp:name(), "TempName", "Object:name() getter"))
    tmp:name("RenamedStruct")
    table.insert(results, value_expect(tmp:name(), "RenamedStruct", "Object:name() setter"))

    ----------------------------
    --- Inheritance chain tests -
    ----------------------------
    -- Bar is parent of Baz
    local bar_struct = ns:find_struct("Bar")
    table.insert(results, value_expect(bar_struct ~= nil, true, "find Bar struct"))
    table.insert(results, value_expect(#bazstruct:parents(), 1, "Baz has 1 parent"))
    table.insert(results, value_expect(bazstruct:parents()[1]:name(), "Bar", "Baz parent is Bar"))

    -- Multiple inheritance chain (TA : Student, Faculty)
    local ta_struct = ns:find_struct("TA")
    table.insert(results, value_expect(ta_struct ~= nil, true, "find TA struct"))
    table.insert(results, value_expect(#ta_struct:parents(), 2, "TA has 2 parents"))
    table.insert(results, value_expect(ta_struct:parents()[1]:name(), "Student", "TA parent[1] is Student"))
    table.insert(results, value_expect(ta_struct:parents()[2]:name(), "Faculty", "TA parent[2] is Faculty"))

    -- find_in_parents through the chain (Student -> Person -> age)
    local student_struct = ns:find_struct("Student")
    table.insert(results, value_expect(student_struct:find_variable_in_parents("age") ~= nil, true, "Student:find_variable_in_parents(age)"))
    table.insert(results, value_expect(student_struct:find_variable_in_parents("age"):name(), "age", "Student inherits age from Person"))

    -- Class inherits from Struct - verify Class has Struct methods
    local test_base = test_struct:struct("BaseForClass")
    local test_derived = test_struct:class("DerivedClass")
    table.insert(results, value_expect(test_derived:is_class(), true, "class is_class()"))
    table.insert(results, value_expect(test_derived:is_struct(), true, "class is_struct() (inherits from Struct)"))
    test_derived:parent(test_base)
    table.insert(results, value_expect(#test_derived:parents(), 1, "class:parent() works"))

    -- Pointer inherits from Reference - verify to() works on Pointer
    local ptr_type = int_type:ptr()
    table.insert(results, value_expect(ptr_type:is_pointer(), true, "ptr is_pointer()"))
    table.insert(results, value_expect(ptr_type:is_reference(), true, "ptr is_reference() (inherits from Reference)"))
    table.insert(results, value_expect(ptr_type:to():name(), "int", "Pointer:to() inherited from Reference"))

    -- EnumClass inherits from Enum - verify value/values/type work
    test_ec:value("X", 10)
    test_ec:value("Y", 20)
    local ec_vals = test_ec:values()
    table.insert(results, value_expect(#ec_vals, 2, "EnumClass:values() works"))

    ----------------------------
    --- Template type tests ----
    ----------------------------
    -- Test template struct definition via parsing
    local template_box_t = ns:find_struct("TemplateBox")
    table.insert(results, value_expect(template_box_t ~= nil, true, "find template struct TemplateBox"))
    table.insert(results, value_expect(template_box_t:is_template(), true, "TemplateBox:is_template()"))
    table.insert(results, value_expect(#template_box_t:template_parameters(), 1, "TemplateBox has 1 template param"))
    table.insert(results, value_expect(template_box_t:template_parameters()[1]:name(), "T", "template param name is T"))

    -- Test multi-param template
    local template_pair_t = ns:find_struct("TemplatePair")
    table.insert(results, value_expect(template_pair_t ~= nil, true, "find template struct TemplatePair"))
    table.insert(results, value_expect(template_pair_t:is_template(), true, "TemplatePair:is_template()"))
    table.insert(results, value_expect(#template_pair_t:template_parameters(), 2, "TemplatePair has 2 template params"))
    table.insert(results, value_expect(template_pair_t:template_parameters()[1]:name(), "K", "first param is K"))
    table.insert(results, value_expect(template_pair_t:template_parameters()[2]:name(), "V", "second param is V"))

    -- Test template instantiation via parsing
    local box_foo_t = ns:find_struct("TemplateBox<Foo>")
    table.insert(results, value_expect(box_foo_t ~= nil, true, "find instantiated TemplateBox<Foo>"))
    table.insert(results, value_expect(box_foo_t:is_template(), false, "instantiated struct is NOT a template"))
    table.insert(results, value_expect(box_foo_t:find_variable("data") ~= nil, true, "instantiated struct has data field"))
    table.insert(results, value_expect(box_foo_t:find_variable("data"):type():is_pointer(), true, "data field is a pointer"))
    table.insert(results, value_expect(box_foo_t:find_variable("data"):type():as_pointer():to():name(), "Foo", "data points to Foo"))

    -- Test multi-param instantiation
    local pair_int_float_t = ns:find_struct("TemplatePair<int, float>")
    table.insert(results, value_expect(pair_int_float_t ~= nil, true, "find instantiated TemplatePair<int, float>"))
    table.insert(results, value_expect(pair_int_float_t:find_variable("key"):type():name(), "int", "pair.key type is int"))
    table.insert(results, value_expect(pair_int_float_t:find_variable("value"):type():name(), "float", "pair.value type is float"))

    -- Test programmatic template API
    local prog_tpl = fresh_ns:struct("ProgTemplate")
    local prog_T = prog_tpl:template_parameter("T")
    table.insert(results, value_expect(prog_T ~= nil, true, "template_parameter() creates param"))
    table.insert(results, value_expect(prog_T:name(), "T", "template param name"))
    table.insert(results, value_expect(prog_tpl:is_template(), true, "struct with params is_template()"))
    local prog_var = prog_tpl:variable("field")
    prog_var:type(prog_T):offset(0)

    -- Instantiate programmatically
    local build_int = fresh_ns:find_type("int")
    local prog_inst = prog_tpl:instantiate({build_int})
    table.insert(results, value_expect(prog_inst ~= nil, true, "instantiate() returns struct"))
    table.insert(results, value_expect(prog_inst:name(), "ProgTemplate<int>", "instantiated name"))
    table.insert(results, value_expect(prog_inst:find_variable("field"):type():name(), "int", "instantiated field type is int"))
    table.insert(results, value_expect(prog_inst:is_template(), false, "instantiated struct is NOT a template"))


    ----------------------------
    -- Template overlay r/w tests
    ----------------------------
    -- Read through template-typed field: baz.tpl_box is a TemplateBox<Foo>
    table.insert(results, value_expect(baz.tpl_box ~= nil, true, "baz.tpl_box overlay exists"))
    table.insert(results, value_expect(baz.tpl_box.data.a, 42, "baz.tpl_box.data.a (Foo* through template)"))
    table.insert(results, value_expect(baz.tpl_box.data.b, 1337, "baz.tpl_box.data.b (Foo* through template)"))
    table.insert(results, value_expect(round(baz.tpl_box.data.c, 1), 77.7, "baz.tpl_box.data.c (float through template)"))

    -- Read TemplatePair<int, float> fields
    table.insert(results, value_expect(baz.tpl_pair.key, 99, "baz.tpl_pair.key (int through template)"))
    table.insert(results, value_expect(round(baz.tpl_pair.value, 3), 2.718, "baz.tpl_pair.value (float through template)"))

    -- Write through template overlay and read back
    local old_key = baz.tpl_pair.key
    baz.tpl_pair.key = 777
    table.insert(results, value_expect(baz.tpl_pair.key, 777, "tpl_pair.key write/read"))
    baz.tpl_pair.key = old_key
    table.insert(results, value_expect(baz.tpl_pair.key, old_key, "tpl_pair.key restored"))

    local old_val = baz.tpl_pair.value
    baz.tpl_pair.value = 1.414
    table.insert(results, value_expect(round(baz.tpl_pair.value, 3), 1.414, "tpl_pair.value write/read"))
    baz.tpl_pair.value = old_val
    table.insert(results, value_expect(round(baz.tpl_pair.value, 3), round(old_val, 3), "tpl_pair.value restored"))

    ----------------------------
    -- Template edge-case tests -
    ----------------------------

    -- Mixed template: non-template fields + template fields in same struct
    local mixed_float_t = ns:find_struct("TemplateMixed<float>")
    table.insert(results, value_expect(mixed_float_t ~= nil, true, "find TemplateMixed<float>"))
    table.insert(results, value_expect(mixed_float_t:find_variable("header"):type():name(), "int", "mixed: non-template field type preserved"))
    table.insert(results, value_expect(mixed_float_t:find_variable("value"):type():name(), "float", "mixed: T substituted to float"))
    table.insert(results, value_expect(mixed_float_t:find_variable("ptr"):type():is_pointer(), true, "mixed: T* is pointer"))
    table.insert(results, value_expect(mixed_float_t:find_variable("ptr"):type():as_pointer():to():name(), "float", "mixed: T* points to float"))
    table.insert(results, value_expect(mixed_float_t:find_variable("footer"):type():name(), "int", "mixed: trailing non-template field preserved"))

    -- Overlay reads on mixed template
    table.insert(results, value_expect(baz.tpl_mixed.header, 0xAA, "tpl_mixed.header read"))
    table.insert(results, value_expect(round(baz.tpl_mixed.value, 2), 6.28, "tpl_mixed.value read (T=float)"))
    table.insert(results, value_expect(baz.tpl_mixed.footer, 0xBB, "tpl_mixed.footer read"))

    -- Write + restore on mixed template
    local old_mixed_hdr = baz.tpl_mixed.header
    baz.tpl_mixed.header = 0xCC
    table.insert(results, value_expect(baz.tpl_mixed.header, 0xCC, "tpl_mixed.header write"))
    baz.tpl_mixed.header = old_mixed_hdr
    table.insert(results, value_expect(baz.tpl_mixed.header, old_mixed_hdr, "tpl_mixed.header restored"))

    -- Array template: T[4] items
    local arr_int_t = ns:find_struct("TemplateArray<int>")
    table.insert(results, value_expect(arr_int_t ~= nil, true, "find TemplateArray<int>"))
    table.insert(results, value_expect(arr_int_t:find_variable("count"):type():name(), "int", "array tpl: non-template field preserved"))
    -- items field type should be an array of int
    local items_type = arr_int_t:find_variable("items"):type()
    table.insert(results, value_expect(items_type:is_array(), true, "array tpl: T[4] is array type"))

    -- Overlay reads on array template
    table.insert(results, value_expect(baz.tpl_arr.count, 4, "tpl_arr.count read"))
    -- Array element access through template: T[4] items
    for i = 0, 3 do
        table.insert(results, value_expect(baz.tpl_arr.items[i], (i + 1) * 10, "tpl_arr.items[" .. i .. "] via ArrayOverlay"))
    end

    -- Also test the pre-existing int[4][3] m field on Bar (never tested before!)
    for i = 0, 3 do
        for j = 0, 2 do
            table.insert(results, value_expect(baz.m[i][j], i + j, "baz.m[" .. i .. "][" .. j .. "] multi-dim array"))
        end
    end

    -- ArrayOverlay __len and ipairs
    table.insert(results, value_expect(#baz.tpl_arr.items, 4, "#tpl_arr.items == 4"))
    table.insert(results, value_expect(#baz.m, 4, "#baz.m == 4 (outer dim)"))
    table.insert(results, value_expect(#baz.m[0], 3, "#baz.m[0] == 3 (inner dim)"))
    -- ipairs iteration (Lua ipairs uses 1-based indexing but our __index is 0-based,
    -- so ipairs(arr) iterates keys 1..#arr which maps to elements 1..N-1 in C++.
    -- For a 0-based C array this skips element 0 — that's a Lua convention mismatch.
    -- Test that #arr returns the correct count regardless.)

    -- Same template, different instantiations (dedup via TemplateUser)
    local box_int_t = ns:find_struct("TemplateBox<int>")
    table.insert(results, value_expect(box_int_t ~= nil, true, "find TemplateBox<int> (second instantiation)"))
    table.insert(results, value_expect(box_int_t:find_variable("data"):type():as_pointer():to():name(), "int", "TemplateBox<int>.data -> int*"))
    -- Verify TemplateBox<Foo> and TemplateBox<int> are different structs
    table.insert(results, value_expect(box_foo_t ~= box_int_t, true, "different instantiations are different structs"))
    -- But both came from the same template
    table.insert(results, value_expect(box_foo_t:size(), box_int_t:size(), "same-shape instantiations have same size"))

    -- Swapped params: TemplatePair<float, int> vs TemplatePair<int, float>
    local pair_swapped_t = ns:find_struct("TemplatePair<float, int>")
    table.insert(results, value_expect(pair_swapped_t ~= nil, true, "find TemplatePair<float, int>"))
    table.insert(results, value_expect(pair_swapped_t:find_variable("key"):type():name(), "float", "swapped pair.key is float"))
    table.insert(results, value_expect(pair_swapped_t:find_variable("value"):type():name(), "int", "swapped pair.value is int"))
    -- Original and swapped are different structs
    table.insert(results, value_expect(pair_int_float_t ~= pair_swapped_t, true, "swapped params produce different struct"))

    -- Idempotent instantiation: calling instantiate again returns the same struct
    local box_foo_t2 = template_box_t:instantiate({ns:find_struct("Foo")})
    table.insert(results, value_expect(box_foo_t2:name(), "TemplateBox<Foo>", "re-instantiate returns same name"))

    -- Programmatic: T used as both value and pointer in same template
    local dual_tpl = fresh_ns:struct("DualUse")
    local dual_T = dual_tpl:template_parameter("T")
    dual_tpl:variable("by_val"):type(dual_T):offset(0)
    dual_tpl:variable("by_ptr"):type(dual_T:ptr()):offset(8)

    local dual_inst = dual_tpl:instantiate({fresh_ns:find_type("int")})
    table.insert(results, value_expect(dual_inst ~= nil, true, "dual-use template instantiated"))
    table.insert(results, value_expect(dual_inst:find_variable("by_val"):type():name(), "int", "dual: T by value -> int"))
    table.insert(results, value_expect(dual_inst:find_variable("by_ptr"):type():is_pointer(), true, "dual: T* is pointer"))
    table.insert(results, value_expect(dual_inst:find_variable("by_ptr"):type():as_pointer():to():name(), "int", "dual: T* -> int*"))

    -- Programmatic: instantiate with wrong arg count returns nil
    local bad_inst = template_box_t:instantiate({ns:find_type("int"), ns:find_type("float")})
    table.insert(results, value_expect(bad_inst == nil, true, "instantiate with wrong arg count returns nil"))

    -- Programmatic: double pointer T**
    local dblptr_tpl = fresh_ns:struct("DblPtr")
    local dblptr_T = dblptr_tpl:template_parameter("T")
    dblptr_tpl:variable("pp"):type(dblptr_T:ptr():ptr()):offset(0)
    local dblptr_inst = dblptr_tpl:instantiate({fresh_ns:find_type("int")})
    table.insert(results, value_expect(dblptr_inst ~= nil, true, "T** template instantiated"))
    local pp_type = dblptr_inst:find_variable("pp"):type()
    table.insert(results, value_expect(pp_type:is_pointer(), true, "T** outer is pointer"))
    table.insert(results, value_expect(pp_type:as_pointer():to():is_pointer(), true, "T** inner is pointer"))
    table.insert(results, value_expect(pp_type:as_pointer():to():as_pointer():to():name(), "int", "T** -> int**"))

    -- Template struct with explicit size: instantiated struct preserves it
    local ptr_size = string.packsize("T")
    local expected_box_size = 8 + ptr_size -- pad[8] + T*
    table.insert(results, value_expect(box_foo_t:size(), expected_box_size, "instantiated TemplateBox<Foo> size"))
    table.insert(results, value_expect(box_int_t:size(), expected_box_size, "instantiated TemplateBox<int> size"))

    -- Template struct with @ offset: instantiated struct preserves pinned offsets
    table.insert(results, value_expect(box_foo_t:find_variable("data"):offset(), 0x8, "instantiated preserves @ 0x8 offset"))
    table.insert(results, value_expect(box_int_t:find_variable("data"):offset(), 0x8, "second instantiation preserves @ 0x8 offset"))

    -- T** parsed template (real-world container pattern: T** data, uint32 cap, uint32 size)
    local list_foo_t = ns:find_struct("TemplateList<Foo>")
    table.insert(results, value_expect(list_foo_t ~= nil, true, "find TemplateList<Foo>"))
    local list_data = list_foo_t:find_variable("data")
    table.insert(results, value_expect(list_data ~= nil, true, "TemplateList has data field"))
    table.insert(results, value_expect(list_data:type():is_pointer(), true, "T** data outer is pointer"))
    table.insert(results, value_expect(list_data:type():as_pointer():to():is_pointer(), true, "T** data inner is pointer"))
    table.insert(results, value_expect(list_data:type():as_pointer():to():as_pointer():to():name(), "Foo", "T** -> Foo**"))
    table.insert(results, value_expect(list_foo_t:find_variable("capacity"):type():name(), "int", "list capacity is int"))
    table.insert(results, value_expect(list_foo_t:find_variable("size"):type():name(), "int", "list size is int"))

    -- Overlay iteration through T** template (the real-world entity list pattern)
    -- baz.tpl_list is a TemplateList<Thing> with 5 Thing* entries
    table.insert(results, value_expect(baz.tpl_list.size, 5, "tpl_list.size"))
    table.insert(results, value_expect(baz.tpl_list.capacity, 5, "tpl_list.capacity"))
    -- Iterate: tpl_list.data[i]:deref().abc == (i+1)*100
    for i = 0, baz.tpl_list.size - 1 do
        local entry = baz.tpl_list.data[i]:deref()
        table.insert(results, value_expect(entry ~= nil, true, "tpl_list.data[" .. i .. "] not nil"))
        table.insert(results, value_expect(entry.abc, (i + 1) * 100, "tpl_list.data[" .. i .. "].abc"))
    end

    -- Write through the template list and read back
    local old_abc = baz.tpl_list.data[0]:deref().abc
    baz.tpl_list.data[0]:deref().abc = 9999
    table.insert(results, value_expect(baz.tpl_list.data[0]:deref().abc, 9999, "tpl_list write through T**"))
    baz.tpl_list.data[0]:deref().abc = old_abc
    table.insert(results, value_expect(baz.tpl_list.data[0]:deref().abc, old_abc, "tpl_list write restored"))

    ----------------------------
    -- Copilot-reported edge cases
    ----------------------------

    -- Comment 2: + delta in template should NOT preserve offset during instantiation
    -- TemplateDelta<T> has: int header; T value + 4
    -- With T=int: header(4) + 4 padding + value(4) = offsets 0, 8
    local delta_t = ns:find_struct("TemplateDelta")
    table.insert(results, value_expect(delta_t ~= nil, true, "find TemplateDelta"))
    table.insert(results, value_expect(delta_t:is_template(), true, "TemplateDelta:is_template()"))

    -- Instantiate TemplateDelta<int> and check value's offset
    local delta_int = delta_t:instantiate({ns:find_type("int")})
    table.insert(results, value_expect(delta_int ~= nil, true, "instantiate TemplateDelta<int>"))
    -- value should be at offset 8: header(4 bytes) + 4 bytes delta
    table.insert(results, value_expect(delta_int:find_variable("value"):offset(), 8, "delta: value offset = header(4) + delta(4) = 8"))

    -- Pathological case: + delta AFTER a template param field
    -- TemplateDeltaAfterT<T> has: T header; int value + 4
    -- In template: header(size 0) + delta 4 = value at offset 4
    -- With T=float(4): header(4) + delta 4 = value should be at offset 8
    local delta2_t = ns:find_struct("TemplateDeltaAfterT")
    table.insert(results, value_expect(delta2_t ~= nil, true, "find TemplateDeltaAfterT"))
    local delta2_float = delta2_t:instantiate({ns:find_type("float")})
    table.insert(results, value_expect(delta2_float ~= nil, true, "instantiate TemplateDeltaAfterT<float>"))
    -- value should be at 4 (float) + 4 (delta) = 8
    table.insert(results, value_expect(delta2_float:find_variable("value"):offset(), 8, "delta-after-T: value at header_size + delta"))

    -- Comment 3: template class should instantiate as Class, not Struct
    local classbox_t = ns:find_struct("TemplateClassBox")
    table.insert(results, value_expect(classbox_t ~= nil, true, "find TemplateClassBox"))
    table.insert(results, value_expect(classbox_t:is_class(), true, "TemplateClassBox is_class()"))
    local classbox_int = classbox_t:instantiate({ns:find_type("int")})
    table.insert(results, value_expect(classbox_int ~= nil, true, "instantiate TemplateClassBox<int>"))
    table.insert(results, value_expect(classbox_int:is_class(), true, "TemplateClassBox<int> is_class()"))

    -- Comment 4: template struct with parent should account for parent size
    local child_t = ns:find_struct("TemplateChild")
    table.insert(results, value_expect(child_t ~= nil, true, "find TemplateChild"))
    local child_int = child_t:instantiate({ns:find_type("int")})
    table.insert(results, value_expect(child_int ~= nil, true, "instantiate TemplateChild<int>"))
    -- extra should be after Foo's fields (Foo size = 0x14 = 20 bytes)
    local foo_size = ns:find_struct("Foo"):size()
    table.insert(results, value_expect(child_int:find_variable("extra"):offset(), foo_size, "template child: extra at parent end"))

    -- Complex template with parent: TemplateChildComplex<T> : Bar { T value; T* ptr; int footer }
    local cplx_t = ns:find_struct("TemplateChildComplex")
    table.insert(results, value_expect(cplx_t ~= nil, true, "find TemplateChildComplex"))
    local bar_size = ns:find_struct("Bar"):size()
    local cplx_int = cplx_t:instantiate({ns:find_type("int")})
    table.insert(results, value_expect(cplx_int ~= nil, true, "instantiate TemplateChildComplex<int>"))
    -- value should start right after Bar
    table.insert(results, value_expect(cplx_int:find_variable("value"):offset(), bar_size, "complex child: value at Bar end"))
    -- ptr should be after value (int=4)
    table.insert(results, value_expect(cplx_int:find_variable("ptr"):offset(), bar_size + 4, "complex child: ptr after value"))
    -- footer should be after ptr (pointer size)
    table.insert(results, value_expect(cplx_int:find_variable("footer"):offset(), bar_size + 4 + ptr_size, "complex child: footer after ptr"))

    -- Template with virtual function: TemplateVirtual<T> { virtual void dummy() @ 0; T data @ 0x8 }
    local virt_t = ns:find_struct("TemplateVirtual")
    table.insert(results, value_expect(virt_t ~= nil, true, "find TemplateVirtual"))
    local virt_int = virt_t:instantiate({ns:find_type("int")})
    table.insert(results, value_expect(virt_int ~= nil, true, "instantiate TemplateVirtual<int>"))
    -- data @ 0x8 should be preserved (explicit offset after vtable pointer)
    table.insert(results, value_expect(virt_int:find_variable("data"):offset(), 0x8, "virtual template: data @ 0x8 preserved"))
    -- size should account for vtable ptr + data
    table.insert(results, value_expect(virt_int:size() >= 0xC, true, "virtual template: size includes vtable + data"))

    -- Template with parent AND virtual (should not double-count vtable)
    local vchild_t = ns:find_struct("TemplateVirtualChild")
    table.insert(results, value_expect(vchild_t ~= nil, true, "find TemplateVirtualChild"))
    local vbase_size = ns:find_struct("VirtualBase"):size()
    local vchild_int = vchild_t:instantiate({ns:find_type("int")})
    table.insert(results, value_expect(vchild_int ~= nil, true, "instantiate TemplateVirtualChild<int>"))
    -- extra should be right after VirtualBase, not double-counted with vtable
    table.insert(results, value_expect(vchild_int:find_variable("extra"):offset(), vbase_size, "virtual child: extra at parent end (no vtable double-count)"))

    -- Template with parent AND its OWN virtual (double-count vtable test)
    local vchild2_t = ns:find_struct("TemplateVirtualChild2")
    table.insert(results, value_expect(vchild2_t ~= nil, true, "find TemplateVirtualChild2"))
    local vchild2_int = vchild2_t:instantiate({ns:find_type("int")})
    table.insert(results, value_expect(vchild2_int ~= nil, true, "instantiate TemplateVirtualChild2<int>"))
    -- extra @ 0x10 is explicit, should be preserved regardless of vtable accounting
    table.insert(results, value_expect(vchild2_int:find_variable("extra"):offset(), 0x10, "virtual child2: extra @ 0x10 preserved"))

    -- Comment 1: bitfields in template struct
    local bf_t = ns:find_struct("TemplateBitfield")
    table.insert(results, value_expect(bf_t ~= nil, true, "find TemplateBitfield"))
    table.insert(results, value_expect(bf_t:is_template(), true, "TemplateBitfield:is_template()"))
    -- Instantiate and verify fields exist
    local bf_int = bf_t:instantiate({ns:find_type("int")})
    table.insert(results, value_expect(bf_int ~= nil, true, "instantiate TemplateBitfield<int>"))
    table.insert(results, value_expect(bf_int:find_variable("flags"):type():name(), "int", "bf: flags is int"))
    table.insert(results, value_expect(bf_int:find_variable("bf_a"):is_bitfield(), true, "bf: bf_a is bitfield"))
    table.insert(results, value_expect(bf_int:find_variable("bf_b"):is_bitfield(), true, "bf: bf_b is bitfield"))
    -- after_bf should be after the bitfield storage unit, not double-counted
    -- flags(int=4) + bitfield storage(int=4) + after_bf should be at offset 8
    table.insert(results, value_expect(bf_int:find_variable("after_bf"):offset(), 8, "bf: after_bf at correct offset"))

    -- Regression: non-template + delta struct with live overlay reads
    table.insert(results, value_expect(baz.delta_test.first, 111, "delta_test.first (non-template + delta)"))
    table.insert(results, value_expect(baz.delta_test.second, 222, "delta_test.second (non-template + delta)"))
    -- Verify the offset: first(4) + pad(4) + second at offset 8
    local dt = ns:find_struct("DeltaTest")
    table.insert(results, value_expect(dt:find_variable("second"):offset(), 8, "DeltaTest.second offset = 4 + delta(4) = 8"))

    -- Non-template virtual function struct
    local vbase = ns:find_struct("VirtualBase")
    table.insert(results, value_expect(vbase ~= nil, true, "find VirtualBase"))
    table.insert(results, value_expect(vbase:has_any_virtual_function(), true, "VirtualBase has virtuals"))
    -- data should be at offset 0x8 (after vtable pointer)
    table.insert(results, value_expect(vbase:find_variable("data"):offset(), 0x8, "VirtualBase.data at 0x8 (after vtable)"))
    -- vtable_index checks
    local vfuncs = vbase:get_all_virtual_function()
    table.insert(results, value_expect(#vfuncs, 2, "VirtualBase has 2 virtual functions"))
    table.insert(results, value_expect(vfuncs[1]:vtable_index(), 0, "first_virtual vtable_index == 0"))
    table.insert(results, value_expect(vfuncs[2]:vtable_index(), 1, "second_virtual vtable_index == 1"))
    -- size should include vtable pointer + data
    table.insert(results, value_expect(vbase:size() >= 0xC, true, "VirtualBase size includes vtable + data"))

    local total_passed = 0

    for k, v in pairs(results) do
        if v == false then
            print("Detected failure at test " .. tostring(k))
        else
            total_passed = total_passed + 1
        end
    end

    print(tostring(total_passed) .. " / " .. tostring(#results) .. " tests passed")

    return #results == total_passed
end

local retval = do_tests()

local time_elapsed = os.clock() - test_start

print("Tests took " .. tostring(time_elapsed) .. " seconds (" .. tostring(time_elapsed * 1000) .. "ms)")

local gen_start = os.clock()
parsed:generate("test/")

local gen_time = os.clock() - gen_start
print("Generation took " .. tostring(gen_time) .. " seconds (" .. tostring(gen_time * 1000) .. "ms)")

return retval
