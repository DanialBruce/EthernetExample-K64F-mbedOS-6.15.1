/*** Ethernet Example with K64F on Mbed ***
 
 This is a simple example on how to connect K64F Development board to a network,using the Ethernet interface.
 Than MQTT is used to test the connection.

 @danialBrcue
 */


#include "mbed.h"
#include "MQTTClientMbedOs.h"
#include "EthernetInterface.h"
#include <cstdio>

// Maximum number of element the application buffer can contain
#define MAXIMUM_BUFFER_SIZE                                                  128

//network, socket and mqtt

EthernetInterface eth;
TCPSocket socket;
MQTTClient client(&socket);
MQTTPacket_connectData mqttData;

// ip addresses
SocketAddress MQTTBrokerAddress;
SocketAddress DeviceAddress;
SocketAddress BuffAddr;

// my vars
char buffer[MAXIMUM_BUFFER_SIZE];


int main()
{
    // Bring up the ethernet interface and connect to network
    eth.connect();
    
    //gather network address, netmask and gateway address and show them
    eth.get_ip_address(&DeviceAddress);
    ThisThread::sleep_for(1s);

    printf("IP address: %s\n", DeviceAddress.get_ip_address() ? DeviceAddress.get_ip_address() : "None");

    eth.get_gateway(&BuffAddr);
    printf("Gateway address: %s\n", BuffAddr.get_ip_address() ?BuffAddr.get_ip_address() : "None");
    
    eth.get_netmask(&BuffAddr);
    printf("Gateway address: %s\n",  BuffAddr.get_ip_address() ? BuffAddr.get_ip_address() : "None");

    // Tell the TCP socket to use the instantiated Ehternet Interface
    socket.open(&eth);

    //get the ip address of 
    eth.gethostbyname(MBED_CONF_APP_MQTT_BROKER_HOSTNAME, &MQTTBrokerAddress, NSAPI_IPv4, "myEthernet");
    //set port to Broker IP address to be used in MQTTClient 
    MQTTBrokerAddress.set_port(1883);
    socket.connect(MQTTBrokerAddress);

    //print the Translated IP address of MQTT host Broker
    printf("MQTT host Broker address: %s\n",  MQTTBrokerAddress.get_ip_address() ? MQTTBrokerAddress.get_ip_address() : "None");

    mqttData = MQTTPacket_connectData_initializer;
        // This MQTTClient ID/name
    mqttData.clientID.cstring = MBED_CONF_APP_MQTT_CLIENT_ID;
    
    client.connect(mqttData);
    //testing MQTTClient connectivty by buffer, can add variables to char array using sprintf()
    sprintf(buffer, "YOUR TEST MESSAGE");

    ThisThread::sleep_for(1s);
    // MQTT Message format to be used to publish our test message
    MQTT::Message msg;
    msg.qos = MQTT::QOS0;
    msg.retained = false;
    msg.dup = false;
    msg.payload = (void*)buffer;
    msg.payloadlen = strlen(buffer);

    ThisThread::sleep_for(1s);

    //Publish
    while (true) {
        printf("Publishing with payload length %d\n", strlen(buffer));
        client.publish(MBED_CONF_APP_MQTT_TOPIC, msg); //Use mbed_app.json file or mainually enter a string Topic
        ThisThread::sleep_for(500ms);
    }
}