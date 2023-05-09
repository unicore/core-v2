using namespace eosio;


    void unicore2::spread_to_refs(eosio::name host, eosio::name username, eosio::asset spread_amount, eosio::asset from_amount, eosio::name token_contract){
        partners2_index refs(_partners, _partners.value);
        auto ref = refs.find(username.value);

        account_index accounts(_me, host.value);
        auto acc = accounts.find(host.value);
        
        eosio::check(acc -> root_token_contract == token_contract, "Wrong token contract");

        eosio::name referer;
        uint8_t count = 1;
            
        if ((ref != refs.end()) && ((ref->referer).value != 0)) {
            referer = ref->referer;
            eosio::name prev_ref = ""_n;
            eosio::asset paid = asset(0, spread_amount.symbol);
        
            /**
             * Вычисляем размер выплаты для каждого уровня партнеров и производим выплаты.
             */

            for (auto level : acc->levels) {
                
                if ((ref != refs.end()) && ((ref->referer).value != 0)) { 
                    uint64_t to_ref_segments = spread_amount.amount * level / 100 / ONE_PERCENT;
                    eosio::asset to_ref_amount = asset(to_ref_segments, spread_amount.symbol);
                    
                    refbalances_index refbalances(_me, referer.value);
                    
                    refbalances.emplace(_me, [&](auto &rb){
                        rb.id = refbalances.available_primary_key();
                        rb.timepoint_sec = eosio::time_point_sec(eosio::current_time_point().sec_since_epoch());
                        rb.host = host;
                        rb.refs_amount = spread_amount;
                        rb.win_amount = from_amount;
                        rb.amount = to_ref_amount;
                        rb.from = username;
                        rb.level = count;
                        rb.percent = level;
                        rb.cashback = ref->cashback;
                        rb.segments = (double)to_ref_segments * TOTAL_SEGMENTS;
                    });
                    paid += to_ref_amount;
                    
                    prev_ref = referer;
                    ref = refs.find(referer.value);
                    
                    referer = ref->referer;
                    
                    count++;

                } else {

                    break;

                }
            };

            /**
             * Все неиспользуемые вознаграждения с вышестояющих уровней отправляются на компании
             */
            eosio::asset back_to_host = spread_amount - paid;
            
            if (back_to_host.amount > 0) {
                    unicore2::spread_to_dacs(host, back_to_host, acc -> root_token_contract);     
            }


            
        } else {
            /**
             * Если рефералов у пользователя нет, то переводим все реферальные средства пользователю.
             * * Если рефералов у пользователя нет, то переводим все реферальные средства компании.
             */
            if (spread_amount.amount > 0){
                unicore2::spread_to_dacs(host, spread_amount, acc -> root_token_contract);
            }
       
        }

    };



  /**
   * @brief      Метод вывода партнерского финансового потока
   * withdraw power quant (withpowerun)
  */
  [[eosio::action]] void unicore2::withrbenefit(eosio::name username, eosio::name host, uint64_t id){
    require_auth(username);
    account_index account(_me, host.value);
 
    auto acc = account.find(host.value);
    auto root_symbol = acc->get_root_symbol();
    
    uint64_t refreezesecs = unicore2::getcondition(host, "refreezesecs");

    refbalances_index refbalances(_me, username.value);
    
    uint64_t count=0;
    uint64_t on_withdraw = 0;
    
    uint128_t sediment = 0;

    auto refbalance = refbalances.find(id);
    eosio::check(refbalance != refbalances.end(), "Ref balance is not found");

    if (uint128_t(refbalance->amount.amount * TOTAL_SEGMENTS) < refbalance->segments) {
      sediment += refbalance->segments - uint128_t(refbalance->amount.amount * TOTAL_SEGMENTS);
    }

    on_withdraw += refbalance->amount.amount;
    
    uint64_t level = refbalance -> level;
    
    eosio::asset on_widthdraw_asset = asset(on_withdraw, refbalance->amount.symbol);
    

    if (on_withdraw > 0) {
        eosio::check(root_symbol == on_widthdraw_asset.symbol, "Cant make transfer action with non-root host symbol");
          
        action(
            permission_level{ _me, "active"_n },
            acc->root_token_contract, "transfer"_n,
            std::make_tuple( _me, username, on_widthdraw_asset, std::string("RFLOW-" + (name{username}.to_string() + "-" + (name{host}).to_string()) ))
        ).send();


    } 

    refbalances.erase(refbalance);
  
}
