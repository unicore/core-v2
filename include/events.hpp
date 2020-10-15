namespace eosio {

  struct [[eosio::table, eosio::contract("unicore")]] events {
    uint64_t id;
    eosio::name creator;
    eosio::name lang;
    eosio::string title;
    eosio::string permlink;
    eosio::string descriptor;
    eosio::time_point_sec start_at;
    eosio::time_point_sec finish_at;
    uint64_t category_id;
    bool is_regular = false;
    bool is_private = false;

    eosio::time_point_sec last_event_date;
    uint64_t location_id;
    eosio::string location;
    eosio::name token_contract;
    eosio::asset price;
    std::string geoposition;
    

    bool is_free = true;
    bool activated = true;
    uint64_t participants_count;
    std::vector<eosio::name> participants;
    eosio::string meta;
    eosio::string report;

    uint64_t primary_key()const { return id; }

    uint64_t bycreator() const {return creator.value; }
    uint64_t bylang() const {return lang.value; }
    
    uint64_t bycategory() const {return category_id; }
    uint64_t bystart() const {return start_at.sec_since_epoch(); }

    uint64_t byregular() const {return is_regular; }
    uint64_t bylocation() const {return location_id; }
    uint64_t byfree() const {return is_free; }
    

    EOSLIB_SERIALIZE( events, (id)(creator)(lang)(title)(permlink)(descriptor)(start_at)(finish_at)(category_id)(is_regular)(is_private)(last_event_date)(location_id)(location)(token_contract)(price)(geoposition)(is_free)(activated)(participants_count)(participants)(meta)(report))
    
  };
    

    typedef eosio::multi_index< "events"_n, events,
        eosio::indexed_by<"bycreator"_n, eosio::const_mem_fun<events, uint64_t, &events::bycreator>>,
        eosio::indexed_by<"bylang"_n, eosio::const_mem_fun<events, uint64_t, &events::bylang>>,
        eosio::indexed_by<"bycategory"_n, eosio::const_mem_fun<events, uint64_t, &events::bycategory>>,
        eosio::indexed_by<"bystart"_n, eosio::const_mem_fun<events, uint64_t, &events::bystart>>,
        eosio::indexed_by<"byregular"_n, eosio::const_mem_fun<events, uint64_t, &events::byregular>>,
        eosio::indexed_by<"bylocation"_n, eosio::const_mem_fun<events, uint64_t, &events::bylocation>>,
        eosio::indexed_by<"byfree"_n, eosio::const_mem_fun<events, uint64_t, &events::byfree>>
        
    > events_index;


    struct [[eosio::table, eosio::contract("unicore")]] locations {
      uint64_t id;
      eosio::string title;
      eosio::name coordinator;
      std::string geoposition;
      std::string meta;

      uint64_t primary_key() const {return id;}
      uint64_t bycoord() const {return coordinator.value; }
      EOSLIB_SERIALIZE(locations, (id)(title)(coordinator)(geoposition)(meta))
    };

    typedef eosio::multi_index< "locations"_n, locations,
      eosio::indexed_by<"bycoord"_n, eosio::const_mem_fun<locations, uint64_t, &locations::bycoord>>
    > locations_index;

    struct [[eosio::table, eosio::contract("unicore")]] categories {
      uint64_t id;
      eosio::name lang;
      eosio::string title;
      std::string meta;

      uint64_t primary_key() const {return id;}
      uint64_t bylang() const {return lang.value; }
      
      EOSLIB_SERIALIZE(categories, (id)(lang)(title)(meta))
    };

    typedef eosio::multi_index< "categories"_n, categories,
      eosio::indexed_by<"bylang"_n, eosio::const_mem_fun<categories, uint64_t, &categories::bylang>>
    > categories_index;

}