namespace eosio {
  
  //@abi table storage
  struct storage {
    uint64_t id;
    eosio::string address;
    bool encrypted = false;
    eosio::string pkey;
    eosio::string meta;
    uint64_t primary_key() const {return id;}

    EOSLIB_SERIALIZE(struct storage, (id)(address)(encrypted)(pkey)(meta))
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


  //@abi action setstorage
  struct setstorage {
    account_name username;
    uint64_t id;
    eosio::string address;
    
    EOSLIB_SERIALIZE(setstorage, (username)(id)(address))
  };

  //@abi action setipfskey
  struct setipfskey {
    account_name username;
    eosio::string pkey;
    eosio::string meta;
    
    EOSLIB_SERIALIZE(setipfskey, (username)(pkey)(meta))
  };


}

