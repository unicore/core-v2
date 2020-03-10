using namespace eosio;

struct shares {

  /**
   * @brief      Создание вестинг-баланса
   * Внутренний метод, используемый при обратном обмене силы сообщества на котировочный токен. 
   * Обеспечивает линейный обмен токенов в продолжительности времени. 
   * @param[in]  owner   The owner
   * @param[in]  amount  The amount
   */
	void make_vesting_action(account_name owner, eosio::asset amount, uint64_t vesting_seconds){
	    eosio_assert(amount.is_valid(), "Amount not valid");
	    // eosio_assert(amount.symbol == _SYM, "Not valid symbol for this vesting contract");
	    eosio_assert(is_account(owner), "Owner account does not exist");
	    
	    vesting_index vests (_self, owner);
	    
	    vests.emplace(_self, [&](auto &v){
	      v.id = vests.available_primary_key();
	      v.owner = owner;
	      v.amount = amount;
	      v.startat = eosio::time_point_sec(now());
	      v.duration = vesting_seconds;
	    });

	}

  /**
   * @brief      Метод обновления баланса.  
   * Обновляет вестинг-баланс до актуальных параметров продолжительности. 
   * @param[in]  op    The operation
   */
  void refresh_action(const refreshsh &op){
    require_auth(op.owner);
    vesting_index vests(_self, op.owner);
    auto v = vests.find(op.id);
    eosio_assert(v != vests.end(), "Vesting object does not exist");
    
    if (eosio::time_point_sec(now() ) > v->startat){
      
      auto elapsed_seconds = (eosio::time_point_sec(now()) - v->startat).to_seconds();
      eosio::asset available;
    
      if( elapsed_seconds < v->duration){
        available = v->amount * elapsed_seconds / v->duration;
      } else {
        available = v->amount;
      }

      available = available - v->withdrawed;
      vests.modify(v, op.owner, [&](auto &m){
      	m.available = available;
      });

    }
  }

  /**
   * @brief      Вывод вестинг-баланса
   * Обеспечивает вывод доступных средств из вестинг-баланса. 
   *
   * @param[in]  op    The operation
   */
  void withdraw_action(const withdrawsh &op){
    require_auth(op.owner);
    vesting_index vests(_self, op.owner);
    auto v = vests.find(op.id);
    eosio_assert(v != vests.end(), "Vesting object does not exist");
    eosio_assert((v->available).amount > 0, "Only positive amount can be withdrawed");
      
    action(
        permission_level{ _self, N(active) },
        N(eosio.token), N(transfer),
        std::make_tuple( _self, op.owner, v->available, std::string("Vesting Withdrawed")) 
    ).send();

    vests.modify(v, op.owner, [&](auto &m){
        m.withdrawed = v->withdrawed + v->available;
        m.available = eosio::asset(0, (v->available).symbol);
      });

    if (v->withdrawed == v->amount){
    	vests.erase(v);
    };
    
  };

  /**
   * @brief      Метод покупки силы сообщества
   * Обеспечивает покупку силы сообщества за котировочный токен по внутренней рыночной цене, определяемой с помощью алгоритма банкор. 
   * @param[in]  buyer   The buyer
   * @param[in]  host    The host
   * @param[in]  amount  The amount
   */
	void buyshares_action ( account_name buyer, account_name host, eosio::asset amount, account_name code ){
		// eosio_assert(amount.symbol == _SYM, "Wrong symbol for buy shares");
		
		account_index accounts(_self, host);
		user_index users(_self,_self);
    auto user = users.find(buyer);
    eosio_assert(user != users.end(), "User is not registered");


		auto exist = accounts.find(host);
		eosio_assert(exist != accounts.end(), "Host is not founded");
		eosio_assert(exist -> quote_token_contract == code, "Wrong quote token contract");
    eosio_assert(exist -> quote_amount.symbol == amount.symbol, "Wrong quote token symbol");
		
    powermarket market(_self, host);
		auto itr = market.find(0);
		auto tmp = *itr;
		uint64_t shares_out;
		market.modify( itr, 0, [&]( auto &es ) {
	    	shares_out = (es.convert( amount, S(0, POWER))).amount;
	    });

        eosio_assert( shares_out > 0, "Amount is not enought for buy 1 share" );

        power_index power(_self, buyer);

        auto pexist = power.find(host);
        if (pexist == power.end()){
	        power.emplace(buyer, [&](auto &p){
	        	p.host = host;
	        	p.power = shares_out;
	        	p.staked = shares_out;	
	        });
		} else {
			propagate_votes_changes(host, buyer, pexist->power, pexist->power + shares_out);
		
			power.modify(pexist, buyer, [&](auto &p){
				p.power += shares_out;
				p.staked += shares_out;
			});
		

		};

		

	};



  /**
   * @brief      Внутренний метод возврата силы при возврате значка.
   * Используется при возврате знака отличия хосту
   *
   * @param[in]  op    The operation
   */

  void back_power_with_badge_action (account_name host, account_name from, uint64_t shares){
    account_index accounts(_self, host);
    auto acc = accounts.find(host);
    eosio_assert(acc != accounts.end(), "Host is not found");
    
    power_index power_to_idx (_self, from);
    auto power_to = power_to_idx.find(host);
      
    //modify
    propagate_votes_changes(host, from, power_to->power, power_to->power - shares);
    
    power_to_idx.modify(power_to, _self, [&](auto &pt){
      pt.power -= shares;
      pt.with_badges -= shares; 
    });

    powermarket market(_self, host);
    auto itr = market.find(0);
    
    market.modify( itr, 0, [&]( auto& es ) {
      es.base.balance = asset((itr -> base).balance.amount + shares, S(0, POWER));
    });

 
  }


  /**
   * @brief      Внутренний метод эмиссии силы.
   * Используется при выдаче знака отличия  
   *
   * @param[in]  op    The operation
   */

  void give_shares_with_badge_action (account_name host, account_name reciever, uint64_t shares){
    account_index accounts(_self, host);
    auto acc = accounts.find(host);
    eosio_assert(acc != accounts.end(), "Host is not found");
    
    power_index power_to_idx (_self, reciever);
    auto power_to = power_to_idx.find(host);
    
    powermarket market(_self, host);

    auto itr = market.find(0);
    
    eosio_assert((itr->base).balance.amount >= shares, "Not enought shares on market");

    
    
    market.modify( itr, 0, [&]( auto& es ) {
      es.base.balance = asset((itr -> base).balance.amount - shares, S(0, POWER));
    });

    //Emplace or modify power object of reciever and propagate votes changes;
    if (power_to == power_to_idx.end()){
      
      power_to_idx.emplace(_self, [&](auto &pt){
        pt.host = host;
        pt.power = shares;
        pt.delegated = 0;
        pt.with_badges = shares;
      });
    
    } else {
      
      //modify
      propagate_votes_changes(host, reciever, power_to->power, power_to->power + shares);
      
      power_to_idx.modify(power_to, _self, [&](auto &pt){
        pt.power += shares;
        pt.with_badges += shares; 
      });

      
    }   
  }

  /**
   * @brief      Метод делегирования силы.
   * Позволяет делегировать купленную силу для принятия каких-либо решений в пользу любого аккаунта. 
   *
   * @param[in]  op    The operation
   */

	void delegate_shares_action (const delshares &op){
	 	
		power_index power_from_idx (_self, op.from);
		power_index power_to_idx (_self, op.reciever);

		delegation_index delegations(_self, op.from);
		
		account_index accounts(_self, op.host);
		auto acc = accounts.find(op.host);

		auto power_from = power_from_idx.find(op.host);
		eosio_assert(power_from != power_from_idx.end(),"Nothing to delegatee");
		eosio_assert(power_from -> power > 0, "Nothing to delegate");
		eosio_assert(op.shares > 0, "Delegate amount must be greater then zero");
		eosio_assert(op.shares <= power_from->staked, "Not enough staked power for delegate");
		
		auto dlgtns = delegations.find(op.reciever);
		auto power_to = power_to_idx.find(op.host);

		if (dlgtns == delegations.end()){

			delegations.emplace(_self, [&](auto &d){
				d.reciever = op.reciever;
				d.shares = op.shares;
			});

		} else {
			delegations.modify(dlgtns, _self, [&](auto &d){
				d.shares += op.shares;
			});
		};

		//modify power object of sender and propagate votes changes;
		propagate_votes_changes(op.host, op.host, power_from->power, power_from->power - op.shares);
		

		power_from_idx.modify(power_from, _self, [&](auto &pf){
			pf.staked -= op.shares;
			pf.power  -= op.shares;

		});

		//Emplace or modify power object of reciever and propagate votes changes;
		if (power_to == power_to_idx.end()){
			power_to_idx.emplace(_self, [&](auto &pt){
				pt.host = op.host;
				pt.power = op.shares;
				pt.delegated = op.shares;		
			});
		} else {
			//modify
			propagate_votes_changes(op.host, op.reciever, power_to->power, power_to->power + op.shares);
			power_to_idx.modify(power_to, _self, [&](auto &pt){
				pt.power += op.shares;
				pt.delegated += op.shares;
			});

			
		}		
	}

  /**
   * @brief      Обновление счетчика голосов
   * Внутренний метод, используемый для обновления голосов у целей в момент покупки/продажи силы. 
   *
   * @param[in]  host       The host
   * @param[in]  voter      The voter
   * @param[in]  old_power  The old power
   * @param[in]  new_power  The new power
   */
	void propagate_votes_changes(account_name host, account_name voter, uint64_t old_power, uint64_t new_power){
		votes_index votes(_self, voter);
		goals_index goals(_self, host);

		//by host;

		auto idx = votes.template get_index<N(host)>();
        auto matched_itr = idx.lower_bound(host);
       
        while(matched_itr != idx.end() && matched_itr->host == host){
			auto goal = goals.find(matched_itr -> goal_id);
				
			goals.modify(goal, _self, [&](auto &g){
				g.total_votes = goal->total_votes - old_power + new_power;
			});



			idx.modify(matched_itr, _self, [&](auto &v){
				v.power = v.power - old_power + new_power;
			});
			matched_itr++;
		};	
	}



  /**
   * @brief      Метод возврата делегированной силы
   *
   * @param[in]  op    The operation
   */
	void undelegate_shares_action (const undelshares &op){
		require_auth(op.reciever);

		power_index power_from_idx (_self, op.from);
		power_index power_to_idx (_self, op.reciever);

		delegation_index delegations(_self, op.reciever);
		auto dlgtns = delegations.find(op.from);

		eosio_assert(dlgtns != delegations.end(), "Nothing to undelegate");
		eosio_assert(dlgtns -> shares >= op.shares, "Not enought shares for undelegate");
		eosio_assert(op.shares > 0, "Undelegate amount must be greater then zero");
		
		auto power_from = power_from_idx.find(op.host);
		auto power_to = power_to_idx.find(op.host);

		if (dlgtns->shares - op.shares == 0){
			delegations.erase(dlgtns);
		} else {
			delegations.modify(dlgtns, op.reciever, [&](auto &d){
				d.shares -= op.shares;
			});
		}

		//modify power object of sender and propagate votes changes;
		propagate_votes_changes(op.host, op.from, power_from->power, power_from->power - op.shares);
		

		power_from_idx.modify(power_from, op.reciever, [&](auto &pf){
			pf.power -= op.shares;
			pf.delegated -= op.shares;

		});

		//modify
		propagate_votes_changes(op.host, op.reciever, power_to->power, power_to->power + op.shares);
		

		power_to_idx.modify(power_to, op.reciever, [&](auto &pt){
			pt.staked += op.shares;
			pt.power  += op.shares;
		});

		
		

	}

  /**
   * @brief      Публичный метод продажи силы рынку за котировочный токен
   *
   * @param[in]  op    The operation
   */
	void sellshares_action ( const sellshares &op ){
		require_auth(op.username);
		auto host = op.host;
		auto username = op.username;
		uint64_t shares = op.shares;

    account_index accounts(_self, host);
    auto exist = accounts.find(host);
    
		power_index power(_self, username);
		auto userpower = power.find(host);
		auto upower = (userpower->staked);
		eosio_assert(upower >= shares, "Not enought power available for sell");

		powermarket market(_self, host);
		auto itr = market.find(0);
		auto tmp = *itr;

		eosio::asset tokens_out;
		market.modify( itr, 0, [&]( auto& es ) {
        	tokens_out = es.convert( asset(shares,S(0, POWER)), exist -> quote_amount.symbol);
	    });
		eosio_assert( tokens_out.amount > 1, "token amount received from selling shares is too low" );
	    
	    make_vesting_action(username, tokens_out, itr->vesting_seconds);
	    
	    propagate_votes_changes(op.host, username, userpower->power, userpower-> power - shares);

	    power.modify(userpower, username, [&](auto &p){
	    	p.power = userpower->power - shares;
	    	p.staked = userpower->staked - shares;
	    });
	    
	};

  /**
   * @brief      Публичный метод создания банкор-рынка.
   *
   * @param[in]  host          The host
   * @param[in]  total_shares  The total shares
   * @param[in]  quote_amount  The quote amount
   */
	void create_bancor_market(std::string name, uint64_t id, account_name host, uint64_t total_shares, eosio::asset quote_amount, account_name quote_token_contract, uint64_t vesting_seconds){
		powermarket market(_self, host);
		auto itr = market.find(id);
		if (itr == market.end()){
			itr = market.emplace( _self, [&]( auto& m ) {
               m.id = id;
               m.vesting_seconds = vesting_seconds;
               m.name = name;
               m.supply.amount = 100000000000000ll;
               m.supply.symbol = S(0,BANCORE);
               m.base.balance.amount = total_shares;
               m.base.balance.symbol = S(0, POWER);
               m.quote.balance.amount = quote_amount.amount;
               m.quote.balance.symbol = quote_amount.symbol;
               m.quote.contract = quote_token_contract;
            });
		} else 
			eosio_assert(false, "Market already created");
	};
};
