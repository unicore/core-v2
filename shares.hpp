namespace eosio {
	
	//@abi table power
	struct power{
		account_name host;
    uint64_t power;
		uint64_t staked;
		uint64_t delegated;
    uint64_t with_badges;
		account_name primary_key() const {return host;}

		EOSLIB_SERIALIZE(struct power, (host)(power)(staked)(delegated)(with_badges))
	};

	typedef eosio::multi_index<N(power), power> power_index;


  //@abi table pstats
  struct pstats{
    account_name host;
    eosio::asset total_available_in_asset;
    uint64_t pflow_last_withdrawed_pool_id;
    uint128_t pflow_available_segments;
    eosio::asset pflow_available_in_asset;
    eosio::asset pflow_withdrawed;

    eosio::asset ref_available_in_asset;
    uint64_t ref_available_segments = 0;
    eosio::asset ref_withdrawed;
    account_name primary_key() const {return host;}

    EOSLIB_SERIALIZE(struct pstats, (host)(total_available_in_asset)(pflow_last_withdrawed_pool_id)(pflow_available_segments)(pflow_available_in_asset)(pflow_withdrawed)(ref_available_in_asset)(ref_available_segments)(ref_withdrawed))
  };

  typedef eosio::multi_index<N(pstats), pstats> pstats_index;

  //@abi table plog
  struct plog{
    uint64_t id;
    account_name host;
    uint64_t pool_id;
    uint64_t cycle_num;
    uint64_t pool_num;
    uint64_t power;
    

    uint64_t primary_key() const {return id;}
    
    uint128_t hostwithpoolid() const { return combine_ids(host, pool_id); }
    
    EOSLIB_SERIALIZE(struct plog, (id)(host)(pool_id)(cycle_num)(pool_num)(power))
  };

  typedef eosio::multi_index<N(plog), plog,
    indexed_by<N(hostwithpoolid), const_mem_fun<plog, uint128_t, &plog::hostwithpoolid>>
  > plog_index;



  //@abi table dlog
  struct dlog{
    uint64_t id;
    account_name host;
    uint64_t pool_id;
    uint128_t segments;
    eosio::asset amount;
    

    uint64_t primary_key() const {return id;}
    
    EOSLIB_SERIALIZE(struct dlog, (id)(host)(pool_id)(segments)(amount))
  };

  typedef eosio::multi_index<N(dlog), dlog> dlog_index;


	//@abi table delegations
	struct delegations{
		account_name reciever;
		uint64_t shares;

		account_name primary_key() const {return reciever;}

		EOSLIB_SERIALIZE(delegations, (reciever)(shares))
	};

	typedef eosio::multi_index<N(delegations), delegations> delegation_index;


    //@abi table vesting i64 
    struct vesting{
    	uint64_t id;
      account_name host;
    	account_name owner;
    	eosio::time_point_sec startat;
    	uint64_t duration;
    	eosio::asset amount;
    	eosio::asset available;
    	eosio::asset withdrawed;

    	uint64_t primary_key() const {return id;}

    	EOSLIB_SERIALIZE(vesting, (id)(host)(owner)(startat)(duration)(amount)(available)(withdrawed))
    };

    typedef eosio::multi_index<N(vesting), vesting> vesting_index;


    //@abi action refreshsh
    struct refreshsh {
    	account_name owner;
    	uint64_t id;

    	EOSLIB_SERIALIZE(refreshsh, (owner)(id))

    };

    //@abi action withbenefit
    struct withbenefit{
      account_name username;
      account_name host;

      EOSLIB_SERIALIZE(withbenefit, (username)(host))
    };

    //@abi action refreshpu
    struct refreshpu{
      account_name username;
      account_name host;

      EOSLIB_SERIALIZE(refreshpu, (username)(host))
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
	struct delshares{
		account_name from;
		account_name reciever;
		account_name host;
		uint64_t shares;

		EOSLIB_SERIALIZE(delshares, (from)(reciever)(host)(shares))
	};

	// @abi action
	struct undelshares{
		account_name from;
		account_name reciever;
		account_name host;
		uint64_t shares;

		EOSLIB_SERIALIZE(undelshares, (from)(reciever)(host)(shares))
	};

}