
    struct  [[eosio::table, eosio::contract("unicore")]] conditions {
        uint64_t key;
        eosio::string key_string;
        uint64_t value;

        uint64_t primary_key() const{return key;}
        EOSLIB_SERIALIZE(conditions, (key)(key_string)(value))
    };

    typedef eosio::multi_index<"conditions"_n, conditions> conditions_index;

