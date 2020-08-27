

	[[eosio::action]] void unicore::setbadge(uint64_t id, eosio::name host, eosio::string caption, eosio::string description, eosio::string iurl, uint64_t total, uint64_t power){
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
			b.total = total;
			b.remain = total;
			b.power = power;
		});	

		} 
		else {
			eosio::check(badge->total == badge->remain, "Cannot edit this badge in reason that badge type is already gifted");

			badges.modify(badge, host, [&](auto &b){
				b.caption = caption;
				b.description = description;
				b.iurl = iurl;
				b.total = total;
				b.remain = total;
				b.power = power;
			});

		}

	};

	// [[eosio::action]] void unicore::giftbadge(eosio::name host, eosio::name to, uint64_t badge_id, eosio::string comment){

	// }

	/**
	 * @brief      Метод передачи значка награждаемому
	 *
	 * @param[in]  host      The host
	 * @param[in]  to        { parameter_description }
	 * @param[in]  badge_id  The badge identifier
	 * @param[in]  comment   The comment
	 * @param[in]  op    The operation
	 */
	void unicore::giftbadge_action(eosio::name host, eosio::name to, uint64_t badge_id, eosio::string comment){
		account_index accounts(_me, host.value);
		auto acc = accounts.find(host.value);

		eosio::check(acc != accounts.end(), "Host is not found");
		badge_index badges(_me, host.value);
		usbadge_index user_badges(_me, to.value);


		partners_index users(_partners, _partners.value);

		auto user = users.find(to.value);
		eosio::check(user != users.end(), "User is not found");
		
		//TODO check annd change badge count

		auto host_badge = badges.find(badge_id);
		eosio::check(host_badge->remain != 0, "Not enough badges for gift");

		badges.modify(host_badge, _me, [&](auto &b){
			b.remain = b.remain - 1;
		});


		user_badges.emplace(_me, [&](auto &ub){
			ub.id = user_badges.available_primary_key();
			ub.badge_id = badge_id;
			ub.caption = host_badge -> caption;
			ub.description = host_badge -> description;
			ub.iurl = host_badge -> iurl;
			ub.comment = comment;
			ub.recieved_at = eosio::time_point_sec(eosio::current_time_point().sec_since_epoch());
			ub.host = host;
			ub.power = host_badge -> power;
		});


		unicore::give_shares_with_badge_action(host, to, host_badge->power);

	};

	/**
	 * @brief      Метод возврата значка
	 * Может быть использован хостом для возврата выданного значка. 
	 *
	 * @param[in]  op    The operation
	 */
	[[eosio::action]] void unicore::backbadge(eosio::name host, eosio::name from, uint64_t badge_id, eosio::string comment){

		require_auth(host);
		account_index accounts(_me, host.value);
		auto acc = accounts.find(host.value);
		eosio::check(acc != accounts.end(), "Host is not found");
		badge_index badges(_me, host.value);
		usbadge_index user_badges(_me, from.value);

		auto usbadge = user_badges.find(badge_id);
		eosio::check(usbadge != user_badges.end(), "Badge is not found");
		auto badge = badges.find(usbadge->badge_id);
		
		badges.modify(badge, host, [&](auto &b){
			b.remain = badge -> remain + 1;
		});
		
		unicore::back_shares_with_badge_action(host, from, badge->power);
		
		user_badges.erase(usbadge);

	}

