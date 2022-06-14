using namespace eosio;

    //add to AWL

    void unicore::checkminpwr(eosio::name host, eosio::name username){
        power3_index upower(_me, host.value);
        auto hp = upower.find(username.value);

        if (hp != upower.end()){
            if (unicore::checkcondition(host, "minpower", hp->staked + hp->with_badges))
            {
                unicore::addtohostwl(host, username);
            } else {
                unicore::rmfromhostwl(host, username);
            }

        }
    }

    void unicore::check_good_status(eosio::name host, eosio::name username, eosio::asset good_amount){
        uint64_t minburn = unicore::getcondition(host, "minburn");
        uint64_t threemburn = unicore::getcondition(host, "threemburn");
        uint64_t sixmburn = unicore::getcondition(host, "sixmburn");
        uint64_t ninemburn = unicore::getcondition(host, "ninemburn");
        uint64_t twelvemburn = unicore::getcondition(host, "twelvemburn");

        uint64_t burnperiod = unicore::getcondition(host, "burnperiod");
        
        uint64_t times = 0;
        
        if (twelvemburn > 0 && good_amount.amount >= twelvemburn * 12){
            times = good_amount.amount / twelvemburn;
        } else if (ninemburn > 0 && good_amount.amount >= ninemburn * 9){
            times = good_amount.amount / ninemburn;
        } else if (sixmburn > 0 && good_amount.amount >= sixmburn * 6) {
            times = good_amount.amount / sixmburn;
        } else if (threemburn > 0 && good_amount.amount >= threemburn * 3){
            times = good_amount.amount / threemburn;
        } else if (minburn > 0 && good_amount.amount >= minburn) {
            times = good_amount.amount / minburn;
        } 
        
        if (times > 0 && username != "eosio"_n) {
            corepartners_index cpartners(_me, host.value);
            auto partner = cpartners.find(username.value);
            
            // if (partner == cpartners.end()) {
            //     cpartners.emplace(_me, [&](auto &p) {
            //         p.username = username;
            //         p.status = "partner"_n;
            //         p.total_good = good_amount;
            //         p.sediment = good_amount;
            //         p.join_at = eosio::time_point_sec(eosio::current_time_point().sec_since_epoch());
            //         p.expiration = eosio::time_point_sec(eosio::current_time_point().sec_since_epoch() + times * burnperiod * 86400);

            //     });
            // } else {
            //     cpartners.modify(partner, _me, [&](auto &p){
            //         p.expiration = eosio::time_point_sec(partner->expiration.sec_since_epoch() + times * burnperiod * 86400);
            //         p.total_good += good_amount;
            //     });
            // }
        }
        
    }

    void unicore::rmfromhostwl(eosio::name host, eosio::name username){
        corepartners_index partners(_me, host.value);

        auto partner = partners.find(username.value);
        if (partner != partners.end()) {
            partners.erase(partner);
        }
    }    

    void unicore::addtohostwl(eosio::name host, eosio::name username){
        corepartners_index partners(_me, host.value);

        auto partner = partners.find(username.value);
        if (partner == partners.end()) {
            partners.emplace(_me, [&](auto &p){
                p.username = username;
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
        uint64_t result = 0;

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
