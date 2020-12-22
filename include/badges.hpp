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

    eosio::string caption;
    eosio::string description;
    eosio::string iurl;
    eosio::string pic;
		eosio::string comment;
		uint64_t power;
    eosio::time_point_sec recieved_at;

		uint64_t primary_key() const {return id;}
		uint64_t host_key() const {return host.value;}
		
		EOSLIB_SERIALIZE(struct usbadges, (id)(host)(badge_id)(caption)(description)(iurl)(pic)(comment)(power)(recieved_at))
	};

	typedef eosio::multi_index<"usbadges"_n, usbadges> usbadge_index;
