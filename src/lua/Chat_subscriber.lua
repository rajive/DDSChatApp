-- Get the reader
local Chat_reader = CONTAINER.READER['_sub::Chat_reader']

Chat_reader:take()

for  i, sample in ipairs(Chat_reader.samples) do

  if (Chat_reader.infos[i].valid_data) then

    print(sample['id'], sample['content'])

  end

end
