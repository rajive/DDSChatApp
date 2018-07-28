-- Global counter (preserved across invocations)  
if not count then count = 0 else count = count + 1  end  

-- Get the writer
local ChatObject_writer = CONTAINER.WRITER['Chat_Pub_publisher::Chat_writer']

ChatObject_writer.instance['user'] = "Rajive Lua" -- key field  

ChatObject_writer.instance['msg'] = "Hello from Lua " .. count

ChatObject_writer:write()

print("Writing", ChatObject_writer.instance['user'], 
                 ChatObject_writer.instance['msg'])
