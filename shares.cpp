using namespace eosio;

struct shares {

	void make_vesting_action(account_name owner, eosio::asset amount){
	    eosio_assert(amount.is_valid(), "Amount not valid");
	    eosio_assert(amount.symbol == CORE_SYMBOL, "Not valid symbol for this vesting contract");
	    eosio_assert(is_account(owner), "Owner account does not exist");
	    
	    vesting_index vests (_self, owner);
	    
	    vests.emplace(_self, [&](auto &v){
	      v.id = vests.available_primary_key();
	      v.owner = owner;
	      v.amount = amount;
	      v.startat = eosio::time_point_sec(now());
	      v.duration = _SHARES_VESTING_DURATION;
	    });

	}

  void refresh_action(const refreshsh &op){
    require_auth(op.owner);
    vesting_index vests(_self, op.owner);
    auto v = vests.find(op.id);
    eosio_assert(v != vests.end(), "Vesting object does not exist");
    
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

  void withdraw_action(const withdrawsh &op){
    require_auth(op.owner);
    vesting_index vests(_self, op.owner);
    auto v = vests.find(op.id);
    eosio_assert(v != vests.end(), "Vesting object does not exist");
    eosio_assert((v->available).amount > 0, "Only positive amount can be withdrawed");
      
    action(
        permission_level{ _self, N(active) },
        N(eosio.token), N(transfer),
        std::make_tuple( _self, op.owner, v->available, std::string("Vesting Withdrawed")) 
    ).send();

    vests.modify(v, op.owner, [&](auto &m){
        m.withdrawed = v->withdrawed + v->available;
        m.available = eosio::asset(0, _SYM);
      });

    if (v->withdrawed == v->amount){
    	vests.erase(v);
    };
    
  };


	void buyshares_action ( account_name buyer, account_name host, eosio::asset amount ){
		eosio_assert(amount.symbol == CORE_SYMBOL, "Wrong symbol for buy shares");
		//TODO check for host
		account_index accounts(_self, _self);
		auto exist = accounts.find(host);
		eosio_assert(exist != accounts.end(), "Host is not founded");

		rammarket market(_self, host);
		auto itr = market.find(S(4, BANCORE));
		auto tmp = *itr;
		eosio::asset shares_out;

		market.modify( itr, 0, [&]( auto& es ) {
        	shares_out = es.convert( amount, S(0, CORE));
	    });

        eosio_assert( shares_out.amount > 0, "Amount is not enought for buy 1 share" );

        power_index power(_self, buyer);
        auto pexist = power.find(host);
        if (pexist == power.end()){
	        power.emplace(buyer, [&](auto &p){
	        	p.host = host;
	        	p.power = shares_out;
	        });
		} else {
			power.modify(pexist, buyer, [&](auto &p){
				p.power += shares_out;
			});
		};

	};

	void sellshares_action ( const sellshares &op ){
		auto host = op.host;
		auto username = op.username;
		uint64_t shares = op.shares;
		
		power_index power(_self, username);
		auto userpower = power.find(host);
		auto upower = (userpower->power).amount;
		eosio_assert(upower >= shares, "Not enought power for sell");

		rammarket market(_self, host);
		auto itr = market.find(S(4, BANCORE));
		auto tmp = *itr;
		eosio::asset tokens_out;
		market.modify( itr, 0, [&]( auto& es ) {
        	tokens_out = es.convert( asset(shares,S(0, CORE)), CORE_SYMBOL);
	    });
		eosio_assert( tokens_out.amount > 1, "token amount received from selling shares is too low" );
	    
	    make_vesting_action(host, tokens_out);

	    power.modify(userpower, username, [&](auto &p){
	    	p.power = asset((userpower->power).amount - shares, S(0, CORE));
	    });
	    
	};

	void create_bancor_market(const bancreate &op){
		auto host = op.host;
		rammarket market(_self, host);
		auto itr = market.find(S(4,BANCORE));
		if (itr == market.end()){
				itr = market.emplace( host, [&]( auto& m ) {
	               m.supply.amount = 100000000000000ll;
	               m.supply.symbol = S(4,BANCORE);
	               m.base.balance.amount = int64_t(1000000);
	               m.base.balance.symbol = S(0, CORE);
	               m.quote.balance.amount = 10000000000;
	               m.quote.balance.symbol = CORE_SYMBOL;
	            });

		} else 
			eosio_assert(false, "Market already created");
	};
};