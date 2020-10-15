/**
 * @brief      Модуль событий
 */



/**
 * @brief      Публичный метод создания события
 *
 * @param[in]  op    The operation
 */
namespace eosio {

  [[eosio::action]] void unicore::addevent(eosio::name host, eosio::name creator, eosio::name lang, eosio::string title, eosio::string permlink, eosio::string descriptor, eosio::time_point_sec start_at, eosio::time_point_sec finish_at, uint64_t category_id, bool is_regular, bool activated, uint64_t location_id, eosio::string geoposition, eosio::string location, eosio::name token_contract, eosio::asset price, eosio::string meta) {
    
    require_auth(creator);

    categories_index categories(_me, host.value);
    auto category = categories.find(category_id);
    eosio::check(category != categories.end(), "Category is not found");

    locations_index locations(_me, host.value);
    auto location_obj = locations.find(location_id);
    eosio::check(location_obj != locations.end(), "Location is not found");

    events_index events(_me, host.value);

    std::vector<eosio::name> participants;

    events.emplace(creator, [&](auto &e){
      e.id = events.available_primary_key();
      e.creator = creator;
      e.lang = lang;
      e.title = title;
      e.permlink = permlink;
      e.descriptor = descriptor;
      e.start_at = start_at;
      e.finish_at = finish_at;
      e.category_id = category_id;
      e.is_regular = is_regular;
      e.participants_count = 0;
      e.last_event_date = eosio::time_point_sec(0);
      e.location_id = location_id;
      e.location = location;
      e.price = price;
      e.geoposition = geoposition;
      e.is_free = price.amount > 0 ? false : true;
      e.activated = activated;
      e.meta = meta;
      e.participants = participants;
      
    });

  }

  [[eosio::action]] void unicore::rmevent(eosio::name host, eosio::name creator, uint64_t id) {
    require_auth(creator);
    
    events_index events(_me, host.value);

    auto event = events.find(id);

    eosio::check(event != events.end(), "Event is not found");
    eosio::check(event -> creator == creator, "Only creator can remove the event");
    
    events.erase(event);

  }

  [[eosio::action]] void unicore::joinevent(eosio::name host, eosio::name username, uint64_t id) {
    require_auth(username);
    
    events_index events(_me, host.value);

    auto event = events.find(id);

    eosio::check(event != events.end(), "Event is not found");
    
    auto participants = event -> participants;
    auto participants_count = event -> participants_count;

    auto itr = std::find(participants.begin(), participants.end(), username);
    
    if (itr != participants.end()){
      participants.erase(itr);
      participants_count--;
    } else {
      participants.push_back(username);  
      participants_count++;
    }
    
    events.modify(event, username, [&](auto &e){
      e.participants = participants;
      e.participants_count = participants_count;
    });

  }


  [[eosio::action]] void unicore::editevent(eosio::name host, eosio::name creator, uint64_t id, eosio::name lang, eosio::string title, eosio::string descriptor, eosio::time_point_sec start_at, eosio::time_point_sec finish_at, uint64_t category_id, bool is_regular, bool activated, uint64_t location_id, eosio::string geoposition, eosio::string location, eosio::name token_contract, eosio::asset price, eosio::string meta){
    
    require_auth(creator);
    
    events_index events(_me, host.value);

    auto event = events.find(id);

    eosio::check(event != events.end(), "Event is not found");
    eosio::check(event -> creator == creator, "Only creator can edit the event");

    events.modify(event, creator, [&](auto &e){
      e.lang = lang;
      e.title = title;
      e.descriptor = descriptor;
      e.start_at = start_at;
      e.finish_at = finish_at;
      e.category_id = category_id;
      e.is_regular = is_regular;
      e.location_id = location_id;
      e.geoposition = geoposition;
      e.location = location;
      e.is_free = price.amount > 0 ? false : true;
      e.activated = activated;
      e.token_contract = token_contract;
      e.price = price;
      e.meta = meta;
    }); 
  }


  [[eosio::action]] void unicore::addlocation(eosio::name host, eosio::name coordinator, eosio::string title, eosio::string geoposition, eosio::string meta){
    
    require_auth(host);
    locations_index locations(_me, host.value);

    locations.emplace(host, [&](auto &l){
      l.id = locations.available_primary_key();
      l.coordinator = coordinator;
      l.geoposition = geoposition;
      l.title = title;
      l.meta = meta;
    });

  }

[[eosio::action]] void unicore::rmlocation(eosio::name host, uint64_t id){
    
    require_auth(host);
    locations_index locations(_me, host.value);
    auto location = locations.find(id);
    eosio::check(location != locations.end(), "Location is not found");
    locations.erase(location);

  }


  [[eosio::action]] void unicore::addcategory(eosio::name host, eosio::name lang, eosio::string title){
    
    require_auth(host);

    categories_index categories(_me, host.value);

    categories.emplace(host, [&](auto &c){
      c.id = categories.available_primary_key();
      c.lang = lang;
      c.title = title;
    });

  }


  [[eosio::action]] void unicore::rmcategory(eosio::name host, uint64_t id){
    
    require_auth(host);
    categories_index categories(_me, host.value);

    auto category = categories.find(id);

    categories.erase(category);

    
  }
}