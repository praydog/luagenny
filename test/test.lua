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
    table.insert(results, value_expect(#known_variables, 10, "#known_variables"))

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
