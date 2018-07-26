# Chat v3 - by user using XML app creation (data oriented interfaces)

Building a peer-to-peer Chat application using [DDS](http://portals.omg.org/dds).

In this version, we use [XML Application Creation](https://community.rti.com/documentation) 
to define the **data-oriented component interfaces** in an XML file.

The XML based application creation provides several benefits.

- Explicitly controlled governance: It becomes the single version controlled 
source of truth about the system architecture. In the previous examples, the 
component interfaces were scattered across implementation code (C++ files).

- Modularity: The same data oriented interface can be used to create multiple 
implementations very easily. For example, the application may be in C++, but the
test code could be scripted in Lua.

- Fewer lines of code: It gets rid of the boilerplate code and error 
checking code, allowing us to focus on the application specific logic.

## Data-Oriented Interfaces
- [if/](./if)


## C++ Implementation

- [impl\_xml\_cpp/](./impl_xml_cpp)

## C# Implementation  

- [impl\_xml\_cs/](./impl_xml_cs)

## Lua (Scripting) Implementation 

- [impl\_xml\_lua/](./impl_xml_lua)

- [Lua](http://www.lua.org)
  - [Lua 101 Cheatsheet](https://github.com/gianpiero/foo/raw/master/doc/Lua101CheatSheet.pdf)

## Web Implementation  

- [impl\_web/](./impl_web)

