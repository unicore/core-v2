
using namespace eosio;

/**
 * @brief      Модуль Знаков Отличия
 */
struct badge_struct {

	/**
	 * @brief      Метод создания значка
	 *
	 *
	 * @param[in]  op    The operation
	 */
	void setbadge_action(const setbadge &op){
		require_auth(op.host);
		
		account_index accounts(_self, op.host.value);
		auto acc = accounts.find(op.host.value);
		check(acc != accounts.end(), "Host is not found");
		
		
		badge_index badges(_self, op.host.value);

		auto badge = badges.find(op.id);
		if (badge == badges.end()){

		badges.emplace(op.host, [&](auto &b){
			b.id = op.id;
			b.caption = op.caption;
			b.description = op.description;
			b.iurl = op.iurl;
			b.total = op.total;
			b.remain = op.total;
			b.power = op.power;
		});	

		} 
		else {
			check(badge->total == badge->remain, "Cannot edit this badge in reason that badge type is already gifted");

			badges.modify(badge, op.host, [&](auto &b){
				b.caption = op.caption;
				b.description = op.description;
				b.iurl = op.iurl;
				b.total = op.total;
				b.remain = op.total;
				b.power = op.power;
			});

		}

	};

	/**
	 * @brief      Метод передачи значка награждаемому
	 *
	 * @param[in]  op    The operation
	 */
	void giftbadge_action(const giftbadge &op){
		account_index accounts(_self, op.host.value);
		auto acc = accounts.find(op.host.value);

		check(acc != accounts.end(), "Host is not found");
		badge_index badges(_self, op.host.value);
		usbadge_index user_badges(_self, op.to.value);


		user_index users(_self, _self.value);

		auto user = users.find(op.to.value);
		check(user != users.end(), "User is not found");
		
		//TODO check annd change badge count

		auto host_badge = badges.find(op.badge_id);
		check(host_badge->remain != 0, "Not enough badges for gift");

		badges.modify(host_badge, _self, [&](auto &b){
			b.remain = b.remain - 1;
		});


		user_badges.emplace(_self, [&](auto &ub){
			ub.id = user_badges.available_primary_key();
			ub.badge_id = op.badge_id;
			ub.caption = host_badge -> caption;
			ub.description = host_badge -> description;
			ub.iurl = host_badge -> iurl;
			ub.comment = op.comment;
			ub.recieved_at = eosio::time_point_sec(eosio::current_time_point().sec_since_epoch());
			ub.host = op.host;
			ub.power = host_badge -> power;
		});


		shares().give_shares_with_badge_action(op.host, op.to, host_badge->power);

	};

	/**
	 * @brief      Метод возврата значка
	 * Может быть использован хостом для возврата выданного значка. 
	 *
	 * @param[in]  op    The operation
	 */
	void backbadge_action(const backbadge &op){

		require_auth(op.host);
		account_index accounts(_self, op.host.value);
		auto acc = accounts.find(op.host.value);
		check(acc != accounts.end(), "Host is not found");
		badge_index badges(_self, op.host.value);
		usbadge_index user_badges(_self, op.from.value);

		auto usbadge = user_badges.find(op.badge_id);
		check(usbadge != user_badges.end(), "Badge is not found");
		auto badge = badges.find(usbadge->badge_id);
		
		badges.modify(badge, op.host, [&](auto &b){
			b.remain = badge -> remain + 1;
		});
		
		shares().back_shares_with_badge_action(op.host, op.from, badge->power);
		user_badges.erase(usbadge);

	}

};
