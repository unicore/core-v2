
namespace eosio {
    

    // @abi table goals
    struct goals{
        uint64_t id;
        account_name username;
        bool activated = false;
        bool completed = false;
        account_name host;
        std::string shortdescr;
        std::string descr;
        eosio::asset cost;
        eosio::asset nominal;
        eosio::asset collected;
        bool reported = false;
        std::string report;
        bool validated = false;
        
        uint64_t primary_key()const { return id; }
        
        EOSLIB_SERIALIZE( goals, (id)(username)(activated)(completed)(host)(shortdescr)(descr)(cost)(nominal)(collected)(reported)(report)(validated))
    };

    typedef eosio::multi_index <N(goals), goals> goals_index;
    //by activated //by username

    // @abi table chain i64
    struct chain{
        uint64_t id;
        account_name username;
        uint64_t goal_id;
        bool in_protocol = false;
        uint64_t primary_key() const {return id;}

        EOSLIB_SERIALIZE(chain, (id)(username)(goal_id)(in_protocol))
    };

    typedef eosio::multi_index<N(chain), chain> chain_index;


    /* ACTIONS */

    // @abi action
    struct setgoal{
        account_name username;
        account_name host;
        std::string shortdescr;
        std::string descr;
        eosio::asset cost;

        EOSLIB_SERIALIZE( setgoal, (username)(host)(shortdescr)(descr)(cost))
    };

      // @abi action
    struct editgoal{
        account_name username;
        account_name host;
        uint64_t goal_id;
        std::string shortdescr;
        std::string descr;
        eosio::asset cost;

        EOSLIB_SERIALIZE( editgoal, (username)(host)(goal_id)(shortdescr)(descr)(cost))

    };

    // @abi action
    struct delgoal{
        account_name username;
        uint64_t goal_id;
        EOSLIB_SERIALIZE( delgoal, (username)(goal_id))
    };

    // @abi action
    struct report{
        account_name username;
        uint64_t goal_id;
        std::string report;
        EOSLIB_SERIALIZE( struct report, (username)(goal_id)(report))
    };

    // // @abi action
    // struct withdraw{
    //     account_name username;
    //     uint64_t goal_id;
    //     EOSLIB_SERIALIZE( withdraw, (username)(goal_id))
    // };    
};






