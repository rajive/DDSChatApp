# Chat v2 - by user

Building a peer-to-peer Chat application using [DDS](http://portals.omg.org/dds).

In this version, we add the concept of a conversation per user. Each user is
modeled as a "user" key field on the data type.

DDS organizes the conversations by user with a DDS Topic. Each user is
maintained independently of the other users with the same DDS Topic.

An app must specify the user for which it is publishing a message. An app can
publish messages on behalf of multiple users.

An app can subscribe to all the users, or a subset of users of interest using
a content filtered topic (CFT).


## Subscribing to selected conversations using a *content filtered topic*

Components that show the use of content-filters include:

 - `impl_cpp_cft`
 - `impl_cpp_instance`
 - `impl_cpp_partitions`


## Accessing a subset of updates from the DataReader view

Components that show how to access a specific instance include:

 - `impl_cpp_instance`
 - `impl_cpp_partitions`


## Tagging conversations using `PARTITION` QoS Policy

In this version, we add the concept of tagging conversations. Each tag
is modeled via a `PARTITION` QoSPolicy.

A tag could be:

- a subject of conversation, e.g. starwars, startrek, funny, spaceships, etc...
- a geographic region: e.g. by timezone, country, etc. OR land, air, water
- the floor of a building
- etc.

An XML AppConfig file can be used to specify the initial configuration of the
regions (`PARTITION` QoSPolicy) for an app at startup.

An app can add or remove tags, simply by changing the `PARTITION` QoS Policy.

An app can publish or subscribe to additional tags i.e. `PARTITION`s
(for example after discovering them), and can stop being  a publisher or
subscriber for the regions (i.e. partitions) specified in the XML AppConfig 
file.

Components that show the use of partitions include:

 - `impl_cpp_partitions`

