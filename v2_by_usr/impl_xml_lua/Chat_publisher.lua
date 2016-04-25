-- Global counter (preserved across invocations)  
if not count then count = 0 else count = count + 1  end  

-- Get the writer
local ChatObject_writer = CONTAINER.WRITER['MyPublisher::ChatObjectWriter']

ChatObject_writer.instance['user'] = "Rajive" -- key field  

ChatObject_writer.instance['msg'] = "Hello from Lua " .. count

ChatObject_writer:write()

print("Writing", ChatObject_writer.instance['user'], 
                 ChatObject_writer.instance['msg'])
