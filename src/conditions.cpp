using namespace eosio;

    //add to AWL

    void unicore::checkminpwr(eosio::name host, eosio::name username){
        power_index upower(_me, username.value);
        auto hp = upower.find(host.value);

        if (hp != upower.end()){
            if (unicore::checkcondition(host, "minpower", hp->staked + hp->with_badges))
            {
                unicore::addtohostwl(host, username);
            } else {
                unicore::rmfromhostwl(host, username);
            }

        }
    }

    void unicore::check_burn_status(eosio::name host, eosio::name username, eosio::asset burn_amount){
        uint64_t minburn = unicore::getcondition(host, "minburn");
        uint64_t burnperiod = unicore::getcondition(host, "burnperiod");

        uint64_t times = burn_amount.amount / minburn;
        print("times", times, burn_amount, minburn);

        if (times > 0 && username != "eosio"_n) {

            cpartners2_index partners(_me, host.value);
            auto partner = partners.find(username.value);
            
            if (partner == partners.end()) {
                partners.emplace(_me, [&](auto &p) {
                    p.partner = username;
                    p.status = "partner"_n;
                    p.join_at = eosio::time_point_sec(eosio::current_time_point().sec_since_epoch());
                    p.expiration = eosio::time_point_sec(eosio::current_time_point().sec_since_epoch() + times * burnperiod * 86400);

                });
            } else {
                partners.modify(partner, _me, [&](auto &p){
                    p.expiration = eosio::time_point_sec(partner->expiration.sec_since_epoch() + times * burnperiod * 86400);
                });
            }
        }
    }

    void unicore::rmfromhostwl(eosio::name host, eosio::name username){
        cpartners2_index partners(_me, host.value);

        auto partner = partners.find(username.value);
        if (partner != partners.end()) {
            partners.erase(partner);
        }
    }    

    void unicore::addtohostwl(eosio::name host, eosio::name username){
        cpartners2_index partners(_me, host.value);

        auto partner = partners.find(username.value);
        if (partner == partners.end()) {
            partners.emplace(_me, [&](auto &p){
                p.partner = username;
                p.status = "partner"_n;
            });
        }
    }    

    bool unicore::checkcondition(eosio::name host, eosio::string key, uint64_t value){
        conditions_index conditions(_me, host.value);
        eosio::name keyname = name(key);

        auto condition = conditions.find(keyname.value);

        if (condition != conditions.end()){
            if (value >= condition -> value && condition -> value != 0)
                return true;
            else return false;
        } 

        return false;
    }

    uint64_t unicore::getcondition(eosio::name host, eosio::string key){
        conditions_index conditions(_me, host.value);
        eosio::name keyname = name(key);

        auto condition = conditions.find(keyname.value);
        uint64_t result;

        if (condition != conditions.end()){
            result = condition -> value;
        };

        return result;
    }


    [[eosio::action]] void unicore::setcondition(eosio::name host, eosio::string key, uint64_t value){
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

    [[eosio::action]] void unicore::rmcondition(eosio::name host, uint64_t key){
        require_auth (host);

        conditions_index conditions(_me, host.value);
        eosio::name keyname = name(key);

        auto condition = conditions.find(keyname.value);

        if (condition != conditions.end()){
            conditions.erase(condition);
        }
    }
