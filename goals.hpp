
namespace eosio {
    

    // @abi table goals
    struct goals{
        
        uint64_t id;
        account_name username;
        account_name host;
        account_name benefactor;
        eosio::time_point_sec created;

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

        std::vector<account_name> voters;
        
        uint64_t primary_key()const { return id; }
        uint64_t by_votes() const { return total_votes; }
        uint64_t by_completed() const {return completed; }
        
        account_name by_username() const {return username; }
        account_name by_host() const {return host;}
        
        
        EOSLIB_SERIALIZE( goals, (id)(username)(host)(benefactor)(created)(title)(description)(target)(available)(total_votes)(validated)(completed)(reported)
            (checked)(report)(withdrawed)(expired_at)(voters))
    };

    typedef eosio::multi_index <N(goals), goals,
        indexed_by<N(total_votes), const_mem_fun<goals, uint64_t, &goals::by_votes>>,
        indexed_by<N(completed), const_mem_fun<goals, uint64_t, &goals::by_completed>>,
        indexed_by<N(username), const_mem_fun<goals, account_name, &goals::by_username>>,
        indexed_by<N(host), const_mem_fun<goals, account_name, &goals::by_host>>
    > goals_index;


    /* ACTIONS */

    // @abi action
    struct setgoal{
        account_name username;
        account_name host;
        std::string title;
        std::string description;
        eosio::asset target;
        uint64_t expiration;
        EOSLIB_SERIALIZE( setgoal, (username)(host)(title)(description)(target)(expiration))
    };

      // @abi action
    struct editgoal{
        uint64_t goal_id;
        account_name username;
        account_name host;
        account_name benefactor;
        std::string title;
        std::string description;
        eosio::asset target;

        EOSLIB_SERIALIZE( editgoal, (goal_id)(username)(host)(benefactor)(title)(description)(target))

    };
    // @abi action
    struct dfundgoal{
        account_name architect;
        account_name host;
        uint64_t goal_id;
        eosio::asset amount;
        std::string comment;

        EOSLIB_SERIALIZE( dfundgoal, (architect)(host)(goal_id)(amount)(comment))
    
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

    //@abi action
    struct check{
        account_name architect;
        account_name host;
        uint64_t goal_id;

        EOSLIB_SERIALIZE(check, (architect)(host)(goal_id))
    };

    // @abi action
    struct gwithdraw{
        account_name username;
        account_name host;
        uint64_t goal_id;
        EOSLIB_SERIALIZE( gwithdraw, (username)(host)(goal_id))
    };   


    // @abi action
    struct gsponsor{
        account_name hoperator;
        account_name host;
        account_name reciever;
        uint64_t goal_id;
        eosio::asset amount;
        EOSLIB_SERIALIZE(gsponsor, (hoperator)(host)(reciever)(goal_id)(amount))
    };
};






