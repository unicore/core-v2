using namespace eosio;

    uint64_t unicore2::getcondition(eosio::name host, eosio::string key){
        conditions_index conditions(_me, host.value);
        eosio::name keyname = name(key);

        auto condition = conditions.find(keyname.value);
        uint64_t result = 0;

        if (condition != conditions.end()){
            result = condition -> value;
        };

        return result;
    }


    [[eosio::action]] void unicore2::setcondition(eosio::name host, eosio::string key, uint64_t value){
        require_auth (host);

        conditions_index conditions(_me, host.value);
        eosio::name keyname = name(key);

        auto condition = conditions.find(keyname.value);

        if (condition == conditions.end()){
            
            conditions.emplace(host, [&](auto &c){
                c.key = keyname.value;
                c.key_string = key;
                c.value = value;
            });

        } else {

            conditions.modify(condition, host, [&](auto &c) {
                c.value = value;
            });

        }
    }

    [[eosio::action]] void unicore2::rmcondition(eosio::name host, uint64_t key){
        require_auth (host);

        conditions_index conditions(_me, host.value);
        eosio::name keyname = name(key);

        auto condition = conditions.find(keyname.value);

        if (condition != conditions.end()){
            conditions.erase(condition);
        }
    }
