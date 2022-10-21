using namespace eosio;
// namespace eosio {
  /**
   * @brief      Создание вестинг-баланса
   * Внутренний метод, используемый при обратном обмене силы сообщества на котировочный токен. 
   * Обеспечивает линейную выдачу жетонов в продолжительности времени. 
   * @param[in]  owner   The owner
   * @param[in]  amount  The amount
   */

	void unicore::make_vesting_action(eosio::name owner, eosio::name host, eosio::name contract, eosio::asset amount, uint64_t vesting_seconds, eosio::name type){
	    eosio::check(amount.is_valid(), "Amount not valid");
	    // eosio::check(amount.symbol == _SYM, "Not valid symbol for this vesting contract");
	    eosio::check(is_account(owner), "Owner account does not exist");
	    
	    vesting_index vests (_me, owner.value);
	    
	    vests.emplace(_me, [&](auto &v){
	      v.id = vests.available_primary_key();
        v.type = type;
        v.host = host;
	      v.owner = owner;
        v.contract = contract;
	      v.amount = amount;
        v.available = asset(0, amount.symbol);
        v.withdrawed = asset(0, amount.symbol);
	      v.startat = eosio::time_point_sec(eosio::current_time_point().sec_since_epoch());
	      v.duration = vesting_seconds;
	    });

	}


  
  void add_base_to_market(eosio::name id, eosio::name host, eosio::asset amount){
      
      market_index market(_me, host.value);
      auto itr = market.find(id.value);
        
      market.modify(itr, _me, [&](auto &m){
        m.base.balance += amount;
      });
  }
  
  void add_quote_to_market(eosio::name id, eosio::name host, eosio::asset amount){
      
      market_index market(_me, host.value);
      auto itr = market.find(id.value);
        
      market.modify(itr, _me, [&](auto &m){
        m.quote.balance += amount;
      });
  }

  void sub_base_from_market(eosio::name id, eosio::name host, eosio::asset amount) {
      
      market_index market(_me, host.value);

      auto itr = market.find(id.value);
      
      eosio::check(itr -> base.balance >= amount, "Cannot substrate base balance");

      market.modify(itr, _me, [&](auto &m){
        m.base.balance -= amount;
      });
  }
  
  void sub_quote_from_market(eosio::name id, eosio::name host, eosio::asset amount){
      
      market_index market(_me, host.value);
      auto itr = market.find(id.value);
      
      eosio::check(itr -> quote.balance >= amount, "Cannot substrate quote balance");

      market.modify(itr, _me, [&](auto &m){
        m.quote.balance -= amount;
      });
  }


  /**
   * @brief      Метод эмиссии силы с аукциона
   * 
   * @param[in]  op    The operation
   */
  [[eosio::action]] void unicore::emitpower(eosio::name host, eosio::name username, uint64_t user_shares){
    eosio::check(has_auth("auction"_n) || has_auth(_me), "missing required authority");
    print("on power emit: ", user_shares);
    
    account_index account(_me, host.value);
    auto acc = account.find(host.value);
    

    // eosio::check(acc -> sale_mode == "auction"_n, "Wrong sale type");
    
    // action(
    //     permission_level{ _saving, "active"_n },
    //     acc->root_token_contract, "transfer"_n,
    //     std::make_tuple( _saving, _me, asset(10000 * user_shares, acc -> quote_amount.symbol), std::string("")) 
    // ).send();


    // add_base_to_market("power"_n, host, asset(user_shares, _POWER));
    // add_quote_to_market("power"_n, host, asset(10000 * user_shares, acc -> quote_amount.symbol));

    account.modify(acc, _me, [&](auto &a){
      a.total_shares += user_shares;
      // a.quote_amount += asset(10000 * user_shares, acc -> quote_amount.symbol);
    });

    power3_index power(_me, host.value);
    auto pexist = power.find(username.value);
    
    if (pexist == power.end()) {

      power.emplace(_me, [&](auto &p) {
        p.username = username;
        p.power = user_shares;
        p.staked = user_shares;    
      });

        
    } else {
        unicore::propagate_votes_changes(host, username, pexist->power, pexist->power + user_shares);
        
        power.modify(pexist, _me, [&](auto &p) {
            p.power += user_shares;
            p.staked += user_shares;
        });
    };


  }



  /**
   * @brief      Метод эмиссии силы с аукциона на цель
   * 
   * @param[in]  op    The operation
   */

  [[eosio::action]] void unicore::emitpower2(eosio::name host, uint64_t goal_id, uint64_t shares) {
    eosio::check(has_auth("auction"_n) || has_auth(_me), "missing required authority");

    account_index account(_me, host.value);
    auto acc = account.find(host.value);
    
    // eosio::check(acc -> sale_mode == "auction"_n, "Wrong sale type");
    
    action(
        permission_level{ _saving, "active"_n },
        acc->root_token_contract, "transfer"_n,
        std::make_tuple( _saving, _me, asset(10000 * shares, acc -> quote_amount.symbol), std::string("")) 
    ).send();


    add_base_to_market("power"_n, host, asset(shares, _POWER));
    add_quote_to_market("power"_n, host, asset(10000 * shares, acc -> quote_amount.symbol));

    account.modify(acc, _me, [&](auto &a){
      a.total_shares += shares;
      a.quote_amount += asset(10000 * shares, acc -> quote_amount.symbol);
    });

    goals_index goals(_me, host.value);
    auto goal = goals.find(goal_id);
    goals.modify(goal, _me, [&](auto &g){
      g.total_power_on_distribution += shares;
      g.remain_power_on_distribution += shares;
    });
  }



  /**
   * @brief      Публичный метод продажи силы рынку за котировочный токен
   *
   * @param[in]  op    The operation
   */
  [[eosio::action]] void unicore::sellshares(eosio::name username, eosio::name host, uint64_t shares){
    require_auth(username);
    
    power3_index power(_me, host.value);
    auto pexist = power.find(username.value);
    
    account_index account(_me, host.value);
    auto acc = account.find(host.value);


    uint64_t max_power_on_sale = pexist -> staked; 

    eosio::check(max_power_on_sale >= shares, "Cannot back more power then you have");

    if (pexist != power.end() && pexist -> staked > 0) {
      
      
      // double available = (double)shares / (double)acc -> total_shares * (double)acc->quote_amount.amount;
      // eosio::asset available_asset = asset((uint64_t)available, acc -> quote_amount.symbol);

      market_index market(_me, host.value);
      auto itr = market.find("power"_n.value);
      auto tmp = *itr;

      eosio::asset tokens_out;

      market.modify( itr, _me, [&]( auto& es ) {
         tokens_out = es.direct_convert( asset(shares,eosio::symbol(eosio::symbol_code("POWER"), 0)), acc -> quote_amount.symbol);
      });


      print("on sell shares: ", shares);
      
      power.modify(pexist, username, [&](auto &p){
        p.power -= shares;
        p.staked -= shares;
      });


     if (tokens_out.amount > 0) {
      //TODO precision?!
        if (itr -> vesting_seconds == 0) {

          action(
              permission_level{ _me, "active"_n },
              acc->root_token_contract, "transfer"_n,
              std::make_tuple( _me, username, tokens_out, std::string("PWITHDRAW-" + (name{username}.to_string() + "-" + (name{host}).to_string()) ))
          ).send();

        } else make_vesting_action(username, host, acc -> quote_token_contract, tokens_out, itr -> vesting_seconds, "powersell"_n);
      
     } else eosio::check(false, "Token amount is too low");


    }

    //   account_index accounts(_me, host.value);
    //   auto exist = accounts.find(host.value);
      
      // power3_index power(_me, host.value);
      // auto userpower = power.find(username.value);
      // auto upower = (userpower->staked);
      // eosio::check(upower >= shares, "Not enought power available for sell");

      // market_index market(_me, host.value);
      // auto itr = market.find(0);
      // auto tmp = *itr;

      // eosio::asset tokens_out;
      // market.modify( itr, _me, [&]( auto& es ) {
    //        tokens_out = es.direct_convert( asset(shares,eosio::symbol(eosio::symbol_code("POWER"), 0)), exist -> quote_amount.symbol);
     //    });

      // eosio::check( tokens_out.amount > 1, "token amount received from selling shares is too low" );
       
    //   uint64_t vesting_seconds = unicore::getcondition(host, "vestingsecs");

    //   make_vesting_action(username, host, exist -> quote_token_contract, tokens_out, vesting_seconds, "powersell"_n);
      
    //   unicore::propagate_votes_changes(host, username, userpower->power, userpower-> power - shares);
    //   log_event_with_shares(username, host, userpower->power - shares);

    //   power.modify(userpower, username, [&](auto &p){
    //    p.power = userpower->power - shares;
    //    p.staked = userpower->staked - shares;
    //   });

        // unicore::checkminpwr(host, username);
  
  };


  /**
   * @brief      Метод обновления вестинг-баланса.  
   * Обновляет вестинг-баланс до актуальных параметров продолжительности. 
   * @param[in]  op    The operation
   */
  [[eosio::action]] void unicore::refreshsh (eosio::name owner, uint64_t id){
    require_auth(owner);
    vesting_index vests(_me, owner.value);
    auto v = vests.find(id);
    eosio::check(v != vests.end(), "Vesting object does not exist");
    
    if (eosio::time_point_sec(eosio::current_time_point().sec_since_epoch() ) > v->startat){
      
      auto elapsed_seconds = (eosio::time_point_sec(eosio::current_time_point().sec_since_epoch()) - v->startat).to_seconds();
      eosio::asset available;
    
      if( elapsed_seconds < v->duration){
        available = v->amount * elapsed_seconds / v->duration;
      } else {
        available = v->amount;
      }

      available = available - v->withdrawed;
      vests.modify(v, owner, [&](auto &m){
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
  [[eosio::action]] void unicore::withdrawsh(eosio::name owner, uint64_t id){
    require_auth(owner);
    vesting_index vests(_me, owner.value);
    auto v = vests.find(id);
    eosio::check(v != vests.end(), "Vesting object does not exist");
    eosio::check((v->available).amount > 0, "Only positive amount can be withdrawed");
    account_index account(_me, (v->host).value);
    auto acc = account.find((v->host).value);

    action(
        permission_level{ _me, "active"_n },
        v->contract, "transfer"_n,
        std::make_tuple( _me, owner, v->available, std::string("Vesting Withdrawed")) 
    ).send();

    vests.modify(v, owner, [&](auto &m){
        m.withdrawed = v->withdrawed + v->available;
        m.available = eosio::asset(0, (v->available).symbol);
      });

    if (v->withdrawed == v->amount){
    	vests.erase(v);
    };
    
  };




  /**
   * @brief      Метод обновления силового финансового потока
   * 
   * Позволяет обновить доступную часть финансового потока, направленного на держателя силы, 
   * а так же собрать доступные реферальные балансы в один объект
  */
  [[eosio::action]] void unicore::refreshpu(eosio::name username, eosio::name host){
    
  }


  /**
   * @brief      Внутренний метод логирования событий покупки силы
   * Используется для расчета доли владения в финансовом потоке cfund в рамках границы пула
   *
  */
   void log_event_with_shares (eosio::name username, eosio::name host, int64_t new_power){
     

   } 



  /**
   * @brief      Внутренний метод возврата силы при возврате значка.
   * Используется при возврате знака отличия хосту
   *
   * @param[in]  op    The operation
   */

  void unicore::back_shares_with_badge_action (eosio::name host, eosio::name from, uint64_t shares){
    account_index accounts(_me, host.value);
    auto acc = accounts.find(host.value);
    eosio::check(acc != accounts.end(), "Host is not found");
    
    power3_index power_to_idx (_me, host.value);
    auto power_to = power_to_idx.find(from.value);
      
    //modify
    unicore::propagate_votes_changes(host, from, power_to->power, power_to->power - shares);

    // sub_base_from_market(host, asset(shares, _POWER));

    power_to_idx.modify(power_to, _me, [&](auto &pt){
      pt.power -= shares;
      pt.with_badges -= shares; 
    });

    market_index market(_me, host.value);
    auto itr = market.find(0);
  
    accounts.modify(acc, _me, [&](auto &a){
      a.total_shares -= shares;
    });  

  }


  /**
   * @brief      Внутренний метод эмиссии силы.
   * Используется при выдаче знака отличия  
   *
   * @param[in]  op    The operation
   */

  void unicore::give_shares_with_badge_action (eosio::name host, eosio::name reciever, uint64_t shares){
    account_index accounts(_me, host.value);
    auto acc = accounts.find(host.value);
    eosio::check(acc != accounts.end(), "Host is not found");
    
    power3_index power_to_idx (_me, host.value);
    auto power_to = power_to_idx.find(reciever.value);
    
    market_index market(_me, host.value);
    
    accounts.modify(acc, _me, [&](auto &a){
        a.total_shares += shares;
    }); 

    // add_base_to_market(host, asset(shares, _POWER));
      

    //Emplace or modify power object of reciever and propagate votes changes;
    if (power_to == power_to_idx.end()) {

      power_to_idx.emplace(_me, [&](auto &pt){
        pt.username = reciever;
        pt.power = shares;
        pt.delegated = 0;
        pt.with_badges = shares;
      });
    
    } else {
      
      //modify
      unicore::propagate_votes_changes(host, reciever, power_to->power, power_to->power + shares);
      
      
      power_to_idx.modify(power_to, _me, [&](auto &pt){
        pt.power += shares;
        pt.with_badges += shares; 
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
	void unicore::propagate_votes_changes(eosio::name host, eosio::name voter, uint64_t old_power, uint64_t new_power){
		votes_index votes(_me, voter.value);
		goals_index goals(_me, host.value);

		//by host;
		auto idx = votes.template get_index<"host"_n>();
    auto matched_itr = idx.lower_bound(host.value);
       

    while(matched_itr != idx.end() && matched_itr->host == host){
			auto goal = goals.find(matched_itr -> goal_id);
			
      if (goal -> status == "waiting"_n || goal -> status == "validated"_n){
      	goals.modify(goal, _me, [&](auto &g){
  				g.positive_votes = goal->positive_votes - old_power + new_power;
  			});
      }

			idx.modify(matched_itr, _me, [&](auto &v) {
				v.power = v.power - old_power + new_power;
			});
			
      matched_itr++;

		};	
	}


  /**
   * @brief      Disable power market
   *
   * @param[in]  host    The host
   */
  
  [[eosio::action]] void unicore::dispmarket(eosio::name host){
    require_auth(host);
    
    account_index account(_me, host.value);

    auto acc = account.find(host.value);
    eosio::check(acc != account.end(), "Host is not found");

    eosio::check(acc->power_market_id != ""_n, "Power market already disabled");

    account.modify(acc, host, [&](auto &a){
      a.power_market_id = ""_n;
    });
    
  }

  /**
   * @brief      Enable power market
   *
   * @param[in]  host    The host
   * @param[in]  status  The status
   */
  [[eosio::action]] void unicore::enpmarket(eosio::name host){
    require_auth(host);
    
    account_index account(_me, host.value);

    auto acc = account.find(host.value);
    eosio::check(acc != account.end(), "Host is not found");

    // eosio::check(acc->power_market_id == ""_n, "Power market already enabled");

    account.modify(acc, host, [&](auto &a){
      a.power_market_id = host;
    });
    
    unicore::create_bancor_market("power"_n, host, acc -> total_shares, acc->quote_amount, acc -> quote_token_contract, 0); 
  
  }

  /**
   * @brief      Приватный метод создания банкор-рынка.
   *
   * @param[in]  host          The host
   * @param[in]  total_shares  The total shares
   * @param[in]  quote_amount  The quote amount
   */
	void unicore::create_bancor_market(eosio::name name, eosio::name host, uint64_t total_shares, eosio::asset quote_amount, eosio::name quote_token_contract, uint64_t vesting_seconds){
		market_index market(_me, host.value);

		auto itr = market.find(name.value);
		
    if (itr == market.end()){

			itr = market.emplace( _me, [&]( auto& m ) {
         m.name = name;
         m.vesting_seconds = vesting_seconds;
         m.supply.amount = 100000000000000ll;
         m.supply.symbol = eosio::symbol(eosio::symbol_code("BANCORE"), 0);
         m.base.balance.amount = total_shares;
         m.base.balance.symbol = eosio::symbol(eosio::symbol_code("POWER"), 0);
         m.quote.balance.amount = quote_amount.amount;
         m.quote.balance.symbol = quote_amount.symbol;
         m.quote.contract = quote_token_contract;
      });
		} else {
			// eosio::check(false, "Market already created");
    }
	};






//PREPARE TO DELETE

  /**
   * @brief      Метод делегирования силы.
   * Позволяет делегировать купленную силу для принятия каких-либо решений в пользу любого аккаунта. 
   *
   * @param[in]  op    The operation
   */

  void unicore::delegate_shares_action(eosio::name from, eosio::name reciever, eosio::name host, uint64_t shares){
   //   require_auth(from);
    // power3_index power_from_idx (_me, host.value);
    // power3_index power_to_idx (_me, host.value);

    // delegation_index delegations(_me, from.value);
    
    // account_index accounts(_me, host.value);
    // auto acc = accounts.find(host.value);

    // auto power_from = power_from_idx.find(from.value);
    // eosio::check(power_from != power_from_idx.end(),"Nothing to delegatee");
    // eosio::check(power_from -> power > 0, "Nothing to delegate");
    // eosio::check(shares > 0, "Delegate amount must be greater then zero");
    // eosio::check(shares <= power_from->staked, "Not enough staked power for delegate");
    
    // auto dlgtns = delegations.find(reciever.value);
    // auto power_to = power_to_idx.find(reciever.value);

    // if (dlgtns == delegations.end()) {

    //  delegations.emplace(_me, [&](auto &d){
    //    d.reciever = reciever;
    //    d.shares = shares;
    //  });

    // } else {
    //  delegations.modify(dlgtns, _me, [&](auto &d){
    //    d.shares += shares;
    //  });
    // };

    // //modify power object of sender and propagate votes changes;
    // unicore::propagate_votes_changes(host, from, power_from->power, power_from->power - shares);
    
  //   log_event_with_shares(from, host, power_from->power - shares);

    // power_from_idx.modify(power_from, _me, [&](auto &pf){
    //  pf.staked -= shares;
    //  pf.power  -= shares;

    // });

    // //Emplace or modify power object of reciever and propagate votes changes;
    // if (power_to == power_to_idx.end()){
    //  power_to_idx.emplace(_me, [&](auto &pt){
    //    pt.username = reciever;
    //    pt.power = shares;
    //    pt.delegated = shares;    
    //  });
    // } else {
    //  //modify
    //  unicore::propagate_votes_changes(host, reciever, power_to->power, power_to->power + shares);
      
  //     log_event_with_shares(reciever, host, power_from->power + shares);
      
  //     power_to_idx.modify(power_to, _me, [&](auto &pt){
    //    pt.power += shares;
    //    pt.delegated += shares;
    //  });

      
    // }    
  }




  /**
   * @brief      Метод вывода силового финансового потока
   * withdraw power quant (withpowerun)
   * Позволяет вывести часть финансового потока, направленного на держателя силы
  */
  [[eosio::action]] void unicore::withpbenefit(eosio::name username, eosio::name host){
    

  }
  



  /**
   * @brief      Метод возврата делегированной силы
   *
   * @param[in]  op    The operation
   */
  [[eosio::action]] void unicore::undelshares(eosio::name from, eosio::name reciever, eosio::name host, uint64_t shares){
    require_auth(reciever);

    // power3_index power_from_idx (_me, host.value);
    // power3_index power_to_idx (_me, host.value);

    // delegation_index delegations(_me, reciever.value);
    // auto dlgtns = delegations.find(from.value);

    // eosio::check(dlgtns != delegations.end(), "Nothing to undelegate");
    // eosio::check(dlgtns -> shares >= shares, "Not enought shares for undelegate");
    // eosio::check(shares > 0, "Undelegate amount must be greater then zero");
    
    // auto power_from = power_from_idx.find(from.value);
    // auto power_to = power_to_idx.find(reciever.value);

    // if (dlgtns->shares - shares == 0){
    //  delegations.erase(dlgtns);
    // } else {
    //  delegations.modify(dlgtns, reciever, [&](auto &d){
    //    d.shares -= shares;
    //  });
    // }

    // //modify power object of sender and propagate votes changes;
    // unicore::propagate_votes_changes(host, from, power_from->power, power_from->power - shares);
    // log_event_with_shares(from, host, power_from->power - shares);

    // power_from_idx.modify(power_from, reciever, [&](auto &pf){
    //  pf.power -= shares;
    //  pf.delegated -= shares;

    // });

    // //modify
    // unicore::propagate_votes_changes(host, reciever, power_to->power, power_to->power + shares);
    // log_event_with_shares(reciever, host, power_from->power + shares);

    // power_to_idx.modify(power_to, reciever, [&](auto &pt){
    //  pt.staked += shares;
    //  pt.power  += shares;
    // });


  }




  /**
   * @brief      Метод покупки силы сообщества
   * Обеспечивает покупку силы сообщества за котировочный токен по внутренней рыночной цене, определяемой с помощью алгоритма банкор. 
   * @param[in]  buyer   The buyer
   * @param[in]  host    The host
   * @param[in]  amount  The amount
   */
  uint64_t unicore::buyshares_action ( eosio::name buyer, eosio::name host, eosio::asset amount, eosio::name code, bool is_frozen ){
    account_index accounts(_me, host.value);
    partners2_index users(_partners,_partners.value);
    auto user = users.find(buyer.value);
    
    auto exist = accounts.find(host.value);
    auto root_symbol = exist -> get_root_symbol();

    eosio::check(exist != accounts.end(), "Host is not founded");
    eosio::check(exist -> quote_token_contract == code, "Wrong quote token contract");
    eosio::check(exist -> quote_amount.symbol == amount.symbol, "Wrong quote token symbol");
    eosio::check(exist -> power_market_id != ""_n, "Can buy shares only when power market is enabled");

    eosio::name main_host = exist->get_ahost();

    spiral_index spiral(_me, main_host.value);
    auto sp = spiral.find(0);

    uint64_t new_shares = amount.amount * (HUNDR_PERCENT - sp -> loss_percent) / HUNDR_PERCENT;

    //TODO buy_shares check!
    // market.modify( itr, _me, [&]( auto& es ) {
    //        tokens_out = es.direct_convert( asset(shares,eosio::symbol(eosio::symbol_code("POWER"), 0)), exist -> quote_amount.symbol);
     //    });


    print("new_shares: ", new_shares);
    
    accounts.modify(exist, _me, [&](auto &a){
      a.total_shares += new_shares;
      a.quote_amount += amount;
    });

    power3_index power(_me, host.value);

    auto pexist = power.find(buyer.value);
    
    if (pexist == power.end()) {

      power.emplace(_me, [&](auto &p){
         p.username = buyer;
         p.power = new_shares;
         p.staked = new_shares; 
      });
      
    
    } else {
      
     unicore::propagate_votes_changes(host, buyer, pexist->power, pexist->power + new_shares);
      
      power.modify(pexist, _me, [&](auto &p){
        p.power += new_shares;
        p.staked += new_shares;
        
     });
    };

    
    return amount.amount;
    
  };



