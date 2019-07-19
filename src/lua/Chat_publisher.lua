-- Global counter (preserved across invocations)  
if not count then count = 0 else count = count + 1  end  

-- Get the writer
local Chat_writer = CONTAINER.WRITER['_pub::Chat_writer']

Chat_writer.instance['id'] = "Rajive Lua" -- key field  
Chat_writer.instance['content'] = "Hello from Lua " .. count

Chat_writer:write()

print("Writing", Chat_writer.instance['id'], 
                 Chat_writer.instance['content'])
