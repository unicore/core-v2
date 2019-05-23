
namespace eosio {
	uint128_t combine_ids2(const uint64_t &x, const uint64_t &y) {
        return (uint128_t{x} << 64) | y;
    }

	//@abi table votes
	struct votes{
		uint64_t id;
		uint64_t goal_id;
		account_name host;
		int64_t power;

		uint64_t primary_key()const { return id; }
		account_name by_host() const {return host;}
		uint128_t id_with_host() const { return combine_ids2(host, goal_id); }

	    EOSLIB_SERIALIZE( votes, (id)(goal_id)(host)(power))
    };


    typedef eosio::multi_index< N(votes), votes,
    indexed_by<N(host), const_mem_fun<votes, account_name, &votes::by_host>>,
	indexed_by<N(id_with_host), const_mem_fun<votes, uint128_t, &votes::id_with_host>>
    > votes_index;


	//@abi action
	struct vote	{
		account_name voter;
		account_name host;
		uint64_t goal_id;
		bool up;
	};
}