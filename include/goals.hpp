
namespace eosio {
    

    // @abi table goals
    struct goals{
        
        uint64_t id;
        uint64_t type;
        eosio::name username;
        eosio::name host;
        eosio::name benefactor;
        eosio::time_point_sec created;
        std::string permlink;
        std::string title;
        std::string description;
        eosio::asset target;
        eosio::asset available;

        int64_t total_votes;
        
        bool validated = false;
        bool completed = false;
        bool reported = false;
        bool checked = false;
        std::string report;
        eosio::asset withdrawed;
        eosio::time_point_sec expired_at;

        std::vector<eosio::name> voters;
        std::string meta;

        uint64_t primary_key()const { return id; }
        uint64_t by_votes() const { return total_votes; }
        uint64_t by_completed() const {return completed; }
        
        eosio::name by_username() const {return username; }
        eosio::name by_host() const {return host;}
        uint128_t by_username_and_host() const { return combine_ids(username.value, host.value); }
        
        EOSLIB_SERIALIZE( goals, (id)(type)(username)(host)(benefactor)(created)(permlink)(title)(description)(target)(available)(total_votes)(validated)(completed)(reported)
            (checked)(report)(withdrawed)(expired_at)(voters)(meta))
    };

    typedef eosio::multi_index <"goals"_n, goals,
        indexed_by<"votes"_n, const_mem_fun<goals, uint64_t, &goals::by_votes>>,
        indexed_by<"completed"_n, const_mem_fun<goals, uint64_t, &goals::by_completed>>,
        indexed_by<"username"_n, const_mem_fun<goals, eosio::name, &goals::by_username>>,
        indexed_by<"host"_n, const_mem_fun<goals, eosio::name, &goals::by_host>>
    > goals_index;


    /* ACTIONS */

    // @abi action
    struct setgoal{
        uint64_t id;
        eosio::name username;
        eosio::name benefactor;
        eosio::name host;
        std::string title;
        std::string permlink;
        std::string description;
        eosio::asset target;
        uint64_t expiration;
        EOSLIB_SERIALIZE( setgoal, (id)(username)(benefactor)(host)(title)(permlink)(description)(target)(expiration))
    };

      // @abi action
    struct editgoal{
        uint64_t goal_id;
        eosio::name username;
        eosio::name host;
        eosio::name benefactor;
        std::string title;
        std::string description;
        eosio::asset target;

        EOSLIB_SERIALIZE( editgoal, (goal_id)(username)(host)(benefactor)(title)(description)(target))

    };


    // @abi action
    struct dfundgoal{
        eosio::name architect;
        eosio::name host;
        uint64_t goal_id;
        eosio::asset amount;
        std::string comment;

        EOSLIB_SERIALIZE( dfundgoal, (architect)(host)(goal_id)(amount)(comment))
    
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

    //@abi action
    struct check{
        eosio::name architect;
        eosio::name host;
        uint64_t goal_id;

        EOSLIB_SERIALIZE(check, (architect)(host)(goal_id))
    };

    // @abi action
    struct gwithdraw{
        eosio::name username;
        eosio::name host;
        uint64_t goal_id;
        EOSLIB_SERIALIZE( gwithdraw, (username)(host)(goal_id))
    };   


    // @abi action
    struct gsponsor{
        eosio::name hoperator;
        eosio::name host;
        eosio::name reciever;
        uint64_t goal_id;
        eosio::asset amount;
        EOSLIB_SERIALIZE(gsponsor, (hoperator)(host)(reciever)(goal_id)(amount))
    };
};






