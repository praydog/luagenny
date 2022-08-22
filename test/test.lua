local test_start = os.clock()

baz = sdkgenny.StructOverlay(bazaddr, parsed:global_ns():find("struct", "Baz"))
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
        print(tostring(name or x) .. " == " .. tostring(v) .. ": " .. tostring(x:is_a(v)))
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
        value_expect(baz.g.a, 43, "baz.g.a"),
        value_expect(baz.g.b, 1338, "baz.g.b"),
        value_expect(round(baz.g.c, 1), 78.7, "round(baz.g.c)"),
        value_expect(baz.hello, "Hello, world!", "baz.hello"),
        value_expect(baz.e, 666, "baz.e"),
        value_expect(baz.ta.age, 20, "baz.ta.age"),
        value_expect(round(baz.ta.gpa, 1), 3.9, "baz.ta.gpa"),
        value_expect(baz.ta.wage, 30000, "baz.ta.wage"),
        value_expect(baz.ta.hours, 40, "baz.ta.hours"),
        value_expect(bazstruct:find("variable", "e") ~= nil, true, "bazstruct:find(\"variable\", \"e\") ~= nil"),
        value_expect(bazstruct:find("variable", "e"):is_a("variable"), true, "bazstruct:find(\"variable\", \"e\"):is_a(\"variable\")"),
        value_expect(bazstruct:find("variable", "e"):as("variable") ~= nil, true, "bazstruct:find(\"variable\", \"e\"):as(\"variable\") ~= nil"),
        value_expect(bazstruct:find("variable", "e"):is_a("class"), false, "bazstruct:find(\"variable\", \"e\"):is_a(\"class\")"),
        value_expect(bazstruct:find("variable", "e"):as("class") == nil, true, "bazstruct:find(\"variable\", \"e\"):as(\"class\") == nil"),
        value_expect(bazstruct:find("variable", "e"):is_a("struct"), false, "bazstruct:find(\"variable\", \"e\"):is_a(\"struct\")"),
        value_expect(bazstruct:find("variable", "e"):as("struct") == nil, true, "bazstruct:find(\"variable\", \"e\"):as(\"struct\") == nil"),
        value_expect(bazstruct:find("variable", "e"):is_a("type"), false, "bazstruct:find(\"variable\", \"e\"):is_a(\"type\")"),
        value_expect(bazstruct:find("variable", "e"):as("type") == nil, true, "bazstruct:find(\"variable\", \"e\"):as(\"type\") == nil"),
        value_expect(bazstruct:find("variable", "e"):type() ~= nil, true, "bazstruct:find(\"variable\", \"e\"):type() ~= nil"),
        value_expect(bazstruct:find("variable", "e"):type():is_a("type"), true, "bazstruct:find(\"variable\", \"e\"):type():is_a(\"type\")"),
        value_expect(bazstruct:find("variable", "e"):type():name() == "int", true, "bazstruct:find(\"variable\", \"e\"):type():name() == \"int\""),
        value_expect(bazstruct:find("variable", "e"):type():metadata()[1] == "i32", true, "bazstruct:find(\"variable\", \"e\"):type():metadata()[0] == \"i32\""),
        value_expect(bazstruct:find("variable", "not_real_var") == nil, true, "bazstruct:find(\"variable\", \"not_real_var\") == nil"),
        
        value_expect(baz.things:type():is_a("pointer"), true, "baz.things:type():is_a(\"pointer\")"),
        value_expect(baz.things:type():to():is_a("struct"), true, "baz.things:type():to():is_a(\"struct\")"),
        value_expect(baz.things:type():name(), "Thing*", "baz.things:type():name()"),
    }

    for i=0, 10-1 do
        table.insert(results, value_expect(baz.things[i].abc, i * 2, "baz.things[" .. tostring(i) .. "].abc"))
    end

    local known_variables = bazstruct:get_all("variable")

    table.insert(results, value_expect(known_variables ~= nil, true, "known_variables ~= nil"))
    table.insert(results, value_expect(#known_variables, 10, "#known_variables"))

    for k, v in pairs(known_variables) do
        local mt = getmetatable(baz)
        local ok, val = pcall(mt.__index, baz, v:name())
        table.insert(results, value_expect(ok, true, "pcall baz." .. v:name()))
    end

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