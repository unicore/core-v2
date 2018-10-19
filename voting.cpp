namespace eosio{

struct voting
{

	void clear_old_votes_action (account_name voter){
		votes_index votes(_self, voter);
		goals_index goals (_self, _self);

		auto idx = votes.begin();
		std::vector<uint64_t> list_for_delete;

		while (idx !=votes.end()){
			auto goal = goals.find(idx->goal_id);
			if (goal->rotation_num != idx->rotation_num){
				list_for_delete.push_back(idx->goal_id);
			};
			idx++;
		}
		for (auto i : list_for_delete){
			auto vote_for_delete = votes.find(i);
			votes.erase(vote_for_delete);
			print("clear votes: ", idx->goal_id, " ,");
		}
		print(";");
	}

	uint64_t count_votes(account_name voter){
		votes_index votes(_self, voter);
		goals_index goals (_self, _self);
		auto idx = votes.begin();
		uint64_t count = 0;

		while (idx !=votes.end()){
			count++;
			idx++;
		}
		print("vote count: ", count);
		return count;
	}

	void vote_action (const vote &op) {
		auto goal_id = op.goal_id;
		auto voter = op.voter;
		clear_old_votes_action(voter);
		uint64_t vote_count = count_votes(voter);

		eosio_assert(vote_count <= _TOTAL_VOTES, "Votes limit is exceeded");

		account_index accounts(_self, _self);
		goals_index goals(_self, _self);
		power_index power(_self, voter);
		votes_index votes(_self, voter);

		auto goal = goals.find(goal_id);
		
		eosio_assert(goal != goals.end(), "Goal is not founded");
		eosio_assert(goal->activated == true, "You cant vote for not activated goal");
		eosio_assert(goal->completed == false, "You cant vote for completed goal");

		auto pow = power.find(goal->host);
		auto vote = votes.find(goal->id);
		eosio_assert(pow != power.end(), "You dont have shares for voting process");
		eosio_assert(pow -> power != 0, "You cant vote with zero shares");
		auto voters = goal -> voters;



		if (vote == votes.end()){
	        voters.push_back(voter);

	        goals.modify(goal, voter, [&](auto &c){
	         	c.total_votes += pow -> power;
	         	c.voters = voters;
	        });

	        votes.emplace(voter, [&](auto &v){
	        	v.goal_id = goal->id;
	        	v.host = goal->host;
	        	v.rotation_num = goal->rotation_num;
	        	v.power = pow -> power;
	        });

		} else {
			if (vote -> rotation_num == goal -> rotation_num){
				//DELETE VOTE
				auto voters = goal -> voters;
				auto itr = std::find(voters.begin(), voters.end(), voter);
				voters.erase(itr);

				goals.modify(goal, voter, [&](auto &g){
					g.voters = voters;
					g.total_votes -= vote -> power; 
				});

				votes.erase(vote);

			} else {
				//IF exist with different rotation_num - erase and emplace
				//TODO move to method for clear all old votes
				votes.erase(vote);
				votes.emplace(voter, [&](auto &v){
					v.goal_id = goal -> id;
					v.host = goal->host;
					v.rotation_num = goal -> rotation_num;
					v.power = pow -> power;
				});
	
		        goals.modify(goal, voter, [&](auto &g){
	         		g.total_votes += pow -> power;
	         		g.voters = voters;
	        	});
			}
		}
		
		
	}

		

};
}