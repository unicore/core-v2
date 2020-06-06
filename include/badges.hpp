namespace eosio {
	


	struct  [[eosio::table]] badges{
		uint64_t id;
		eosio::string caption;
		eosio::string description;
		eosio::string iurl;
		uint64_t total;
		uint64_t remain;
		uint64_t power;

		uint64_t primary_key() const {return id;}

		EOSLIB_SERIALIZE(struct badges, (id)(caption)(description)(iurl)(total)(remain)(power))
	};

	typedef eosio::multi_index<"badges"_n, badges> badge_index;


	struct  [[eosio::table]] usbadges{
		uint64_t id;
		eosio::name host;
		uint64_t badge_id;

        eosio::string caption;
        eosio::string description;
        eosio::string iurl;
		eosio::string comment;
		uint64_t power;
        eosio::time_point_sec recieved_at;

		uint64_t primary_key() const {return id;}
		uint64_t host_key() const {return host.value;}
		
		EOSLIB_SERIALIZE(struct usbadges, (id)(host)(badge_id)(caption)(description)(iurl)(comment)(power)(recieved_at))
	};

	typedef eosio::multi_index<"usbadges"_n, usbadges> usbadge_index;


    struct  [[eosio::table]] setbadge {
    	uint64_t id;
    	eosio::name host;
    	eosio::string caption;
    	eosio::string description;
    	eosio::string iurl;
    	uint64_t total;
    	uint64_t power;

    	EOSLIB_SERIALIZE(setbadge, (id)(host)(caption)(description)(iurl)(total)(power))

    };


    // //@abi action delbadge
    // struct delbadge {
    // 	eosio::name host;
    // 	uint64_t badge_id;
    	
    // 	EOSLIB_SERIALIZE(delbadge, (host)(badge_id))

    // };

    struct [[eosio::action]] giftbadge {
    	eosio::name host;
    	eosio::name to;
    	uint64_t badge_id;
    	eosio::string comment;

    	EOSLIB_SERIALIZE(giftbadge, (host)(to)(badge_id)(comment))
    };

    struct [[eosio::action]] backbadge {
    	eosio::name host;
    	eosio::name from;
    	uint64_t badge_id;
    	eosio::string comment;

    	EOSLIB_SERIALIZE(backbadge, (host)(from)(badge_id)(comment))
    };


}