# Chat v1 - singleton

Building a peer-to-peer Chat application using [DDS](http://portals.omg.org/dds).


- Generate datatype handling code and example code from idl

- Mv `USER_QOS_PROFILES.xml` to `res/qos/v1/Chat_qos.xml`

  - Remove  
   
        <publication_name> 
    
        and 
    
        <subscription_name>
    
  - Name your participant

		<participant_name>
			<name>Rajive</name>
			:
		</participant_name>

- Modify the generated example code as follows.

- Use `My::Topic::Chat::TOPIC`

   - Update `Chat_publisher.cxx`
   - Update `Chat_subscriber.cxx`
   

- `src/cpp/v1/listener/`: Modify the Publisher program to produce something interesting

		/* Modify the data to be sent here */
		snprintf(instance->content, My::MSG_LEN, "Hello from Rajive %d", count);


- `src/cpp/v1/polling/`: Modify the Subscriber program to use **polling**

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


- `src/cpp/v1/waitset/`: Modify the Subscriber program to use **waitsets**

   - Setup StatusCondition trigger
   
	        /* Setup StatusCondition */
	        StatusCondition* status_condition = reader->get_statuscondition();
	        if (status_condition == NULL) {
	            printf("get_statuscondition error\n");
	            subscriber_shutdown(participant);
	            return -1;
	        }
	        retcode = status_condition->set_enabled_statuses(DDS_DATA_AVAILABLE_STATUS);
	        if (retcode != DDS_RETCODE_OK) {
	            printf("set_enabled_statuses error\n");
	            subscriber_shutdown(participant);
	            return -1;
	        }

   - Setup WaitSet
   
	        /* Setup WaitSet */
	        WaitSet* waitset = new WaitSet();
	        retcode = waitset->attach_condition(status_condition);
	        if (retcode != DDS_RETCODE_OK) {
	            // ... error
	            delete waitset;
	            subscriber_shutdown(participant);
	            return -1;
	        }

	- Wait for condition to trigger
	
	        ConditionSeq active_conditions; // holder for active conditions
	        /* Main loop */
			for (count=0; (sample_count == 0) || (count < sample_count); ++count) {
                /* Wait for condition to trigger */
                retcode = waitset->wait(active_conditions, receive_period);
	        	if (retcode == DDS_RETCODE_OK) {
	            	reader_listener->on_data_available(reader);
	            }
				// :
			}
			
	- Cleanup
	
        	delete waitset;
