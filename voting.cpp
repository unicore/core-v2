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
		// auto users_with_id = reports.template get_index<N(user_with_task)>();

		// auto idx = votes.begin();

		auto idx = votes.template get_index<N(host)>();
		auto i_bv = idx.lower_bound(host);

		uint64_t count = 0;

		while (i_bv != idx.end() && (i_bv->host == host)){
			count++;
			i_bv++;
		}
		print("count: ", count);
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
		auto itr = market.find(S(4, POWER));
		auto liquid_shares = acc->total_shares - itr->base.balance.amount;

		// print("LIQUIDSHARES ", liquid_shares);

		
		eosio_assert(goal != goals.end(), "Goal is not founded");

		auto pow = power.find(goal->host);
		eosio_assert(pow != power.end(), "You dont have shares for voting process");
		eosio_assert(pow -> power != 0, "You cant vote with zero power");
		
		auto voters = goal -> voters;
		

		// auto vote = votes.find(goal->id);
		auto goal_id_with_host_idx = votes.template get_index<N(id_with_host)>();
		auto votes_ids = combine_ids2(host, goal->id);
		
		auto vote = goal_id_with_host_idx.find(votes_ids);

		// eosio_assert(vote!= goal_id_with_host_idx.end(), "this is end");

		if (vote == goal_id_with_host_idx.end()){
			//ADD VOTE
			eosio_assert(vote_count < _TOTAL_VOTES, "Votes limit is exceeded");

      eosio_assert(goal->completed == false, "You cant vote for completed goal");
      
      if (op.up == false){
      	eosio_assert(acc->voting_only_up == false, "Downvoting is prohibited");
      }

      voters.push_back(voter);
      
      goals.modify(goal, voter, [&](auto &g){
       	op.up == true ? g.total_votes = goal->total_votes + pow -> power : g.total_votes =  goal->total_votes - pow->power;
       	
       	g.voters = voters;
       	int64_t votes_percent = g.total_votes * 100 * PERCENT_PRECISION / liquid_shares;
       	g.validated = votes_percent >= acc->consensus_percent && g.total_votes > 0;
		
      });

      votes.emplace(voter, [&](auto &v){
      	v.id = votes.available_primary_key();
      	v.goal_id = goal->id;
      	v.host = goal->host;
      	v.power = op.up == true ? pow -> power : -pow->power;
      });
	        
		} else {
			auto voters = goal -> voters;
			auto itr = std::find(voters.begin(), voters.end(), voter);
			voters.erase(itr);
			goals.modify(goal, voter, [&](auto &g){
				g.voters = voters;
				vote->power < 0 ? g.total_votes = goal->total_votes + abs(vote->power) : g.total_votes = goal->total_votes - vote->power;
				int64_t votes_percent = g.total_votes * PERCENT_PRECISION / liquid_shares;
				
				g.validated = goal->completed ? true : votes_percent >= acc->consensus_percent / 100 && g.total_votes > 0;
				
			});

			goal_id_with_host_idx.erase(vote);
		}
		// clear_old_votes_action(voter, host);
		
		
	}

		

};
}