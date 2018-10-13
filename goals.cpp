#include <eosiolib/eosio.hpp>
#include <eosiolib/asset.hpp>
#include <eosiolib/time.hpp>
#include <eosiolib/print.hpp>

using namespace eosio;

struct goal {
//TODO goalenter
    
	void set_goal_action(const setgoal &op){
		require_auth(op.username);
		
		goals_index goals(_self,_self);
        //CHECK for exist username in goals. If exist, reject.
		auto username = op.username;
        auto shortdescr = op.shortdescr;
        auto descr = op.descr;
        auto cost = op.cost;
        auto host = op.host;
        //TODO cost < maxcost;

        eosio_assert(shortdescr.length() <= 100, "Short Description is a maximum 100 symbols. Describe the goal shortly.");
        eosio_assert ( cost.symbol == _SYM, "Rejected. Invalid symbol for this contract.");
       
        goals.emplace(username, [&](auto &g){
        	g.id = goals.available_primary_key();
        	g.username = username;
        	g.host = host;
        	g.shortdescr = shortdescr;
        	g.descr = descr;
        	g.cost = cost;
        });
	};

	void edit_goal_action(const editgoal &op){
		require_auth(op.username);
		
		goals_index goals(_self,_self);
        
		auto username = op.username;
        auto shortdescr = op.shortdescr;
        auto descr = op.descr;
        auto cost = op.cost;
        auto goal_id = op.goal_id;
        eosio_assert ( cost.symbol == _SYM, "Rejected. Invalid symbol for this contract.");
        eosio_assert(shortdescr.length() <= 100, "Short Description is a maximum 100 symbols. Describe the goal shortly.");

        auto goal = goals.find(goal_id);
        eosio_assert(goal != goals.end(), "Goal is not exist");
        eosio_assert(goal -> activated == false, "Impossible edit goal after activation");


        //TODO проверка на существование

        goals.modify(goal, username, [&](auto &g){
        	g.shortdescr = shortdescr;
        	g.descr = descr;
        	g.cost = cost;
        });

	}

	void del_goal_action(const delgoal &op){
		require_auth(op.username);
		goals_index goals(_self,_self);
        
		auto username = op.username;
		auto goal_id = op.goal_id;

		auto goal = goals.find(goal_id);
		eosio_assert(goal != goals.end(), "Goal is not exist");
        
		goals.erase(goal);

	}

	void report_action(const report &op){
		require_auth(op.username);
		
		goals_index goals(_self,_self);
		auto username = op.username;
		auto goal_id = op.goal_id;
		auto report = op.report;
		auto goal = goals.find(goal_id);

		goals.modify(goal, username, [&](auto &g){
			g.report = report;
			g.reported = true;
		});
	}

	void activate_action(account_name from, uint64_t goal_id, eosio::asset quantity){
		require_auth(from);
		
		//TODO check quantity for pool size
		goals_index goals(_self, _self);
		chain_index chain(_self, _self);
		print(goal_id, " ");
		auto goal = goals.find(goal_id);

		eosio_assert(goal != goals.end(), "Goal is not exist");

		auto host = goal->host;		
		rate_index rates(_self, host);
		spiral_index spiral(_self, host);
		
		

		auto sp = spiral.find(0);
		eosio_assert(sp != spiral.end(), "Host is not exist");
		
		auto rate1 = rates.find(0);
		auto rate2 = rates.find(1);
		auto buy_rate1 = rate1->buy_rate;
		auto buy_rate2 = rate2->buy_rate;
		
		auto size_of_pool = sp->size_of_pool;
		        
		eosio::asset amount = asset(size_of_pool / 2 * (buy_rate1 + buy_rate2), _SYM);
		print(amount);
		eosio_assert(amount == quantity, "Wrong amount for activate");

		goals.modify(goal, from,[&](auto &g){
			g.collected = amount;
			g.nominal = amount;
			g.activated = true;
		});

		chain.emplace(from, [&](auto &c){
			c.id = chain.available_primary_key();
			c.goal_id = goal_id;
			c.username = from;
		});

	}

	void withdraw(const withdraw &op){
		require_auth(op.username);

		//TODO can withdraw if activated = true, but not in the head
		//TODO if not completed - back nominal or sediment
		//TODO что делать с выигрышем, если цель не достигнута, но превышает номинал? Перечислять в core
		//TODO can withdraw if activated = false, completed = true

	}

	void adjust_balance(){

		//TODO в протоколе вывод должен совершаться на баланс пользователя в целях
				
 		//If completed, set complete flag=true and activated=false. 
 		//Move goal to completed. Without validated report cant do again. 
		//Здесь мы принимаем платежи от Протокола и вносим их на баланс цели. 
	};

	// @abi action
	void next(){
		require_auth(_self);
		goals_index goals(_self, _self);
		chain_index chain(_self, _self);

		auto ch_goal = chain.cbegin();
		
		if (ch_goal != chain.end()){
			if (ch_goal -> in_protocol == true){
				chain.emplace(_self, [&](auto &c){
					c.id = chain.available_primary_key();
					c.username = ch_goal->username;
					c.goal_id = ch_goal -> goal_id;
				});
				
				chain.erase(ch_goal);
				auto ch_goal_next = chain.cbegin();
				//PAYER to _self
				chain.modify(ch_goal_next, _self, [&](auto &c){
					c.in_protocol = true;
				});
				auto goal = goals.find(ch_goal_next->goal_id);
				print("must send to protocol1: ", goal->nominal);
				
				//TODO отправка нового чела в протокол
			} else {

				auto goal = goals.find(ch_goal->goal_id);

				chain.modify(ch_goal, 0, [&](auto &c){
					c.in_protocol = true;
				});
				print("im make enter");
				// action(
		  //           permission_level{ _self, N(active) },
		  //           _self, N(gpriorenter),
		  //           std::make_tuple(goal->username, goal->host)
		  //       ).send();

				// print("i want send");
		  //        action(
	   //              permission_level{ _self, N(active) },
	   //              N(eosio.token), N(transfer),
	   //              std::make_tuple( _self, _self, goal->nominal, std::string("null")) 
		  //       ).send();

		         print("finish send");
				print("must send to protocol2: ", goal->nominal);
				//TODO проверка и отправка в протокол по приоритетному входу

			}
		}
		
	}
};
