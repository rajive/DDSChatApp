# Running the Web Integration Service Components

## Web Integration Service Configuration

Extends the domain participants defined in the `ChatIfLib` in 
`if/Chat.xml` and groups them into a
web application called `ChatApp`, contained in the service configuration
called `WebGateway`.


## Running the Web Integration Service


- Define the list of DDS-XML files to be loaded via the `NDDS_QOS_PROFILES` 
  environment variable 

		 export NDDS_QOS_PROFILES="res/qos/v2/Chat_qos.xml;if/Chat.xml"

- Start the web integration service
		
      rtiwebintegrationservice -cfgName WebGateway -cfgFile svc/web/CUSTOM_WEB_INTEGRATION_SERVICE.xml


- Check that the web integration service is running

      open http://localhost:8080

- Open AdminConsole to see the Web Integration Service DDS endpoints


## List the datatypes via RESTful (http) web client

	curl -X GET -H "Cache-Control:no-cache" http://localhost:8080/dds/rest1/types

## List the web applications via RESTful (http) web client

	curl -X GET -H "Cache-Control:no-cache" http://localhost:8080/dds/rest1/applications
	

## Publish data via RESTful (http) web client

	curl -X POST -H "Content-Type:application/dds-web+xml" -H "Cache-Control:no-cache" -d "<data>
    	<user>Rajive</user>
    	<msg>Hello From Web on `date`</msg>
	</data>" http://localhost:8080/dds/rest1/applications/ChatApp/domain_participants/MyPublicationParticipant/publishers/MyPublisher/data_writers/ChatObjectWriter


## Subscribe to data via RESTful (http) web client

*Note: piping through `xmllint --format -` to pretty print the XML*
 
**take**:

	curl -X GET -H "Cache-Control:no-cache" http://localhost:8080/dds/rest1/applications/ChatApp/domain_participants/MySubscriptionParticipant/subscribers/MySubscriber/data_readers/ChatObjectReader | xmllint --format -
	

**read**:

	curl -X GET -H "Cache-Control:no-cache" http://localhost:8080/dds/rest1/applications/ChatApp/domain_participants/MySubscriptionParticipant/subscribers/MySubscriber/data_readers/ChatObjectReader?removeFromReaderCache=false | xmllint --format -



