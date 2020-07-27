namespace eosio {

struct  [[eosio::table, eosio::contract("unicore")]] cmscontent {
    uint64_t id;
    uint8_t lang_id;
    eosio::string name;
    eosio::string content;

    uint64_t primary_key() const {return id;}

    EOSLIB_SERIALIZE(struct cmscontent, (id)(lang_id)(name)(content))
  };

  typedef eosio::multi_index<"cmscontent"_n, cmscontent> cmscontent_index;

  //   typedef eosio::multi_index<"dataorders"_n, dataorders,
  //     eosio::indexed_by<"buyerandid"_n, eosio::const_mem_fun<dataorders, uint128_t, 
  //                             &dataorders::buyerandid>>
  // > dataorders_index;



  struct  [[eosio::table, eosio::contract("unicore")]] cmsconfig {
    uint64_t id;
    eosio::string config;

    uint64_t primary_key() const {return id;}

    EOSLIB_SERIALIZE(struct cmsconfig, (id)(config))
  };

  typedef eosio::multi_index<"cmsconfig"_n, cmsconfig> cmsconfig_index;
}