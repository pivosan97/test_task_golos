#pragma once

#include <cstdint>
#include <unordered_map>


enum class MessageType : uint8_t {
	Block, Balance, Test
};


struct message {
	uint64_t from;
	uint64_t to;
	uint64_t sig;
	uint64_t data;
	MessageType type;
};


// Base class for network node
class host {
public:
	uint64_t connect();
	virtual void process_msg(const message& msg) = 0;
	virtual ~host() = default;

protected:
	uint64_t my_addr = {0};
};


// This is the Internet, the greatest tecknology of 21st century
// Network simulator
// Note that it is singltone
class Internet final {
public:
	Internet(const Internet& inet) = delete;
	Internet& operator=(const Internet& inet) = delete;

	static Internet& instance() {
		static Internet inet;
		return inet;
	}

	uint64_t connect(host *h) {
		hosts[next_addr] = h;
		return next_addr++;
	}

	bool send(const message& msg) {
		auto host_it = hosts.find(msg.to);
		if(host_it != hosts.end()) {
			host_it->second->process_msg(msg);
			return true;
		}
		return false;
	}

	void multicast(const message& msg) {
		for(auto it : hosts) {
			if(it.first != msg.from) {
				it.second->process_msg(msg);
			}
		}
	}

private:
	Internet() = default;
	~Internet() = default;

	uint64_t next_addr = {1};
	std::unordered_map<uint64_t, host *> hosts;
};


inline uint64_t host::connect() {
	my_addr = Internet::instance().connect(this);
	return my_addr;
}