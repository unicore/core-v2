
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
        	g.benefactor = username;
        	g.created = eosio::time_point_sec(now());
        	g.host = host;
        	g.title = title;
        	g.description = description;
        	g.target = target;
        	g.withdrawed = asset(0, root_symbol);
        	g.available = asset(0, root_symbol);
        	g.validated = validated;
        	g.expired_at = eosio::time_point_sec (now() + op.expiration);
        });
	};

	void fund_goal_action(const dfundgoal &op){
		require_auth(op.architect);
	    // account_name architect;
     //    account_name host;
     //    uint64_t goal_id;
     //    eosio::asset amount;
     //    std::string comment;
    

		account_index accounts(_self, _self);
		auto acc = accounts.find(op.host);
		eosio_assert(acc->architect == op.architect, "Only architect can direct fund the goal");

	    emission_index emis(_self, _self);
        auto emi = emis.find(op.host);

        eosio_assert(emi->fund >= op.amount, "Not enough tokens for fund the goal");

        donate_action(op.architect, op.host, op.goal_id, op.amount, _self);

        emis.modify(emi, op.architect, [&](auto &e){
        	e.fund = e.fund - op.amount;
        });

	};


	void edit_goal_action(const editgoal &op){
		require_auth(op.username);
		
		goals_index goals(_self,op.host);
        account_index accounts (_self, _self);

        auto acc = accounts.find(op.host);
        auto root_symbol = (acc->root_token).symbol;

        auto username = op.username;
        auto benefactor = op.benefactor;
        auto title = op.title;
        auto description = op.description;
        auto host = op.host;
        auto goal_id = op.goal_id;
        auto target = op.target;
        
        eosio_assert(target.symbol == root_symbol, "Wrong symbol for this host");
    
        eosio_assert(title.length() <= 100, "Short Description is a maximum 100 symbols");
 
        auto goal = goals.find(goal_id);
        eosio_assert(goal != goals.end(), "Goal is not exist");

        if (goal->target != op.target){
        	eosio_assert(goal -> validated == false, "Impossible edit amount after validation");
		};

        goals.modify(goal, username, [&](auto &g){
        	g.title = title;
        	g.description = description;
        	g.benefactor = benefactor;
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

		account_index accounts(_self, _self);
		auto acc = accounts.find(op.host);
		eosio_assert(acc != accounts.end(), "Host is not found");

		auto goal = goals.find(goal_id);

		eosio_assert(op.username == goal->benefactor, "Only benefactor can set report");
		
		goals.modify(goal, username, [&](auto &g){
			g.report = report;
			g.reported = true;
		});
	}

	void check_action(const check &op){
		require_auth(op.architect);
		account_index accounts(_self, _self);
		auto acc = accounts.find(op.host);
		eosio_assert(acc != accounts.end(), "Host is not found");

		goals_index goals(_self, op.host);
		auto goal = goals.find(op.goal_id);
		eosio_assert(goal != goals.end(), "Goal is not found");
		eosio_assert(op.architect == acc->architect, "Only architect can check report for now");
		eosio_assert(goal->reported == true, "Goals without reports cannot be checked");

		goals.modify(goal, op.architect, [&](auto &g){
			g.checked = true;
		});
	};

	void donate_action(account_name from, account_name host, uint64_t goal_id, eosio::asset quantity, account_name code){
		// require_auth(from);
		
		goals_index goals(_self, host);
		account_index accounts(_self, _self);
		auto acc = accounts.find(host);

		if (code != _self) //For direct donate from fund and by architects action
			eosio_assert(acc->root_token_contract == code, "Wrong root token contract for this host");
		
		eosio_assert((acc->root_token).symbol == quantity.symbol, "Wrong root symbol for this host");
		
		auto goal = goals.find(goal_id);
		
		eosio_assert(goal != goals.end(), "Goal is not exist");

		bool completed = goal->available + goal->withdrawed + quantity >= goal->target;		
		
		goals.modify(goal, _self, [&](auto &g){
			g.available += quantity;
			g.completed = completed;
		});

	}


	void gsponsor_action(const gsponsor &op){
		require_auth (op.hoperator);

		account_index accounts(_self, _self);
		auto acc = accounts.find(op.host);
		eosio_assert(acc->hoperator == op.hoperator, "Wrong operator for this host");
		goals_index goals(_self, op.host);

		auto goal = goals.find(op.goal_id);
		eosio_assert(goal != goals.end(), "Goal is not founded");
		auto root_symbol = (acc->root_token).symbol;

		eosio_assert(op.amount.symbol == root_symbol, "Wrong root symbol");

		eosio_assert(goal->available >= op.amount, "Not enough tokens for sponsorhip action. ");

		action(
                permission_level{ _self, N(active) },
                acc->root_token_contract, N(transfer),
                std::make_tuple( _self, op.reciever, op.amount, std::string("Sponsor")) 
        ).send();

        goals.modify(goal, op.hoperator, [&](auto &g){
        	g.available = g.available - op.amount;
        	g.withdrawed = g.withdrawed + op.amount;
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
		eosio_assert(acc != accounts.end(), "Host is not founded");

		auto root_symbol = (acc->root_token).symbol;

		eosio_assert(goal->username == username, "You are not owner of this goal");
		eosio_assert((goal->available).amount > 0, "Cannot withdraw a zero amount");

        eosio::asset on_withdraw = goal->available;
    	
    	action(
            permission_level{ _self, N(active) },
		    acc->root_token_contract, N(transfer),
    	    std::make_tuple( _self, goal->benefactor, on_withdraw, std::string("Goal Withdraw")) 
        ).send();

        goals.modify(goal, username, [&](auto &g){
        	g.available = asset(0, root_symbol);
        	g.withdrawed += on_withdraw;
        });

	}

};
