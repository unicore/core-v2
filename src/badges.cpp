

	[[eosio::action]] void unicore::setbadge(uint64_t id, eosio::name host, eosio::string caption, eosio::string description, eosio::string iurl, eosio::string pic, uint64_t total, uint64_t power){
		require_auth(host);
		
		account_index accounts(_me, host.value);
		auto acc = accounts.find(host.value);
		eosio::check(acc != accounts.end(), "Host is not found");
		
		
		badge_index badges(_me, host.value);

		auto badge = badges.find(id);
		if (badge == badges.end()){

		badges.emplace(host, [&](auto &b){
			b.id = id;
			b.caption = caption;
			b.description = description;
			b.iurl = iurl;
			b.pic = pic;
			b.total = total;
			b.remain = total;
			b.power = power;
		});	

		} 
		else {
			eosio::check(badge->total == badge->remain, "Cannot edit this badge in reason that badge type is already gifted");

			badges.modify(badge, host, [&](auto &b){
				b.caption = caption;
				b.pic = pic;
				b.description = description;
				b.iurl = iurl;
				b.total = total;
				b.remain = total;
				b.power = power;
			});

		}

	};

	[[eosio::action]] void unicore::giftbadge(eosio::name host, eosio::name to, uint64_t badge_id, eosio::string comment, bool netted = false, uint64_t goal_id = 0, uint64_t task_id = 0){
		
		require_auth(host);
		
		unicore::giftbadge_action(host, to, badge_id, comment, netted, true, goal_id, task_id);
	}

	/**
	 * @brief      Метод передачи значка награждаемому
	 *
	 * @param[in]  host      The host
	 * @param[in]  to        { parameter_description }
	 * @param[in]  badge_id  The badge identifier
	 * @param[in]  comment   The comment
	 * @param[in]  op    The operation
	 */
	void unicore::giftbadge_action(eosio::name host, eosio::name to, uint64_t badge_id, eosio::string comment, bool netted = false, bool own = true, uint64_t goal_id = 0, uint64_t task_id = 0){
		
		account_index accounts(_me, host.value);
		auto acc = accounts.find(host.value);

		eosio::check(acc != accounts.end(), "Host is not found");
		badge_index badges(_me, host.value);
		usbadge_index user_badges(_me, to.value);

		
		// partners2_index users(_partners, _partners.value);

		// auto user = users.find(to.value);
		// eosio::check(user != users.end(), "User is not found");
		
		//TODO check annd change badge count

		auto host_badge = badges.find(badge_id);
		eosio::check(host_badge->remain != 0, "Not enough badges for gift");

		badges.modify(host_badge, _me, [&](auto &b){
			b.remain = b.remain - 1;
		});

		auto hostandbadge_idx = user_badges.template get_index<"hostandbadge"_n>();
		auto badge_ids = combine_ids(host.value, badge_id);
		
		auto user_badge = hostandbadge_idx.find(badge_ids);

		if (host_badge -> power > 0){
			unicore::give_shares_with_badge_action(host, to, host_badge->power);
		}
		
		
		if (netted == true) {
			eosio::check(goal_id != 0 && task_id != 0, "Netted badge should have a goal and task ids");
			
			//modify goal and task to increse badge_count
			goals_index goals(_me, host.value);
			auto goal = goals.find(goal_id);
			goals.modify(goal, _me, [&](auto &g){
				g.gifted_badges += 1;
				g.gifted_power += host_badge -> power;
			});

			if (goal -> parent_id != 0) {
				auto parent_goal = goals.find(goal->parent_id);
				goals.modify(parent_goal, _me, [&](auto &pg){
					pg.gifted_badges += 1;
					pg.gifted_power += host_badge -> power;
				});
			}

			if (own == true){
				tasks_index tasks(_me, host.value);
				auto task = tasks.find(task_id);
				tasks.modify(task, _me, [&](auto &t){
					t.gifted_badges += 1;
					t.gifted_power += host_badge -> power;
				});	
			};
			
		}

		if (user_badge == hostandbadge_idx.end()){

			user_badges.emplace(_me, [&](auto &ub){
				ub.id = user_badges.available_primary_key();
				ub.badge_id = badge_id;
				ub.caption = host_badge -> caption;
				ub.description = host_badge -> description;
				ub.iurl = host_badge -> iurl;
				ub.pic = host_badge -> pic;
				ub.comment = comment;
				ub.first_recieved_at = eosio::time_point_sec(eosio::current_time_point().sec_since_epoch());
				ub.last_recieved_at = eosio::time_point_sec(eosio::current_time_point().sec_since_epoch());
				ub.host = host;
				ub.power = host_badge -> power;
				ub.netted = netted;
				ub.goal_id = goal_id;
				ub.task_id = task_id;
			});			

		} else {

			eosio::check(user_badge -> netted == netted, "Cannot add not netted badge to netted");

			hostandbadge_idx.modify(user_badge, _me, [&](auto &ub){
				ub.count += 1;
				ub.last_recieved_at = eosio::time_point_sec(eosio::current_time_point().sec_since_epoch());
			});

		}
	};

	/**
	 * @brief      Метод возврата значка
	 * Может быть использован хостом для возврата выданного значка. 
	 *
	 * @param[in]  op    The operation
	 */
	[[eosio::action]] void unicore::backbadge(eosio::name host, eosio::name from, uint64_t usbadge_id, eosio::string comment) {

		require_auth(host);
		account_index accounts(_me, host.value);
		auto acc = accounts.find(host.value);
		eosio::check(acc != accounts.end(), "Host is not found");
		badge_index badges(_me, host.value);
		usbadge_index user_badges(_me, from.value);

		auto usbadge = user_badges.find(usbadge_id);
		eosio::check(usbadge != user_badges.end(), "Badge is not found");
		auto badge = badges.find(usbadge->badge_id);
		
		badges.modify(badge, host, [&](auto &b){
			b.remain = badge -> remain + 1;
		});
		
		unicore::back_shares_with_badge_action(host, from, badge->power);
		
		if (usbadge -> netted == true){
			//modify goal and task to increse badge_count
			goals_index goals(_me, host.value);
			auto goal = goals.find(usbadge -> goal_id);
			goals.modify(goal, _me, [&](auto &g){
				g.gifted_badges -= 1;
				g.gifted_power -= usbadge -> power;
			});

			tasks_index tasks(_me, host.value);
			auto task = tasks.find(usbadge -> task_id);
			tasks.modify(task, _me, [&](auto &t){
				t.gifted_badges -= 1;
				t.gifted_power -= usbadge -> power;
			});
		}

		if (usbadge -> count > 1) {
			
			user_badges.modify(usbadge, _self, [&](auto &ub){
				ub.count -= 1;
			});

		} else {
			user_badges.erase(usbadge);	
		}
		

	}

