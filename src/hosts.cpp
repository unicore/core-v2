

namespace eosio {

/**
 * @brief      Структура хоста
 */


    /**
     * @brief      Returns a string representation of an asset.
     *
     * @param[in]  val   The value
     *
     * @return     String representation of the asset.
     */
    std::string unicore::symbol_to_string(eosio::asset val) const
    {
        uint64_t v = val.symbol.code().raw();
        v >>= 8;
        string result;
        while (v > 0) {
                char c = static_cast<char>(v & 0xFF);
                result += c;
                v >>= 8;
        }
        return result;
    }

    /**
     * @brief      Sets the architect.
     * Устанавливает архитектора сообщества, обладающего полномочиями специальных действий. 
     * 
     * @param[in]  op    The new value
     */
    [[eosio::action]] void unicore::setarch(name host, name architect){
        require_auth(host);

        account_index accounts(_me, host.value);
        auto acc = accounts.find(host.value);
        eosio::check(acc != accounts.end(), "Host is not found");
        accounts.modify(acc, _me, [&](auto &a){
            a.architect = architect;
        });

    };

    [[eosio::action]] void unicore::setahost(eosio::name host, eosio::name ahostname){
        require_auth(host);

        ahosts_index coreahosts(_me, _me.value);
        auto corehost = coreahosts.find(host.value);

        eosio::check(corehost != coreahosts.end(), "Core host is not found");
        eosio::check(corehost -> type == "platform"_n, "Host is not a platform");

        ahosts_index ahosts(_me, host.value);
        auto ahost = ahosts.find(ahostname.value);

        eosio::check(ahost == ahosts.end(), "Host is already exist in a platform");
        print("in here");

        auto coreahost = coreahosts.find(ahostname.value);
        eosio::check(coreahost != coreahosts.end(), "Core ahost is not found");

        ahosts.emplace(_me, [&](auto &a){
            a.username = ahostname;
            a.type = coreahost -> type;
            a.title = coreahost -> title;
            a.purpose = coreahost -> purpose;
            a.manifest = coreahost -> manifest;
            a.comments_is_enabled = coreahost -> comments_is_enabled;
            a.meta = coreahost -> meta;
            a.is_host = coreahost -> is_host;
        });
    
    }


    [[eosio::action]] void unicore::closeahost(eosio::name host){
        require_auth(_self);

        ahosts_index coreahosts(_me, _me.value);
        auto corehost = coreahosts.find(host.value);

        eosio::check(corehost != coreahosts.end(), "Core host is not found");
        
        coreahosts.erase(corehost);

    }

    [[eosio::action]] void unicore::openahost(eosio::name host){
        require_auth(_self);

        ahosts_index coreahosts(_me, _me.value);
        auto corehost = coreahosts.find(host.value);

        eosio::check(corehost == coreahosts.end(), "Core host is already found");
        account_index accounts(_me, host.value);
        auto acc = accounts.find(host.value);
        
        eosio::check(acc != accounts.end(), "Host is not found");

        coreahosts.emplace(_self, [&](auto &c){
            c.username = acc -> username;
            c.is_host = true;
            c.title = acc -> title;
            c.purpose = acc -> purpose;
            c.comments_is_enabled = false;
            c.meta = acc -> meta;
            c.type = "simple"_n;
        });

    }

    [[eosio::action]] void unicore::rmahost(eosio::name host, eosio::name ahostname){
        require_auth(host);

        ahosts_index coreahosts(_me, _me.value);
        auto corehost = coreahosts.find(host.value);

        eosio::check(corehost != coreahosts.end(), "Core host is not found");
        // eosio::check(corehost -> type == "platform"_n, "Host is not a platform");

        ahosts_index ahosts(_me, host.value);
        auto ahost = ahosts.find(ahostname.value);

        eosio::check(ahost != ahosts.end(), "AHOST is not found under platform");
        ahosts.erase(ahost);

    }

    [[eosio::action]] void unicore::setwebsite(eosio::name host, eosio::name ahostname, eosio::string website, eosio::name type){
        require_auth(host);

        ahosts_index coreahosts(_me, _me.value);
        auto corehost = coreahosts.find(ahostname.value);

        eosio::check(corehost != coreahosts.end(), "Core host is not found");
        auto hash = corehost -> hashit(website);
        
        auto hash_index = coreahosts.template get_index<"byuuid"_n>();
        auto exist = hash_index.find(hash);


        if ( exist != hash_index.end() && exist -> username != host && exist->website != "") {
            eosio::check(exist == hash_index.end(), "Current website is already setted to some host");
        }

        if (host.value == ahostname.value)
            coreahosts.modify(corehost, host, [&](auto &ch){
                ch.website = website;
                ch.hash = hash;
                ch.type = type;
            });        


        ahosts_index coreahostsbyusername(_me, _me.value);
        auto coreahostbyusername = coreahostsbyusername.find(ahostname.value);

        ahosts_index ahosts(_me, host.value);
        auto ahost = ahosts.find(ahostname.value);
        
        if (ahost == ahosts.end()){
            
            ahosts.emplace(host,[&](auto &ah) {
                ah.username = coreahostbyusername -> username;
                ah.is_host = coreahostbyusername -> is_host;
                ah.title = coreahostbyusername -> title;
                ah.purpose = coreahostbyusername -> purpose;
                ah.manifest = coreahostbyusername -> manifest;
                ah.comments_is_enabled = coreahostbyusername->comments_is_enabled;
                ah.meta = coreahostbyusername -> meta;
                ah.website = website;
                ah.hash = hash;
                ah.type = type;
            });

        } else {
            ahosts.modify(ahost, host, [&](auto &ah){
                ah.website = website;
                ah.hash = hash;
                ah.type = type;
            });            
        }    
    
        


        
    }
    /**
     * @brief      Метод апгрейда аккаунта до статуса сообщества
     * Принимает ряд параметров, такие как процент консенсуса, реферальный процент, уровни вознаграждений финансовых партнеров, 
     * корпоративный процент, а так же параметры рынка силы. 
     *  
     * @param[in]  op    The operation
     */
    [[eosio::action]] void unicore::upgrade(eosio::name username, eosio::name platform, std::string title, std::string purpose, uint64_t total_shares, eosio::asset quote_amount, eosio::name quote_token_contract, eosio::asset root_token, eosio::name root_token_contract, bool voting_only_up, uint64_t consensus_percent, uint64_t referral_percent, uint64_t dacs_percent, uint64_t cfund_percent, uint64_t hfund_percent, std::vector<uint64_t> levels, uint64_t emission_percent, uint64_t gtop, std::string meta){
        require_auth(username);
        
        // eosio::check(purpose.length() > 0, "Purpose should contain a symbols. Describe the idea shortly.");

        //eosio::check title lenght
        // eosio::check((title.length() < 1024) && (title.length() > 0) , "Title should be more then 10 symbols and less then 1024");
        partners_index users(_partners, _partners.value);
        
        auto user = users.find(username.value);
        // eosio::check(user != users.end(), "User is not registered");


        eosio::check(title.length() < 100, "Title is a maximum 100 symbols");
        eosio::check(purpose.find("&#") , "Symbols '&#' is prohibited in purpose");

        eosio::check(quote_amount.amount > 0, "Quote amount must be greater then zero");
        // eosio::check(quote_amount.symbol == _SYM, "Quote symbol for market is only CORE");
        
        eosio::check(consensus_percent <= 100 * ONE_PERCENT, "consensus_percent should be between 0 and 100 * ONE_PERCENT (1000000)");
        
        eosio::check(referral_percent + dacs_percent + cfund_percent + hfund_percent == 100 * ONE_PERCENT, "All payment percents should equal 100 * ONE_PERCENT (1000000)");
        
        eosio::check(emission_percent <= 1000 * ONE_PERCENT, "emission_percent should be between 0 and 1000 * ONE_PERCENT (10000000)");
        eosio::check(gtop <= 100, "Goal top should be less then 100");

        //eosio::check for referal levels;
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
        //END eosio::check

        account_index accounts(_me, username.value);
        
        auto itr = accounts.find(username.value);
        eosio::check(itr == accounts.end(), "Account is already upgraded to Host");

        //eosio::check and set shares quantity
       
       	eosio::check(total_shares >= 100, "Total shares must be greater or equal 100");
        //eosio::check for exist quote and root tokens

        auto failure_if_root_not_exist1   = eosio::token::get_supply(root_token_contract, root_token.symbol.code() );
        auto failure_if_root_not_exist2   = eosio::token::get_supply(quote_token_contract, quote_amount.symbol.code() );

        
        auto to_pay = quote_amount;

        auto ref = users.find(username.value);
        eosio::name referer = ref->referer;

        std::vector<eosio::name> empty_dacs;
        std::vector<eosio::name> empty_fhosts;

        gpercents_index gpercents(_me, _me.value);
        auto syspercent = gpercents.find("system"_n.value);
        eosio::check(syspercent != gpercents.end(), "Contract is not active");


        accounts.emplace(_me, [&](auto &a){
            a.username = username;
            a.architect = username;
            a.hoperator = username;
            a.type = "tin"_n;
            a.chat_mode = "simple"_n;
            a.activated = false;
            a.dac_mode = 0;
            a.total_dacs_weight = 0;
            a.total_shares = total_shares;
            a.quote_amount = quote_amount;
            a.quote_symbol = quote_amount.symbol.code().to_string();
            a.quote_token_contract = quote_token_contract;
            a.quote_precision = quote_amount.symbol.precision();
            a.root_token = root_token;
            a.asset_on_sale = asset(0, root_token.symbol);
            a.asset_on_sale_precision = a.asset_on_sale.symbol.precision();
            a.asset_on_sale_symbol = a.asset_on_sale.symbol.code().to_string();
            a.voting_only_up = voting_only_up;
            a.symbol = root_token.symbol.code().to_string();
            a.precision = root_token.symbol.precision();
            a.root_token_contract = root_token_contract;
            a.meta = meta;
            a.registered_at = eosio::time_point_sec(eosio::current_time_point().sec_since_epoch());
            a.payed = false;
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
        });

        ahosts_index coreahosts(_me, _me.value);
        coreahosts.emplace(username, [&](auto &a){
            a.username = username;
            a.title = title;
            a.purpose = purpose;
            a.is_host = true;
            a.manifest = "";
            a.comments_is_enabled = false;
        });

        if (platform != _self){
            ahosts_index ahosts(_me, platform.value);
            ahosts.emplace(username, [&](auto &a){
                a.username = username;
                a.title = title;
                a.purpose = purpose;
                a.is_host = true;
                a.manifest = "";
                a.comments_is_enabled = false;
            });
        }
        

        emission_index emis(_me, username.value);
        auto emi = emis.find(username.value);
        eosio::check(emi == emis.end(), "Emission object already created");

        emis.emplace(username, [&](auto &e){
            e.host = username;
            e.gtop = gtop;
            e.percent = emission_percent;
            e.fund = asset(0, root_token.symbol);
        });


    }

    /**
     * @brief      Метод создания дочернего хоста
     * Позволяет сообществу на границе циклов изменять параметры финансового ядра, 
     * распологая их в области памяти аккаунта дочернего хоста. 
     *
     * @param[in]  op    The operation
     */
   [[eosio::action]] void unicore::cchildhost(eosio::name parent_host, eosio::name chost) {
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
        
    	eosio::asset to_pay = asset(10000, root_symbol);
        
    	hosts.modify(acc, parent_host, [&](auto &h){
    		h.chosts = childs;
    		h.payed = false;
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
    void unicore::pay_for_upgrade(eosio::name username, eosio::asset amount, eosio::name code){
    	account_index hosts(_me, username.value);

    	auto host = hosts.find(username.value);
    	eosio::check(host != hosts.end(), "Host is not founded");
        
   		eosio::check(amount.symbol == host->quote_amount.symbol, "Wrong symbol for market");
        
        eosio::check(code == host->quote_token_contract, "Wrong symbol for market");
        
	   	//eosio::check for enough upgrade amount for quote asset
    	eosio::check(amount == host->to_pay, "Amount is not equal amount for upgrade");
    	eosio::check(host->payed == 0, "Already payed");

    	std::vector<eosio::name> childs = host->chosts;
	   
    	hosts.modify(host, _me, [&](auto &h){
			h.payed = true;
			h.to_pay = asset(0, _SYM);
		});    	

        if (childs.begin() == childs.end())
    	{
    		unicore::create_bancor_market("POWER MARKET", 0, username, host->total_shares, host->quote_amount, host->quote_token_contract, _SHARES_VESTING_DURATION);
		};

	  
    };

    // /**
    //  * @brief      Метод редактирования времени 
    //  *
    //  * @param[in]  op    The operation
    //  */
    // void ehosttime_action(const ehosttime &op){
    //     require_auth (architect);

    //     account_index hosts(_me, _me);
    //     auto host = hosts.find(host.value);
    //     eosio::check(host->architect == architect, "You are not architect of currenty commquanty");
        
    //     eosio::check((priority_seconds < 7884000), "Pool Priority Seconds must be greater or equal then 0 sec and less then 7884000 sec");
    //     eosio::check((pool_timeout >= 60) && (pool_timeout < 7884000),"Pool Timeout must be greater or equal then 1 sec and less then 7884000 sec");
    
    //     spiral_index spiral(_me, host);
    //     auto sp = spiral.find(0);

    //     spiral.modify(sp, architect, [&](auto &s){
    //         s.pool_timeout = pool_timeout;
    //         s.priority_seconds = priority_seconds;
    //     });
    // }




     /**
     * @brief      Метод редактирования информации о хосте
     *
     * @param[in]  op    The operation
     */

    [[eosio::action]] void unicore::edithost(eosio::name architect, eosio::name host, eosio::string title, eosio::string purpose, eosio::string manifest, eosio::string meta){
        require_auth (architect);

        account_index accs(_me, host.value);
        auto acc = accs.find(host.value);
        eosio::check(acc->architect == architect, "You are not architect of current commquanty");

        eosio::check(acc != accs.end(), "Host is not founded");
        
        // eosio::check(purpose.length() > 0, "Purpose should contain a symbols. Describe the idea shortly.");

        //eosio::check title lenght
        // eosio::check((title.length() < 1024) && (title.length() > 0) , "Title should be more then 10 symbols and less then 1024");
        

        accs.modify(acc, architect, [&](auto &h){
            h.purpose = purpose;
            h.meta = meta;
            h.title = title;
        });

        ahosts_index coreahosts(_me, _me.value);
        auto coreahost = coreahosts.find(host.value);
        eosio::check(coreahost != coreahosts.end(), "AHost is not found");

        coreahosts.modify(coreahost, architect, [&](auto &a){
            a.title = title;
            a.purpose = purpose;
            a.manifest = manifest;
        });


        ahosts_index ahosts(_me, host.value);
        auto ahost = ahosts.find(host.value);
        if (ahost != ahosts.end()){
            ahosts.modify(ahost, architect, [&](auto &a){
                a.title = title;
                a.purpose = purpose;
                a.manifest = manifest;
            });
        }

    }


}