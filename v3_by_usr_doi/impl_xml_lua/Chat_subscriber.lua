-- Get the reader
local ChatObject_reader = CONTAINER.READER['MySubscriber::ChatObjectReader']

ChatObject_reader:read()

for  i, chat_object in ipairs(ChatObject_reader.samples) do

  if (ChatObject_reader.infos[i].valid_data) then

    print(chat_object['user'], chat_object['msg'])

  end

end

print("\n")
