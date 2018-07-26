
/*   
ChatObject_publisher.cs

A publication of data of type ChatObject

This file is derived from code automatically generated by the rtiddsgen 
command:

rtiddsgen -language C# -example <arch> Chat.idl

Example publication of type ChatObject automatically generated by 
'rtiddsgen'. To test them follow these steps:

(1) Compile this file and the example subscription.

(2) Start the subscription with the command
objs\<arch>${constructMap.nativeFQNameInModule}_subscriber <domain_id> <sample_count>

(3) Start the publication with the command
objs\<arch>${constructMap.nativeFQNameInModule}_publisher <domain_id> <sample_count>

(4) [Optional] Specify the list of discovery initial peers and 
multicast receive addresses via an environment variable or a file 
(in the current working directory) called NDDS_DISCOVERY_PEERS. 

You can run any number of publishers and subscribers programs, and can 
add and remove them dynamically from the domain.

Example:

To run the example application on domain <domain_id>:

bin\<Debug|Release>\ChatObject_publisher <domain_id> <sample_count>
bin\<Debug|Release>\ChatObject_subscriber <domain_id> <sample_count>
*/

using System;
using System.Collections.Generic;
using System.Text;

namespace My{

    public class ChatObjectPublisher {

        public static void Main(string[] args) {

            // --- Get domain ID --- //
            int domain_id = 0;
            if (args.Length >= 1) {
                domain_id = Int32.Parse(args[0]);
            }

            // --- Get max loop count; 0 means infinite loop  --- //
            int sample_count = 0;
            if (args.Length >= 2) {
                sample_count = Int32.Parse(args[1]);
            }

            /* Uncomment this to turn on additional logging
            NDDS.ConfigLogger.get_instance().set_verbosity_by_category(
                NDDS.LogCategory.NDDS_CONFIG_LOG_CATEGORY_API, 
                NDDS.LogVerbosity.NDDS_CONFIG_LOG_VERBOSITY_STATUS_ALL);
            */

            // --- Run --- //
            try {
                ChatObjectPublisher.publish(
                    domain_id, sample_count);
            }
            catch(DDS.Exception)
            {
                Console.WriteLine("error in publisher");
            }
        }

        static void publish(int domain_id, int sample_count) {

            // --- Register userGenerated datatype ---
            DDS.DomainParticipantFactory.get_instance().
                    register_type_support(
                    ChatObjectTypeSupport.get_instance(),
                    "My::ChatObject");

            // --- Create participant --- //

            /* To customize the participant QoS, use 
            the configuration file USER_QOS_PROFILES.xml */
            DDS.DomainParticipant participant =
                DDS.DomainParticipantFactory.get_instance().
                        create_participant_from_config(
                              "MyParticipant_Library::MyPublicationParticipant");
            if (participant == null)
            {
                shutdown(participant);
                throw new ApplicationException("create_participant error");
            }

            // --- Lookup writer --- //

            /* To customize data writer QoS, use 
            the configuration file USER_QOS_PROFILES.xml */
            DDS.DataWriter writer = participant.lookup_datawriter_by_name(
                                             "MyPublisher::ChatObjectWriter");
            if (writer == null) {
                shutdown(participant);
                throw new ApplicationException("lookup_datawriter error");
            }
            ChatObjectDataWriter ChatObject_writer =
            (ChatObjectDataWriter)writer;

            // --- Write --- //

            /* Create data sample for writing */
            ChatObject instance = ChatObjectTypeSupport.create_data();
            if (instance == null) {
                shutdown(participant);
                throw new ApplicationException(
                    "ChatObjectTypeSupport.create_data error");
            }

            /* For a data type that has a key, if the same instance is going to be
            written multiple times, initialize the key here
            and register the keyed instance prior to writing */
            /* >>> */
            DDS.InstanceHandle_t instance_handle = DDS.InstanceHandle_t.HANDLE_NIL;
            instance.user = "Rajive";
            // instance_handle = ChatObject_writer.register_instance(instance);
            /* <<< */

            /* Main loop */
            const System.Int32 send_period = 4000; // milliseconds
            for (int count=0;
            (sample_count == 0) || (count < sample_count);
            ++count) {
                Console.WriteLine("Writing ChatObject, count {0}", count);

                /* Modify the data to be sent here */
                /* >>> */
               instance.msg = "Hello " + count;
               /* <<< */

                try {
                    ChatObject_writer.write(instance, ref instance_handle);
                }
                catch(DDS.Exception e) {
                    Console.WriteLine("write error {0}", e);
                }

                System.Threading.Thread.Sleep(send_period);
            }

            /*
            try {
                ChatObject_writer.unregister_instance(
                    instance, ref instance_handle);
            } catch(DDS.Exception e) {
                Console.WriteLine("unregister instance error: {0}", e);
            }
            */

            // --- Shutdown --- //

            /* Delete data sample */
            try {
                ChatObjectTypeSupport.delete_data(instance);
            } catch(DDS.Exception e) {
                Console.WriteLine(
                    "ChatObjectTypeSupport.delete_data error: {0}", e);
            }

            /* Delete all entities */
            shutdown(participant);
        }

        static void shutdown(
            DDS.DomainParticipant participant) {

            /* Delete all entities */

            if (participant != null) {
                participant.delete_contained_entities();
                DDS.DomainParticipantFactory.get_instance().delete_participant(
                    ref participant);
            }

            /* RTI Connext provides finalize_instance() method on
            domain participant factory for people who want to release memory
            used by the participant factory. Uncomment the following block of
            code for clean destruction of the singleton. */
            /*
            try {
                DDS.DomainParticipantFactory.finalize_instance();
            } catch (DDS.Exception e) {
                Console.WriteLine("finalize_instance error: {0}", e);
                throw e;
            }
            */
        }
    }

} /* namespace My */
