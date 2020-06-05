using namespace eosio;

struct shares {

  /**
   * @brief      Создание вестинг-баланса
   * Внутренний метод, используемый при обратном обмене силы сообщества на котировочный токен. 
   * Обеспечивает линейную выдачу жетонов в продолжительности времени. 
   * @param[in]  owner   The owner
   * @param[in]  amount  The amount
   */
	void make_vesting_action(eosio::name owner, eosio::name host, eosio::asset amount, uint64_t vesting_seconds){
	    check(amount.is_valid(), "Amount not valid");
	    // check(amount.symbol == _SYM, "Not valid symbol for this vesting contract");
	    check(is_account(owner), "Owner account does not exist");
	    
	    vesting_index vests (_self, owner);
	    
	    vests.emplace(_self, [&](auto &v){
	      v.id = vests.available_primary_key();
        v.host = host;
	      v.owner = owner;
	      v.amount = amount;
	      v.startat = eosio::time_point_sec(now());
	      v.duration = vesting_seconds;
	    });

	}

  /**
   * @brief      Метод обновления вестинг-баланса.  
   * Обновляет вестинг-баланс до актуальных параметров продолжительности. 
   * @param[in]  op    The operation
   */
  void refresh_action(const refreshsh &op){
    require_auth(op.owner);
    vesting_index vests(_self, op.owner);
    auto v = vests.find(op.id);
    check(v != vests.end(), "Vesting object does not exist");
    
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
    check(v != vests.end(), "Vesting object does not exist");
    check((v->available).amount > 0, "Only positive amount can be withdrawed");
    account_index account(_self, v->host);
    auto acc = account.find(v->host);

    action(
        permission_level{ _self, "active"_n },
        acc->quote_token_contract, "transfer"_n,
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
   * @brief      Метод вывода силового финансового потока
   * withdraw power unit (withpowerun)
   * Позволяет вывести часть финансового потока, направленного на держателя силы
  */
  void withdraw_power_units_action(const withbenefit &op){
    require_auth(op.username);
    pstats_index pstats(_self, op.username);
    auto pstat = pstats.find(op.host);
    check(pstat != pstats.end(), "Power Stat Object is not found. Refresh your balances first.");
    check((pstat -> pflow_available_in_asset).amount > 0, "Not enough tokens for transfer");
    account_index account(_self, op.host);

    auto acc = account.find(op.host);
    auto root_symbol = acc->get_root_symbol();
    auto on_withdraw = pstat -> pflow_available_in_asset + pstat -> ref_available_in_asset;
    
    if (on_withdraw.amount > 0){
      action(
          permission_level{ _self, "active"_n },
          acc->root_token_contract, "transfer"_n,
          std::make_tuple( _self, op.username, on_withdraw, std::string("PFLOW-" + (name{op.username}.to_string() + "-" + (name{op.host}).to_string()) ))
      ).send();


      pstats.modify(pstat, _self, [&](auto &ps){
        
        ps.total_available_in_asset = asset(0, root_symbol);
        
        ps.pflow_available_segments -= (pstat -> pflow_available_in_asset).amount * TOTAL_SEGMENTS;
        ps.pflow_available_in_asset = asset(0, root_symbol);
        ps.pflow_withdrawed += pstat -> pflow_available_in_asset;

        ps.ref_available_segments -= (pstat -> ref_available_in_asset).amount * TOTAL_SEGMENTS;
        ps.ref_available_in_asset = asset(0, root_symbol);
        ps.ref_withdrawed += pstat -> ref_available_in_asset;
      });    
    }

  }


  /**
   * @brief      Метод обновления силового финансового потока
   * 
   * Позволяет обновить доступную часть финансового потока, направленного на держателя силы, 
   * а так же собрать доступные реферальные балансы в один объект
  */
  void refresh_power_units_action(const refreshpu &op){
    require_auth(op.username);
    power_index power(_self, op.username);
    // auto pexist = power.find(op.host);
    account_index account(_self, op.host);

    auto acc = account.find(op.host);
    auto root_symbol = acc->get_root_symbol();
    check(acc != account.end(), "Host is not found");
    // check(pexist != power.end(), "User never have any power");


    pstats_index pstats(_self, op.username);
    auto pstat = pstats.find(op.host);
    auto min_pool_id = 0;

    bool skip = false;

    if (pstat != pstats.end())
      min_pool_id = pstat -> pflow_last_withdrawed_pool_id;

    

    refbalances_index refbalances(_self, op.username);
    auto refbalance = refbalances.begin();
    uint64_t count=0;
    while(refbalance != refbalances.end() && count < 50){

    // }
    // if (refbalance != refbalances.end()){
      if (pstat == pstats.end()){

        pstats.emplace(_self, [&](auto &ps){
          ps.host = op.host;
          ps.total_available_in_asset = refbalance->amount;

          ps.pflow_last_withdrawed_pool_id = 0;
          ps.pflow_available_segments = 0;
          ps.pflow_available_in_asset = asset(0, root_symbol);
          ps.pflow_withdrawed = asset(0, root_symbol);
          
          ps.ref_available_in_asset = refbalance->amount;
          ps.ref_available_segments = refbalance->segments;
          ps.ref_withdrawed = asset(0, root_symbol);
        });
        
        pstat = pstats.begin();

      } else {
        pstats.modify(pstat, _self, [&](auto &ps){
          ps.total_available_in_asset += refbalance->amount;
          ps.ref_available_segments += refbalance->segments;
          ps.ref_available_in_asset += refbalance->amount;
        });          
      }

      refbalances.erase(refbalance);
      refbalance = refbalances.begin();
      count++;
    }
  

    sincome_index sincomes(_self, op.host);
    auto sincome = sincomes.lower_bound(min_pool_id);

    if ((min_pool_id != 0) && (sincome != sincomes.end()))
      sincome++;

    uint64_t count2 = 0;
    
    while(sincome != sincomes.end() && count2 < 50) {
    // if (sincome != sincomes.end()){

      plog_index plogs(_self, op.username);

      auto pool_id_with_host_idx = plogs.template get_index<"hostwithpoolid"_n>();
      auto log_id = combine_ids(op.host, sincome-> pool_id);


      auto plog = pool_id_with_host_idx.lower_bound(log_id);  
      
      if ((plog != pool_id_with_host_idx.begin()) && (plog -> pool_id > sincome->pool_id)){ 
          plog--;
      };

      if (plog == pool_id_with_host_idx.end()){
        skip = true;
      };

     
      if (acc->current_pool_id > sincome -> pool_id){ //Только полностью закрытые объекты sincome принимаем в распределение
        if ( sincome->liquid_power > 0 &&  sincome-> hfund_in_segments > 0 && !skip ){ 
          
          // check(plog -> pool_id <= sincome->pool_id, "System error1");
          check(plog -> power <= sincome->liquid_power, "System error2");

          uint128_t user_segments = (double)plog -> power / (double)sincome -> liquid_power * sincome -> hfund_in_segments;
          
          
          uint64_t user_segments_64 = (uint64_t)(user_segments / TOTAL_SEGMENTS);
          eosio::asset segment_asset = asset(user_segments_64, root_symbol);
          
          print(" usegments: ", (double)user_segments);
          print(" hfund_in_segments: ", (double)sincome->hfund_in_segments);
          print(" is_equal: ", (double)sincome->hfund_in_segments == (double)user_segments);
          print(" distr_segmnts: ", (double)sincome->distributed_segments);

          check((double)sincome->distributed_segments + (double)user_segments <= (double)sincome->hfund_in_segments, "Overflow fund on distibution event.");

          sincomes.modify(sincome, _self, [&](auto &s){
            s.distributed_segments += user_segments;
          });
          
          if (pstat == pstats.end()){
            pstats.emplace(_self, [&](auto &ps){
              ps.host = op.host;
              ps.total_available_in_asset = segment_asset;
          
              ps.pflow_last_withdrawed_pool_id = sincome -> pool_id;
              ps.pflow_available_segments = user_segments;
              ps.pflow_available_in_asset = segment_asset;
              ps.pflow_withdrawed = asset(0, root_symbol);

              ps.ref_available_in_asset = asset(0, root_symbol);
              ps.ref_available_segments = 0;
              ps.ref_withdrawed = asset(0, root_symbol);
            });
          } else {
            pstats.modify(pstat, _self, [&](auto &ps){
              ps.total_available_in_asset += segment_asset;
              ps.pflow_last_withdrawed_pool_id = sincome -> pool_id;
              ps.pflow_available_segments += user_segments;
              ps.pflow_available_in_asset += segment_asset;
            });          
          }
          dlog_index dlogs(_self, op.username);
          
          dlogs.emplace(_self, [&](auto &dl){
            dl.id = dlogs.available_primary_key();
            dl.host = op.host;
            dl.pool_id = sincome -> pool_id;
            dl.segments = user_segments;
            dl.amount = segment_asset;
          });
          //TODO Удалить все устаревшие plog
          //Устаревшие это те, которые больше никогда не будут учитываться в распределении.
          //(есть более новые актуальные записи)
          
        } else { //SKIP.- Нет ликвидной силы на момент вывода балансов из пула
            if (pstat == pstats.end()){
              pstats.emplace(_self, [&](auto &ps){
                ps.host = op.host;
                ps.total_available_in_asset = asset(0, root_symbol);
                ps.pflow_last_withdrawed_pool_id = sincome -> pool_id;
                ps.pflow_available_segments = 0;
                ps.pflow_available_in_asset = asset(0, root_symbol);
                ps.pflow_withdrawed = asset(0, root_symbol);

                ps.ref_available_in_asset = asset(0, root_symbol);
                ps.ref_available_segments = 0;
                ps.ref_withdrawed = asset(0, root_symbol);

              });

              pstat = pstats.begin();
            } else {
              pstats.modify(pstat, _self, [&](auto &ps){
                ps.pflow_last_withdrawed_pool_id = sincome -> pool_id;
              });          
            }
          //чистим plog
        }
      }
      sincome++;
      count2++;
    }  


    //Расчет выплат произвоить только для объектов sincome с айди, меньше чем текущий в объекте хоста
    //by host and cycle
    //Проверка лога. Получаем объект измененной силы на пуле. 
    //Его объекта нет, то получаем первый и последний объект на текущем цикле.
    //Если объектов нет, используем силу.  
    //Если в логе под текущим номером пула ничего нет, то провяем последний вывод
    //записи из лога все, кроме первой, если она в текущем цикле.
    //Проверяем выводились ли токены прежде
    //Если нет, расчитываем объем токенов из фонда. 
    //Где храним объект фонда распределения? На хосте. 
    //записываем последний вывод и сколько всего получено
    //считать долю в сегментах
    //выдаю долю в сегментах
    //проверка на новом цикле
  }


  /**
   * @brief      Внутренний метод логирования событий покупки силы
   * Используется для расчета доли владения в финансовом потоке cfund в рамках границы пула
   *
  */
   void log_event_with_shares (eosio::name username, eosio::name host, int64_t new_power){
      account_index accounts(_self, host);
      auto acc = accounts.find(host);

      plog_index plogs(_self, username);
 
      auto pool_id_with_host_idx = plogs.template get_index<"hostwithpoolid"_n>();
      auto log_ids = combine_ids(host, acc->current_pool_id);
      
      auto log = pool_id_with_host_idx.find(log_ids);


      if (log == pool_id_with_host_idx.end()){
        plogs.emplace(_self, [&](auto &l){
          l.id = plogs.available_primary_key();
          l.host = host;
          l.pool_id = acc->current_pool_id;
          l.power = new_power;
          l.cycle_num = acc->current_cycle_num;
          l.pool_num = acc->current_pool_num;
        });

      } else {
        pool_id_with_host_idx.modify(log, _self, [&](auto &l){
          l.power = new_power;
        });
      }

      sincome_index sincome(_self, host);
      auto sinc = sincome.find(acc->current_pool_id);
      
      rate_index rates(_self, host);
      auto rate = rates.find(acc->current_pool_num - 1);
      eosio::name main_host = acc->get_ahost();
      auto root_symbol = acc->get_root_symbol();

      powermarket market(_self, host);
      auto itr = market.find(0);
      auto liquid_power = acc->total_shares - itr->base.balance.amount;

      if (sinc == sincome.end()){
        sincome.emplace(_self, [&](auto &s){
            s.max = rate -> system_income;
            s.pool_id = acc->current_pool_id;
            s.ahost = main_host;
            s.cycle_num = acc->current_cycle_num;
            s.pool_num = acc->current_pool_num;
            s.liquid_power = liquid_power;
            s.total = asset(0, root_symbol);
            s.paid_to_sys = asset(0, root_symbol);
            s.paid_to_dacs = asset(0, root_symbol);
            s.paid_to_cfund = asset(0, root_symbol);
            s.paid_to_hfund = asset(0, root_symbol);
            s.paid_to_refs = asset(0, root_symbol);
            s.hfund_in_segments = 0;
            s.distributed_segments = 0;
        }); 

      } else {

        sincome.modify(sinc, _self, [&](auto &s){
          s.liquid_power = liquid_power;
        });
      } 
      // auto idx = votes.template get_index<"host"_n>();
      // auto i_bv = idx.lower_bound(host);


   } 


  /**
   * @brief      Метод покупки силы сообщества
   * Обеспечивает покупку силы сообщества за котировочный токен по внутренней рыночной цене, определяемой с помощью алгоритма банкор. 
   * @param[in]  buyer   The buyer
   * @param[in]  host    The host
   * @param[in]  amount  The amount
   */
	void buyshares_action ( eosio::name buyer, eosio::name host, eosio::asset amount, eosio::name code ){
		// check(amount.symbol == _SYM, "Wrong symbol for buy shares");
		
		account_index accounts(_self, host);
		user_index users(_self,_self);
    auto user = users.find(buyer);
    check(user != users.end(), "User is not registered");


		auto exist = accounts.find(host);
		check(exist != accounts.end(), "Host is not founded");
		check(exist -> quote_token_contract == code, "Wrong quote token contract");
    check(exist -> quote_amount.symbol == amount.symbol, "Wrong quote token symbol");
		
    powermarket market(_self, host);
		auto itr = market.find(0);
		auto tmp = *itr;
		uint64_t shares_out;
		market.modify( itr, 0, [&]( auto &es ) {
	    	shares_out = (es.convert( amount, S(0, POWER))).amount;
	    });

    check( shares_out > 0, "Amount is not enought for buy 1 share" );

    power_index power(_self, buyer);

    auto pexist = power.find(host);
    

    if (pexist == power.end()){
      power.emplace(_self, [&](auto &p){
      	p.host = host;
      	p.power = shares_out;
      	p.staked = shares_out;	
      });
      log_event_with_shares(buyer, host, shares_out);
		
    } else {
			propagate_votes_changes(host, buyer, pexist->power, pexist->power + shares_out);
		  
      log_event_with_shares(buyer, host, pexist->power + shares_out);

			power.modify(pexist, _self, [&](auto &p){
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

  void back_shares_with_badge_action (eosio::name host, eosio::name from, uint64_t shares){
    account_index accounts(_self, host);
    auto acc = accounts.find(host);
    check(acc != accounts.end(), "Host is not found");
    
    power_index power_to_idx (_self, from);
    auto power_to = power_to_idx.find(host);
      
    //modify
    propagate_votes_changes(host, from, power_to->power, power_to->power - shares);
    log_event_with_shares(from, host, power_to->power - shares);

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

  void give_shares_with_badge_action (eosio::name host, eosio::name reciever, uint64_t shares){
    account_index accounts(_self, host);
    auto acc = accounts.find(host);
    check(acc != accounts.end(), "Host is not found");
    
    power_index power_to_idx (_self, reciever);
    auto power_to = power_to_idx.find(host);
    
    powermarket market(_self, host);

    auto itr = market.find(0);
    
    check((itr->base).balance.amount >= shares, "Not enought shares on market");

    
    
    market.modify( itr, 0, [&]( auto& es ) {
      es.base.balance = asset((itr -> base).balance.amount - shares, S(0, POWER));
    });

    //Emplace or modify power object of reciever and propagate votes changes;
    if (power_to == power_to_idx.end()){
      log_event_with_shares(reciever, host, shares);
      power_to_idx.emplace(_self, [&](auto &pt){
        pt.host = host;
        pt.power = shares;
        pt.delegated = 0;
        pt.with_badges = shares;
      });
    
    } else {
      
      //modify
      propagate_votes_changes(host, reciever, power_to->power, power_to->power + shares);
      
      log_event_with_shares(reciever, host, power_to->power + shares);

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
	 	require_auth(op.from);
		power_index power_from_idx (_self, op.from);
		power_index power_to_idx (_self, op.reciever);

		delegation_index delegations(_self, op.from);
		
		account_index accounts(_self, op.host);
		auto acc = accounts.find(op.host);

		auto power_from = power_from_idx.find(op.host);
		check(power_from != power_from_idx.end(),"Nothing to delegatee");
		check(power_from -> power > 0, "Nothing to delegate");
		check(op.shares > 0, "Delegate amount must be greater then zero");
		check(op.shares <= power_from->staked, "Not enough staked power for delegate");
		
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
		propagate_votes_changes(op.host, op.from, power_from->power, power_from->power - op.shares);
		
    log_event_with_shares(op.from, op.host, power_from->power - op.shares);

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
			
      log_event_with_shares(op.reciever, op.host, power_from->power + op.shares);
      
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
	void propagate_votes_changes(eosio::name host, eosio::name voter, uint64_t old_power, uint64_t new_power){
		votes_index votes(_self, voter);
		goals_index goals(_self, host);

		//by host;

		auto idx = votes.template get_index<"host"_n>();
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

		check(dlgtns != delegations.end(), "Nothing to undelegate");
		check(dlgtns -> shares >= op.shares, "Not enought shares for undelegate");
		check(op.shares > 0, "Undelegate amount must be greater then zero");
		
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
		log_event_with_shares(op.from, op.host, power_from->power - op.shares);

		power_from_idx.modify(power_from, op.reciever, [&](auto &pf){
			pf.power -= op.shares;
			pf.delegated -= op.shares;

		});

		//modify
		propagate_votes_changes(op.host, op.reciever, power_to->power, power_to->power + op.shares);
		log_event_with_shares(op.reciever, op.host, power_from->power + op.shares);

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
		check(upower >= shares, "Not enought power available for sell");

		powermarket market(_self, host);
		auto itr = market.find(0);
		auto tmp = *itr;

		eosio::asset tokens_out;
		market.modify( itr, 0, [&]( auto& es ) {
        	tokens_out = es.convert( asset(shares,S(0, POWER)), exist -> quote_amount.symbol);
	    });
		check( tokens_out.amount > 1, "token amount received from selling shares is too low" );
	    
	    make_vesting_action(username, host, tokens_out, itr->vesting_seconds);
	    
	    propagate_votes_changes(op.host, username, userpower->power, userpower-> power - shares);
      log_event_with_shares(username, op.host, userpower->power - shares);

	    power.modify(userpower, username, [&](auto &p){
	    	p.power = userpower->power - shares;
	    	p.staked = userpower->staked - shares;
	    });
	    
	};

  /**
   * @brief      Приватный метод создания банкор-рынка.
   *
   * @param[in]  host          The host
   * @param[in]  total_shares  The total shares
   * @param[in]  quote_amount  The quote amount
   */
	void create_bancor_market(std::string name, uint64_t id, eosio::name host, uint64_t total_shares, eosio::asset quote_amount, eosio::name quote_token_contract, uint64_t vesting_seconds){
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
			check(false, "Market already created");
	};
};
