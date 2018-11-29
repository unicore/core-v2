
namespace eosio {
    

    // @abi table goals
    struct goals{
        uint64_t id;
        account_name username;
        eosio::time_point_sec created;

        bool activated = false;
        bool in_protocol = false;
        bool completed = false;
        bool reported = false;
        bool validated = false;
        account_name host;
        uint64_t lepts_for_each_pool;
        std::string shortdescr;
        std::string descr;
        eosio::asset target;
        eosio::asset activation_amount;
        eosio::asset available;
        std::string report;
        uint64_t rotation_num;
        uint64_t total_votes;
        std::vector<account_name> voters;
        std::vector<uint64_t> balance_ids;
        eosio::asset withdrawed;
        uint64_t primary_key()const { return id; }
        uint64_t by_votes() const { return total_votes; }
        uint64_t by_activated() const {return activated; }
        uint64_t by_completed() const {return completed; }
        uint64_t by_in_protocol() const {return in_protocol; }
        account_name by_username() const {return username; }
        account_name by_host() const {return host;}
        uint64_t by_rotation_num() const {return rotation_num;}
        EOSLIB_SERIALIZE( goals, (id)(username)(created)(activated)(in_protocol)(completed)(reported)
            (validated)(host)(lepts_for_each_pool)(shortdescr)(descr)(target)(activation_amount)
            (available)(report)(rotation_num)(total_votes)(voters)(balance_ids)(withdrawed))
    };

    typedef eosio::multi_index <N(goals), goals,
        indexed_by<N(total_votes), const_mem_fun<goals, uint64_t, &goals::by_votes>>,
        indexed_by<N(activated), const_mem_fun<goals, uint64_t, &goals::by_activated>>,
        indexed_by<N(completed), const_mem_fun<goals, uint64_t, &goals::by_completed>>,
        indexed_by<N(username), const_mem_fun<goals, account_name, &goals::by_username>>,
        indexed_by<N(host), const_mem_fun<goals, account_name, &goals::by_host>>,
        indexed_by<N(in_protocol), const_mem_fun<goals, uint64_t, &goals::by_in_protocol>>,
        indexed_by<N(rotation_num), const_mem_fun<goals, uint64_t, &goals::by_rotation_num>>
    > goals_index;


    /* ACTIONS */

    // @abi action
    struct setgoal{
        account_name username;
        account_name host;
        std::string shortdescr;
        std::string descr;
        uint64_t lepts_for_each_pool;
        eosio::asset target;

        EOSLIB_SERIALIZE( setgoal, (username)(host)(shortdescr)(descr)(lepts_for_each_pool)(target))
    };

      // @abi action
    struct editgoal{
        uint64_t goal_id;
        account_name username;
        account_name host;
        std::string shortdescr;
        std::string descr;
        uint64_t lepts_for_each_pool;
        eosio::asset target;

        EOSLIB_SERIALIZE( editgoal, (username)(host)(goal_id)(shortdescr)(descr)(lepts_for_each_pool)(target))

    };

    // @abi action
    struct delgoal{
        account_name username;
        account_name host;
        uint64_t goal_id;
        EOSLIB_SERIALIZE( delgoal, (username)(host)(goal_id))
    };

    // @abi action
    struct report{
        account_name username;
        account_name host;
        uint64_t goal_id;
        std::string report;
        EOSLIB_SERIALIZE( struct report, (username)(host)(goal_id)(report))
    };

    // @abi action
    struct gwithdraw{
        account_name username;
        account_name host;
        uint64_t goal_id;
        EOSLIB_SERIALIZE( gwithdraw, (username)(host)(goal_id))
    };    
};






