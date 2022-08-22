baz = sdkgenny.StructOverlay.new(bazaddr, parsed:global_ns():find("struct", "Baz"))
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
        print("test " .. tostring(test_count) .. " failed, got " .. tostring(x) .. ", expected " .. tostring(expected))
        return false
    end

    print("test " .. tostring(test_count) .. " passed " .. "[ " .. metadata .. " ] == " .. tostring(expected))
    return true
end

function do_tests()
    print("testing...")

    local results = {
        value_expect(baz.g.a, 43, "baz.g.a"),
        value_expect(baz.g.b, 1338, "baz.g.b"),
        value_expect(math.ceil(baz.g.c), math.ceil(78.699996948242), "math.ceil(baz.g.c)"),
        value_expect(baz.hello, "Hello, world!", "baz.hello"),
        value_expect(baz.e, 666, "baz.e")
    }

    for i=0, 10-1 do
        table.insert(results, value_expect(baz.things[i].abc, i * 2, "baz.things[" .. tostring(i) .. "].abc"))
    end

    for k, v in pairs(results) do
        if v == false then
            print("Detected failure at test " .. tostring(k))
            return false
        end
    end

    return true
end

parsed:generate("test/")

return do_tests()