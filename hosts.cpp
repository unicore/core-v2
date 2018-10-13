

using namespace eosio;

struct hosts {

    void upgrade_action(const upgrade &op){
        require_auth(op.username);
        
        eosio_assert(op.purpose.length() < 1024, "Purpose is a maximum 1024 symbols. Describe the idea shortly.");
        eosio_assert(op.purpose.find("&#") , "Symbols '&#' is prohibited in purpose");
        
        fee_index fee(_self,_self);
        auto f = fee.find(0);
        eosio::asset upgrade_fee = f->createhost;
        
        account_index accounts(_self, _self);
        
        auto itr = accounts.find(op.username);
        eosio_assert(itr == accounts.end(), "Account is already upgraded to Host");

        accounts.emplace(_self, [&](auto &a){
            a.username = op.username;
            a.activated = false;
            a.purpose = op.purpose;
            a.registered_at = eosio::time_point_sec(now());
            a.payed = false;
        });

    }

    account_name get_random_account_name(){
    	eosio::string allowed = "12345abcdefghijklmnopqrstuvwxyz";


	    checksum256 result;
        auto mixedBlock = tapos_block_prefix() * tapos_block_num();

         const char *mixedChar = reinterpret_cast<const char *>(&mixedBlock);
         sha256( (char *)mixedChar, sizeof(mixedChar), &result);
         const char *p64 = reinterpret_cast<const char *>(&result);
         eosio::string str_acc = "";

         for(int i = 0; i<_REGISTRATOR_SUFFIX_LENGHT; i++) {
           auto r = (abs((int64_t)p64[i]) % (31));
           str_acc +=allowed[r];
         }

         str_acc += _REGISTRATOR_STRING_SUFFIX;
         
         return eosio::string_to_name(str_acc.c_str());

    };

    void pay_for_upgrade(account_name username, eosio::asset amount){
    	require_auth(username);
    	account_index hosts(_self, _self);
    	eosio_assert(amount.symbol == _SYM, "Wrong asset");

    	fee_index fees(_self, _self);
    	auto fee = fees.find(0);
    	eosio::asset upgrade_fee = fee->createhost;
    	eosio_assert(amount == upgrade_fee, "Amount is not equal current upgrade fee");

    	auto host = hosts.find(username);
    	eosio_assert(host != hosts.end(), "Host is not founded");
    	eosio_assert(host->payed == 0, "Already payed");

		const auto owner_auth = authority{
        	1,{},{{{N(tt.tc), N(eosio.code)}, 1}},{}};
    
		const auto active_auth = authority{
			1,{},{{{N(tt.tc), N(eosio.code)}, 1}},{}};
    	
	    const auto ram_amount = buyrambytes(8 * 1024);
	    
	    const auto cpu = asset(1500);
	    const auto net = asset(500);
    
	    account_name user = get_random_account_name();

	    //transfer fee to registrator
        action(
            permission_level{ _self, N(active) },
            N(eosio.token), N(transfer),
            std::make_tuple( _self, N(goals), amount, std::string("null")) 
        ).send();

	    // create account

	    action(
            permission_level{ N(goals), N(active) },
            N(eosio), N(newaccount),
            std::make_tuple(N(goals), user, owner_auth, active_auth)
        ).send();

	    // buy ram
	    action(
            permission_level{ N(goals), N(active) },
            N(eosio), N(buyram),
            std::make_tuple(N(goals), user, ram_amount)
        ).send();

	    // delegate and transfer cpu and net
	    action(
            permission_level{ N(goals), N(active) },
            N(eosio), N(delegatebw),
            std::make_tuple(N(goals), user, net, cpu, 1)
        ).send();

	    
    	hosts.modify(host, username, [&](auto &h){
    		h.payed = true;
    		h.box_account = user;
    	});
    };

};