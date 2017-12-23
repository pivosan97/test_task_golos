#pragma once

#include <iostream>
#include <unordered_map>
#include <memory>

#include "network.h"


class database_impl;

struct block final {
    block(uint64_t actions, uint64_t args) : actions(actions), args(args) {}

    uint64_t actions;
    uint64_t args;
};


// Removed databse from class members
// Now it is passed to apply method as parameter
class handler {
public:
    virtual void apply(database_impl &db, const block &) = 0;

    virtual uint64_t id() const = 0;
};


class handler_storage final {
public:
    handler_storage() = default;

    template<typename Handler, typename ...Args>
    void add(Args... args) {
        handler *tmp = new Handler((args)...);
        storage.emplace(tmp->id(), tmp);
    }

    bool transformation(database_impl &db, const block &b) {
        auto it = storage.find(b.actions);
        if (it != storage.end()) {
            it->second->apply(db, b);
            return true;
        }
        return false;
    }

private:
    std::unordered_map<uint64_t, handler *> storage;
};


class change_balance final : public handler {
public:
    void apply(database_impl &db, const block &) override;

    uint64_t id() const override {
        //code handler
        return 1;
    }
};


// Database entity is splitted in two classes
// One with more public methods and another with less public methods and private data
// It is close to pImpl idiom
class database_impl final {
public:
    database_impl() {
        handlers.add<change_balance>();
    }

    database_impl(const database_impl &) = delete;
    database_impl &operator=(const database_impl &)= delete;

    uint64_t balance() const {
        return balance_;
    }

    void balance(uint64_t balance_) {
        this->balance_ = balance_;
    }

    void push_block(const block &b) {
        handlers.transformation(*this, b);
    }

private:
    handler_storage handlers;
    uint64_t balance_ = {0};
};


// Kind of API to database that is implemented in database_impl
class database final {
public:
    database() {
        // Memory will be freed in database destructor
        pimpl = std::unique_ptr<database_impl>(new database_impl());
    }

    database(const database &) = delete;
    database &operator=(const database &)= delete;
    
    // Objects of classes with pImpl idiom can be easily moved 
    database(database &&) = default;
    database &operator=(database &&) = default;

    uint64_t balance() const {
        return pimpl->balance();
    }

    void push_block(const block &b) {
        pimpl->push_block(b);
    }

private:
    std::unique_ptr<database_impl> pimpl;
};


inline void change_balance::apply(database_impl &db, const block &b) {
    std::cerr << "(_*_)" << std::endl;
    db.balance(b.args);
}


// Service that provides network access to databse interface
class database_network_service final : public host {
public:
    explicit database_network_service(database &db) : db(db) {
    }

    void process_msg(const message& msg) override {
        switch (msg.type) {
            // Request to get balance
            case MessageType::Balance: {
                message response = {my_addr, msg.from, my_addr, db.balance(), MessageType::Balance};
                Internet::instance().send(response);
                break;
            }

            // Request to add new block
            case MessageType::Block: {
                // Check signature (extremly simplified)
                if(msg.from == msg.sig) {

                    // This is a place to think about establishing consensus to accept new block
                    // In this mock we make assumption that there are no double-spend attacks
                    // So all nodes just accepts block after it was multicasted to network

                    db.push_block({1, msg.data});
                }
                break;
            }

            default: {
            }
        }
    }

private:
    database &db;
};