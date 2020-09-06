namespace eosio {

    struct [[eosio::table, eosio::contract("unicore")]] partners {
        eosio::name username;
        eosio::name referer;
        uint64_t id;
        uint64_t cashback;
        std::string meta;
        
        uint64_t primary_key() const{return username.value;}
        uint64_t byreferer() const{return referer.value;}
        uint64_t byid() const {return id;}

        EOSLIB_SERIALIZE(partners, (username)(referer)(id)(cashback)(meta))
    };

    typedef eosio::multi_index<"partners"_n, partners,
    eosio::indexed_by<"byreferer"_n, eosio::const_mem_fun<partners, uint64_t, &partners::byreferer>>,
    eosio::indexed_by<"byid"_n, eosio::const_mem_fun<partners, uint64_t, &partners::byid>>
    > partners_index;



    struct [[eosio::table, eosio::contract("unicore")]] refbalances{
        uint64_t id;
        eosio::name host;
        eosio::time_point_sec timepoint_sec;
        eosio::asset refs_amount;
        eosio::asset win_amount;
        eosio::asset amount;
        eosio::name from;
        uint64_t cashback;
        uint8_t level;
        uint64_t lpercent;
        uint128_t segments;
        uint64_t primary_key() const {return id;}

        EOSLIB_SERIALIZE(refbalances, (id)(host)(timepoint_sec)(refs_amount)(win_amount)(amount)(from)(cashback)(level)(lpercent)(segments))
    };

    typedef eosio::multi_index<"refbalances"_n, refbalances> refbalances_index;


    struct [[eosio::table, eosio::contract("unicore")]] rstat{
        eosio::name host;
        eosio::asset withdrawed;
        double sediment;

        uint64_t primary_key() const {return host.value;}

        EOSLIB_SERIALIZE(rstat, (host)(withdrawed)(sediment))
    };

    typedef eosio::multi_index<"rstat"_n, rstat> rstat_index;



}