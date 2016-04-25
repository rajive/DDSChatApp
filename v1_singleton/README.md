# Chat v1 - singleton

Building a peer-to-peer Chat application using [DDS](http://portals.omg.org/dds).


- Generate datatype handling code and example code from idl

- Modify the generated example code as follows.

- Use `My::CHAT_TOPIC_NAME`

   - Update `Chat_publisher.cxx`
   - Update `Chat_subscriber.cxx`


- Mv `USER_QOS_PROFILES.xml` to `if/`

- Name your participant

		<participant_name>
			<name>Rajive</name>
			:
		</participant_name>

- Modify the Publisher program to produce something interesting

		/* Modify the data to be sent here */

		snprintf(instance->msg, My::MSG_LEN, "Hello from Rajive %d", count);


- `impl_cpp_polling/`: Modify the Subscriber program to use **polling**

  - Do not attach the listener to the DataReader

		/* To customize the data reader QoS, use
	    the configuration file USER_QOS_PROFILES.xml */

	    reader = subscriber->create_datareader(
	        topic, DATAREADER_QOS_DEFAULT, NULL /* listener */,
	        STATUS_MASK_NONE);

  - Poll the reader in the main loop

			/* Main loop */
		    for (count=0; (sample_count == 0) || (count < sample_count); ++count) {
		    	reader_listener->on_data_available(reader);
				// :
			}

  - Kill the Subscriber, Rebuild the subscriber and run it again


- `impl_cpp_waitset/`: Modify the Subscriber program to use **waitsets**
