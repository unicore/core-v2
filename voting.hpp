
namespace eosio {

	//@abi table votes
	struct votes{
		uint64_t goal_id;
		eosio::name host;
		uint64_t rotation_num;	
		uint64_t power;

		uint64_t primary_key()const { return goal_id; }
		eosio::name by_host() const {return host;}

	    EOSLIB_SERIALIZE( votes, (goal_id)(host)(rotation_num)(power))
    };

    typedef eosio::multi_index< "votes"_n, votes,
    indexed_by<"host"_n, const_mem_fun<votes, eosio::name, &votes::by_host>>> votes_index;

//Indexed by host;

	//@abi action
	struct vote	{
		eosio::name voter;
		eosio::name host;
		uint64_t goal_id;
	};
}