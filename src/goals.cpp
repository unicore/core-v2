
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
struct goal {
    void validate_permlink( const std::string& permlink )
        {
           check( 3 < permlink.size() && permlink.size() < 255, "Permlink is not a valid size." );
           
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
                      check( false, "Invalid permlink character");
                }
             }
        }

  /**
   * @brief      Метод создания цели
   * 
   * @param[in]  op    The new value
   */
	void set_goal_action(const setgoal &op){
		require_auth(op.username);
		
		goals_index goals(_self,op.host);
		account_index accounts(_self, op.host);

		user_index users(_self,_self);
    auto user = users.find(op.username);
    check(user != users.end(), "User is not registered");

		auto acc = accounts.find(op.host);
    auto root_symbol = (acc->root_token).symbol;

		auto username = op.username;
    auto title = op.title;
    auto description = op.description;
    auto host = op.host;
    auto target = op.target;
    bool validated = acc->consensus_percent == 0;
    
    validate_permlink(op.permlink);
    
    check(target.symbol == root_symbol, "Wrong symbol for this host");
    
    check(title.length() <= 100, "Short Description is a maximum 100 symbols. Describe the goal shortly");
    auto goal = goals.find(op.id);

    if (goal == goals.end()){
      goals.emplace(username, [&](auto &g){
        g.id = op.id;
        g.username = username;
        g.benefactor = op.benefactor;
        g.created = eosio::time_point_sec(now());
        g.host = host;
        g.permlink = op.permlink;
        g.title = title;
        g.description = description;
        g.target = target;
        g.withdrawed = asset(0, root_symbol);
        g.available = asset(0, root_symbol);
        g.validated = validated;
        g.expired_at = eosio::time_point_sec (now() + op.expiration);
      });      
    } else {
      check(username == goal->username, "Only creator can modify the goal");

      goals.modify(goal, username, [&](auto &g){
        g.benefactor = op.benefactor;
        g.title = title;
        g.description = description;
        g.target = target;
      });
    }

	};


  /**
   * @brief      Метод прямого финансирования цели
   * Используется архитектором для финансирования цели из фонда 
   *
   * @param[in]  op    The operation
   */
	void fund_goal_action(const dfundgoal &op){
		require_auth(op.architect);

		account_index accounts(_self, op.host);
		auto acc = accounts.find(op.host);
		check(acc->architect == op.architect, "Only architect can direct fund the goal");

    emission_index emis(_self, op.host);
    auto emi = emis.find(op.host);

    check(emi->fund >= op.amount, "Not enough tokens for fund the goal");

    donate_action(op.architect, op.host, op.goal_id, op.amount, _self);

    emis.modify(emi, op.architect, [&](auto &e){
    	e.fund = e.fund - op.amount;
    });

	};




 /**
  * @brief      Метод редактирования цели
  *
  * @param[in]  op    The operation
  */

	void edit_goal_action(const editgoal &op){
		require_auth(op.username);
		
		goals_index goals(_self,op.host);
    account_index accounts (_self, op.host);

    auto acc = accounts.find(op.host);
    auto root_symbol = (acc->root_token).symbol;

    auto username = op.username;
    auto benefactor = op.benefactor;
    auto title = op.title;
    auto description = op.description;
    auto host = op.host;
    auto goal_id = op.goal_id;
    auto target = op.target;
    
    check(target.symbol == root_symbol, "Wrong symbol for this host");

    check(title.length() <= 100, "Short Description is a maximum 100 symbols");

    auto goal = goals.find(goal_id);
    check(goal != goals.end(), "Goal is not exist");
    check(goal->username == op.username, "Dont have permissions for edit goal");

    if (goal->target != op.target){
    	check(goal -> validated == false, "Impossible edit amount after validation");
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
	void del_goal_action(const delgoal &op){
		require_auth(op.username);
		goals_index goals(_self, op.host);
        
		auto username = op.username;
		auto goal_id = op.goal_id;

		auto goal = goals.find(goal_id);
		check(goal != goals.end(), "Goal is not exist");
    check(goal->username == op.username, "Wrong creator of goal");

		goals.erase(goal);

	}

  /**
   * @brief      Метод создания отчета о завершенной цели
   *
   * @param[in]  op    The operation
   */
	void report_action(const report &op){
		require_auth(op.username);
		
		goals_index goals(_self, op.host);
		auto username = op.username;
		auto goal_id = op.goal_id;
		auto report = op.report;

		account_index accounts(_self, op.host);
		auto acc = accounts.find(op.host);
		check(acc != accounts.end(), "Host is not found");

		auto goal = goals.find(goal_id);

		check(op.username == goal->benefactor, "Only benefactor can set report");
		
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
	void check_action(const check &op){
		require_auth(op.architect);
		account_index accounts(_self, op.host);
		auto acc = accounts.find(op.host);
		check(acc != accounts.end(), "Host is not found");

		goals_index goals(_self, op.host);
		auto goal = goals.find(op.goal_id);
		check(goal != goals.end(), "Goal is not found");
		check(op.architect == acc->architect, "Only architect can check report for now");
		check(goal->reported == true, "Goals without reports cannot be checked");

		goals.modify(goal, op.architect, [&](auto &g){
			g.checked = true;
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
	void donate_action(eosio::name from, eosio::name host, uint64_t goal_id, eosio::asset quantity, eosio::name code){
		// require_auth(from);
		
		goals_index goals(_self, host);
		account_index accounts(_self, host);
		auto acc = accounts.find(host);

		if (code != _self) //For direct donate from fund and by architects action
			check(acc->root_token_contract == code, "Wrong root token contract for this host");
		
		check((acc->root_token).symbol == quantity.symbol, "Wrong root symbol for this host");
		
		auto goal = goals.find(goal_id);
		
		check(goal != goals.end(), "Goal is not exist");

		bool completed = goal->available + goal->withdrawed + quantity >= goal->target;		
		
		goals.modify(goal, _self, [&](auto &g){
			g.available += quantity;
			g.completed = completed;
		});

	}

  /**
   * @brief      Метод финансирования цели через оператора сообщества.
   * Позволяет оператору сообщества расходовать баланс целей на перечисления прямым спонсорам. 
   * Используется в риверсной экономической модели, когда корневой токен сообщества является котировочным токеном силы сообщества, 
   * и накаким другим способом изначально не распределяется, кроме как на спонсоров целей (дефицитная ICO - модель).
   *
   * @param[in]  op    The operation
   */
  void gsponsor_action(const gsponsor &op){
    require_auth (op.hoperator);

    account_index accounts(_self, op.host);
    auto acc = accounts.find(op.host);
    check(acc->hoperator == op.hoperator, "Wrong operator for this host");
    goals_index goals(_self, op.host);

    auto goal = goals.find(op.goal_id);
    check(goal != goals.end(), "Goal is not founded");
    auto root_symbol = (acc->root_token).symbol;

    check(op.amount.symbol == root_symbol, "Wrong root symbol");

    check(goal->available >= op.amount, "Not enough tokens for sponsorhip action. ");

    action(
      permission_level{ _self, "active"_n },
      acc->root_token_contract, "transfer"_n,
      std::make_tuple( _self, op.reciever, op.amount, std::string("Sponsor")) 
    ).send();

    goals.modify(goal, op.hoperator, [&](auto &g){
      g.available = g.available - op.amount;
      g.withdrawed = g.withdrawed + op.amount;
    });

  }


/**
 * @brief Метод установки скорости эмиссии и размера листа финансирования
 *       
 *
 * @param[in]  op    The new value
 */
    void set_emission_action(const setemi&op){
      require_auth(op.host);
      account_index hosts (_self, op.host);
      auto host = hosts.find(op.host);
      check(host != hosts.end(), "Host not exist");
      
      auto ahost = host->get_ahost();
      auto root_symbol = host->get_root_symbol();
  
      emission_index emis(_self, op.host);
      auto emi = emis.find(op.host);
      check(op.gtop <= 100, "Goal top should be less then 100");
      check(op.percent <= 1000 * PERCENT_PRECISION, "Emission percent should be less then 100 * PERCENT_PRECISION");
      
      emis.modify(emi, op.host, [&](auto &e){
          e.percent = op.percent;
          e.gtop = op.gtop;
      });
    }


  /**
   * @brief      Метод вывода баланса цели
   * Выводит доступный баланс цели на аккаунт бенефактора (постановщика) цели.
   *
   * @param[in]  op    The operation
   */
	void gwithdraw_action(const gwithdraw &op){
		require_auth(op.username);
		auto username = op.username;
		auto goal_id = op.goal_id;
		
		goals_index goals(_self, op.host);
		auto goal = goals.find(goal_id);
		
		check(goal != goals.end(), "Goal is not founded");
		
		account_index accounts(_self, goal->host);
		auto acc = accounts.find(goal->host);
		check(acc != accounts.end(), "Host is not founded");

		auto root_symbol = (acc->root_token).symbol;

		check(goal->username == username, "You are not owner of this goal");
		check((goal->available).amount > 0, "Cannot withdraw a zero amount");

    eosio::asset on_withdraw = goal->available;
    	
  	action(
      permission_level{ _self, "active"_n },
      acc->root_token_contract, "transfer"_n,
      std::make_tuple( _self, goal->benefactor, on_withdraw, std::string("Goal Withdraw")) 
    ).send();

    goals.modify(goal, username, [&](auto &g){
    	g.available = asset(0, root_symbol);
    	g.withdrawed += on_withdraw;
    });

	}

};
