# Chat v2 - by user: with XML app creation (data oriented interfaces)

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

## Ensure that DDS-XML files are loaded in the correct order

    export NDDS_QOS_PROFILES="res/qos/v2/Chat_qos.xml;if/Chat.xml"
   
## Data-Oriented Interfaces
- [if/](./if)


## C++ Implementation

- [src/c++11/v2/partitions_xml/](./src/c++11/v2/partitions_xml)

## C# Implementation  

- [src/cs/v2/waitset_cft_xml/](./src/cs/v2/waitset_cft_xml)

## Lua (Scripting) Implementation 

- [src/lua/v2/](./src/lua/v2/)

- [Lua](http://www.lua.org)
  - [Lua 101 Cheatsheet](https://github.com/gianpiero/foo/raw/master/doc/Lua101CheatSheet.pdf)

## Web Implementation  

- [svc/web/](./svc/web)

