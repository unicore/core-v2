
namespace eosio {
    

    // @abi table goals
    struct goals{
        uint64_t id;
        eosio::name username;
        eosio::time_point_sec created;

        bool activated = false;
        bool in_protocol = false;
        bool completed = false;
        bool reported = false;
        bool validated = false;
        eosio::name host;
        uint64_t lepts_for_each_pool;
        std::string shortdescr;
        std::string descr;
        eosio::asset target;
        eosio::asset activation_amount;
        eosio::asset available;
        std::string report;
        uint64_t rotation_num;
        uint64_t total_votes;
        std::vector<eosio::name> voters;
        std::vector<uint64_t> balance_ids;
        eosio::asset withdrawed;
        uint64_t primary_key()const { return id; }
        uint64_t by_votes() const { return total_votes; }
        uint64_t by_activated() const {return activated; }
        uint64_t by_completed() const {return completed; }
        uint64_t by_in_protocol() const {return in_protocol; }
        eosio::name by_username() const {return username; }
        eosio::name by_host() const {return host;}
        uint64_t by_rotation_num() const {return rotation_num;}
        EOSLIB_SERIALIZE( goals, (id)(username)(created)(activated)(in_protocol)(completed)(reported)
            (validated)(host)(lepts_for_each_pool)(shortdescr)(descr)(target)(activation_amount)
            (available)(report)(rotation_num)(total_votes)(voters)(balance_ids)(withdrawed))
    };

    typedef eosio::multi_index <"goals"_n, goals,
        eosio::indexed_by<"totalvotes"_n, eosio::const_mem_fun<goals, uint64_t, &goals::by_votes>>,
        eosio::indexed_by<"activated"_n, eosio::const_mem_fun<goals, uint64_t, &goals::by_activated>>,
        eosio::indexed_by<"completed"_n, eosio::const_mem_fun<goals, uint64_t, &goals::by_completed>>,
        eosio::indexed_by<"username"_n, eosio::const_mem_fun<goals, eosio::name, &goals::by_username>>,
        eosio::indexed_by<"host"_n, eosio::const_mem_fun<goals, eosio::name, &goals::by_host>>,
        eosio::indexed_by<"inprotocol"_n, eosio::const_mem_fun<goals, uint64_t, &goals::by_in_protocol>>,
        eosio::indexed_by<"rotationnum"_n, eosio::const_mem_fun<goals, uint64_t, &goals::by_rotation_num>>
    > goals_index;


    /* ACTIONS */

    // @abi action
    struct setgoal{
        eosio::name username;
        eosio::name host;
        std::string shortdescr;
        std::string descr;
        uint64_t lepts_for_each_pool;
        eosio::asset target;

        EOSLIB_SERIALIZE( setgoal, (username)(host)(shortdescr)(descr)(lepts_for_each_pool)(target))
    };

      // @abi action
    struct editgoal{
        uint64_t goal_id;
        eosio::name username;
        eosio::name host;
        std::string shortdescr;
        std::string descr;
        uint64_t lepts_for_each_pool;
        eosio::asset target;

        EOSLIB_SERIALIZE( editgoal, (username)(host)(goal_id)(shortdescr)(descr)(lepts_for_each_pool)(target))

    };

    // @abi action
    struct delgoal{
        eosio::name username;
        eosio::name host;
        uint64_t goal_id;
        EOSLIB_SERIALIZE( delgoal, (username)(host)(goal_id))
    };

    // @abi action
    struct report{
        eosio::name username;
        eosio::name host;
        uint64_t goal_id;
        std::string report;
        EOSLIB_SERIALIZE( struct report, (username)(host)(goal_id)(report))
    };

    // @abi action
    struct gwithdraw{
        eosio::name username;
        eosio::name host;
        uint64_t goal_id;
        EOSLIB_SERIALIZE( gwithdraw, (username)(host)(goal_id))
    };    
};






