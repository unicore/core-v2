namespace eosio {

  struct  [[eosio::table]] cmscontent {
    uint64_t id;
    uint8_t lang_id;
    eosio::string name;
    eosio::string content;

    uint64_t primary_key() const {return id;}

    EOSLIB_SERIALIZE(struct cmscontent, (id)(lang_id)(name)(content))
  };

  typedef eosio::multi_index<"cmscontent"_n, cmscontent> cmscontent_index;

  //   typedef eosio::multi_index<"dataorders"_n, dataorders,
  //     indexed_by<"buyerandid"_n, const_mem_fun<dataorders, uint128_t, 
  //                             &dataorders::buyerandid>>
  // > dataorders_index;



  struct  [[eosio::table]] cmsconfig {
    uint64_t id;
    eosio::string config;

    uint64_t primary_key() const {return id;}

    EOSLIB_SERIALIZE(struct cmsconfig, (id)(config))
  };

  typedef eosio::multi_index<"cmsconfig"_n, cmsconfig> cmsconfig_index;





  struct [[eosio::action]] setcontent
  {
    eosio::name username;
    uint64_t id;
    uint64_t lang_id;
    eosio::string content;

    EOSLIB_SERIALIZE(setcontent, (username)(id)(lang_id)(content))
  };

  
  struct [[eosio::action]] rmcontent
  {
    eosio::name username;
    uint64_t id;

    EOSLIB_SERIALIZE(rmcontent, (username)(id))
  };




  struct [[eosio::action]] setcmsconfig
  {
    eosio::name username;
    eosio::string config;
    
    EOSLIB_SERIALIZE(setcmsconfig, (username)(config))
  };

}

