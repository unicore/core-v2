

using namespace eosio;

struct hosts {

	// void create_token_contract(const createtoken &op){
	// 	auto host = op.host;
	// 	require_auth(op.host);
	// 	auto symbol_and_precision = op.symbol_and_precision;
	// 	auto quantity_for_pay = op.quantity;

	// 	eosio_assert(symbol_and_precision.amount == 0, "Amount new token should be zero");

	// 	const auto owner_auth = authority{
 //        	1,{},{{{_self, N(owner)}, 1}},{}};
    
	// 	const auto active_auth = authority{
	// 		1,{},{{{host, N(active)}, 1}},{}};
    	
	//     const auto ram_amount = buyrambytes(200 * 1024);
	//     const auto cpu = asset(1500);
	//     const auto net = asset(500);

 //    	eosio_assert(ram_amount + cpu + net <= quantity.amount, "Not enough core token amount for pay RAM, NET and CPU");

	//     eosio::name user = get_random_eosio::name();

	//     //TODO CHECK IT (REGISTRATOR ACCOUNT)

	//     //transfer fee to registrator

 //        action(
 //            permission_level{ _self, N(active) },
 //            acc->root_token_contract, N(transfer),
 //            std::make_tuple( _self, N(goals), amount, std::string("null")) 
 //        ).send();

	//     // create account

	//     action(
 //            permission_level{ N(goals), N(active) },
 //            N(eosio), N(newaccount),
 //            std::make_tuple(N(goals), user, owner_auth, active_auth)
 //        ).send();

	//     // buy ram
	//     action(
 //            permission_level{ N(goals), N(active) },
 //            N(eosio), N(buyram),
 //            std::make_tuple(N(goals), user, ram_amount)
 //        ).send();

	//     // delegate and transfer cpu and net
	//     action(
 //            permission_level{ N(goals), N(active) },
 //            N(eosio), N(delegatebw),
 //            std::make_tuple(N(goals), user, net, cpu, 1)
 //        ).send();


	// }

    void upgrade_action(const upgrade &op){
        require_auth(op.username);
        
        eosio_assert(op.purpose.length() < 1024, "Purpose is a maximum 1024 symbols. Describe the idea shortly.");
        //check title lenght

        eosio_assert(op.title.length() < 100, "Title is a maximum 100 symbols");
        eosio_assert(op.purpose.find("&#") , "Symbols '&#' is prohibited in purpose");

        eosio_assert(op.quote_amount.amount > 0, "Quote amount must be greater then zero");
        eosio_assert(op.quote_amount.symbol == _SYM, "Quote symbol for market is only CORE");
        
        eosio_assert(op.goal_validation_percent <= 100 * PERCENT_PRECISION, "goal_validation_percent should be between 0 and 100 * PERCENT_PRECISION (1000000)");
        eosio_assert(op.referral_percent <= 100 * PERCENT_PRECISION, "referral_percent should be between 0 and 100 * PERCENT_PRECISION (1000000)");

        //CHECK for referal levels;
        uint64_t level_count = 0;
        uint64_t percent_count = 0;
        uint64_t prev_level = 100 * PERCENT_PRECISION;

        for (auto level : op.levels){
            level_count++;
            eosio_assert(level <= prev_level, "Percentage on each referal level should decrease or not change");
            eosio_assert(level != 0, "Zero-level is prohibited.");
            percent_count += level;
        };

        eosio_assert(percent_count == 100 * PERCENT_PRECISION, "Error. Summ of all levels should be 100 * PERCENT_PRECISION (1000000)");
        eosio_assert(level_count <= _MAX_LEVELS, "Exceed the maximum number of levels");
        //END CHECK

        account_index accounts(_self, _self);
        
        auto itr = accounts.find(op.username);
        eosio_assert(itr == accounts.end(), "Account is already upgraded to Host");

        //check and set shares quantity
       
       	eosio_assert(op.total_shares >= 100, "Total shares must be greater or equal 100");
        //check for exist quote and root tokens


        auto failure_if_root_not_exist = eosio::token(op.root_token_contract).get_supply(eosio::symbol_type(op.root_token.symbol).name()).amount;
        
        fee_index fees(_self, _self);
        auto fee = fees.find(0);
        auto to_pay = fee->createhost + op.quote_amount;

        referal_index refs(_self, _self);
        auto ref = refs.find(op.username);
        eosio::name referer = ref->referer;

        accounts.emplace(_self, [&](auto &a){
            a.username = op.username;
            a.hoperator = referer;
            a.activated = false;
            a.title = op.title;
            a.purpose = op.purpose;
            a.total_shares = op.total_shares;
            a.quote_amount = op.quote_amount;
            a.root_token = op.root_token;
            a.root_token_contract = op.root_token_contract;
            a.meta = op.meta;
            a.registered_at = eosio::time_point_sec(now());
            a.payed = false;
            a.to_pay = to_pay;
            a.active_host = op.username;
            a.goal_validation_percent = op.goal_validation_percent;
            a.is_whitelisted = op.is_whitelisted;
            a.referral_percent = op.referral_percent;
            a.levels= op.levels;
        });

    }

    // eosio::name get_random_eosio::name(){
    // 	eosio::string allowed = "12345abcdefghijklmnopqrstuvwxyz";


	   //  checksum256 result;
    //     auto mixedBlock = tapos_block_prefix() * tapos_block_num();

    //      const char *mixedChar = reinterpret_cast<const char *>(&mixedBlock);
    //      sha256( (char *)mixedChar, sizeof(mixedChar), &result);
    //      const char *p64 = reinterpret_cast<const char *>(&result);
    //      eosio::string str_acc = "";

    //      for(int i = 0; i<_REGISTRATOR_SUFFIX_LENGHT; i++) {
    //        auto r = (abs((int64_t)p64[i]) % (31));
    //        str_acc += allowed[r];
    //      }

    //      //str_acc += _REGISTRATOR_STRING_SUFFIX;
         
    //      return eosio::string_to_name(str_acc.c_str());

    // };


    void create_child_host_action(const cchildhost &op){
    	auto parent_host = op.parent_host;
    	auto child_host = op.child_host;
    	account_index hosts(_self, _self);
    	auto acc = hosts.find(parent_host);
    	eosio_assert(acc != hosts.end(), "Parent host is not exist");
    	require_auth(parent_host);
    	eosio_assert(acc->activated == true, "Main host should be activated before set a child host");
    	eosio_assert( is_account( child_host ), "Child account does not exist");
    	
    	std::vector<eosio::name> childs = acc->childrens;

    	//check for exist in main hosts;
    	auto is_exist = hosts.find(child_host);
    	eosio_assert(is_exist == hosts.end(), "Child host already registered like father host");
		eosio_assert(acc->non_active_child == false, "Founded not activated child host");
    	
    	// //Check for exist not active child
    	if (childs.begin() != childs.end()){
    		eosio::name last_child = childs.back() - 1;
		}
    	
    	childs.push_back(child_host);
    	fee_index fees(_self, _self);
    	eosio::asset to_pay;

        auto fee = fees.find(0);
        
        if ((fee->createhost).amount == 0){
        	to_pay = asset(1, _SYM);

        } else {

			to_pay = fee->createhost;
        }
        
    	hosts.modify(acc, parent_host, [&](auto &h){
    		h.childrens = childs;
    		h.payed = false;
    		h.to_pay = to_pay;
    		h.parameters_setted = false;
    		h.non_active_child = true;
    	});


    }

    void pay_for_upgrade(eosio::name username, eosio::asset amount){
    	account_index hosts(_self, _self);

    	auto host = hosts.find(username);
    	
   		//PAY for upgrade only in CORE!.
    	eosio_assert(amount.symbol == _SYM, "Wrong asset. Only CORE token can be used for upgrade");
    	
	   	//Check for enough upgrade amount for quote asset
    	eosio_assert(amount == host->to_pay, "Amount is not equal amount for upgrade");
    	eosio_assert(host != hosts.end(), "Host is not founded");
    	eosio_assert(host->payed == 0, "Already payed");

    	std::vector<eosio::name> childs = host->childrens;
	
    	hosts.modify(host, _self, [&](auto &h){
			h.payed = true;
			h.to_pay = asset(0, _SYM);
		});    	

        if (childs.begin() == childs.end())
    	{
    		shares().create_bancor_market(username, host->total_shares, host->quote_amount);
		};

	  
    };

};