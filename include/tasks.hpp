
namespace eosio {


	//@abi table tasks
	struct tasks{
		uint64_t task_id;
		uint64_t goal_id;
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
		bool validated = true;
		bool completed = false;
		bool active = true;
		eosio::time_point_sec expired_at;

		uint64_t primary_key()const { return task_id; }

	    EOSLIB_SERIALIZE( tasks, (task_id)(goal_id)(type)(priority)(period)(is_public)(title)(data)(requested)(funded)(remain)(for_each)(curator)(with_badge)(badge_id)(validated)(completed)(active)(expired_at))
    };

    typedef eosio::multi_index< "tasks"_n, tasks> tasks_index;


    //@abi table reports
    struct reports{
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
		  uint128_t userwithtask() const { return combine_ids(username.value, task_id); }

    	EOSLIB_SERIALIZE(reports, (report_id)(task_id)(goal_id)(type)(username)(curator)(data)(requested)(balance)(withdrawed)(need_check)(approved)(comment)(expired_at))
    };

    typedef eosio::multi_index< "reports"_n, reports,
    	indexed_by<"userwithtask"_n, const_mem_fun<reports, uint128_t, 
                              &reports::userwithtask>>
    > reports_index;


	//@abi action
	struct settask {
		eosio::name host;
    eosio::name username;
		uint64_t goal_id;
		eosio::string title;
		eosio::string data;
		eosio::asset requested;
		bool is_public;
		eosio::asset for_each;
		bool with_badge;
		uint64_t badge_id;
		uint64_t expiration;
	};

	//@abi action
	struct fundtask {
		eosio::name host;
		uint64_t task_id;
		eosio::asset amount;
		eosio::string comment;
	};

	//@abi action
	struct tactivate
	{
		eosio::name host;
		uint64_t task_id;
	};
	
	//@abi action
	struct tdeactivate
	{
		eosio::name host;
		uint64_t task_id;
	};

	//@abi action
	struct setreport {
		eosio::name host;
		eosio::name username;
		uint64_t task_id;
		eosio::string data;
	};

	//@abi action
	struct editreport{
		eosio::name host;
		eosio::name username;
		uint64_t report_id;
		eosio::string data;
	};

	//@abi action
	struct approver{
		eosio::name host;
		uint64_t report_id;
		eosio::string comment;

	};

	//@abi action
	struct disapprover{
		eosio::name host;
		uint64_t report_id;
		eosio::string comment;
	};
}