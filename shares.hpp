namespace eosio {
	
	//@abi table power
	struct power{
		account_name host;
		eosio::asset power;

		uint64_t primary_key() const {return host;}
		EOSLIB_SERIALIZE(struct power, (host)(power))
	};

	typedef eosio::multi_index<N(power), power> power_index;


    //@abi table vesting i64 
    struct vesting{
    	uint64_t id;
    	account_name owner;
    	eosio::time_point_sec startat;
    	uint64_t duration;
    	eosio::asset amount;
    	eosio::asset available;
    	eosio::asset withdrawed;

    	uint64_t primary_key() const {return id;}

    	EOSLIB_SERIALIZE(vesting, (id)(owner)(startat)(duration)(amount)(available)(withdrawed))
    };

    typedef eosio::multi_index<N(vesting), vesting> vesting_index;


    //@abi action refreshsh
    struct refreshsh {
    	account_name owner;
    	uint64_t id;

    	EOSLIB_SERIALIZE(refreshsh, (owner)(id))

    };

    //@abi action withdrawsh
    struct withdrawsh {
    	account_name owner;
    	uint64_t id;
     	EOSLIB_SERIALIZE(withdrawsh, (owner)(id))
    };

	// @abi action
	struct sellshares {
		account_name username;
		account_name host;
		uint64_t shares;
		EOSLIB_SERIALIZE(sellshares, (username)(host)(shares))
	};

	// @abi action
	struct bancreate{
		account_name host;
		EOSLIB_SERIALIZE(bancreate, (host))

	};

}