
using namespace eosio;

struct goal {

/*
Модуль Целей
*/


	void set_goal_action(const setgoal &op){
		require_auth(op.username);
		
		goals_index goals(_self,op.host);
		account_index accounts(_self, _self);

		user_index users(_self,_self);
        auto user = users.find(op.username);
        eosio_assert(user != users.end(), "User is not registered");

		auto acc = accounts.find(op.host);
        auto root_symbol = (acc->root_token).symbol;

		auto username = op.username;
        auto title = op.title;
        auto description = op.description;
        auto host = op.host;
        auto target = op.target;
        bool validated = acc->consensus_percent == 0;
        
        eosio_assert(target.symbol == root_symbol, "Wrong symbol for this host");
        
        eosio_assert(title.length() <= 100, "Short Description is a maximum 100 symbols. Describe the goal shortly.");
       
        goals.emplace(username, [&](auto &g){
        	g.id = goals.available_primary_key();
        	g.username = username;
        	g.created = eosio::time_point_sec(now());
        	g.host = host;
        	g.title = title;
        	g.description = description;
        	g.target = target;
        	g.withdrawed = asset(0, root_symbol);
        	g.available = asset(0, root_symbol);
        	g.validated = validated;
        });
	};

	void edit_goal_action(const editgoal &op){
		require_auth(op.username);
		
		goals_index goals(_self,op.host);
        account_index accounts (_self, _self);

        auto acc = accounts.find(op.host);
        auto root_symbol = (acc->root_token).symbol;

        auto username = op.username;
        auto title = op.title;
        auto description = op.description;
        auto host = op.host;
        auto goal_id = op.goal_id;
        auto target = op.target;
        
        eosio_assert(target.symbol == root_symbol, "Wrong symbol for this host");
    
        eosio_assert(title.length() <= 100, "Short Description is a maximum 100 symbols");
 
        auto goal = goals.find(goal_id);
        eosio_assert(goal != goals.end(), "Goal is not exist");

        eosio_assert(goal -> validated == false, "Impossible edit goal after validation");

        goals.modify(goal, username, [&](auto &g){
        	g.title = title;
        	g.description = description;
        	g.host = host;
        	g.target = target;
        });

	}

	void del_goal_action(const delgoal &op){
		require_auth(op.username);
		goals_index goals(_self, op.host);
        
		auto username = op.username;
		auto goal_id = op.goal_id;

		auto goal = goals.find(goal_id);
		eosio_assert(goal != goals.end(), "Goal is not exist");
        
		goals.erase(goal);

	}


	void report_action(const report &op){
		require_auth(op.username);
		
		goals_index goals(_self, op.host);
		auto username = op.username;
		auto goal_id = op.goal_id;
		auto report = op.report;
		auto goal = goals.find(goal_id);

		goals.modify(goal, username, [&](auto &g){
			g.report = report;
			g.reported = true;
		});
	}


	void donate_action(account_name from, account_name host, uint64_t goal_id, eosio::asset quantity, account_name code){
		require_auth(from);
		
		goals_index goals(_self, host);
		account_index accounts(_self, _self);
		auto acc = accounts.find(host);

		eosio_assert(acc->root_token_contract == code, "Wrong root token contract for this host");
		eosio_assert((acc->root_token).symbol == quantity.symbol, "Wrong root symbol for this host");
		
		auto goal = goals.find(goal_id);
		
		eosio_assert(goal != goals.end(), "Goal is not exist");

		bool completed = goal->available + goal->withdrawed + quantity >= goal->target;		
		
		goals.modify(goal, from,[&](auto &g){
			g.available += quantity;
			g.completed = completed;
		});

	}

	void gwithdraw_action(const gwithdraw &op){
		require_auth(op.username);
		auto username = op.username;
		auto goal_id = op.goal_id;
		
		goals_index goals(_self, op.host);
		auto goal = goals.find(goal_id);
		
		eosio_assert(goal != goals.end(), "Goal is not founded");
		
		account_index accounts(_self, _self);
		auto acc = accounts.find(goal->host);
		auto root_symbol = (acc->root_token).symbol;

		eosio_assert(goal->username == username, "You are not owner of this goal");
		eosio_assert((goal->available).amount > 0, "Cannot withdraw a zero amount");

        eosio::asset on_withdraw = goal->available;
    	
    	action(
            permission_level{ _self, N(active) },
		    acc->root_token_contract, N(transfer),
    	    std::make_tuple( _self, username, on_withdraw, std::string("Goal Withdraw")) 
        ).send();

        goals.modify(goal, username, [&](auto &g){
        	g.available = asset(0, root_symbol);
        	g.withdrawed += on_withdraw;
        });

	}

};
