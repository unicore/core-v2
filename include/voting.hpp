
	struct [[eosio::table, eosio::contract("unicore")]] votes{
		uint64_t id;
		uint64_t goal_id;
		eosio::name host;
		int64_t power;

		uint64_t primary_key()const { return id; }
		uint64_t byhost() const {return host.value;}
		uint128_t idwithhost() const { return eosio::combine_ids(host.value, goal_id); }

	  EOSLIB_SERIALIZE( votes, (id)(goal_id)(host)(power))
    
    };


    typedef eosio::multi_index< "votes"_n, votes,
    eosio::indexed_by<"host"_n, eosio::const_mem_fun<votes, uint64_t, &votes::byhost>>,
	  eosio::indexed_by<"idwithhost"_n, eosio::const_mem_fun<votes, uint128_t, &votes::idwithhost>>
    > votes_index;
