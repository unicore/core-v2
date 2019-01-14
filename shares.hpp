namespace eosio {
	
	//@abi table power
	struct power{
		eosio::name host;
		uint64_t power;
		uint64_t staked;
		uint64_t delegated;

		eosio::name primary_key() const {return host;}

		EOSLIB_SERIALIZE(struct power, (host)(power)(staked)(delegated))
	};

	typedef eosio::multi_index<"power"_n, power> power_index;


	//@abi table delegations
	struct delegations{
		eosio::name reciever;
		uint64_t shares;

		eosio::name primary_key() const {return reciever;}

		EOSLIB_SERIALIZE(delegations, (reciever)(shares))
	};

	typedef eosio::multi_index<"delegations"_n, delegations> delegation_index;


    //@abi table vesting i64 
    struct vesting{
    	uint64_t id;
    	eosio::name owner;
    	eosio::time_point_sec startat;
    	uint64_t duration;
    	eosio::asset amount;
    	eosio::asset available;
    	eosio::asset withdrawed;

    	uint64_t primary_key() const {return id;}

    	EOSLIB_SERIALIZE(vesting, (id)(owner)(startat)(duration)(amount)(available)(withdrawed))
    };

    typedef eosio::multi_index<"vesting"_n, vesting> vesting_index;


    //@abi action refreshsh
    struct refreshsh {
    	eosio::name owner;
    	uint64_t id;

    	EOSLIB_SERIALIZE(refreshsh, (owner)(id))

    };

    //@abi action withdrawsh
    struct withdrawsh {
    	eosio::name owner;
    	uint64_t id;
     	EOSLIB_SERIALIZE(withdrawsh, (owner)(id))
    };

	// @abi action
	struct sellshares {
		eosio::name username;
		eosio::name host;
		uint64_t shares;
		EOSLIB_SERIALIZE(sellshares, (username)(host)(shares))
	};

	// @abi action
	struct delshares{
		eosio::name from;
		eosio::name reciever;
		eosio::name host;
		uint64_t shares;

		EOSLIB_SERIALIZE(delshares, (from)(reciever)(host)(shares))
	};

	// @abi action
	struct undelshares{
		eosio::name from;
		eosio::name reciever;
		eosio::name host;
		uint64_t shares;

		EOSLIB_SERIALIZE(undelshares, (from)(reciever)(host)(shares))
	};

}