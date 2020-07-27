

    struct [[eosio::table, eosio::contract("unicore")]]  goals {
        uint64_t id;
        uint64_t hash;
        uint64_t parent_hash;
        uint64_t type;
        eosio::name creator;
        eosio::name host;
        eosio::name benefactor;
        eosio::time_point_sec created;
        std::string parent_permlink;
        std::string permlink;
        std::string title;
        std::string description;
        eosio::asset target;
        eosio::asset available;

        int64_t total_votes;
        uint64_t total_tasks;
        bool validated = false;
        bool filled = false;
        bool reported = false;
        bool checked = false;
        std::string report;
        eosio::asset withdrawed;

        eosio::time_point_sec expired_at;

        std::vector<eosio::name> voters;
        std::string meta;
        bool with_badge = false;
        uint64_t badge_id;

        uint64_t primary_key()const { return id; }
        // double byvotes() const { 
        //     return (double)total_votes; 
        // }
        
        uint64_t byvotes() const { 
            return pow(2, 63) + total_votes;
        }

        uint64_t byhash() const {return hash;}

        uint64_t byparenthash() const {return parent_hash;}

        uint64_t byfilled() const {return filled; }
        uint64_t bycreated() const {return created.sec_since_epoch(); }

        uint64_t byusername() const {return creator.value; }
        uint64_t byhost() const {return host.value;}
        uint128_t by_username_and_host() const { return eosio::combine_ids(creator.value, host.value); }
        
        EOSLIB_SERIALIZE( goals, (id)(hash)(parent_hash)(type)(creator)(host)(benefactor)(created)(parent_permlink)(permlink)(title)(description)(target)(available)(total_votes)(total_tasks)(validated)(filled)(reported)
            (checked)(report)(withdrawed)(expired_at)(voters)(meta)(with_badge)(badge_id))
    };

    typedef eosio::multi_index <"goals"_n, goals,
        eosio::indexed_by<"votes"_n, eosio::const_mem_fun<goals, uint64_t, &goals::byvotes>>,
        eosio::indexed_by<"hash"_n, eosio::const_mem_fun<goals, uint64_t, &goals::byhash>>,
        eosio::indexed_by<"parenthash"_n, eosio::const_mem_fun<goals, uint64_t, &goals::byparenthash>>,
        eosio::indexed_by<"filled"_n, eosio::const_mem_fun<goals, uint64_t, &goals::byfilled>>,
        eosio::indexed_by<"creator"_n, eosio::const_mem_fun<goals, uint64_t, &goals::byusername>>,
        eosio::indexed_by<"host"_n, eosio::const_mem_fun<goals, uint64_t, &goals::byhost>>,
        eosio::indexed_by<"created"_n, eosio::const_mem_fun<goals, uint64_t, &goals::bycreated>>
        
    > goals_index;




