namespace eosio {
	
	//@abi table badges
	struct badges{
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

	typedef eosio::multi_index<N(badges), badges> badge_index;

	//@abi table usbadges
	struct usbadges{
		uint64_t id;
		account_name host;
		uint64_t badge_id;

        eosio::string caption;
        eosio::string description;
        eosio::string iurl;
		eosio::string comment;
		uint64_t power;
        eosio::time_point_sec recieved_at;

		uint64_t primary_key() const {return id;}
		uint64_t host_key() const {return host;}
		
		EOSLIB_SERIALIZE(struct usbadges, (id)(host)(badge_id)(caption)(description)(iurl)(comment)(power)(recieved_at))
	};

	typedef eosio::multi_index<N(usbadges), usbadges> usbadge_index;


    //@abi action setbadge
    struct setbadge {
    	uint64_t id;
    	account_name host;
    	eosio::string caption;
    	eosio::string description;
    	eosio::string iurl;
    	uint64_t total;
    	uint64_t power;

    	EOSLIB_SERIALIZE(setbadge, (id)(host)(caption)(description)(iurl)(total)(power))

    };


    // //@abi action delbadge
    // struct delbadge {
    // 	account_name host;
    // 	uint64_t badge_id;
    	
    // 	EOSLIB_SERIALIZE(delbadge, (host)(badge_id))

    // };

    //@abi action giftbadge
    struct giftbadge {
    	account_name host;
    	account_name to;
    	uint64_t badge_id;
    	eosio::string comment;

    	EOSLIB_SERIALIZE(giftbadge, (host)(to)(badge_id)(comment))
    };

    //@abi action backbadge
    struct backbadge {
    	account_name host;
    	account_name from;
    	uint64_t badge_id;
    	eosio::string comment;

    	EOSLIB_SERIALIZE(backbadge, (host)(from)(badge_id)(comment))
    };


}