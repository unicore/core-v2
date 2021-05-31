/*!
   \brief Структура партнёров и их партнёров.
*/

    struct [[eosio::table]] partners2 {
        eosio::name username;
        eosio::name referer;
        std::string nickname;
        eosio::checksum256 nickhash;
        
        uint64_t id;
        uint64_t cashback;
        
        eosio::name status;
        
        std::string meta;
        
        uint64_t primary_key() const{return username.value;}
        uint64_t byreferer() const{return referer.value;}
        uint64_t byid() const {return id;}
        uint64_t bystatus() const {return status.value;}

        eosio::checksum256 bynickhash() const { return nickhash; }
        
        EOSLIB_SERIALIZE(partners2, (username)(referer)(nickname)(nickhash)(id)(cashback)(status)(meta))


    };
      
    typedef eosio::multi_index<"partners2"_n, partners2,
      eosio::indexed_by<"byreferer"_n, eosio::const_mem_fun<partners2, uint64_t, &partners2::byreferer>>,
      eosio::indexed_by<"byid"_n, eosio::const_mem_fun<partners2, uint64_t, &partners2::byid>>,
      eosio::indexed_by<"bynickhash"_n, eosio::const_mem_fun<partners2, eosio::checksum256, &partners2::bynickhash>>,
      eosio::indexed_by<"bystatus"_n, eosio::const_mem_fun<partners2, uint64_t, &partners2::bystatus>>
    > partners2_index;



/*!
   \brief Структура полученных реферальных балансов от партнёров на партнёра.
*/

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
        uint64_t percent;
        double segments;
        uint64_t primary_key() const {return id;}

        EOSLIB_SERIALIZE(refbalances, (id)(host)(timepoint_sec)(refs_amount)(win_amount)(amount)(from)(cashback)(level)(percent)(segments))
    };

    typedef eosio::multi_index<"refbalances"_n, refbalances> refbalances_index;


/*!
   \brief Структура статистики реферальных балансов и осадок, доступный на получение по мере накопления.
*/
    struct [[eosio::table, eosio::contract("unicore")]] rstat{
        eosio::name host;
        eosio::asset withdrawed;
        double sediment;

        uint64_t primary_key() const {return host.value;}

        EOSLIB_SERIALIZE(rstat, (host)(withdrawed)(sediment))
    };

    typedef eosio::multi_index<"rstat"_n, rstat> rstat_index;

