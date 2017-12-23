#pragma once

#include <iostream>
#include "network.h"

// Client that send new transactions
// PUSH and potentially PULL requests
// Generally it can be united with database and be real blockchain node that sends and verifies transactions
class active_client final : public host {
public:
	void change_balance(uint64_t balance) const {
		// Target address is not needed for multicasting
		// Signature is sended addr (extremly simplified)
		message msg = {my_addr, 0, my_addr, balance, MessageType::Block};
		Internet::instance().multicast(msg);
	}

	void process_msg(const message& msg) override {
		// somehow process some message
	}
};

// Client that observe database statistics
// Only PULL requests
class passive_client final : public host {
public:
	explicit passive_client(uint64_t db_api_addr) : db_api_addr(db_api_addr) {
	} 

	bool request_balance() const {
		// Signature and data is not needed
		message msg = {my_addr, db_api_addr, 0, 0, MessageType::Balance};
		return Internet::instance().send(msg);
	}

	void process_msg(const message& msg) override {
		if(msg.type == MessageType::Balance) {
			// Check signature (extremly simplified)
			if(msg.from == msg.sig) {
				std::cout << "Response received: balance = " << msg.data << std::endl;
			}
		}
	}

private:
	uint64_t db_api_addr = {0};
};