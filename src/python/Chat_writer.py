###############################################################################
# (c) 2005-2015 Copyright, Real-Time Innovations.  All rights reserved.       #
# No duplications, whole or partial, manual or electronic, may be made        #
# without express written permission.  Any such copies, or revisions thereof, #
# must display this notice unaltered.                                         #
# This code contains trade secrets of Real-Time Innovations, Inc.             #
###############################################################################

from time import sleep

# Updating the system path is not required if you have pip-installed
# rticonnextdds-connector
from sys import path as sys_path
from os import path as os_path
# file_path = os_path.dirname(os_path.realpath(__file__))
# sys_path.append(file_path + "/../../../")
sys_path.append("/Users/rajive/Code/rticonnextdds-connector-py")

import rticonnextdds_connector as rti

with rti.open_connector(
        config_name="MyServiceIfLib::Pub",
        url="if/MyService.xml") as connector:

    output = connector.get_output("_pub::Chat_writer")

    print("Waiting for subscriptions...")
    output.wait_for_subscriptions()

    print("Writing...")
    for i in range(1, 100):
        output.instance.set_string("id", "Rajive Python")
        output.instance.set_string("content", "Hello from Rajive Python" + str(i))        
        output.write()

        sleep(0.5) # Write at a rate of one sample every 0.5 seconds, for ex.

    print("Exiting...")
    output.wait() # Wait for all subscriptions to receive the data before exiting