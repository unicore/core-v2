
void unicore2::add_balance(eosio::name payer, eosio::asset quantity, eosio::name contract){
    require_auth(payer);

    ubalances_index ubalances(_me, payer.value);
    
    auto balances_by_contract_and_symbol = ubalances.template get_index<"byconsym"_n>();
    auto contract_and_symbol_index = combine_ids(contract.value, quantity.symbol.code().raw());

    auto balance = balances_by_contract_and_symbol.find(contract_and_symbol_index);

    if (balance  == balances_by_contract_and_symbol.end()){
      ubalances.emplace(_me, [&](auto &b) {
        b.id = ubalances.available_primary_key();
        b.contract = contract;
        b.quantity = quantity;
      }); 
    } else {
      balances_by_contract_and_symbol.modify(balance, _me, [&](auto &b) {
        b.quantity += quantity;
      });
    };
  
}



void unicore2::sub_balance(eosio::name username, eosio::asset quantity, eosio::name contract){
    ubalances_index ubalances(_me, username.value);
    
    auto balances_by_contract_and_symbol = ubalances.template get_index<"byconsym"_n>();
    auto contract_and_symbol_index = combine_ids(contract.value, quantity.symbol.code().raw());

    auto balance = balances_by_contract_and_symbol.find(contract_and_symbol_index);
    
    eosio::check(balance != balances_by_contract_and_symbol.end(), "Balance is not found");
    
    eosio::check(balance -> quantity >= quantity, "Not enought user balance for create order");

    if (balance -> quantity == quantity) {

      balances_by_contract_and_symbol.erase(balance);

    } else {

      balances_by_contract_and_symbol.modify(balance, _me, [&](auto &b) {
        b.quantity -= quantity;
      });  

    }
    
}