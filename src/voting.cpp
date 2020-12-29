namespace eosio{


	void clear_old_votes_action (eosio::name voter, eosio::name host){
		votes_index votes(_me, voter.value);
		goals_index goals (_me, host.value);

		auto idx = votes.begin();
		std::vector<uint64_t> list_for_delete;

		while (idx !=votes.end()){
			auto goal = goals.find(idx->goal_id);
			if (goal->filled){
				list_for_delete.push_back(idx->goal_id);
			};
			idx++;
		}
		for (auto i : list_for_delete){
			auto vote_for_delete = votes.find(i);
			votes.erase(vote_for_delete);
		}
	}

	uint64_t count_votes(eosio::name voter, eosio::name host){
		votes_index votes(_me, voter.value);
		goals_index goals (_me, host.value);
		// auto users_with_id = reports.template get_index<"userwithtask"_n>();

		// auto idx = votes.begin();

		auto idx = votes.template get_index<"host"_n>();
		auto i_bv = idx.lower_bound(host.value);

		uint64_t count = 0;

		while (i_bv != idx.end() && (i_bv->host == host)){
			count++;
			i_bv++;
		}
		print("count: ", count);
		return count;
	}



	[[eosio::action]] void unicore::vote(eosio::name voter, eosio::name host, uint64_t goal_id, bool up){
		require_auth(voter);
		
		partners_index users(_partners,_partners.value);
    auto user = users.find(voter.value);
    eosio::check(user != users.end(), "User is not registered");

		uint64_t vote_count = count_votes(voter, host);
		
		goals_index goals(_me, host.value);
		power_index power(_me, voter.value);
		votes_index votes(_me, voter.value);

		auto goal = goals.find(goal_id);
		account_index accounts (_me, (goal->host).value);
		auto acc = accounts.find((goal->host).value);

		powermarket market(_me, host.value);
		auto itr = market.find(0);
		auto liquid_shares = acc->total_shares - itr->base.balance.amount;

		// print("LIQUIDSHARES ", liquid_shares);

		eosio::check(goal != goals.end(), "Goal is not founded");

		auto pow = power.find((goal->host).value);
		eosio::check(pow != power.end(), "You dont have shares for voting process");
		eosio::check(pow -> power != 0, "You cant vote with zero power");
		
		auto voters = goal -> voters;
		

		// auto vote = votes.find(goal->id);
		auto goal_idwithhost_idx = votes.template get_index<"idwithhost"_n>();
		auto votes_ids = eosio::combine_ids(host.value, goal->id);
		
		auto vote = goal_idwithhost_idx.find(votes_ids);

		// eosio::check(vote!= goal_idwithhost_idx.end(), "this is end");

		if (vote == goal_idwithhost_idx.end()){
			//ADD VOTE
			eosio::check(vote_count < _TOTAL_VOTES, "Votes limit is exceeded");

      // eosio::check(goal->filled == false, "You cant vote for filled goal");
      
      if (up == false){
      	eosio::check(acc->voting_only_up == false, "Downvoting is prohibited");
      }

      voters.push_back(voter);
      
      goals.modify(goal, voter, [&](auto &g){
       	up == true ? g.total_votes = goal->total_votes + pow -> power : g.total_votes =  goal->total_votes - pow->power;
       	
       	g.voters = voters;
       	int64_t votes_percent = g.total_votes * 100 * ONE_PERCENT / liquid_shares;
       	g.validated = votes_percent >= acc->consensus_percent && g.total_votes > 0;
		
      });

      votes.emplace(voter, [&](auto &v){
      	v.id = votes.available_primary_key();
      	v.goal_id = goal->id;
      	v.host = goal->host;
      	v.power = up == true ? pow -> power : -pow->power;
      });
	        
		} else {
			
			auto voters = goal -> voters;
			auto itr = std::find(voters.begin(), voters.end(), voter);
			voters.erase(itr);
			goals.modify(goal, voter, [&](auto &g){
				g.voters = voters;
				vote->power < 0 ? g.total_votes = goal->total_votes + abs(vote->power) : g.total_votes = goal->total_votes - vote->power;
				int64_t votes_percent = g.total_votes * ONE_PERCENT / liquid_shares;
				
				g.validated = goal->filled ? true : votes_percent >= acc->consensus_percent / 100 && g.total_votes > 0;
				
			});

			goal_idwithhost_idx.erase(vote);
		}
		// clear_old_votes_action(voter, host);
		
		
	}

		

};