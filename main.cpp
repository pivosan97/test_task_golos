#include <iostream>

#include "core.h"
#include "client.h"

// Example
int main(int argc, char **argv) {
    // Create databases (assume that they are on different hosts)
    database db_1, db_2, db_3;

    // Create database network services and connect them to the network
    database_network_service db_net_1(db_1), db_net_2(db_2), db_net_3(db_3);
    db_net_1.connect();
    db_net_2.connect();
    // This node provides network interface to get database statistics
    uint64_t api_addr = db_net_3.connect();

    // Create client that observs database statistics
    // Connect it to database api through the network
    passive_client passive_cl(api_addr);
    passive_cl.connect();

    std::cout << "Sending request to api to get balance" << std::endl;
    bool res = passive_cl.request_balance();
    if(!res) {
        std::cout << "Failed to send request" << std::endl;
    }

    // Create client that can sends new transaction and connect it to the network
    active_client active_cl;
    active_cl.connect();

    // Multicast new block to all nodes in the network
    std::cout << "\nMaking transaction to change balance to 2009" << std::endl;
    active_cl.change_balance(2009);

    std::cout << "\nSending request to api to get balance" << std::endl;
    res = passive_cl.request_balance();
    if(!res) {
        std::cout << "Failed to send request" << std::endl;
    }

    return 0;
}