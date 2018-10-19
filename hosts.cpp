

using namespace eosio;

struct hosts {

    void upgrade_action(const upgrade &op){
        require_auth(op.username);
        
        eosio_assert(op.purpose.length() < 1024, "Purpose is a maximum 1024 symbols. Describe the idea shortly.");
        //check title lenght

        eosio_assert(op.title.length() < 100, "Title is a maximum 100 symbols");
        eosio_assert(op.purpose.find("&#") , "Symbols '&#' is prohibited in purpose");

        eosio_assert(op.quote_amount.amount > 0, "Start quote amount must be greater then zero");
        eosio_assert(op.quote_amount.symbol == CORE_SYMBOL, "Quote symbol for market is only CORE");
        
        account_index accounts(_self, _self);
        
        auto itr = accounts.find(op.username);
        eosio_assert(itr == accounts.end(), "Account is already upgraded to Host");

        //check and set shares quantity
       
       	eosio_assert(op.total_shares > 100, "Total shares must be greater then 100");
        //check for exist quote and root tokens
        auto failure_if_root_not_exist = eosio::token(N(eosio.token)).get_supply(eosio::symbol_type(op.root_token.symbol).name()).amount;
        auto failure_if_quote_not_exist = eosio::token(N(eosio.token)).get_supply(eosio::symbol_type(op.quote_amount.symbol).name()).amount;

        fee_index fees(_self, _self);
        auto fee = fees.find(0);
        auto to_pay = fee->createhost + op.quote_amount;

        accounts.emplace(_self, [&](auto &a){
            a.username = op.username;
            a.hoperator = op.hoperator;
            a.activated = false;
            a.title = op.title;
            a.purpose = op.purpose;
            a.total_shares = op.total_shares;
            a.quote_amount = op.quote_amount;
            a.root_token = op.root_token;
            a.meta = op.meta;
            a.registered_at = eosio::time_point_sec(now());
            a.payed = false;
            a.to_pay = to_pay;
            a.active_host = op.username;
        });

    }

    // account_name get_random_account_name(){
    // 	eosio::string allowed = "12345abcdefghijklmnopqrstuvwxyz";


	   //  checksum256 result;
    //     auto mixedBlock = tapos_block_prefix() * tapos_block_num();

    //      const char *mixedChar = reinterpret_cast<const char *>(&mixedBlock);
    //      sha256( (char *)mixedChar, sizeof(mixedChar), &result);
    //      const char *p64 = reinterpret_cast<const char *>(&result);
    //      eosio::string str_acc = "";

    //      for(int i = 0; i<_REGISTRATOR_SUFFIX_LENGHT; i++) {
    //        auto r = (abs((int64_t)p64[i]) % (31));
    //        str_acc +=allowed[r];
    //      }

    //      str_acc += _REGISTRATOR_STRING_SUFFIX;
         
    //      return eosio::string_to_name(str_acc.c_str());

    // };


    void create_child_host_action(const cchildhost &op){
    	auto parent_host = op.parent_host;
    	auto child_host = op.child_host;
    	print("im not creating");
    	account_index hosts(_self, _self);
    	auto acc = hosts.find(parent_host);
    	eosio_assert(acc != hosts.end(), "Parent host is not exist");
    	require_auth(parent_host);

    	eosio_assert( is_account( child_host ), "Child account does not exist");
    	
    	std::vector<account_name> childs = acc->childrens;

    	//check for exist in main hosts;
    	auto is_exist = hosts.find(child_host);
    	eosio_assert(is_exist == hosts.end(), "Child host already registered like father host");
    	
    	// //Check for exist not active child
    	if (childs.begin() != childs.end()){
    		account_name last_child = childs.back() - 1;
			eosio_assert(acc->active_host == last_child, "Founded not activated child host");
    	}
    	
    	childs.push_back(child_host);
    	fee_index fees(_self, _self);
    	eosio::asset to_pay;

        auto fee = fees.find(0);
        
        if ((fee->createhost).amount == 0){
        	to_pay = asset(1, CORE_SYMBOL);

        } else {

			to_pay = fee->createhost;
        }
        
    	hosts.modify(acc, parent_host, [&](auto &h){
    		h.childrens = childs;
    		h.payed = false;
    		h.to_pay = to_pay;
    	});


    }

    void pay_for_upgrade(account_name username, eosio::asset amount){
    	account_index hosts(_self, _self);

    	auto host = hosts.find(username);
    	
   		//PAY for upgrade only in CORE!.
    	eosio_assert(amount.symbol == CORE_SYMBOL, "Wrong asset");
    	
	   	//Check for enough upgrade amount for quote asset
    	eosio_assert(amount == host->to_pay, "Amount is not equal amount for upgrade");
    	eosio_assert(host != hosts.end(), "Host is not founded");
    	eosio_assert(host->payed == 0, "Already payed");

    	std::vector<account_name> childs = host->childrens;
	
    	hosts.modify(host, _self, [&](auto &h){
			h.payed = true;
			h.to_pay = asset(0, CORE_SYMBOL);
		});    	

        if (childs.begin() != childs.end())
    	{
    		shares().create_bancor_market(username, host->total_shares, host->quote_amount);
		};

		// const auto owner_auth = authority{
  //       	1,{},{{{_self, N(eosio.code)}, 1}},{}};
    
		// const auto active_auth = authority{
		// 	1,{},{{{_self, N(eosio.code)}, 1}},{}};
    	
	 //    const auto ram_amount = buyrambytes(8 * 1024);
	    
	 //    const auto cpu = asset(1500);
	 //    const auto net = asset(500);
    
	 //    account_name user = get_random_account_name();

	 //    //TODO CHECK IT (REGISTRATOR ACCOUNT)

	 //    //transfer fee to registrator

  //       action(
  //           permission_level{ _self, N(active) },
  //           N(eosio.token), N(transfer),
  //           std::make_tuple( _self, N(goals), amount, std::string("null")) 
  //       ).send();

	 //    // create account

	 //    action(
  //           permission_level{ N(goals), N(active) },
  //           N(eosio), N(newaccount),
  //           std::make_tuple(N(goals), user, owner_auth, active_auth)
  //       ).send();

	 //    // buy ram
	 //    action(
  //           permission_level{ N(goals), N(active) },
  //           N(eosio), N(buyram),
  //           std::make_tuple(N(goals), user, ram_amount)
  //       ).send();

	 //    // delegate and transfer cpu and net
	 //    action(
  //           permission_level{ N(goals), N(active) },
  //           N(eosio), N(delegatebw),
  //           std::make_tuple(N(goals), user, net, cpu, 1)
  //       ).send();

	  
    };

};