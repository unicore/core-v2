
using namespace eosio;

/**
 * @brief      Модуль целей. 
 * Использует долю потока эмиссии для финансирования целей сообщества. 
 * Каждый участник может предложить цель к финансированию. Суть и смысл цели ограничен только желаниями сообщества.
 * Каждая цель должна пройти минимальный порог процента собранных голосов от держателей силы сообщества.
 * Цели, прошедшие порог консенсуса сообщества, попадают в лист финансирования. 
 * Все цели в листе финансирования получают равный поток финансирования относительно друг друга.
 * Финансирование поступает в объект баланса цели в момент перехода на каждый следующий пул, или напрямую из фонда сообщества по подписи архитектора.  
 * 
 */

    void validate_permlink( const std::string& permlink )
        {
           eosio::check( 3 < permlink.size() && permlink.size() < 255, "Permlink is not a valid size." );
           
             for( auto c : permlink )
             {
                switch( c )
                {
                   case 'a': case 'b': case 'c': case 'd': case 'e': case 'f': case 'g': case 'h': case 'i':
                   case 'j': case 'k': case 'l': case 'm': case 'n': case 'o': case 'p': case 'q': case 'r':
                   case 's': case 't': case 'u': case 'v': case 'w': case 'x': case 'y': case 'z': case '0':
                   case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
                   case '-':
                      break;
                   default:
                      eosio::check( false, "Invalid permlink character");
                }
             }
        }

  /**
   * @brief      Метод создания цели
   * 
   * @param[in]  op    The new value
   */
	[[eosio::action]] void unicore::setgoal(eosio::name username, eosio::name benefactor, eosio::name host, std::string title, std::string permlink, std::string description, eosio::asset target, uint64_t expiration){
		require_auth(username);
		
		goals_index goals(_me, host.value);
		account_index accounts(_me, host.value);

		user_index users(_me, _me.value);
    auto user = users.find(username.value);
    eosio::check(user != users.end(), "User is not registered");

		auto acc = accounts.find(host.value);
    auto root_symbol = (acc->root_token).symbol;

    bool validated = acc->consensus_percent == 0;
    
    validate_permlink(permlink);
    
    //check for uniq permlink
    auto hash = hash64(permlink);
    
    auto idx_bv = goals.template get_index<"hash"_n>();
    auto exist_permlink = idx_bv.find(hash);

    eosio::check(exist_permlink == idx_bv.end(), "Goal with current permlink is already exist");

    eosio::check(target.symbol == root_symbol, "Wrong symbol for this host");
    
    eosio::check(title.length() <= 100, "Short Description is a maximum 100 symbols. Describe the goal shortly");
      auto goal_id =  acc -> total_goals + 1;

      goals.emplace(username, [&](auto &g){
        g.id = goal_id;
        g.hash = hash;
        g.creator = username;
        g.benefactor = benefactor;
        g.created = eosio::time_point_sec(eosio::current_time_point().sec_since_epoch());
        g.host = host;
        g.permlink = permlink;
        g.title = title;
        g.description = description;
        g.target = target;
        g.withdrawed = asset(0, root_symbol);
        g.available = asset(0, root_symbol);
        g.validated = validated;
        g.expired_at = eosio::time_point_sec (eosio::current_time_point().sec_since_epoch() + expiration);
      });      

      accounts.modify(acc, username, [&](auto &a){
        a.total_goals = acc -> total_goals + 1;
      });

      if (username == acc->architect) {
        unicore::dfundgoal(username, host, goal_id, target, "autoset by architect");
      }
	};


  /**
   * @brief      Метод прямого финансирования цели
   * Используется архитектором для финансирования цели из фонда 
   *
   * @param[in]  op    The operation
   */
	[[eosio::action]] void unicore::dfundgoal(eosio::name architect, eosio::name host, uint64_t goal_id, eosio::asset amount, std::string comment){
		require_auth(architect);

		account_index accounts(_me, host.value);
		auto acc = accounts.find(host.value);
		eosio::check(acc->architect == architect, "Only architect can direct fund the goal");

    emission_index emis(_me, host.value);
    auto emi = emis.find(host.value);

    eosio::check(emi->fund >= amount, "Not enough tokens for fund the goal");

    donate_action(architect, host, goal_id, amount, _me);

    emis.modify(emi, architect, [&](auto &e){
    	e.fund = e.fund - amount;
    });

	};




 /**
  * @brief      Метод редактирования цели
  *
  * @param[in]  op    The operation
  */

	[[eosio::action]] void unicore::editgoal(uint64_t goal_id, eosio::name username, eosio::name host, eosio::name benefactor, std::string title, std::string description, eosio::asset target){
		require_auth(username);
		
		goals_index goals(_me,host.value);
    account_index accounts (_me, host.value);

    auto acc = accounts.find(host.value);
    auto root_symbol = (acc->root_token).symbol;
    
    eosio::check(target.symbol == root_symbol, "Wrong symbol for this host");

    eosio::check(title.length() <= 100, "Short Description is a maximum 100 symbols");

    auto goal = goals.find(goal_id);
    eosio::check(goal != goals.end(), "Goal is not exist");
    eosio::check(goal->creator == username, "Dont have permissions for edit goal");

    if (goal->target != target){
    	eosio::check(goal -> validated == false, "Impossible edit amount after validation");
    };

    goals.modify(goal, username, [&](auto &g){
    	g.title = title;
    	g.description = description;
    	g.benefactor = benefactor;
    	g.target = target;
    });

	}



  /**
   * @brief      Метод удаления цели
   *
   * @param[in]  op    The operation
   */
	[[eosio::action]] void unicore::delgoal(eosio::name username, eosio::name host, uint64_t goal_id){
		require_auth(username);
		goals_index goals(_me, host.value);
    
		auto goal = goals.find(goal_id);
		eosio::check(goal != goals.end(), "Goal is not exist");
    eosio::check(goal->creator == username, "Wrong creator of goal");

		goals.erase(goal);

	}

  /**
   * @brief      Метод создания отчета о завершенной цели
   *
   * @param[in]  op    The operation
   */
	[[eosio::action]] void unicore::report(eosio::name username, eosio::name host, uint64_t goal_id, std::string report){
		require_auth(username);
		
		goals_index goals(_me, host.value);
	
  	account_index accounts(_me, host.value);
		auto acc = accounts.find(host.value);
		eosio::check(acc != accounts.end(), "Host is not found");

		auto goal = goals.find(goal_id);

		eosio::check(username == goal->benefactor, "Only benefactor can set report");
		
		goals.modify(goal, username, [&](auto &g){
			g.report = report;
			g.reported = true;
		});
	}

  /**
   * @brief      Метод проверки отчета
   * Отчет о достижении цели на текущий момент проверяется только архитектором. 
   *
   * @param[in]  op    The operation
   */
	[[eosio::action]] void unicore::check(eosio::name architect, eosio::name host, uint64_t goal_id){
		require_auth(architect);
		account_index accounts(_me, host.value);
		auto acc = accounts.find(host.value);
		eosio::check(acc != accounts.end(), "Host is not found");

		goals_index goals(_me, host.value);
		auto goal = goals.find(goal_id);
		eosio::check(goal != goals.end(), "Goal is not found");
		eosio::check(architect == acc->architect, "Only architect can eosio::check report for now");
		eosio::check(goal->reported == true, "Goals without reports cannot be eosio::checked");

		goals.modify(goal, architect, [&](auto &g){
			g.checked = true;
		});

    accounts.modify(acc, architect, [&](auto &a){
      a.achieved_goals = acc -> achieved_goals + 1;
    });
	};

  /**
   * @brief      Метод спонсорского взноса на цель
   * Позволяет любому участнику произвести финансирование цели из числа собственных средств. 
   *
   * @param[in]  from      The from
   * @param[in]  host      The host
   * @param[in]  goal_id   The goal identifier
   * @param[in]  quantity  The quantity
   * @param[in]  code      The code
   */
	void unicore::donate_action(eosio::name from, eosio::name host, uint64_t goal_id, eosio::asset quantity, eosio::name code){
		// require_auth(from);
		
		goals_index goals(_me, host.value);
		account_index accounts(_me, host.value);
		auto acc = accounts.find(host.value);

		if (code != _me) //For direct donate from fund and by architects action
			eosio::check(acc->root_token_contract == code, "Wrong root token contract for this host");
		
		eosio::check((acc->root_token).symbol == quantity.symbol, "Wrong root symbol for this host");
		
		auto goal = goals.find(goal_id);
		
		eosio::check(goal != goals.end(), "Goal is not exist");

		bool filled = goal->available + goal->withdrawed + quantity >= goal->target;		
		
		goals.modify(goal, _me, [&](auto &g){
			g.available += quantity;
			g.filled = filled;
		});


    if (acc -> sale_is_enabled == true){
      eosio::asset converted_quantity = unicore::buy_action(from, host, quantity, acc->root_token_contract, false);
      unicore::buyshares_action ( from, host, converted_quantity, acc->quote_token_contract );
    }


	}

  /**
   * @brief      Метод финансирования цели через оператора сообщества.
   * Позволяет оператору сообщества расходовать баланс целей на перечисления прямым спонсорам. 
   * Используется в риверсной экономической модели, когда корневой токен сообщества является котировочным токеном силы сообщества, 
   * и накаким другим способом изначально не распределяется, кроме как на спонсоров целей (дефицитная ICO - модель).
   *
   * @param[in]  op    The operation
   */
  [[eosio::action]] void unicore::gsponsor(eosio::name hoperator, eosio::name host, eosio::name reciever, uint64_t goal_id, eosio::asset amount){
    require_auth (hoperator);

    account_index accounts(_me, host.value);
    auto acc = accounts.find(host.value);
    eosio::check(acc->hoperator == hoperator, "Wrong operator for this host");
    goals_index goals(_me, host.value);

    auto goal = goals.find(goal_id);
    eosio::check(goal != goals.end(), "Goal is not founded");
    auto root_symbol = (acc->root_token).symbol;

    eosio::check(amount.symbol == root_symbol, "Wrong root symbol");

    eosio::check(goal->available >= amount, "Not enough tokens for sponsorhip action. ");

    action(
      permission_level{ _me, "active"_n },
      acc->root_token_contract, "transfer"_n,
      std::make_tuple( _me, reciever, amount, std::string("Sponsor")) 
    ).send();

    goals.modify(goal, hoperator, [&](auto &g){
      g.available = g.available - amount;
      g.withdrawed = g.withdrawed + amount;
    });

  }


/**
 * @brief Метод установки скорости эмиссии и размера листа финансирования
 *       
 *
 * @param[in]  op    The new value
 */
   [[eosio::action]] void unicore::setemi(eosio::name hostname, uint64_t percent, uint64_t gtop){
      require_auth(hostname);
      account_index hosts (_me, hostname.value);
      auto host = hosts.find(hostname.value);
      eosio::check(host != hosts.end(), "Host not exist");
      
      auto ahost = host->get_ahost();
      auto root_symbol = host->get_root_symbol();
  
      emission_index emis(_me, hostname.value);
      auto emi = emis.find(hostname.value);
      eosio::check(gtop <= 100, "Goal top should be less then 100");
      eosio::check(percent <= 1000 * PERCENT_PRECISION, "Emission percent should be less then 100 * PERCENT_PRECISION");
      
      emis.modify(emi, hostname, [&](auto &e){
          e.percent = percent;
          e.gtop = gtop;
      });
    }


  /**
   * @brief      Метод вывода баланса цели
   * Выводит доступный баланс цели на аккаунт бенефактора (постановщика) цели.
   *
   * @param[in]  op    The operation
   */
	[[eosio::action]] void unicore::gwithdraw(eosio::name username, eosio::name host, uint64_t goal_id){
		require_auth(username);
		
		goals_index goals(_me, host.value);
		auto goal = goals.find(goal_id);
		
		eosio::check(goal != goals.end(), "Goal is not founded");
		
		account_index accounts(_me, (goal->host).value);
		auto acc = accounts.find((goal->host).value);
		eosio::check(acc != accounts.end(), "Host is not founded");

		auto root_symbol = (acc->root_token).symbol;
    eosio::check(acc->direct_goal_withdraw == true, "Direct withdraw from goal is prohibited.");
		eosio::check(goal->creator == username, "You are not owner of this goal");
		eosio::check((goal->available).amount > 0, "Cannot withdraw a zero amount");

    eosio::asset on_withdraw = goal->available;
    	
  	action(
      permission_level{ _me, "active"_n },
      acc->root_token_contract, "transfer"_n,
      std::make_tuple( _me, goal->benefactor, on_withdraw, std::string("Goal Withdraw")) 
    ).send();

    goals.modify(goal, username, [&](auto &g){
    	g.available = asset(0, root_symbol);
    	g.withdrawed += on_withdraw;
    });

	};
