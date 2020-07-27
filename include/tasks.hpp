

	struct [[eosio::table, eosio::contract("unicore")]] tasks{
		uint64_t task_id;
		uint64_t goal_id;
        eosio::name creator;
        
        uint64_t type;
		uint64_t priority;
		uint64_t period;
		bool is_public = true;
		eosio::string title;
		eosio::string data;
		eosio::asset requested;
		eosio::asset funded;
		eosio::asset remain;
		eosio::asset for_each;
		eosio::name curator;
		bool with_badge;
		uint64_t badge_id;
		bool validated = false;
		bool completed = false;
		bool active = true;
		eosio::time_point_sec expired_at;

		uint64_t primary_key()const { return task_id; }

        uint64_t bygoal() const {return goal_id; }
        uint128_t goalandtask() const { return eosio::combine_ids(goal_id, task_id); }
        uint64_t bytype() const {return type; }
        uint64_t bypriority() const {return priority; }
        uint64_t byhasbadge() const {return with_badge; }
        uint64_t bybadge() const {return badge_id; }
        uint128_t crewithtask() const { return eosio::combine_ids(creator.value, task_id); }
        uint128_t crewithgoal() const { return eosio::combine_ids(creator.value, goal_id); }

	    EOSLIB_SERIALIZE( tasks, (task_id)(goal_id)(creator)(type)(priority)(period)(is_public)(title)(data)(requested)(funded)(remain)(for_each)(curator)(with_badge)(badge_id)(validated)(completed)(active)(expired_at))
    };

    typedef eosio::multi_index< "tasks"_n, tasks,
        eosio::indexed_by<"bygoal"_n, eosio::const_mem_fun<tasks, uint64_t, &tasks::bygoal>>,
        eosio::indexed_by<"goalandtask"_n, eosio::const_mem_fun<tasks, uint128_t, &tasks::goalandtask>>,
        eosio::indexed_by<"bytype"_n, eosio::const_mem_fun<tasks, uint64_t, &tasks::bytype>>,
        eosio::indexed_by<"bypriority"_n, eosio::const_mem_fun<tasks, uint64_t, &tasks::bypriority>>,
        eosio::indexed_by<"byhasbadge"_n, eosio::const_mem_fun<tasks, uint64_t, &tasks::byhasbadge>>,
        eosio::indexed_by<"bybadge"_n, eosio::const_mem_fun<tasks, uint64_t, &tasks::bybadge>>,
        eosio::indexed_by<"crewithtask"_n, eosio::const_mem_fun<tasks, uint128_t, &tasks::crewithtask>>,
        eosio::indexed_by<"crewithgoal"_n, eosio::const_mem_fun<tasks, uint128_t, &tasks::crewithgoal>>
        
        
    > tasks_index;


    struct [[eosio::table, eosio::contract("unicore")]] reports{
    	uint64_t report_id;
    	uint64_t task_id; 
    	uint64_t goal_id;
        uint64_t type;
    	eosio::name username;
    	eosio::name curator;
    	eosio::string data;
    	eosio::asset requested;
        eosio::asset balance;
        eosio::asset withdrawed;
    	bool need_check = true;
    	bool approved = false;
    	eosio::string comment;
        eosio::time_point_sec expired_at;
    	
        uint64_t primary_key() const {return report_id;}
		
        uint128_t userwithtask() const { return eosio::combine_ids(username.value, task_id); }

    	EOSLIB_SERIALIZE(reports, (report_id)(task_id)(goal_id)(type)(username)(curator)(data)(requested)(balance)(withdrawed)(need_check)(approved)(comment)(expired_at))
    };

    typedef eosio::multi_index< "reports"_n, reports,
    	eosio::indexed_by<"userwithtask"_n, eosio::const_mem_fun<reports, uint128_t, 
                              &reports::userwithtask>>
    > reports_index;
