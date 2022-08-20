# sdkgenny-lua
Lua bindings for [sdkgenny](https://github.com/cursey/sdkgenny)

## Examples
```
PS F:\Programming\projects\sdkgenny-lua\build\Debug> .\test.exe
0x223aa80d010
> baz = sdkgenny.StructOverlay.new(0x223aa80d010, parsed:global_ns():struct("Baz")) 
> print(tostring(baz.things:address()))
2352207548688
> print(tostring(baz.things[0]))        
sol.luagenny::api::StructOverlay: 00000223AA7D8338
> print(tostring(baz.things[0].abc)) 
0
> print(tostring(baz.things[1].abc))
2
> print(tostring(baz.things[2].abc))
4
> print(tostring(baz.hello))
Hello, world!
> print(tostring(baz.im_true))
true
> print(tostring(baz.im_false))
false
> print(tostring(baz.im_also_true))
true
> print(tostring(baz.ta.hours))     
40
```
