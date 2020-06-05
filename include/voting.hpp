
namespace eosio {
	uint128_t combine_ids2(const uint64_t &x, const uint64_t &y) {
        return (uint128_t{x} << 64) | y;
    }

	//@abi table votes
	struct votes{
		uint64_t id;
		uint64_t goal_id;
		eosio::name host;
		int64_t power;

		uint64_t primary_key()const { return id; }
		eosio::name by_host() const {return host;}
		uint128_t idwithhost() const { return combine_ids2(host.value, goal_id); }

	    EOSLIB_SERIALIZE( votes, (id)(goal_id)(host)(power))
    };


    typedef eosio::multi_index< "votes"_n, votes,
    indexed_by<"host"_n, const_mem_fun<votes, eosio::name, &votes::by_host>>,
	indexed_by<"idwithhost"_n, const_mem_fun<votes, uint128_t, &votes::idwithhost>>
    > votes_index;


	//@abi action
	struct vote	{
		eosio::name voter;
		eosio::name host;
		uint64_t goal_id;
		bool up;
	};
}