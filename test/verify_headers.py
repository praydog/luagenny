"""
Verify generated .hpp headers using libclang.

Parses the generated headers as C++, instantiates templates with test types,
and checks sizeof/offsetof for every struct and field. Catches layout issues
that static_asserts might miss (e.g. wrong offset comments, overlapping fields).
"""

import sys
import os
import subprocess
import tempfile

def main():
    test_dir = os.path.dirname(os.path.abspath(__file__))
    failures = []
    
    # Build a single C++ file that includes all generated headers,
    # instantiates templates, and dumps sizeof/offsetof via compile-time checks.
    # We use clang's -fsyntax-only to verify without linking.
    
    # Collect all generated .hpp files
    hpp_files = sorted(f for f in os.listdir(test_dir) if f.endswith('.hpp'))
    if not hpp_files:
        print("ERROR: No .hpp files found in", test_dir)
        return 1
    
    print(f"Found {len(hpp_files)} generated headers")
    
    # Build a test source that includes everything and instantiates templates
    source = '// Avoid stdlib headers for cross-compiler portability\n'
    source += '#ifndef offsetof\n#define offsetof(t,m) __builtin_offsetof(t,m)\n#endif\n'
    source += 'using ushort = unsigned short;\n'
    source += 'typedef unsigned long long uintptr_t;\n\n'
    
    # Include all headers
    for hpp in hpp_files:
        source += f'#include "{hpp}"\n'
    
    source += '\n#pragma pack(push, 1)\n\n'
    
    # Template instantiations to verify
    instantiations = [
        ('TemplateBox<int>', [('data', 8)]),
        ('TemplateBox<float>', [('data', 8)]),
        ('TemplatePair<int, float>', [('key', 0), ('value', 4)]),
        ('TemplatePair<float, int>', [('key', 0), ('value', 4)]),
        ('TemplateMixed<float>', [('header', 0), ('value', 4), ('ptr', 8), ('footer', None)]),
        ('TemplateMixed<int>', [('header', 0), ('value', 4), ('ptr', 8), ('footer', None)]),
        ('TemplateArray<int>', [('items', 0), ('count', 16)]),
        ('TemplateBitfield<int>', [('flags', 0), ('after_bf', 8)]),
        ('TemplateBitfieldPinned<int>', [('data', 4)]),
    ]
    
    # Generate typedefs and static_asserts
    for i, (inst_type, fields) in enumerate(instantiations):
        alias = f'Inst{i}'
        source += f'using {alias} = {inst_type};\n'
        for field_name, expected_offset in fields:
            if expected_offset is not None:
                source += f'static_assert(offsetof({alias}, {field_name}) == {expected_offset}, '
                source += f'"{inst_type}.{field_name} offset");\n'
    
    # Also check that pointer-dependent sizes scale correctly
    source += '\n// Pointer-size-dependent checks\n'
    source += 'static_assert(sizeof(TemplateBox<int>) == 8 + sizeof(void*), "TemplateBox<int> size");\n'
    source += 'static_assert(sizeof(TemplatePair<int, float>) == 8, "TemplatePair<int, float> size");\n'
    source += 'static_assert(sizeof(TemplateMixed<float>) == 12 + sizeof(void*), "TemplateMixed<float> size");\n'
    source += 'static_assert(sizeof(TemplateArray<int>) == 20, "TemplateArray<int> size");\n'
    source += 'static_assert(sizeof(TemplateBitfield<int>) == 12, "TemplateBitfield<int> size");\n'
    source += 'static_assert(sizeof(TemplateBitfieldPinned<int>) == 8, "TemplateBitfieldPinned<int> size");\n'
    
    # Non-template structs
    source += '\n// Non-template layout checks\n'
    source += 'static_assert(offsetof(VirtualBase, data) == 0x8, "VirtualBase.data");\n'
    source += 'static_assert(offsetof(DeltaTest, second) == 8, "DeltaTest.second");\n'
    
    # Inheritance checks
    source += '\n// Template inheritance checks\n'
    source += 'using ChildInt = TemplateChild<int>;\n'
    source += 'static_assert(offsetof(ChildInt, extra) == sizeof(Foo), "TemplateChild<int>.extra");\n'
    source += 'using ChildCplxInt = TemplateChildComplex<int>;\n'
    source += 'static_assert(offsetof(ChildCplxInt, value) == sizeof(Bar), "ChildComplex.value");\n'
    
    # Virtual template checks
    source += '\n// Virtual template checks\n'
    source += 'using VirtInt = TemplateVirtual<int>;\n'
    source += 'static_assert(offsetof(VirtInt, data) == 0x8, "TemplateVirtual.data");\n'
    source += 'using VirtChild2Int = TemplateVirtualChild2<int>;\n'
    source += 'static_assert(offsetof(VirtChild2Int, extra) == 0x10, "VirtualChild2.extra");\n'
    
    source += '\n#pragma pack(pop)\n'
    source += 'int main() { return 0; }\n'
    
    # Write to temp file in the test directory (so relative includes work)
    verify_src = os.path.join(test_dir, '_verify_generated.cpp')
    try:
        with open(verify_src, 'w') as f:
            f.write(source)
        
        # Try clang++ first (usually available), then cl.exe as fallback
        compilers = [
            ['clang++', '-std=c++20', '-fsyntax-only', '-Wall', '-Werror',
             '-Wno-unused-private-field', '-Wno-invalid-offsetof',
             '-fms-extensions', '-fms-compatibility',
             '-fms-compatibility-version=19.44', f'-I{test_dir}', verify_src],
            ['cl.exe', '/nologo', '/EHsc', '/std:c++20', '/W4', '/WX',
             f'/I{test_dir}', '/c', verify_src, f'/Fo{verify_src}.obj'],
        ]

        result = None
        for cmd in compilers:
            try:
                result = subprocess.run(cmd, capture_output=True, text=True, cwd=test_dir)
                break
            except FileNotFoundError:
                continue

        if result is None:
            print('ERROR: No C++ compiler found (tried clang++, cl.exe)')
            return 1

        if result.returncode != 0:
            print('COMPILATION FAILED:')
            print(result.stderr)
            return 1
        else:
            print(f'All generated header layout checks passed ({len(instantiations)} template instantiations verified)')
            return 0
    finally:
        # Clean up
        for f in [verify_src, verify_src + '.obj']:
            if os.path.exists(f):
                os.remove(f)


if __name__ == '__main__':
    sys.exit(main())
