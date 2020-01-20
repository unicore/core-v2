namespace eosio {

  //@abi table storage
  struct storage {
    uint64_t id;
    eosio::string name;
    eosio::string address;
    bool encrypted = false;
    eosio::string pkey;
    eosio::string meta;
    uint64_t primary_key() const {return id;}

    EOSLIB_SERIALIZE(struct storage, (id)(name)(address)(encrypted)(pkey)(meta))
  };

  typedef eosio::multi_index<N(storage), storage> storage_index;


  //@abi table ipfskeys
  struct ipfskeys{
    uint64_t id;
    bool revoked = false;
    eosio::time_point_sec revokedAt;
    eosio::string pkey;
    eosio::string meta;

    uint64_t primary_key() const {return id;}

    EOSLIB_SERIALIZE(struct ipfskeys, (id)(revoked)(revokedAt)(pkey)(meta))
  };

  typedef eosio::multi_index<N(ipfskeys), ipfskeys> ipfskeys_index;


  //@abi table userdatacnts
  struct userdatacnts
  {
    account_name username;
    uint64_t total_sales = 0;
    uint64_t total_buys = 0;
    uint64_t total_disputes = 0;
    uint64_t p_disputes = 0;
    uint64_t n_disputes = 0;

    uint64_t primary_key() const {return username;}

    EOSLIB_SERIALIZE(struct userdatacnts, (username)(total_sales)(total_buys)(total_disputes)(p_disputes)(n_disputes))

  };

  typedef eosio::multi_index<N(userdatacnts), userdatacnts> userdatacounts_index;



  //@abi table orbdata
  struct orbdata {
    uint64_t id;
    eosio::string data;
    account_name root_token_contract;
    eosio::asset amount;
    uint16_t sales_count = 0;
    uint16_t dispute_count = 0;
    uint64_t p_disputes = 0;
    uint64_t n_disputes = 0;
    bool removed = false;
    eosio::string meta;

    uint64_t primary_key() const {return id;}

    EOSLIB_SERIALIZE(struct orbdata, (id)(data)(root_token_contract)(amount)(sales_count)(dispute_count)(p_disputes)(n_disputes)(removed)(meta))
  };

  typedef eosio::multi_index<N(orbdata), orbdata> orbdata_index;

  //@abi table mydataordrs
  struct mydataordrs {
    account_name owner;
    uint64_t order_id;

    uint128_t primary_key() const { return combine_ids(owner, order_id); }

    EOSLIB_SERIALIZE(struct mydataordrs, (owner)(order_id))
  };

  typedef eosio::multi_index< N(mydataordrs), mydataordrs> mydataordrs_index;


  //@abi table dataorders
  struct dataorders {
    uint64_t id;  
    uint64_t orbdata_id;   
    eosio::time_point_sec opened_at;
    eosio::time_point_sec expired_at;
    account_name owner;
    account_name buyer;
    account_name curator;
    eosio::asset locked_amount;

    bool approved = false;
    eosio::string key;

    bool dispute = false;
    eosio::string meta;

    uint64_t primary_key() const {return id;}
    uint128_t by_buyer_and_id() const {return combine_ids(buyer, orbdata_id);}

    EOSLIB_SERIALIZE(struct dataorders, (id)(orbdata_id)(opened_at)(expired_at)(owner)(buyer)(curator)(locked_amount)(approved)
      (key)(dispute)(meta))
  };

  typedef eosio::multi_index<N(dataorders), dataorders,
      indexed_by<N(by_buyer_and_id), const_mem_fun<dataorders, uint128_t, 
                              &dataorders::by_buyer_and_id>>
  > dataorders_index;


  //@abi action selldata
  struct selldata
  {
    account_name username;
    uint64_t id;
    eosio::string data;
    account_name root_token_contract;
    eosio::asset amount;  

    EOSLIB_SERIALIZE(selldata, (username)(id)(data)(root_token_contract)(amount))
  };

  //@abi action dataapprove
  struct dataapprove
  {
    account_name username;
    account_name owner;
    account_name who;
    uint64_t order_id;
    eosio::string message; 

    EOSLIB_SERIALIZE(dataapprove, (username)(owner)(who)(order_id)(message))
  };




  //@abi action datadispute
  struct datadispute
  {
    account_name username;
    eosio::string data;
    account_name root_token_contract;
    eosio::asset amount;  

    EOSLIB_SERIALIZE(datadispute, (username)(data)(root_token_contract)(amount))
  };

  //@abi action datarelease
  struct datarelease
  {
    account_name username;
    eosio::string data;
    account_name root_token_contract;
    eosio::asset amount;  

    EOSLIB_SERIALIZE(datarelease, (username)(data)(root_token_contract)(amount))
  };






  //@abi action setstorage
  struct setstorage {
    account_name username;
    uint64_t id;
    eosio::string name;
    eosio::string address;
    
    EOSLIB_SERIALIZE(setstorage, (username)(id)(name)(address))
  };

 //@abi action removeroute
  struct removeroute {
    account_name username;
    uint64_t id;
    
    EOSLIB_SERIALIZE(removeroute, (username)(id))
  };


  //@abi action setipfskey
  struct setipfskey {
    account_name username;
    eosio::string pkey;
    eosio::string meta;
    
    EOSLIB_SERIALIZE(setipfskey, (username)(pkey)(meta))
  };


}

