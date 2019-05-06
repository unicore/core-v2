namespace eosio{

struct voting
{

	void clear_old_votes_action (account_name voter, account_name host){
		votes_index votes(_self, voter);
		goals_index goals (_self, host);

		auto idx = votes.begin();
		std::vector<uint64_t> list_for_delete;

		while (idx !=votes.end()){
			auto goal = goals.find(idx->goal_id);
			if (goal->completed){
				list_for_delete.push_back(idx->goal_id);
			};
			idx++;
		}
		for (auto i : list_for_delete){
			auto vote_for_delete = votes.find(i);
			votes.erase(vote_for_delete);
		}
	}

	uint64_t count_votes(account_name voter, account_name host){
		votes_index votes(_self, voter);
		goals_index goals (_self, _self);
		auto idx = votes.begin();
		uint64_t count = 1;

		while (idx !=votes.end()){
			count++;
			idx++;
		}
		return count;
	}



	void vote_action (const vote &op) {
		require_auth(op.voter);
		auto goal_id = op.goal_id;
		auto host = op.host;
		auto voter = op.voter;
		
		user_index users(_self,_self);
        auto user = users.find(op.voter);
        eosio_assert(user != users.end(), "User is not registered");

		uint64_t vote_count = count_votes(voter, host);

		
		goals_index goals(_self, host);
		power_index power(_self, voter);
		votes_index votes(_self, voter);

		auto goal = goals.find(goal_id);
		account_index accounts (_self, _self);
		auto acc = accounts.find(goal->host);

		powermarket market(_self, host);
		auto itr = market.find(S(4,BANCORE));
		auto liquid_shares = acc->total_shares - itr->base.balance.amount;

		print("LIQUIDSHARES ", liquid_shares);

		
		eosio_assert(goal != goals.end(), "Goal is not founded");

		auto pow = power.find(goal->host);
		auto vote = votes.find(goal->id);
		eosio_assert(pow != power.end(), "You dont have shares for voting process");
		eosio_assert(pow -> power != 0, "You cant vote with zero power");
		auto voters = goal -> voters;

		if (vote == votes.end()){
			//ADD VOTE
			eosio_assert(vote_count <= _TOTAL_VOTES, "Votes limit is exceeded");

	        eosio_assert(goal->completed == false, "You cant vote for completed goal");
	        voters.push_back(voter);

	        goals.modify(goal, voter, [&](auto &g){
	         	g.total_votes += pow -> power;
	         	g.voters = voters;
	         	uint64_t votes_percent = g.total_votes * 100 * PERCENT_PRECISION / liquid_shares;
	         	print("VOTESPERCENT", votes_percent);
				g.validated = votes_percent >= acc->consensus_percent;
	        });

	        votes.emplace(voter, [&](auto &v){
	        	v.goal_id = goal->id;
	        	v.host = goal->host;
	        	v.power = pow -> power;
	        });

		} else {
			auto voters = goal -> voters;
			auto itr = std::find(voters.begin(), voters.end(), voter);
			voters.erase(itr);

			goals.modify(goal, voter, [&](auto &g){
				g.voters = voters;
				g.total_votes -= vote -> power; 
				uint64_t votes_percent = g.total_votes * PERCENT_PRECISION / liquid_shares;
				print("VOTESPERCENT", votes_percent);
				g.validated = goal->completed ? true : votes_percent >= acc->consensus_percent / 100;
				
			});

			votes.erase(vote);
		}
		// clear_old_votes_action(voter, host);
		
		
	}

		

};
}