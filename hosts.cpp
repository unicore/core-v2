

using namespace eosio;

struct hosts_struct {

    std::string asset_to_string(eosio::asset val){
        uint64_t v = val.symbol.value;
        v >>= 8;
        string result;
        while (v > 0) {
                char c = static_cast<char>(v & 0xFF);
                result += c;
                v >>= 8;
        }
        return result;
    };

    void set_architect_action (const setarch &op){
        require_auth(op.host);

        account_index accounts(_self, _self);
        auto acc = accounts.find(op.host);
        eosio_assert(acc != accounts.end(), "Host is not found");
        accounts.modify(acc, _self, [&](auto &a){
            a.architect = op.architect;
        });

    };

    void deactivate_action(const deactivate &op){
        require_auth(op.architect);

        account_index accounts(_self, _self);
        auto acc = accounts.find(op.host);

        eosio_assert(acc != accounts.end(), "Host is not found");
        eosio_assert(acc->activated == true, "Host is already deactivated");
        eosio_assert(acc->current_pool_num == 1, "Cannot deactivate protocol after rotation launch.");
        eosio_assert(acc->current_cycle_num == 1, "Cannot deactivate protocol after rotation launch.");
        
        accounts.modify(acc, _self, [&](auto &a){
            a.activated = false;
        });
    };

    void upgrade_action(const upgrade &op){
        require_auth(op.username);
        
        eosio_assert(op.purpose.length() > 0, "Purpose should contain a symbols. Describe the idea shortly.");

        //check title lenght
        eosio_assert((op.title.length() < 1024) && (op.title.length() > 0) , "Title should be more then 10 symbols and less then 1024");
        user_index users(_self,_self);
        auto user = users.find(op.username);
        eosio_assert(user != users.end(), "User is not registered");


        eosio_assert(op.title.length() < 100, "Title is a maximum 100 symbols");
        eosio_assert(op.purpose.find("&#") , "Symbols '&#' is prohibited in purpose");

        eosio_assert(op.quote_amount.amount > 0, "Quote amount must be greater then zero");
        eosio_assert(op.quote_amount.symbol == _SYM, "Quote symbol for market is only CORE");
        
        eosio_assert(op.consensus_percent <= 100 * PERCENT_PRECISION, "consensus_percent should be between 0 and 100 * PERCENT_PRECISION (1000000)");
        eosio_assert(op.referral_percent <= 100 * PERCENT_PRECISION, "referral_percent should be between 0 and 100 * PERCENT_PRECISION (1000000)");
        
        eosio_assert(op.dacs_percent <= 100 * PERCENT_PRECISION, "dacs_percent should be between 0 and 100 * PERCENT_PRECISION (1000000)");
        eosio_assert(op.referral_percent + op.dacs_percent == 100 * PERCENT_PRECISION, "Referral and Dacs percent should me 100% PERCENT_PRECISION in their summ");

        //CHECK for referal levels;
        uint64_t level_count = 0;
        uint64_t percent_count = 0;
        uint64_t prev_level = 100 * PERCENT_PRECISION;

        for (auto level : op.levels){
            level_count++;
            eosio_assert(level <= prev_level, "Percentage on each referal level should decrease or not change");
            //TODO WHY?
            eosio_assert(level != 0, "Zero-level is prohibited.");
            percent_count += level;
        };

        eosio_assert(percent_count == 100 * PERCENT_PRECISION, "Summ of all levels should be 100%");
        eosio_assert(level_count <= _MAX_LEVELS, "Exceed the maximum number of levels");
        //END CHECK

        account_index accounts(_self, _self);
        
        auto itr = accounts.find(op.username);
        eosio_assert(itr == accounts.end(), "Account is already upgraded to Host");

        //check and set shares quantity
       
       	eosio_assert(op.total_shares >= 100, "Total shares must be greater or equal 100");
        //check for exist quote and root tokens


        auto failure_if_root_not_exist = eosio::token(op.root_token_contract).get_supply(eosio::symbol_type(op.root_token.symbol).name()).amount;
        
        
        auto to_pay = op.quote_amount;

        user_index refs(_self, _self);
        auto ref = refs.find(op.username);
        account_name referer = ref->referer;

        std::vector<account_name> empty_dacs;
        std::vector<account_name> empty_fhosts;

        accounts.emplace(_self, [&](auto &a){
            a.username = op.username;
            a.architect = op.username;
            a.hoperator = op.username;
            a.activated = false;
            a.title = op.title;
            a.purpose = op.purpose;
            a.dacs = empty_dacs;
            a.dac_mode = 0;
            a.total_shares = op.total_shares;
            a.quote_amount = op.quote_amount;
            a.root_token = op.root_token;
            a.symbol = asset_to_string(op.root_token);
            a.precision = op.root_token.symbol.precision();
            a.root_token_contract = op.root_token_contract;
            a.meta = op.meta;
            a.registered_at = eosio::time_point_sec(now());
            a.payed = false;
            a.to_pay = to_pay;
            a.ahost = op.username;
            a.fhosts = empty_fhosts;
            a.fhosts_mode = 0;
            a.consensus_percent = op.consensus_percent;
            a.referral_percent = op.referral_percent;
            a.dacs_percent = op.dacs_percent;
            a.levels= op.levels;
        });

        users.modify(user, _self, [&](auto &u){
            u.is_host = true;
        });

        emission_index emis(_self, _self);
        auto emi = emis.find(op.username);
        eosio_assert(emi == emis.end(), "Emission object already created");

        emis.emplace(op.username, [&](auto &e){
            e.host = op.username;
            e.percent = 0;
            e.fund = asset(0, op.root_token.symbol);
        });


    }


    void set_emission_action(const setemi&op){
        require_auth(op.host);
        account_index hosts (_self, _self);
        auto host = hosts.find(op.host);
        eosio_assert(host != hosts.end(), "Host not exist");
        
        auto ahost = host->get_ahost();
        auto root_symbol = host->get_root_symbol();
    
        emission_index emis(_self, _self);
        auto emi = emis.find(op.host);
        eosio_assert(op.gtop < 100, "Goal top should be less then 100");
        eosio_assert(op.percent < 100 * PERCENT_PRECISION, "Emission percent should be less then 100 * PERCENT_PRECISION");
        
        emis.modify(emi, op.host, [&](auto &e){
            e.percent = op.percent;
            e.gtop = op.gtop;
        });

    }

    
    void create_chost_action(const cchildhost &op){
    	auto parent_host = op.parent_host;
    	auto chost = op.chost;
    	account_index hosts(_self, _self);
    	auto acc = hosts.find(parent_host);
    	eosio_assert(acc != hosts.end(), "Parent host is not exist");
    	require_auth(parent_host);
    	eosio_assert(acc->activated == true, "Main host should be activated before set a child host");
    	eosio_assert( is_account( chost ), "Child account does not exist");
    	
    	std::vector<account_name> childs = acc->chosts;

    	//check for exist in main hosts;
    	auto is_exist = hosts.find(chost);
    	eosio_assert(is_exist == hosts.end(), "Child host already registered like father host");
		eosio_assert(acc->non_active_chost == false, "Founded not activated child host");
    	
    	// //Check for exist not active child
    	if (childs.begin() != childs.end()){
    		account_name last_child = childs.back() - 1;
		}
    	
    	childs.push_back(chost);

    	eosio::asset to_pay = asset(10000, _SYM);
        
    	hosts.modify(acc, parent_host, [&](auto &h){
    		h.chosts = childs;
    		h.payed = false;
    		h.to_pay = to_pay;
    		h.parameters_setted = false;
    		h.non_active_chost = true;
    	});


    }

    void pay_for_upgrade(account_name username, eosio::asset amount){
    	account_index hosts(_self, _self);

    	auto host = hosts.find(username);
    	eosio_assert(host != hosts.end(), "Host is not founded");
        
   		//PAY for upgrade only in CORE!.
    	eosio_assert(amount.symbol == _SYM, "Wrong asset. Only CORE token can be used for upgrade");
    	
	   	//Check for enough upgrade amount for quote asset
    	eosio_assert(amount == host->to_pay, "Amount is not equal amount for upgrade");
    	eosio_assert(host->payed == 0, "Already payed");

    	std::vector<account_name> childs = host->chosts;
	   
    	hosts.modify(host, _self, [&](auto &h){
			h.payed = true;
			h.to_pay = asset(0, _SYM);
		});    	

        if (childs.begin() == childs.end())
    	{
    		shares().create_bancor_market(username, host->total_shares, host->quote_amount);
		};

	  
    };

    void edithost_action(const edithost &op){
        require_auth (op.architect);

        account_index hosts(_self, _self);
        auto host = hosts.find(op.host);
        eosio_assert(host->architect == op.architect, "You are not architect of currenty community");

        eosio_assert(host != hosts.end(), "Host is not founded");
        
        eosio_assert(op.purpose.length() > 0, "Purpose should contain a symbols. Describe the idea shortly.");

        //check title lenght
        eosio_assert((op.title.length() < 1024) && (op.title.length() > 0) , "Title should be more then 10 symbols and less then 1024");
        

        hosts.modify(host, op.architect, [&](auto &h){
            h.purpose = op.purpose;
            h.meta = op.meta;
            h.title = op.title;
        });
        
    }

};