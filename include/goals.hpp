
/*!
   \brief Структура целей хоста Двойной Спирали.
*/

    struct [[eosio::table, eosio::contract("unicore")]]  goals {
        uint64_t id;
        uint64_t parent_id;
        eosio::name type;
        eosio::name creator;
        eosio::name benefactor;
        eosio::name host;
        eosio::name status;
        bool is_batch = false;
        std::vector<uint64_t> batch;
        uint64_t benefactors_weight;
        eosio::time_point_sec created;
        eosio::time_point_sec start_at;
        eosio::time_point_sec finish_at;
        eosio::time_point_sec expired_at;
        uint64_t duration;
        uint64_t priority;
        uint64_t cashback;
        uint64_t participants_count;
        std::string parent_permlink;
        std::string permlink;
        std::string title;
        std::string description;
        
        eosio::asset target;
        uint64_t debt_count;
        eosio::asset target1;
        eosio::asset target2;
        eosio::asset target3;
        eosio::asset available;

        int64_t total_votes;
        uint64_t total_tasks;
        bool validated = false;
        bool activated = false;
        bool filled = false;
        bool reported = false;
        bool checked = false;
        bool comments_is_enabled = true;
        eosio::name who_can_create_tasks;
        std::string report;
        eosio::asset withdrawed;

        std::vector<eosio::name> voters;
        std::string meta;
        uint64_t gifted_badges = 0;
        uint64_t gifted_power = 0;
        uint64_t reports_count = 0;
        bool is_encrypted = false;
        std::string public_key;

        uint64_t primary_key()const { return id; }
        // double byvotes() const { 
        //     return (double)total_votes; 
        // }
        
        uint64_t byvotes() const { 
            return pow(2, 63) + total_votes;
        }

        uint64_t bytype() const {return type.value;}
        uint64_t bystatus() const {return status.value;}
        uint64_t byparentid() const {return parent_id;}

        uint64_t byfilled() const {return filled; }
        uint64_t bycreated() const {return created.sec_since_epoch(); }
        uint64_t bypriority() const {return priority; }
        uint64_t byusername() const {return creator.value; }
        uint64_t byhost() const {return host.value;}
        uint128_t by_username_and_host() const { return combine_ids(creator.value, host.value); }
        
        EOSLIB_SERIALIZE( goals, (id)(parent_id)(type)(creator)(benefactor)(host)(status)(is_batch)(batch)(benefactors_weight)(created)(start_at)(finish_at)(expired_at)(duration)(priority)(cashback)(participants_count)(parent_permlink)(permlink)(title)(description)(target)(debt_count)(target1)(target2)(target3)(available)(total_votes)(total_tasks)(validated)(activated)(filled)(reported)
            (checked)(comments_is_enabled)(who_can_create_tasks)(report)(withdrawed)(voters)(meta)(gifted_badges)(gifted_power)(reports_count)(is_encrypted)(public_key))
    };

    typedef eosio::multi_index <"goals"_n, goals,
        eosio::indexed_by<"byhost"_n, eosio::const_mem_fun<goals, uint64_t, &goals::byhost>>,
        eosio::indexed_by<"bystatus"_n, eosio::const_mem_fun<goals, uint64_t, &goals::bystatus>>,
        eosio::indexed_by<"type"_n, eosio::const_mem_fun<goals, uint64_t, &goals::bytype>>,
        eosio::indexed_by<"votes"_n, eosio::const_mem_fun<goals, uint64_t, &goals::byvotes>>,
        eosio::indexed_by<"byparentid"_n, eosio::const_mem_fun<goals, uint64_t, &goals::byparentid>>,
        eosio::indexed_by<"filled"_n, eosio::const_mem_fun<goals, uint64_t, &goals::byfilled>>,
        eosio::indexed_by<"creator"_n, eosio::const_mem_fun<goals, uint64_t, &goals::byusername>>,
        eosio::indexed_by<"created"_n, eosio::const_mem_fun<goals, uint64_t, &goals::bycreated>>,
        eosio::indexed_by<"bypriority"_n, eosio::const_mem_fun<goals, uint64_t, &goals::bypriority>>
        
    > goals_index;


/*!
   \brief Структура участников цели хоста Двойной Спирали
*/

    struct [[eosio::table, eosio::contract("unicore")]]  goalspartic {
        uint64_t id;
        uint64_t goal_id;
        eosio::name username;
        eosio::name role;
        bool completed = false;
        eosio::time_point_sec expiration;
        
        uint64_t primary_key()const { return id; }
        uint128_t byusergoal() const { return combine_ids(username.value, goal_id); }
        
        uint64_t byusername()const {return username.value;}
        uint64_t byrole() const {return role.value;}

        EOSLIB_SERIALIZE( goalspartic, (id)(goal_id)(username)(role)(completed)(expiration))

    };

    typedef eosio::multi_index <"goalspartic"_n, goalspartic,
        eosio::indexed_by<"byusergoal"_n, eosio::const_mem_fun<goalspartic, uint128_t, &goalspartic::byusergoal>>,
        eosio::indexed_by<"byusername"_n, eosio::const_mem_fun<goalspartic, uint64_t, &goalspartic::byusername>>,
        eosio::indexed_by<"byrole"_n, eosio::const_mem_fun<goalspartic, uint64_t, &goalspartic::byrole>>
    > goalspartic_index;


/*!
   \brief Структура бенефакторов цели хоста Двойной Спирали
*/

    struct [[eosio::table, eosio::contract("unicore")]] benefactors {
        uint64_t id;
        uint64_t goal_id;
        eosio::name benefactor;
        uint64_t weight;
        eosio::asset income;
        uint128_t income_in_segments;
        eosio::asset withdrawed;
        eosio::asset income_limit;
        uint64_t primary_key() const {return id;}  
        uint64_t bygoalid() const {return goal_id;}

        uint128_t bybengoal() const { return combine_ids(benefactor.value, goal_id); }
        
        EOSLIB_SERIALIZE(benefactors, (id)(goal_id)(benefactor)(weight)(income)(income_in_segments)(withdrawed)(income_limit))      
    };

    typedef eosio::multi_index <"benefactors"_n, benefactors,
    eosio::indexed_by<"bygoalid"_n, eosio::const_mem_fun<benefactors, uint64_t, &benefactors::bygoalid>>,
    eosio::indexed_by<"bybengoal"_n, eosio::const_mem_fun<benefactors, uint128_t, &benefactors::bybengoal>>
    > benefactors_index;


