using namespace eosio;

/**
 * @brief      Структура хоста
*/


/**
 * @brief 

 *
*/
[[eosio::action]] void unicore2::changemode(eosio::name host, eosio::name mode) {
    require_auth(host);

    account_index accounts(_me, host.value);
    auto acc = accounts.find(host.value);
    
    eosio::check(acc != accounts.end(), "Host is not found");
    eosio::check(acc -> type == "tin"_n, "Cannot change mode after first change");

    accounts.modify(acc, host, [&](auto &h){
        h.type = mode;
    });
}

    
[[eosio::action]] void unicore2::setstartdate(eosio::name host, eosio::time_point_sec start_at){
    require_auth(host);

    account_index hosts(_me, host.value);
    pool_index pools(_me, host.value);

    auto acc = hosts.find(host.value);
    
    eosio::check(acc -> current_pool_num == 1, "Only on the first pool start date can be changed");
    auto pool1 = pools.find(acc -> current_pool_id);
    auto pool2 = pools.find(acc-> current_pool_id + 1);

    pools.modify(pool1, host, [&](auto &p){
        p.pool_started_at = start_at;
    });

    pools.modify(pool2, host, [&](auto &p){
        p.pool_started_at = start_at;
    });
}


    /**
     * @brief      Sets the architect.
     * Устанавливает архитектора сообщества, обладающего полномочиями специальных действий. 
     * 
     * @param[in]  op    The new value
     */
    [[eosio::action]] void unicore2::setarch(name host, name architect){
        
        account_index accounts(_me, host.value);
        auto acc = accounts.find(host.value);
        eosio::check(acc != accounts.end(), "Host is not found");
        
        eosio::check(has_auth(_me) || has_auth(host), "missing required authority");
        
        accounts.modify(acc, _me, [&](auto &a){
            a.architect = architect;
        });

    };


    /**
     * @brief      Метод установки уровней вознаграждений
     * @param[in]  op    The operation
     */
    [[eosio::action]] void unicore2::setlevels(eosio::name host, std::vector<uint64_t> levels){
        require_auth(host);
        account_index accounts(_me, host.value);
        
        auto acc = accounts.find(host.value);
        eosio::check(acc != accounts.end(), "Account is not a host");

        //eosio::check for referal levels;
        uint64_t level_count = 0;
        uint64_t percent_count = 0;
        uint64_t prev_level = 100 * ONE_PERCENT;

        for (auto level : levels){
            level_count++;
            eosio::check(level != 0, "Zero-level is prohibited.");
            percent_count += level;
        };

        eosio::check(percent_count == 100 * ONE_PERCENT, "Summ of all levels should be 100%");
        eosio::check(level_count <= _MAX_LEVELS, "Exceed the maximum number of levels");
        
        accounts.modify(acc, host, [&](auto &a){
            a.levels = levels;
        });


    }

    /**
     * @brief      Метод апгрейда аккаунта до статуса сообщества
     * Принимает ряд параметров, такие как процент консенсуса, реферальный процент, уровни вознаграждений финансовых партнеров, 
     * корпоративный процент, а так же параметры рынка силы. 
     *  
     * @param[in]  op    The operation
     */
    [[eosio::action]] void unicore2::upgrade(eosio::name username, eosio::name platform, std::string title, std::string purpose, uint64_t total_shares, eosio::asset quote_amount, eosio::name quote_token_contract, eosio::asset root_token, eosio::name root_token_contract, bool voting_only_up, uint64_t consensus_percent, uint64_t referral_percent, uint64_t dacs_percent, uint64_t cfund_percent, uint64_t hfund_percent, std::vector<uint64_t> levels, uint64_t emission_percent, uint64_t gtop, std::string meta){
        require_auth(username);
        
        partners2_index users(_partners, _partners.value);
        
        auto user = users.find(username.value);
        eosio::check(title.length() < 100, "Title is a maximum 100 symbols");
        eosio::check(purpose.find("&#") , "Symbols '&#' is prohibited in purpose");

        eosio::check(consensus_percent <= 100 * ONE_PERCENT, "consensus_percent should be between 0 and 100 * ONE_PERCENT (1000000)");
        eosio::check(referral_percent + dacs_percent + cfund_percent + hfund_percent == 100 * ONE_PERCENT, "All payment percents should equal 100 * ONE_PERCENT (1000000)");
        eosio::check(emission_percent <= 1000 * ONE_PERCENT, "emission_percent should be between 0 and 1000 * ONE_PERCENT (10000000)");
        eosio::check(gtop <= 100, "Goal top should be less then 100");

        uint64_t level_count = 0;
        uint64_t percent_count = 0;
        uint64_t prev_level = 100 * ONE_PERCENT;

        for (auto level : levels){
            level_count++;
            // eosio::check(level <= prev_level, "Percentage on each referal level should decrease or not change");
            //TODO WHY?
            eosio::check(level != 0, "Zero-level is prohibited.");
            percent_count += level;
        };

        eosio::check(percent_count == 100 * ONE_PERCENT, "Summ of all levels should be 100%");
        eosio::check(level_count <= _MAX_LEVELS, "Exceed the maximum number of levels");

        account_index accounts(_me, username.value);
        
        auto itr = accounts.find(username.value);
        eosio::check(itr == accounts.end(), "Account is already upgraded to Host");

        auto failure_if_root_not_exist1   = eosio::token::get_supply(root_token_contract, root_token.symbol.code() );
                
        uint64_t to_pay_amount = unicore2::getcondition(platform, "hostfee");
        eosio::asset to_pay = asset(to_pay_amount, root_token.symbol);

        //Создаём запись о платформе создания для возврата платежа с оплатой
        account3_index accounts3(_me, _me.value);
        accounts3.emplace(_me, [&](auto &cr){
            cr.username = username;
            cr.platform = platform;
        });

        auto ref = users.find(username.value);
        eosio::name referer = ref->referer;

        std::vector<eosio::name> empty_dacs;
        std::vector<eosio::name> empty_fhosts;

        gpercents_index gpercents(_me, _me.value);
        auto syspercent = gpercents.find("system"_n.value);
        eosio::check(syspercent != gpercents.end(), "Contract is not active");


        accounts.emplace(_me, [&](auto &a){
            a.sale_mode = "direct"_n;
            a.sale_is_enabled = true;

            a.username = username;
            a.architect = username;
            a.hoperator = username;
            a.type = "tin"_n;
            a.chat_mode = "simple"_n;
            a.activated = false;
            a.dac_mode = 0;
            a.total_dacs_weight = 0;
            a.total_shares = total_shares;

            a.quote_amount = asset(0, root_token.symbol);
            a.quote_symbol = root_token.symbol.code().to_string();
            a.quote_token_contract = root_token_contract;
            a.quote_precision = root_token.symbol.precision();
            
            a.asset_on_sale = asset(0, _POWER);
            a.asset_on_sale_precision = _POWER.precision();
            a.asset_on_sale_symbol = _POWER.code().to_string();
            a.sale_shift = 0;
            a.voting_only_up = voting_only_up;
            
            a.root_token = root_token;
            a.symbol = root_token.symbol.code().to_string();
            a.precision = root_token.symbol.precision();
            a.root_token_contract = root_token_contract;
            
            a.meta = meta;
            a.registered_at = eosio::time_point_sec(eosio::current_time_point().sec_since_epoch());
            a.payed = to_pay.amount == 0 ? true : false;
            a.to_pay = to_pay;
            a.ahost = username;
            a.fhosts = empty_fhosts;
            a.fhosts_mode = 0;
            a.consensus_percent = consensus_percent;
            a.referral_percent = referral_percent;
            a.dacs_percent = dacs_percent;
            a.cfund_percent = cfund_percent;
            a.hfund_percent = hfund_percent;
            a.sys_percent = syspercent->value;
            a.levels= levels;
            a.title = title;
            a.purpose = purpose;
            a.power_market_id = username;
        });


        
    }

    /**
     * @brief      Метод создания дочернего хоста
     * Позволяет сообществу на границе циклов изменять параметры финансового ядра, 
     * распологая их в области памяти аккаунта дочернего хоста. 
     *
     * @param[in]  op    The operation
     */
   [[eosio::action]] void unicore2::cchildhost(eosio::name parent_host, eosio::name chost) {
    	account_index hosts(_me, parent_host.value);
    	auto acc = hosts.find(parent_host.value);
    	eosio::check(acc != hosts.end(), "Parent host is not exist");
    	require_auth(parent_host);
    	eosio::check(acc->activated == true, "Main host should be activated before set a child host");
    	eosio::check( is_account( chost ), "Child account does not exist");
    	
    	std::vector<eosio::name> childs = acc->chosts;

    	//eosio::check for exist in main hosts;
    	auto is_exist = hosts.find(chost.value);
    	eosio::check(is_exist == hosts.end(), "Child host already registered like father host");
		eosio::check(acc->non_active_chost == false, "Founded not activated child host");
    	
    	// //eosio::check for exist not active child
    	if (childs.begin() != childs.end()){
    		eosio::name last_child = childs.back();
		}
    	
    	childs.push_back(chost);

        auto root_symbol = acc->get_root_symbol();
        
    	eosio::asset to_pay = asset(0, root_symbol);
        
    	hosts.modify(acc, parent_host, [&](auto &h){
    		h.chosts = childs;
    		h.payed = true;
    		h.to_pay = to_pay;
    		h.parameters_setted = false;
    		h.non_active_chost = true;
    	});


    }

    /**
     * @brief      Метод оплаты комиссии создания дочернего хоста
     *
     * @param[in]  username  The username
     * @param[in]  amount    The amount
     */
    void unicore2::pay_for_upgrade(eosio::name username, eosio::asset amount, eosio::name code){
    	account_index hosts(_me, username.value);

    	auto host = hosts.find(username.value);
    	eosio::check(host != hosts.end(), "Host is not founded");
        
   		eosio::check(amount.symbol == host->to_pay.symbol, "Wrong symbol for market");
        
        eosio::check(code == host->root_token_contract, "Wrong symbol for market");
        
	   	//eosio::check for enough upgrade amount for quote asset
    	eosio::check(amount == host->to_pay, "Amount is not equal amount for upgrade");
    	eosio::check(host->payed == 0, "Already payed");

    	std::vector<eosio::name> childs = host->chosts;
	   
    	hosts.modify(host, _me, [&](auto &h){
			h.payed = true;
			h.to_pay = asset(0, _SYM);
		});    	

        
    };

    /**
     * @brief      Метод редактирования времени 
     *
     * @param[in]  op    The operation
     */
    [[eosio::action]] void unicore2::settiming(eosio::name host, uint64_t pool_timeout, uint64_t priority_seconds) {
        require_auth (host);

        account_index hosts(_me, host.value);
        auto acc = hosts.find(host.value);
        
        eosio::check((priority_seconds <= 315400000), "Pool Priority Seconds must be greater or equal then 0 sec and less then 315400000 sec");
        eosio::check((pool_timeout >= 60) && (pool_timeout < 7884000),"Pool Timeout must be greater or equal then 1 sec and less then 7884000 sec");
        eosio::check(acc -> current_pool_num < 3, "Timing changes only possible on the waiting mode");

        eosio::name main_host = acc->get_ahost();
        spiral_index spiral(_me, main_host.value);
        auto sp = spiral.find(0);

        spiral.modify(sp, host, [&](auto &s){
            s.pool_timeout = pool_timeout;
            s.priority_seconds = priority_seconds;
        });
    }


    [[eosio::action]] void unicore2::setflows(eosio::name host, uint64_t ref_percent, uint64_t dacs_percent, uint64_t cfund_percent, uint64_t hfund_percent){
        require_auth (host);        
        
        account_index hosts(_me, host.value);
        auto acc = hosts.find(host.value);
        
        eosio::check(ref_percent + dacs_percent + cfund_percent + hfund_percent == HUNDR_PERCENT, "All payment percents should equal 100 * ONE_PERCENT (1000000)");
        // eosio::check(acc -> current_pool_num < 3, "Flows changes only possible on the waiting mode");

        hosts.modify(acc, host, [&](auto &h){
            h.referral_percent = ref_percent;
            h.dacs_percent = dacs_percent;
            h.cfund_percent = cfund_percent;
            h.hfund_percent = hfund_percent;
        });

    }

     /**
     * @brief      Метод редактирования информации о хосте
     *
     * @param[in]  op    The operation
     */

    [[eosio::action]] void unicore2::edithost(eosio::name architect, eosio::name host, eosio::name platform, eosio::string title, eosio::string purpose, eosio::string manifest, eosio::name power_market_id, eosio::string meta){
        account_index accs(_me, host.value);
        auto acc = accs.find(host.value);
        
        eosio::check(has_auth(host) || has_auth(acc -> architect), "missing required authority");

        eosio::name payer = has_auth(host) ? host : acc -> architect;

        eosio::check(acc != accs.end(), "Host is not founded");
        
        accs.modify(acc, payer, [&](auto &h){
            h.purpose = purpose;
            h.meta = meta;
            h.title = title;
        });

    }


// }