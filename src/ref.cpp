using namespace eosio;
  /**
   * @brief      Метод вывода остатка партнерского финансового потока
   * withdraw power quant (withpowerun)
  */
  
  [[eosio::action]] void unicore::withrsegment(eosio::name username, eosio::name host){
    require_auth(username);

    rstat_index rstats(_me, username.value);
    account_index account(_me, host.value);
 
    auto acc = account.find(host.value);
    auto root_symbol = acc->get_root_symbol();
   
    auto rstat = rstats.find(host.value);
    
    eosio::check(rstat != rstats.end(), "Referal object for current username is not found");

    uint128_t segments = (uint128_t)rstat -> sediment;
    uint64_t amount = segments / TOTAL_SEGMENTS;

    if (amount > 0) {
      uint128_t new_sediment = segments - amount * TOTAL_SEGMENTS;

      eosio::asset amount_in_asset = asset(amount, root_symbol);

      rstats.modify(rstat,  username, [&](auto &rs){
        rs.withdrawed +=  amount_in_asset;
        rs.sediment = double(new_sediment);
      });

      uint64_t refreezesecs = unicore::getcondition(host, "refreezesecs");

      if (refreezesecs > 0) {

        make_vesting_action(username, host, acc -> root_token_contract, amount_in_asset, refreezesecs, "refwithdraw"_n);
      

      } else {


        action(
            permission_level{ _me, "active"_n },
            acc->root_token_contract, "transfer"_n,
            std::make_tuple( _me, username, amount_in_asset, std::string("SEDIMENT-" + (name{username}.to_string() + "-" + (name{host}).to_string()) ))
        ).send();
      }
    } else {
      eosio::check(false, "Not enought segments for convertation");
    }
    


  }
  /**
   * @brief      Метод вывода партнерского финансового потока
   * withdraw power quant (withpowerun)
  */
  [[eosio::action]] void unicore::withrbenefit(eosio::name username, eosio::name host, uint64_t id){
    require_auth(username);
    account_index account(_me, host.value);
 
    auto acc = account.find(host.value);
    auto root_symbol = acc->get_root_symbol();
    
    uint64_t refreezesecs = unicore::getcondition(host, "refreezesecs");

    refbalances_index refbalances(_me, username.value);
    
    uint64_t count=0;
    uint64_t on_withdraw = 0;
    
    uint128_t sediment = 0;

    // auto id = ids.begin();

    auto refbalance = refbalances.find(id);
    eosio::check(refbalance != refbalances.end(), "Some ref balance is not found");

    if (uint128_t(refbalance->amount.amount * TOTAL_SEGMENTS) < refbalance->segments) {
      sediment += refbalance->segments - uint128_t(refbalance->amount.amount * TOTAL_SEGMENTS);
    }

    on_withdraw += refbalance->amount.amount;

    uint64_t level = refbalance -> level;
    
    refbalances.erase(refbalance);
    

    print("on_withdraw: ", on_withdraw);
    eosio::asset on_widthdraw_asset = asset(on_withdraw, root_symbol);

    if (on_withdraw > 0) {

      if (refreezesecs > 0) {
        
        if (level == 1) {
          //make transfer
        
          action(
            permission_level{ _me, "active"_n },
            acc->root_token_contract, "transfer"_n,
            std::make_tuple( _me, username, on_widthdraw_asset, std::string("RFLOW-" + (name{username}.to_string() + "-" + (name{host}).to_string()) ))
          ).send();
        
        } else {

          make_vesting_action(username, host, acc -> root_token_contract, on_widthdraw_asset, refreezesecs, "refwithdraw"_n);  
        
        }
        
      
      } else {
        
        action(
            permission_level{ _me, "active"_n },
            acc->root_token_contract, "transfer"_n,
            std::make_tuple( _me, username, on_widthdraw_asset, std::string("RFLOW-" + (name{username}.to_string() + "-" + (name{host}).to_string()) ))
        ).send();

      }

      rstat_index rstats(_me, username.value);

      auto rstat = rstats.find(host.value);
      if (rstat == rstats.end()){
        rstats.emplace(username, [&](auto &rs){
          rs.host = host;
          rs.withdrawed = on_widthdraw_asset;
          rs.sediment = double(sediment);
        });

      } else {
        rstats.modify(rstat, username, [&](auto &rs){
          rs.withdrawed += on_widthdraw_asset;
          rs.sediment += sediment;
        });
      }


  } else {
      eosio::check(false, "Nothing to withdraw");
    }

  }
