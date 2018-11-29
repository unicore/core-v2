
using namespace eosio;

struct goal {

      /*
		Цена входа за цель устанавливается в Лептах. 
		Результативная цена в токенах высчитывается исходя из рейта первых двух пулов. 
		Активация происходит ценой входа. 
		В случае такого проигрыша, что собранная сумма становится меньше активационной - активейтед в фалс.
		Любой может помочь в активации
		Любой может задонатить на цель
		В случае вывода целевого баланса, монеты попадают на баланс цели.
		*/



	uint64_t count_active_goal_balances(account_name username, account_name host){
		balance_index balances(_self, username);
		auto idx = balances.template get_index<N(is_goal)>();
        auto matched_itr = idx.lower_bound(1);
		uint64_t count = 0;

		while(matched_itr != idx.end()){
			if ((matched_itr->host == host) && (matched_itr->withdrawed == false))
				count++;

			matched_itr++;
		}	
	
		return count;
	}

	eosio::asset get_goal_amount(uint64_t lepts_for_each_pool, account_name host){
		account_index accounts(_self, _self);

		auto acc = accounts.find(host);
        auto main_host = acc->get_active_host();

        rate_index rates(_self, main_host);
		spiral_index spiral(_self, main_host);
		
        auto root_symbol = (acc->root_token).symbol;


		auto sp = spiral.find(0);
		eosio_assert(sp != spiral.end(), "Host is not exist");
		
		auto rate1 = rates.find(0);
		auto rate2 = rates.find(1);
		auto buy_rate1 = rate1->buy_rate;
		auto buy_rate2 = rate2->buy_rate;
		
		auto size_of_pool = sp->size_of_pool;
		eosio_assert(lepts_for_each_pool <= size_of_pool / 2, "Lepts for each pool is more then possible" );
		
		eosio::asset amount = asset(lepts_for_each_pool * (buy_rate1 + buy_rate2), root_symbol);
	
		return amount;
	}


	void set_goal_action(const setgoal &op){
		require_auth(op.username);
		
		goals_index goals(_self,op.host);
		account_index accounts(_self, _self);

		auto acc = accounts.find(op.host);
        auto root_symbol = (acc->root_token).symbol;

        //CHECK for exist username in goals. If exist, reject.
		auto username = op.username;
        auto shortdescr = op.shortdescr;
        auto descr = op.descr;
        auto lepts_for_each_pool = op.lepts_for_each_pool;
        auto host = op.host;
        auto target = op.target;
        auto min_amount = get_goal_amount(lepts_for_each_pool, host);
        bool validated = acc->goal_validation_percent == 0;

        
        //TODO CHECK FOR exist goal for same host
  //       auto idx = goals.template get_index<N(username)>();
  //       auto matched_goal = idx.lower_bound( (account_name)username );
		// eosio_assert(matched_goal == idx.end(), "Only one goal per host for each user");
        
        eosio_assert(target.symbol == root_symbol, "Wrong symbol for this host");
        eosio_assert(lepts_for_each_pool > 0, "Lepts for each pool must be greater then 0");

        eosio_assert(shortdescr.length() <= 100, "Short Description is a maximum 100 symbols. Describe the goal shortly.");
       
        goals.emplace(username, [&](auto &g){
        	g.id = goals.available_primary_key();
        	g.username = username;
        	g.created = eosio::time_point_sec(now());
        	g.host = host;
        	g.shortdescr = shortdescr;
        	g.descr = descr;
        	g.activation_amount = min_amount;
        	g.target = target;
        	g.lepts_for_each_pool = lepts_for_each_pool;
        	g.rotation_num = 1;
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
        auto shortdescr = op.shortdescr;
        auto descr = op.descr;
        auto lepts_for_each_pool = op.lepts_for_each_pool;
        auto host = op.host;
        auto goal_id = op.goal_id;
        auto target = op.target;
        auto min_amount = get_goal_amount(lepts_for_each_pool, host);

        //TODO cost < maxcost;
        //TODO CANNOT EDIT AFTER ACTIVATE
        eosio_assert(target.symbol == root_symbol, "Wrong symbol for this host");
        eosio_assert(lepts_for_each_pool > 0, "Lepts for each pool must be greater then 0");

        eosio_assert(shortdescr.length() <= 100, "Short Description is a maximum 100 symbols. Describe the goal shortly.");
        

        auto goal = goals.find(goal_id);
        eosio_assert(goal != goals.end(), "Goal is not exist");
        eosio_assert(goal -> activated == false, "Impossible edit goal after activation");

        goals.modify(goal, username, [&](auto &g){
        	g.shortdescr = shortdescr;
        	g.descr = descr;
        	g.host = host;
        	g.activation_amount = min_amount;
        	g.target = target;
        	g.lepts_for_each_pool = lepts_for_each_pool;
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
		
		//TODO check quantity enough and not overflow the pool size
		goals_index goals(_self, host);
		account_index accounts(_self, _self);
		auto acc = accounts.find(host);

		eosio_assert(acc->root_token_contract == code, "Wrong root token contract for this host");
		eosio_assert((acc->root_token).symbol == quantity.symbol, "Wrong root symbol for this host");
		auto goal = goals.find(goal_id);
		

		eosio_assert(goal != goals.end(), "Goal is not exist");

		//eosio_assert(goal->nominal <= quantity, "Wrong amount for activate");

		bool activated = quantity >= goal->activation_amount;
		bool completed = goal->available + quantity >= goal->target;
		//TODO CHECK IT 
		bool validated = acc->goal_validation_percent <= goal->total_votes / acc->total_shares;

		goals.modify(goal, from,[&](auto &g){
			g.available += quantity;
			g.activated = activated;
			g.completed = completed;
			g.validated = validated;
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
		//eosio_assert(goal->in_protocol == false, "Cannot withdaw goal before goal is go out from Core");
		
		uint64_t gbalances_count = count_active_goal_balances(username, goal->host);
        eosio_assert(gbalances_count == 0, "Cannot withdraw balance until all goal balances withdrawed from Core");
        
        //первый раз вывод до минимума, второй раз - с удалением. 
        if (goal->available > goal->activation_amount){
        	eosio::asset on_withdraw = goal->available - goal->activation_amount;
        	action(
	            permission_level{ _self, N(active) },
   		        acc->root_token_contract, N(transfer),
        	    std::make_tuple( _self, username, on_withdraw, std::string("Goal Withdraw")) 
	        ).send();

	        goals.modify(goal, username, [&](auto &g){
	        	g.available = goal->activation_amount;
	        	g.withdrawed += on_withdraw;
	        });

        } else {
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
	        //goals.erase(goal);
        };

}

};
