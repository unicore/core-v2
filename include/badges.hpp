	struct  [[eosio::table, eosio::contract("unicore")]] badges{
		uint64_t id;
		eosio::string caption;
		eosio::string description;
		eosio::string iurl;
		eosio::string pic;
		uint64_t total;
		uint64_t remain;
		uint64_t power;

		uint64_t primary_key() const {return id;}

		EOSLIB_SERIALIZE(struct badges, (id)(caption)(description)(iurl)(pic)(total)(remain)(power))
	};

	typedef eosio::multi_index<"badges"_n, badges> badge_index;


	struct  [[eosio::table, eosio::contract("unicore")]] usbadges{
		uint64_t id;
		eosio::name host;
		uint64_t badge_id;
		bool netted = false;
    uint64_t goal_id;
    uint64_t task_id;
    
		uint64_t count = 1;
    eosio::string caption;
    eosio::string description;
    eosio::string iurl;
    eosio::string pic;
		eosio::string comment;
		uint64_t power;
    eosio::time_point_sec first_recieved_at;
    eosio::time_point_sec last_recieved_at;

		uint64_t primary_key() const {return id;}
		uint64_t host_key() const {return host.value;}
		uint128_t hostandbadge() const { return eosio::combine_ids(host.value, badge_id); }
		uint128_t hostandtask() const { return eosio::combine_ids(host.value, task_id); }
		uint128_t hostandgoal() const { return eosio::combine_ids(host.value, goal_id); }

		EOSLIB_SERIALIZE(struct usbadges, (id)(host)(badge_id)(netted)(goal_id)(task_id)(count)(caption)(description)(iurl)(pic)(comment)(power)(first_recieved_at)(last_recieved_at))
	};

	typedef eosio::multi_index<"usbadges"_n, usbadges,
	eosio::indexed_by<"hostandbadge"_n, eosio::const_mem_fun<usbadges, uint128_t, &usbadges::hostandbadge>>,
	eosio::indexed_by<"hostandtask"_n, eosio::const_mem_fun<usbadges, uint128_t, &usbadges::hostandtask>>,
	eosio::indexed_by<"hostandgoal"_n, eosio::const_mem_fun<usbadges, uint128_t, &usbadges::hostandgoal>>
	> usbadge_index;
